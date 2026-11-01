/*
* Copyright (C) 2026 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/*
 * Functional test for the adlt adaptation of the prelinker in
 * ldso/linux/dynlink.c:
 *
 *   - every nDSO of one merged adlt library is a separate struct dso sharing
 *     dev/ino, so the relro cache must hold exactly one entry for the file
 *     (prelink_dedup_adlt_dsos());
 *   - that entry only carries the load address, its offset must be 0 because
 *     the RELRO of a merged library is never snapshotted or shared
 *     (prelink_skip_relro()), which keeps a process that loads a few nDSOs
 *     from faulting in the whole merged RELRO segment;
 *   - both nDSOs must still load at the recorded address and work.
 *
 * The test needs a real merged adlt library (built with ld.lld --adlt), so it
 * can only run on an OHOS device with an adlt enabled build.
 */

#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <link.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "functionalext.h"

#define LIST_PATH "/data/local/tmp/prelink_dedup_list"
/*
 * The nDSO symlinks must be used: the loader derives the nDSO index from the
 * opened name. runtest_linux.sh copies libc-test-lib and hdc follows symlinks,
 * so that copy holds standalone nDSOs, and with "-rpath=./:..." a bare name
 * would resolve there. Use absolute paths and verify (dev, ino) up front.
 */
#define LIB_DIR "/data/local/tmp/libc-test-lib/"
#define PAGE_SIZE_ (0x1000ULL)
#define MERGED_LIB "libadlt_prelink_dedup.so"
#define NDSO_A_LIB "libadlt_prelink_dedup_a.so"
#define NDSO_B_LIB "libadlt_prelink_dedup_b.so"

/* Layout of the relro_cache memfd written by do_dlprelink_record(). */
struct relro_cache_header {
	uint64_t interp_base;
	uint64_t vdso_base;
	uint64_t reserve_base;
	uint64_t dso_nr;
	uint64_t cache_size;
};

struct relro_cache_entry {
	dev_t dev;
	ino_t ino;
	void *base;
	size_t offset;
};

static int write_list(const char *list_path, const char *const *paths, size_t nr)
{
	int fd = open(list_path, O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (fd < 0) {
		return -1;
	}
	for (size_t i = 0; i < nr; i++) {
		size_t len = strlen(paths[i]);
		if (write(fd, paths[i], len) != (ssize_t)len ||
		    write(fd, "\n", 1) != 1) {
			close(fd);
			return -1;
		}
	}
	close(fd);
	return 0;
}

static int record_in_child(int memfd, const char *list_path)
{
	int ws = 0;
	pid_t pid;

	fflush(stdout);
	pid = fork();
	if (pid < 0) {
		return -1;
	}
	if (pid == 0) {
		int ret = dlprelink_record(memfd, list_path);
		if (ret != 0) {
			t_error("dlprelink_record failed: %d (%s)\n", ret, dlerror());
		}
		_exit(ret == 0 ? 0 : 1);
	}
	if (waitpid(pid, &ws, 0) != pid || !WIFEXITED(ws) || WEXITSTATUS(ws) != 0) {
		return -1;
	}
	return 0;
}

/*
 * Collect the entries of the merged library. Returns the number of entries,
 * stores the first one in *first.
 */
static size_t find_entries(int fd, const struct stat *st, struct relro_cache_entry *first)
{
	struct relro_cache_header hdr;
	struct relro_cache_entry entry;
	size_t found = 0;

	if (pread(fd, &hdr, sizeof(hdr), 0) != sizeof(hdr) || hdr.dso_nr == 0) {
		return 0;
	}
	for (uint64_t i = 0; i < hdr.dso_nr; i++) {
		off_t off = (off_t)sizeof(hdr) + (off_t)i * (off_t)sizeof(entry);
		if (pread(fd, &entry, sizeof(entry), off) != sizeof(entry)) {
			return 0;
		}
		if (entry.dev == st->st_dev && entry.ino == st->st_ino) {
			if (found == 0) {
				*first = entry;
			}
			found++;
		}
	}
	return found;
}

/*
 * /proc/self/maps also lists the .adlt/.got/.bolt.remap auxiliary mappings at
 * arbitrary addresses, so only the program headers give the real load range.
 */
struct lib_range {
	const char *name_prefix;
	int found;
	unsigned long long base;
	unsigned long long load_lo;
	unsigned long long load_hi;
	unsigned long long relro_lo;
	unsigned long long relro_hi;
};

static int lib_range_cb(struct dl_phdr_info *info, size_t size, void *data)
{
	struct lib_range *range = data;
	unsigned long long lo = ULLONG_MAX;
	unsigned long long hi = 0;

	(void)size;
	if (!info->dlpi_name || !strstr(info->dlpi_name, range->name_prefix)) {
		return 0;
	}
	for (ElfW(Half) i = 0; i < info->dlpi_phnum; i++) {
		const ElfW(Phdr) *ph = &info->dlpi_phdr[i];
		unsigned long long seg_lo;
		unsigned long long seg_hi;
		if (ph->p_type == PT_GNU_RELRO) {
			range->relro_lo = (unsigned long long)info->dlpi_addr + ph->p_vaddr;
			range->relro_hi = range->relro_lo + ph->p_memsz;
			continue;
		}
		if (ph->p_type != PT_LOAD) {
			continue;
		}
		seg_lo = (unsigned long long)info->dlpi_addr +
			(ph->p_vaddr & ~(unsigned long long)(PAGE_SIZE_ - 1));
		seg_hi = (unsigned long long)info->dlpi_addr +
			((ph->p_vaddr + ph->p_memsz + PAGE_SIZE_ - 1) &
				~(unsigned long long)(PAGE_SIZE_ - 1));
		if (seg_lo < lo) {
			lo = seg_lo;
		}
		if (seg_hi > hi) {
			hi = seg_hi;
		}
	}
	/* nDSOs of one merged library share the same program headers. */
	if (!range->found) {
		range->found = 1;
		range->base = (unsigned long long)info->dlpi_addr;
		range->load_lo = lo;
		range->load_hi = hi;
	}
	return 1;
}

static void get_lib_range(const char *name_prefix, struct lib_range *range)
{
	memset(range, 0, sizeof(*range));
	range->name_prefix = name_prefix;
	dl_iterate_phdr(lib_range_cb, range);
}

/* No /memfd:relro_cache range may overlap the merged RELRO range. */
static int relro_is_private(const struct lib_range *range)
{
	char line[1024];
	int shared = 0;

	FILE *fp = fopen("/proc/self/maps", "r");
	if (!fp) {
		return 0;
	}
	while (!shared && fgets(line, sizeof(line), fp)) {
		unsigned long long lo;
		unsigned long long hi;
		if (!strstr(line, "memfd:relro_cache")) {
			continue;
		}
		if (sscanf(line, "%llx-%llx", &lo, &hi) != 2) {
			continue;
		}
		shared = range->relro_hi != 0 &&
			lo < range->relro_hi && hi > range->relro_lo;
	}
	fclose(fp);
	return shared == 0;
}

static int run_ndso(const char *path, const char *sym, int expect)
{
	int (*fn)(void);
	void *handle = dlopen(path, RTLD_LAZY | RTLD_LOCAL);

	if (!handle) {
		t_error("dlopen %s failed: %s\n", path, dlerror());
		return -1;
	}
	fn = (int (*)(void))dlsym(handle, sym);
	if (!fn) {
		t_error("dlsym %s failed: %s\n", sym, dlerror());
		dlclose(handle);
		return -1;
	}
	int ret = fn();
	if (ret != expect) {
		t_error("%s returned %d, want %d\n", sym, ret, expect);
		dlclose(handle);
		return -1;
	}
	return 0;
}


int main(int argc, char *argv[])
{
	char path_a[PATH_MAX];
	char path_b[PATH_MAX];
	char merged[PATH_MAX];
	const char *paths[2];
	struct stat st_merged;
	struct relro_cache_entry entry;
	size_t nr_merged;
	unsigned long long recorded_base;
	struct lib_range range;
	int memfd;
	const char *fn = __func__;

	(void)argc;
	(void)argv;

	if (snprintf(path_a, sizeof(path_a), "%s%s", LIB_DIR, NDSO_A_LIB) >= (int)sizeof(path_a) ||
	    snprintf(path_b, sizeof(path_b), "%s%s", LIB_DIR, NDSO_B_LIB) >= (int)sizeof(path_b) ||
	    snprintf(merged, sizeof(merged), "%s%s", LIB_DIR, MERGED_LIB) >= (int)sizeof(merged)) {
		t_error("library path too long\n");
		return 1;
	}
	if (access(path_a, R_OK) != 0 || access(path_b, R_OK) != 0) {
		t_error("adlt test libraries are missing.\n"
			"  expected: %s and %s\n"
			"  build with musl_use_adlt=true, then on the device run:\n"
			"    sh /data/local/tmp/libc-test-lib/libadlt_prelink_dedup.sh\n",
			path_a, path_b);
		return 1;
	}
	if (stat(merged, &st_merged) != 0) {
		t_error("stat %s failed: %s\n", merged, strerror(errno));
		return 1;
	}
	/* A standalone nDSO here would (dev, ino) mismatch and silently skip prelink. */
	struct stat st_a;
	struct stat st_b;
	if (stat(path_a, &st_a) != 0 || stat(path_b, &st_b) != 0) {
		t_error("stat nDSO paths failed: %s\n", strerror(errno));
		return 1;
	}
	if (st_a.st_dev != st_merged.st_dev || st_a.st_ino != st_merged.st_ino ||
	    st_b.st_dev != st_merged.st_dev || st_b.st_ino != st_merged.st_ino) {
		t_error("nDSO paths do not resolve to the merged library.\n"
			"  %s -> dev=%llu ino=%llu\n"
			"  %s -> dev=%llu ino=%llu\n"
			"  %s -> dev=%llu ino=%llu\n"
			"  ensure adlt_tools.py created the symlinks in %s\n",
			path_a, (unsigned long long)st_a.st_dev, (unsigned long long)st_a.st_ino,
			path_b, (unsigned long long)st_b.st_dev, (unsigned long long)st_b.st_ino,
			merged, (unsigned long long)st_merged.st_dev, (unsigned long long)st_merged.st_ino,
			LIB_DIR);
		return 1;
	}
	paths[0] = path_a;
	paths[1] = path_b;
	printf("merged library: %s\n", merged);
	printf("nDSO paths: %s , %s\n", path_a, path_b);

	if (dlprelink_reserve_mem() != 0) {
		t_error("dlprelink_reserve_mem failed: %s\n", dlerror());
		return 1;
	}
	memfd = memfd_create("relro_cache", MFD_ALLOW_SEALING);
	if (memfd < 0) {
		t_error("memfd_create failed: %s\n", strerror(errno));
		return 1;
	}
	if (write_list(LIST_PATH, paths, 2) != 0) {
		t_error("write list failed: %s\n", strerror(errno));
		remove(LIST_PATH);
		close(memfd);
		return 1;
	}
	if (record_in_child(memfd, LIST_PATH) != 0) {
		t_error("prelink record failed\n");
		remove(LIST_PATH);
		close(memfd);
		return 1;
	}
	remove(LIST_PATH);

	nr_merged = find_entries(memfd, &st_merged, &entry);
	if (nr_merged != 1) {
		t_error("[%s] merged library recorded %zu times, want 1\n", fn, nr_merged);
		close(memfd);
		return 1;
	}
	if (entry.offset != 0) {
		t_error("[%s] merged library relro must not be recorded, offset=%zu\n",
			fn, entry.offset);
		close(memfd);
		return 1;
	}
	recorded_base = (unsigned long long)(uintptr_t)entry.base;
	if (recorded_base == 0) {
		t_error("[%s] merged library has no recorded load address\n", fn);
		close(memfd);
		return 1;
	}

	if (fcntl(memfd, F_ADD_SEALS,
		F_SEAL_SEAL | F_SEAL_GROW | F_SEAL_SHRINK | F_SEAL_FUTURE_WRITE) != 0) {
		t_error("F_ADD_SEALS failed: %s\n", strerror(errno));
		close(memfd);
		return 1;
	}
	if (dlprelink_register(memfd) != 0) {
		t_error("dlprelink_register failed: %s\n", dlerror());
		close(memfd);
		return 1;
	}

	if (run_ndso(path_a, "prelink_dedup_a", 100) != 0 ||
	    run_ndso(path_b, "prelink_dedup_b", 200) != 0) {
		close(memfd);
		return 1;
	}
	get_lib_range("libadlt_prelink_dedup", &range);
	if (!range.found) {
		t_error("[%s] merged library not found by dl_iterate_phdr\n", fn);
	} else {
		/* dlpi_addr is the load base, which is what prelink records. */
		if (range.base != recorded_base) {
			t_error("[%s] merged library loaded at 0x%llx, want 0x%llx "
				"(PT_LOAD range 0x%llx-0x%llx, PT_GNU_RELRO range 0x%llx-0x%llx)\n",
				fn, range.base, recorded_base,
				range.load_lo, range.load_hi,
				range.relro_lo, range.relro_hi);
		}

		/* RELRO must stay private to the file mapping. */
		if (!relro_is_private(&range)) {
			t_error("[%s] merged library relro must not be shared from relro_cache "
				"(PT_GNU_RELRO range 0x%llx-0x%llx)\n",
				fn, range.relro_lo, range.relro_hi);
		}
	}

	close(memfd);
	return t_status ? 1 : 0;
}
