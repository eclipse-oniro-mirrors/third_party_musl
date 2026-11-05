/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Functional test for the prelinker address carving in
 * ldso/linux/dynlink.c (map_library / task_map_library):
 *
 *   - the start address of every prelinked DSO must be aligned to
 *     max(segment maximum alignment, LIBRARY_ALIGNMENT);
 *   - the prelinker cursor must advance to the aligned end of the previous
 *     DSO, so two prelinked DSOs never share an address;
 *   - the relro cache must stay usable after the address change.
 *
 * This cannot run on a host: it needs the OHOS ldso prelink interface
 * (dlprelink_reserve_mem / dlprelink_record / dlprelink_register) and a kernel
 * that provides transparent hugepages.
 */

#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <link.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "functionalext.h"

/* Must match ldso/linux/cfi.h and ldso/linux/dynlink.c. */
#define LIBRARY_ALIGNMENT (0x40000ULL)
#define KPMD_SIZE (0x200000ULL)
#define PAGE_SIZE_ (0x1000ULL)
/* Entry offset that means "no RELRO snapshot was written". */
#define PRELINK_NO_RELRO_OFFSET 0

#define LIST_PATH "/data/local/tmp/prelink_align_list"
#define LIB_DIR "/data/local/tmp/libc-test-lib/"
/* output_name in BUILD.gn is "prelink_align_*", so the files are lib*.so. */
#define HUGE_LIB "libprelink_align_huge.so"
#define NORMAL_LIB "libprelink_align_normal.so"
#define HUGE_SYM "prelink_align_probe"
#define NORMAL_SYM "prelink_align_normal"

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

struct dso_span {
	const char *name;
	int found;
	uintptr_t base;
	uintptr_t start;
	uintptr_t end;
};

#define CHECK_ULL(fun, got, expected)                                        \
	do {                                                                     \
		unsigned long long g_ = (unsigned long long)(got);                    \
		unsigned long long e_ = (unsigned long long)(expected);               \
		if (g_ != e_) {                                                       \
			t_error("[%s] %s: got 0x%llx, want 0x%llx\n", fun, #got, g_, e_); \
		}                                                                     \
	} while (0)

#define CHECK_TRUE(fun, cond)                                    \
	do {                                                         \
		if (!(cond)) {                                           \
			t_error("[%s] check failed: %s\n", fun, #cond);      \
		}                                                        \
	} while (0)

static int hugepage_enabled(void)
{
	char buf[64] = {0};
	int fd = open("/sys/kernel/mm/transparent_hugepage/enabled", O_RDONLY);
	if (fd < 0) {
		return 0;
	}
	ssize_t n = read(fd, buf, sizeof(buf) - 1);
	close(fd);
	if (n <= 0) {
		return 0;
	}
	return strstr(buf, "[never]") == NULL;
}

static int basename_is(const char *path, const char *name)
{
	const char *slash;
	if (!path) {
		return 0;
	}
	slash = strrchr(path, '/');
	slash = slash ? slash + 1 : path;
	return strcmp(slash, name) == 0;
}

static int span_cb(struct dl_phdr_info *info, size_t size, void *data)
{
	struct dso_span *span = data;
	uintptr_t lo = UINTPTR_MAX;
	uintptr_t hi = 0;

	(void)size;
	if (!basename_is(info->dlpi_name, span->name)) {
		return 0;
	}
	for (ElfW(Half) i = 0; i < info->dlpi_phnum; i++) {
		const ElfW(Phdr) *ph = &info->dlpi_phdr[i];
		uintptr_t seg_lo;
		uintptr_t seg_hi;
		if (ph->p_type != PT_LOAD) {
			continue;
		}
		seg_lo = (uintptr_t)(ph->p_vaddr & ~(PAGE_SIZE_ - 1));
		seg_hi = (uintptr_t)((ph->p_vaddr + ph->p_memsz + PAGE_SIZE_ - 1) &
			~(PAGE_SIZE_ - 1));
		if (seg_lo < lo) {
			lo = seg_lo;
		}
		if (seg_hi > hi) {
			hi = seg_hi;
		}
	}
	if (lo == UINTPTR_MAX) {
		return 0;
	}
	span->found = 1;
	span->base = (uintptr_t)info->dlpi_addr;
	span->start = (uintptr_t)info->dlpi_addr + lo;
	span->end = (uintptr_t)info->dlpi_addr + hi;
	return 1;
}

static int get_dso_span(const char *name, struct dso_span *span)
{
	memset(span, 0, sizeof(*span));
	span->name = name;
	dl_iterate_phdr(span_cb, span);
	return span->found;
}

/*
 * The cache itself is mapped too, at an unrelated address, so look for a
 * memfd mapping that overlaps the DSO range.
 */
static int relro_shared_for(uintptr_t start, uintptr_t end)
{
	char line[1024];
	int found = 0;
	FILE *fp = fopen("/proc/self/maps", "r");

	if (!fp) {
		return 0;
	}
	while (fgets(line, sizeof(line), fp)) {
		unsigned long long lo;
		unsigned long long hi;
		if (!strstr(line, "memfd:relro_cache")) {
			continue;
		}
		if (sscanf(line, "%llx-%llx", &lo, &hi) != 2) {
			continue;
		}
		if (lo < end && hi > start) {
			found = 1;
			break;
		}
	}
	fclose(fp);
	return found;
}

/* Some runners copy the DSOs next to the executable instead. */
static int find_lib(char *buf, size_t size, char *argv0, char *name)
{
	char sibling[512];

	if (t_pathrel(sibling, sizeof(sibling), argv0, name) && access(sibling, R_OK) == 0) {
		snprintf(buf, size, "%s", sibling);
		return 0;
	}
	snprintf(buf, size, "%s%s", LIB_DIR, name);
	if (access(buf, R_OK) == 0) {
		return 0;
	}
	t_error("cannot find %s near the executable or in %s\n", name, LIB_DIR);
	return -1;
}

/* Must match PRELINKER_RESERVE_MEMORY_LENGTH in ldso/linux/dynlink.c. */
#if UINTPTR_MAX == UINT64_MAX
#define RESERVE_LEN (8ULL * 1024 * 1024 * 1024)
#else
#define RESERVE_LEN (128ULL * 1024 * 1024)
#endif
#define CACHE_SIZE_ (0x4000ULL)
/* Page aligned, above the metadata area, below CACHE_SIZE_. */
#define GOOD_OFFSET (0x1000ULL)

/* Find an address able to hold the reserve area. */
static uintptr_t probe_reserve_area(void)
{
	void *p = mmap(NULL, (size_t)RESERVE_LEN, PROT_NONE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (p == MAP_FAILED) {
		return 0;
	}
	munmap(p, (size_t)RESERVE_LEN);
	return (uintptr_t)p;
}

/*
 * Entry 0 is ldso, exempt from the reserve area check; entry 1 is validated
 * like any regular DSO. A child process is used because the register state is
 * process wide.
 */
static void cache_case(const char *fn, const char *what, uintptr_t reserve_base,
	size_t offset, uintptr_t base, uint64_t declared_size, uint64_t file_size,
	int want_accept)
{
	struct relro_cache_header hdr;
	struct relro_cache_entry entries[2];
	int memfd;

	memset(&hdr, 0, sizeof(hdr));
	memset(entries, 0, sizeof(entries));
	hdr.dso_nr = 2;
	hdr.cache_size = declared_size;
	hdr.reserve_base = reserve_base;
	entries[0].offset = PRELINK_NO_RELRO_OFFSET;
	entries[0].base = (void *)PAGE_SIZE_;
	entries[1].offset = offset;
	entries[1].base = (void *)base;

	memfd = memfd_create("relro_cache", MFD_ALLOW_SEALING);
	if (memfd < 0) {
		t_error("[%s] memfd_create failed: %s\n", fn, strerror(errno));
		return;
	}
	if (ftruncate(memfd, (off_t)file_size) != 0 ||
	    pwrite(memfd, &hdr, sizeof(hdr), 0) != (ssize_t)sizeof(hdr) ||
	    pwrite(memfd, entries, sizeof(entries), (off_t)sizeof(hdr)) != (ssize_t)sizeof(entries)) {
		t_error("[%s] failed to build %s cache: %s\n", fn, what, strerror(errno));
		close(memfd);
		return;
	}
	if (fcntl(memfd, F_ADD_SEALS,
		F_SEAL_SEAL | F_SEAL_GROW | F_SEAL_SHRINK | F_SEAL_FUTURE_WRITE) != 0) {
		t_error("[%s] F_ADD_SEALS failed: %s\n", fn, strerror(errno));
		close(memfd);
		return;
	}

	int ws = 0;
	pid_t pid;
	fflush(stdout);
	pid = fork();
	if (pid == 0) {
		int ret = dlprelink_register(memfd);
		_exit(ret == 0 ? 0 : 1);
	}
	if (pid < 0) {
		t_error("[%s] fork failed: %s\n", fn, strerror(errno));
	} else if (waitpid(pid, &ws, 0) != pid) {
		t_error("[%s] waitpid failed: %s\n", fn, strerror(errno));
	} else if (!WIFEXITED(ws)) {
		t_error("[%s] %s: child died on signal %d\n", fn, what, WTERMSIG(ws));
	} else if (want_accept && WEXITSTATUS(ws) != 0) {
		printf("[%s] %s cache was rejected, want acceptance\n", fn, what);
	} else if (!want_accept && WEXITSTATUS(ws) == 0) {
		t_error("[%s] %s cache was accepted, want rejection\n", fn, what);
	}
	close(memfd);
}

/* An unordered cache must be rejected: lookup_relro_cache() bsearches it. */
static void cache_order_case(const char *fn, const char *what,
	ino_t ino1, dev_t dev1, ino_t ino2, dev_t dev2)
{
	struct relro_cache_header hdr;
	struct relro_cache_entry entries[3];
	uintptr_t r = probe_reserve_area();
	int memfd;

	if (r == 0) {
		t_error("[%s] cannot probe a reserve area: %s\n", fn, strerror(errno));
		return;
	}

	memset(&hdr, 0, sizeof(hdr));
	memset(entries, 0, sizeof(entries));
	hdr.dso_nr = 3;
	hdr.cache_size = CACHE_SIZE_;
	hdr.reserve_base = r;
	entries[0].offset = PRELINK_NO_RELRO_OFFSET;
	entries[1].ino = ino1;
	entries[1].dev = dev1;
	entries[1].base = (void *)(r + 0x100000);
	entries[1].offset = PRELINK_NO_RELRO_OFFSET;
	entries[2].ino = ino2;
	entries[2].dev = dev2;
	entries[2].base = (void *)(r + 0x200000);
	entries[2].offset = PRELINK_NO_RELRO_OFFSET;

	memfd = memfd_create("relro_cache", MFD_ALLOW_SEALING);
	if (memfd < 0) {
		t_error("[%s] memfd_create failed: %s\n", fn, strerror(errno));
		return;
	}
	if (ftruncate(memfd, (off_t)CACHE_SIZE_) != 0 ||
	    pwrite(memfd, &hdr, sizeof(hdr), 0) != (ssize_t)sizeof(hdr) ||
	    pwrite(memfd, entries, sizeof(entries), (off_t)sizeof(hdr)) != (ssize_t)sizeof(entries)) {
		t_error("[%s] failed to build %s cache: %s\n", fn, what, strerror(errno));
		close(memfd);
		return;
	}
	if (fcntl(memfd, F_ADD_SEALS,
		F_SEAL_SEAL | F_SEAL_GROW | F_SEAL_SHRINK | F_SEAL_FUTURE_WRITE) != 0) {
		t_error("[%s] F_ADD_SEALS failed: %s\n", fn, strerror(errno));
		close(memfd);
		return;
	}

	int ws = 0;
	pid_t pid;
	fflush(stdout);
	pid = fork();
	if (pid == 0) {
		int ret = dlprelink_register(memfd);
		_exit(ret == 0 ? 0 : 1);
	}
	if (pid < 0) {
		t_error("[%s] fork failed: %s\n", fn, strerror(errno));
	} else if (waitpid(pid, &ws, 0) != pid) {
		t_error("[%s] waitpid failed: %s\n", fn, strerror(errno));
	} else if (!WIFEXITED(ws)) {
		t_error("[%s] %s: child died on signal %d\n", fn, what, WTERMSIG(ws));
	} else if (WEXITSTATUS(ws) == 0) {
		t_error("[%s] %s cache was accepted, want rejection\n", fn, what);
	}
	close(memfd);
}

/*
 * A base inside the area can still be too close to its end, so the check has to
 * happen at map time, where map_len is known. Failing it must not fail the load:
 * the DSO falls back to a normal mapping and must still be usable.
 */
static void check_prelinked_range_bounded(const char *fn, const char *path)
{
	struct relro_cache_header hdr;
	struct relro_cache_entry entries[2];
	struct stat st;
	uintptr_t r = probe_reserve_area();
	int memfd;

	if (r == 0) {
		t_error("[%s] cannot probe a reserve area: %s\n", fn, strerror(errno));
		return;
	}
	if (stat(path, &st) != 0) {
		t_error("[%s] stat %s failed: %s\n", fn, path, strerror(errno));
		return;
	}
	if ((uint64_t)st.st_size <= PAGE_SIZE_) {
		printf("[%s] %s is not larger than a page, skipping range case\n", fn, path);
		return;
	}

	memset(&hdr, 0, sizeof(hdr));
	memset(entries, 0, sizeof(entries));
	hdr.dso_nr = 2;
	hdr.cache_size = CACHE_SIZE_;
	hdr.reserve_base = r;
	entries[0].offset = PRELINK_NO_RELRO_OFFSET;
	entries[0].base = (void *)PAGE_SIZE_;
	/* Only one page left in the area, while the DSO maps more than that. */
	entries[1].ino = st.st_ino;
	entries[1].dev = st.st_dev;
	entries[1].base = (void *)(r + (uintptr_t)RESERVE_LEN - PAGE_SIZE_);
	entries[1].offset = PRELINK_NO_RELRO_OFFSET;

	memfd = memfd_create("relro_cache", MFD_ALLOW_SEALING);
	if (memfd < 0) {
		t_error("[%s] memfd_create failed: %s\n", fn, strerror(errno));
		return;
	}
	if (ftruncate(memfd, (off_t)CACHE_SIZE_) != 0 ||
	    pwrite(memfd, &hdr, sizeof(hdr), 0) != (ssize_t)sizeof(hdr) ||
	    pwrite(memfd, entries, sizeof(entries), (off_t)sizeof(hdr)) != (ssize_t)sizeof(entries)) {
		t_error("[%s] failed to build out of range cache: %s\n", fn, strerror(errno));
		close(memfd);
		return;
	}
	if (fcntl(memfd, F_ADD_SEALS,
		F_SEAL_SEAL | F_SEAL_GROW | F_SEAL_SHRINK | F_SEAL_FUTURE_WRITE) != 0) {
		t_error("[%s] F_ADD_SEALS failed: %s\n", fn, strerror(errno));
		close(memfd);
		return;
	}

	int ws = 0;
	pid_t pid;
	fflush(stdout);
	pid = fork();
	if (pid == 0) {
		if (dlprelink_register(memfd) != 0) {
			_exit(2);
		}
		void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
		if (!h) {
			/* A failed load is never acceptable: prelink is optional. */
			_exit(3);
		}
		/* The load must succeed, but not at the recorded address. */
		struct dso_span span;
		int at_recorded = get_dso_span(path, &span) &&
			span.base == (uintptr_t)entries[1].base;
		dlclose(h);
		_exit(at_recorded ? 4 : 0);
	}
	if (pid < 0) {
		t_error("[%s] fork failed: %s\n", fn, strerror(errno));
	} else if (waitpid(pid, &ws, 0) != pid) {
		t_error("[%s] waitpid failed: %s\n", fn, strerror(errno));
	} else if (!WIFEXITED(ws)) {
		t_error("[%s] out of range DSO: child died on signal %d\n", fn, WTERMSIG(ws));
	} else if (WEXITSTATUS(ws) == 2) {
		printf("[%s] out of range cache was rejected early, load not exercised\n", fn);
	} else if (WEXITSTATUS(ws) == 3) {
		t_error("[%s] out of range DSO failed to load, want a normal mapping\n", fn);
	} else if (WEXITSTATUS(ws) == 4) {
		t_error("[%s] DSO was still mapped at the out of range address, want fallback\n", fn);
	}
	close(memfd);
}

/*
 * The cache is untrusted input and must fail closed. The first case is well
 * formed, so it only reports a diagnostic: a platform whose prelink prctl
 * refuses the probed address could reject it for an unrelated reason.
 */
static void check_malformed_cache_rejected(const char *fn)
{
	uintptr_t r = probe_reserve_area();

	if (r == 0) {
		t_error("[%s] cannot probe a reserve area: %s\n", fn, strerror(errno));
		return;
	}

	cache_case(fn, "well formed", r, GOOD_OFFSET, r + 0x100000, CACHE_SIZE_, CACHE_SIZE_, 1);

	cache_case(fn, "zero reserve base", 0, GOOD_OFFSET, 0x100000, CACHE_SIZE_, CACHE_SIZE_, 0);
	cache_case(fn, "unaligned reserve base", r + 1, GOOD_OFFSET, r + 0x100000,
		CACHE_SIZE_, CACHE_SIZE_, 0);
	cache_case(fn, "cache size past eof", r, GOOD_OFFSET, r + 0x100000,
		CACHE_SIZE_, CACHE_SIZE_ / 2, 0);
	cache_case(fn, "offset in metadata", r, 8, r + 0x100000, CACHE_SIZE_, CACHE_SIZE_, 0);
	cache_case(fn, "offset past cache", r, 0x8000, r + 0x100000, CACHE_SIZE_, CACHE_SIZE_, 0);
	cache_case(fn, "unaligned offset", r, GOOD_OFFSET + 8, r + 0x100000,
		CACHE_SIZE_, CACHE_SIZE_, 0);
	cache_case(fn, "unaligned base", r, GOOD_OFFSET, r + 0x100001, CACHE_SIZE_, CACHE_SIZE_, 0);
	cache_case(fn, "base outside reserve area",
		r, GOOD_OFFSET, r + RESERVE_LEN, CACHE_SIZE_, CACHE_SIZE_, 0);
	cache_case(fn, "base below reserve area", r, GOOD_OFFSET, r - 0x100000,
		CACHE_SIZE_, CACHE_SIZE_, 0);
	cache_order_case(fn, "entries out of order", 20, 1, 10, 1);
	cache_order_case(fn, "duplicate entries", 10, 1, 10, 1);
}

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

static int find_entries(int fd, dev_t dev, ino_t ino,
	struct relro_cache_entry *first, size_t *nr_dup)
{
	struct relro_cache_header hdr;
	struct relro_cache_entry entry;
	size_t found = 0;

	if (pread(fd, &hdr, sizeof(hdr), 0) != sizeof(hdr) || hdr.dso_nr == 0) {
		return -1;
	}
	for (uint64_t i = 0; i < hdr.dso_nr; i++) {
		off_t off = (off_t)sizeof(hdr) + (off_t)i * (off_t)sizeof(entry);
		if (pread(fd, &entry, sizeof(entry), off) != sizeof(entry)) {
			return -1;
		}
		if (entry.dev == dev && entry.ino == ino) {
			if (found == 0) {
				*first = entry;
			}
			found++;
		}
	}
	*nr_dup = found;
	return found ? 0 : -1;
}

static int resolve_call(const char *path, const char *sym, int expect)
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
	if (fn() != expect) {
		t_error("%s returned unexpected value\n", sym);
		dlclose(handle);
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char huge[512];
	char normal[512];
	char huge_real[PATH_MAX];
	char normal_real[PATH_MAX];
	const char *paths[2];
	struct stat st_huge;
	struct stat st_normal;
	struct relro_cache_entry e_huge;
	struct relro_cache_entry e_normal;
	struct dso_span span_huge;
	struct dso_span span_normal;
	size_t nr_huge = 0;
	size_t nr_normal = 0;
	uint64_t huge_align;
	int memfd;
	const char *fn = __func__;

	(void)argc;
	if (find_lib(huge, sizeof(huge), argv[0], HUGE_LIB) != 0 ||
	    find_lib(normal, sizeof(normal), argv[0], NORMAL_LIB) != 0) {
		return 1;
	}
	if (!realpath(huge, huge_real) || !realpath(normal, normal_real)) {
		t_error("realpath failed: %s\n", strerror(errno));
		return 1;
	}
	/* Load the plain DSO first, so the hugepage DSO is not aligned for free. */
	paths[0] = normal_real;
	paths[1] = huge_real;

	/* The hugepage-aligned DSO only needs 2 MiB alignment when THP is on. */
	huge_align = hugepage_enabled() ? KPMD_SIZE : LIBRARY_ALIGNMENT;
	printf("transparent hugepage enabled: %s, expected hugepage DSO align: 0x%llx\n",
		huge_align == KPMD_SIZE ? "yes" : "no", (unsigned long long)huge_align);

	/*
	 * Both run before dlprelink_reserve_mem(), so the reserve area is still
	 * unset and every child can pick the address it declares in the header.
	 */
	check_malformed_cache_rejected(fn);
	check_prelinked_range_bounded(fn, normal_real);

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

	if (stat(huge_real, &st_huge) != 0 || stat(normal_real, &st_normal) != 0) {
		t_error("stat failed: %s\n", strerror(errno));
		close(memfd);
		return 1;
	}
	if (find_entries(memfd, st_huge.st_dev, st_huge.st_ino, &e_huge, &nr_huge) != 0) {
		t_error("hugepage DSO is not recorded\n");
		close(memfd);
		return 1;
	}
	if (find_entries(memfd, st_normal.st_dev, st_normal.st_ino, &e_normal, &nr_normal) != 0) {
		t_error("normal DSO is not recorded\n");
		close(memfd);
		return 1;
	}
	/* Two different files are each recorded exactly once. */
	CHECK_ULL(fn, nr_huge, 1);
	CHECK_ULL(fn, nr_normal, 1);

	/* Both DSOs have their first PT_LOAD at vaddr 0, so base == map. */
	CHECK_ULL(fn, (uintptr_t)e_huge.base % huge_align, 0);
	CHECK_ULL(fn, (uintptr_t)e_normal.base % LIBRARY_ALIGNMENT, 0);
	CHECK_TRUE(fn, e_huge.base != e_normal.base);

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

	/* Both DSOs must load at the recorded address and still work. */
	if (resolve_call(huge_real, HUGE_SYM, 42) != 0 ||
	    resolve_call(normal_real, NORMAL_SYM, 7) != 0) {
		close(memfd);
		return 1;
	}

	CHECK_TRUE(fn, get_dso_span(HUGE_LIB, &span_huge));
	CHECK_TRUE(fn, get_dso_span(NORMAL_LIB, &span_normal));
	if (span_huge.found && span_normal.found) {
		CHECK_ULL(fn, span_huge.base, (uintptr_t)e_huge.base);
		CHECK_ULL(fn, span_normal.base, (uintptr_t)e_normal.base);
		/* Prelinked DSOs must not be carved from an overlapping range. */
		CHECK_TRUE(fn, span_huge.end <= span_normal.start ||
			span_normal.end <= span_huge.start);
		/*
		 * do_prelink() gives up silently when every page differs from the
		 * snapshot, so print what was recorded for the failing DSO.
		 */
		int huge_shared = relro_shared_for(span_huge.start, span_huge.end);
		int normal_shared = relro_shared_for(span_normal.start, span_normal.end);
		if (!huge_shared) {
			t_error("[%s] hugepage DSO relro not shared (entry offset=%zu, "
				"span 0x%llx-0x%llx)\n", fn, e_huge.offset,
				(unsigned long long)span_huge.start,
				(unsigned long long)span_huge.end);
		}
		if (!normal_shared) {
			t_error("[%s] normal DSO relro not shared (entry offset=%zu, "
				"span 0x%llx-0x%llx)\n", fn, e_normal.offset,
				(unsigned long long)span_normal.start,
				(unsigned long long)span_normal.end);
		}
		/* At least one DSO must be shared, otherwise prelink did nothing here. */
		CHECK_TRUE(fn, huge_shared || normal_shared);
	}

	close(memfd);
	return t_status ? 1 : 0;
}
