/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * so_dedup functionalext tests for the OH_LinkDedupedSo(library_path,
 * original_sha256) interface exported from libc.so.
 *
 * The test compiles src/linux/so_dedup.c together with this file (see
 * BUILD.gn) so the DEDUP_ROOT sandbox overrides apply to
 * the implementation itself; every fixture path below derives from
 * SO_DEDUP_TEST_ROOT, the single source of truth shared by both.
 *
 * Scenarios also assert that the implementation logged the matching message:
 * the implementation logs through the musl hilog adapter (MUSL_LOGE/MUSL_LOGI),
 * so main() switches the musl.log.* parameters on before running the
 * scenarios, and check_hilog_keyword() dumps the hilog history and reuses
 * the shared check_log() from libc-test's common library (libtest.a) to
 * look for the message that the scenario's code path must have printed.
 */

#include <errno.h>
#include <hilog_adapter.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <securec.h>

#include "functionalext.h"
#include "so_dedup.h"
#include "test.h"

#ifndef SO_DEDUP_TEST_ROOT
#define SO_DEDUP_TEST_ROOT "/data/local/tmp/libc-test/so_dedup"
#endif

#define TEST_ROOT SO_DEDUP_TEST_ROOT
#define LIST_PATH TEST_ROOT "/deduplist.json"
#define SRC24 TEST_ROOT "/deduplist/arm64-v8a/24.0.0/libc++_shared.so"
#define SRC26 TEST_ROOT "/deduplist/arm64-v8a/26.0.0/libc++_shared.so"
#define SRC27 TEST_ROOT "/deduplist/arm64-v8a/27.0.0/libc++_shared.so"
#define SRC26_1 TEST_ROOT "/deduplist/arm64-v8a/26.1.0/libc++_shared.so"
#define SRC27_1 TEST_ROOT "/deduplist/arm64-v8a/27.1.0/libc++_shared.so"
#define SRC30 TEST_ROOT "/deduplist/arm64-v8a/30.0.0/libc++_shared.so"
#define SRC31 TEST_ROOT "/deduplist/arm64-v8a/31.0.0/libc++_shared.so"
#define SRC_HILOG TEST_ROOT "/deduplist/arm64-v8a/26.0.0/libhilog.so"
#define APP_LIBS TEST_ROOT "/targets/com.test.app/libs/arm64"
#define TARGET_SO APP_LIBS "/libc++_shared.so"
#define TARGET_HILOG APP_LIBS "/libhilog.so"
#define STR_BUF_MAX 4096

/* one preset sha256 per api version */
#define HASH_24 "b57bda08e7b85d1921adbb571bcf283e0ca114075b06d9719cb4d81fa7a3d4df"
#define HASH_26 "e0907ad848d5573f2f9344fed8cdfb63ac470253238af5a8c45cb6b82d123408"
#define HASH_27 "b53507ef563f8c7c174e90f0f1f3a13b7fba3375d263ea5191bf65ef499bfd41"
#define HASH_26_1 "532cefdfcbb4c2d1f26a4772cde64a818d7d7959397090c65845a14c198685a5"
#define HASH_27_1 "3eb1a81b904a68661ff7ade5aa561185f191a617ca4678e89de6e9d7ae58c037"
#define HASH_30 "d6a5c4d2884fb1b00aa764343c2cf80f3182b1b66514c6c11f0c732dac25b8f0"
#define HASH_31 "de5e9ed9ef79a189212ea8b5c21066c3a686a933690e9ca22232342322b0143a"
#define HASH_HILOG "80ce07ef24051272eac3cdf73ebf651e47f285356bb8c2c9175a41037a35a10e"
#define HASH_UNMATCHED "34269830a73282e3796d55770c7c1b2e9082328fd232913c814635aedc64c57f"

/* ---------- filesystem helpers ---------- */

static void mkdir_p(const char *path)
{
	char buffer[PATH_MAX];
	if (snprintf_s(buffer, sizeof buffer, sizeof buffer - 1, "%s", path) < 0) {
		t_error("%s: path too long: %s\n", __func__, path);
		return;
	}
	for (char *c = buffer + 1; *c != '\0'; c++) {
		if (*c == '/') {
			*c = '\0';
			if (mkdir(buffer, 0755) != 0 && errno != EEXIST) {
				t_error("%s: mkdir %s errno=%d\n", __func__, buffer, errno);
					}
			*c = '/';
		}
	}
	if (mkdir(buffer, 0755) != 0 && errno != EEXIST) {
		t_error("%s: mkdir %s errno=%d\n", __func__, buffer, errno);
	}
}

static void write_bytes(const char *path, const char *content, size_t length)
{
	FILE *f = fopen(path, "wb");
	if (f == NULL) {
		t_error("%s: fopen %s errno=%d\n", __func__, path, errno);
		return;
	}
	if (fwrite(content, 1, length, f) != length) {
		t_error("%s: fwrite %s incomplete\n", __func__, path);
	}
	if (fclose(f) != 0) {
		t_error("%s: fclose %s errno=%d\n", __func__, path, errno);
	}
}

static void write_file(const char *path, const char *content)
{
	write_bytes(path, content, strlen(content));
}

/* fresh inode each time: every call re-reads the file */
/* check the most recent log line for a keyword, same idea as libc's
 * unit_test_hilog_adapter.c CheckHiLogPrint; requires hilogd (real device) */
/* assert that the implementation logged the given keyword: dump the hilog
 * history into the sandbox, then reuse check_log() from libc-test's common
 * library (libtest.a, declared in test.h, reports through t_error the same
 * way gwp_asan tests use it); requires hilogd (real device).
 * The size guard keeps check_log() off its empty-file path, where it closes
 * the FILE twice. */
static void check_hilog_keyword(const char *keyword)
{
	struct stat st;
	(void)system("/bin/hilog -x 2>/dev/null > " TEST_ROOT "/hilog_dump.txt");
	if (stat(TEST_ROOT "/hilog_dump.txt", &st) != 0 || st.st_size == 0) {
		t_error("%s: hilog dump missing or empty (hilogd unavailable?)\n",
			__func__);
		return;
	}
	check_log(TEST_ROOT "/hilog_dump.txt", keyword, false);
}

static void write_json(const char *content)
{
	char buffer[STR_BUF_MAX];
	if (snprintf_s(buffer, sizeof buffer, sizeof buffer - 1, "%s", content) < 0) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	unlink(LIST_PATH);
	write_bytes(LIST_PATH, buffer, strlen(buffer));
}

static void write_baseline(void)
{
	char buffer[STR_BUF_MAX];
	if (snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
		"{\"dedupMinApiVersion\":26,\"deviceType\":[\"phone\",\"tablet\",\"2in1\"],"
		"\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
		"\"24.0.0\":\"" HASH_24 "\","
		"\"26.0.0\":\"" HASH_26 "\","
		"\"27.0.0\":\"" HASH_27 "\"}]}") < 0) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	write_json(buffer);
}

static int readlink_is(const char *link, const char *expected)
{
	char buffer[PATH_MAX];
	ssize_t n = readlink(link, buffer, sizeof buffer - 1);
	if (n <= 0) {
		return 0;
	}
	buffer[n] = '\0';
	return strcmp(buffer, expected) == 0;
}

static void remove_tree(const char *path)
{
	char command[PATH_MAX + 16];
	if (snprintf_s(command, sizeof command, sizeof command - 1,
		"rm -rf '%s'", path) < 0) {
		return;
	}
	(void)system(command);
}

static void setup_sandbox(void)
{
	remove_tree(TEST_ROOT);
	mkdir_p(TEST_ROOT "/deduplist/arm64-v8a/24.0.0");
	mkdir_p(TEST_ROOT "/deduplist/arm64-v8a/26.0.0");
	mkdir_p(TEST_ROOT "/deduplist/arm64-v8a/27.0.0");
	mkdir_p(APP_LIBS);
	write_file(SRC24, "preset-so-placeholder-24");
	write_file(SRC26, "preset-so-placeholder-26");
	write_file(SRC27, "preset-so-placeholder-27");
	write_baseline();
}

/* fill a buffer with a repeated character, avoiding memset */
static void fill_char(char *buffer, char c, size_t count)
{
	for (size_t i = 0; i < count; i++) {
		buffer[i] = c;
	}
}

/* nested empty-array string: depth levels of [ ] around a number */
static void build_deep_array(char *buffer, size_t size, int depth)
{
	size_t offset = 0;
	for (int i = 0; i < depth && offset + 2 < size; i++) {
		buffer[offset++] = '[';
	}
	buffer[offset++] = '0';
	for (int i = 0; i < depth && offset + 2 < size; i++) {
		buffer[offset++] = ']';
	}
	buffer[offset] = '\0';
}

/* nested object string: depth levels of {"a": around a number */
static void build_deep_object(char *buffer, size_t size, int depth)
{
	size_t offset = 0;
	for (int i = 0; i < depth && offset + 8 < size; i++) {
		buffer[offset++] = '{';
		buffer[offset++] = '"';
		buffer[offset++] = 'a';
		buffer[offset++] = '"';
		buffer[offset++] = ':';
	}
	buffer[offset++] = '0';
	for (int i = 0; i < depth && offset + 2 < size; i++) {
		buffer[offset++] = '}';
	}
	buffer[offset] = '\0';
}

/* ---------- test cases ---------- */

/**
 * @tc.name     : so_dedup_layer1_hash_match_0010
 * @tc.desc     : hash equals a configured version sha256, symlink created at the derived path
 * @tc.level    : Level 0
 */
static void so_dedup_layer1_hash_match_0010(void)
{
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC26));
	check_hilog_keyword("symlink created");
	unlink(TARGET_SO);
	/* a different hash maps to its own version */
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_24), SO_DEDUP_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC24));
	unlink(TARGET_SO);
}

/**
 * @tc.name     : so_dedup_layer2_latest_fallback_0020
 * @tc.desc     : unmatched hash falls back to the newest configured version
 * @tc.level    : Level 0
 */
static void so_dedup_layer2_latest_fallback_0020(void)
{
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_UNMATCHED), SO_DEDUP_VERSION_MISMATCH);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC27));
	check_hilog_keyword("layer2 fallback");
	unlink(TARGET_SO);
}

/**
 * @tc.name     : so_dedup_no_match_unknown_library_0030
 * @tc.desc     : library name absent from deduplist.json returns NO_MATCH
 * @tc.level    : Level 0
 */
static void so_dedup_no_match_unknown_library_0030(void)
{
	const char *target = APP_LIBS "/libunknown.so";
	struct stat st;
	EXPECT_EQ(__func__, OH_LinkDedupedSo(target, HASH_26), SO_DEDUP_NO_MATCH);
	check_hilog_keyword("not found in deduplist.json");
	EXPECT_EQ(__func__, lstat(target, &st), -1);
	EXPECT_EQ(__func__, errno, ENOENT);
}

/**
 * @tc.name     : so_dedup_no_match_all_sources_missing_0040
 * @tc.desc     : configured version whose preset file is absent returns NO_MATCH
 * @tc.level    : Level 1
 */
static void so_dedup_no_match_all_sources_missing_0040(void)
{
	char buffer[STR_BUF_MAX];
	if (snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
		"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
		"\"30.0.0\":\"" HASH_30 "\"}]}") < 0) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	write_json(buffer);
	/* layer 1 matches 30.0.0 but the file does not exist; layer 2 same version */
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_30), SO_DEDUP_NO_MATCH);
	/* nothing matches at all either */
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_UNMATCHED), SO_DEDUP_NO_MATCH);
	check_hilog_keyword("no available preset so");
	write_baseline();
}

/**
 * @tc.name     : so_dedup_layer1_missing_source_falls_to_latest_0050
 * @tc.desc     : hash-matched version has no preset file, latest version takes over
 * @tc.level    : Level 1
 */
static void so_dedup_layer1_missing_source_falls_to_latest_0050(void)
{
	char buffer[STR_BUF_MAX];
	mkdir_p(TEST_ROOT "/deduplist/arm64-v8a/30.0.0");
	mkdir_p(TEST_ROOT "/deduplist/arm64-v8a/31.0.0");
	write_file(SRC31, "preset-so-placeholder-31");
	if (snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
		"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
		"\"30.0.0\":\"" HASH_30 "\","
		"\"31.0.0\":\"" HASH_31 "\"}]}") < 0) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	write_json(buffer);
	/* 30.0.0 matches but SRC30 is missing: fallback links to 31.0.0 */
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_30), SO_DEDUP_VERSION_MISMATCH);
	check_hilog_keyword("preset file missing");
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC31));
	unlink(TARGET_SO);
	write_baseline();
}

/**
 * @tc.name     : so_dedup_latest_version_compare_0060
 * @tc.desc     : newest version selected by major then minor comparison
 * @tc.level    : Level 1
 */
static void so_dedup_latest_version_compare_0060(void)
{
	char buffer[STR_BUF_MAX];
	mkdir_p(TEST_ROOT "/deduplist/arm64-v8a/26.1.0");
	mkdir_p(TEST_ROOT "/deduplist/arm64-v8a/27.0.0");
	mkdir_p(TEST_ROOT "/deduplist/arm64-v8a/27.1.0");
	write_file(SRC26_1, "preset-so-placeholder-26_1");
	write_file(SRC27, "preset-so-placeholder-27");
	write_file(SRC27_1, "preset-so-placeholder-27_1");

	/* major wins: 27.0.0 over 26.1.0 */
	if (snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
		"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
		"\"26.1.0\":\"" HASH_26_1 "\","
		"\"27.0.0\":\"" HASH_27 "\"}]}") < 0) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	write_json(buffer);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_UNMATCHED), SO_DEDUP_VERSION_MISMATCH);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC27));
	unlink(TARGET_SO);

	/* minor wins: 27.1.0 over 27.0.0 */
	if (snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
		"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
		"\"27.0.0\":\"" HASH_27 "\","
		"\"27.1.0\":\"" HASH_27_1 "\"}]}") < 0) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	write_json(buffer);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_UNMATCHED), SO_DEDUP_VERSION_MISMATCH);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC27_1));
	unlink(TARGET_SO);
	write_baseline();
}

/**
 * @tc.name     : so_dedup_already_linked_0080
 * @tc.desc     : relinking an existing own symlink returns ALREADY_LINKED and keeps it
 * @tc.level    : Level 0
 */
static void so_dedup_already_linked_0080(void)
{
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_LINKED);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ALREADY_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC26));
	unlink(TARGET_SO);
}

/**
 * @tc.name     : so_dedup_existing_regular_target_0090
 * @tc.desc     : target already a regular file returns ERROR/EEXIST and leaves it untouched
 * @tc.level    : Level 0
 */
static void so_dedup_existing_regular_target_0090(void)
{
	const char *content = "already-extracted-so";
	write_file(TARGET_SO, content);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);
	EXPECT_EQ(__func__, errno, EEXIST);
	char buffer[STR_BUF_MAX];
	FILE *f = fopen(TARGET_SO, "r");
	EXPECT_PTRNE(__func__, f, NULL);
	if (f != NULL) {
		size_t n = fread(buffer, 1, strlen(content), f);
		fclose(f);
		EXPECT_EQ(__func__, n, strlen(content));
		EXPECT_EQ(__func__, strncmp(buffer, content, strlen(content)), 0);
	}
	unlink(TARGET_SO);
}

/**
 * @tc.name     : so_dedup_existing_foreign_symlink_0100
 * @tc.desc     : target is a symlink to another file, returns ERROR/EEXIST
 * @tc.level    : Level 1
 */
static void so_dedup_existing_foreign_symlink_0100(void)
{
	const char *other = APP_LIBS "/libother.so";
	write_file(other, "some-other-so");
	EXPECT_EQ(__func__, symlink(other, TARGET_SO), 0);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);
	EXPECT_EQ(__func__, errno, EEXIST);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, other));
	unlink(TARGET_SO);
	unlink(other);
}

/**
 * @tc.name     : so_dedup_config_unavailable_0110
 * @tc.desc     : missing deduplist.json returns CONFIG_UNAVAILABLE and recovers when restored
 * @tc.level    : Level 0
 */
static void so_dedup_config_unavailable_0110(void)
{
	unlink(LIST_PATH);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_CONFIG_UNAVAILABLE);
	check_hilog_keyword("deduplist.json not found");
	write_baseline();
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_LINKED);
	unlink(TARGET_SO);
	unlink(LIST_PATH);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_CONFIG_UNAVAILABLE);
	write_baseline();
}

/**
 * @tc.name     : so_dedup_config_is_directory_0120
 * @tc.desc     : deduplist.json replaced by a directory returns ERROR/EBADMSG
 * @tc.level    : Level 1
 */
static void so_dedup_config_is_directory_0120(void)
{
	unlink(LIST_PATH);
	EXPECT_EQ(__func__, mkdir(LIST_PATH, 0755), 0);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);
	EXPECT_EQ(__func__, errno, EBADMSG);
	check_hilog_keyword("not a regular file");
	EXPECT_EQ(__func__, rmdir(LIST_PATH), 0);
	write_baseline();
}

/**
 * @tc.name     : so_dedup_config_empty_file_0140
 * @tc.desc     : zero-length deduplist.json returns ERROR/EBADMSG
 * @tc.level    : Level 1
 */
static void so_dedup_config_empty_file_0140(void)
{
	unlink(LIST_PATH);
	write_bytes(LIST_PATH, "", 0);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);
	EXPECT_EQ(__func__, errno, EBADMSG);
	check_hilog_keyword("deduplist.json is empty");
	write_baseline();
}

/**
 * @tc.name     : so_dedup_errno_preserved_0180
 * @tc.desc     : non-ERROR returns keep the caller's errno value untouched
 * @tc.level    : Level 1
 */
static void so_dedup_errno_preserved_0180(void)
{
	errno = 4242;
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_LINKED);
	EXPECT_EQ(__func__, errno, 4242);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ALREADY_LINKED);
	EXPECT_EQ(__func__, errno, 4242);
	unlink(TARGET_SO);

	errno = 4243;
	EXPECT_EQ(__func__, OH_LinkDedupedSo(APP_LIBS "/libunknown.so", HASH_26), SO_DEDUP_NO_MATCH);
	EXPECT_EQ(__func__, errno, 4243);
	unlink(LIST_PATH);
	errno = 4244;
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_CONFIG_UNAVAILABLE);
	EXPECT_EQ(__func__, errno, 4244);
	write_baseline();
}

/* table of malformed deduplist.json test cases */
static const struct {
	const char *desc;
	const char *json;
} malformed_cases[] = {
		{ "root is an array", "[]" },
		{ "empty root object", "{}" },
		{ "whitespace only", "   " },
		{ "root is a number", "123" },
		{ "trailing garbage", "{}x" },
		{ "libraries is an object", "{\"libraries\":{}}" },
		{ "empty libraries array", "{\"libraries\":[]}" },
		{ "library element not an object", "{\"libraries\":[\"x\"]}" },
		{ "library without versions",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\"}]}" },
		{ "library without name",
			"{\"libraries\":[{\"abi\":\"arm64-v8a\",\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "library without abi",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "duplicate name field",
			"{\"libraries\":[{\"name\":\"a\",\"name\":\"libc++_shared.so\","
			"\"abi\":\"arm64-v8a\",\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "duplicate abi field",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"a\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "duplicate library entries",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":\"" HASH_26 "\"},"
			"{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "duplicate libraries key",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}],\"libraries\":[]}" },
		{ "libraries array truncated",
			"{\"libraries\":[" },
		{ "root key not a string",
			"{123:1}" },
		{ "empty library object",
			"{\"libraries\":[{}]}" },
		{ "library objects missing comma",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":\"" HASH_26 "\"} {\"name\":\"libc++_shared.so\","
			"\"abi\":\"arm64-v8a\",\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "unclosed string at end of file",
			"{\"x\":\"abc" },
		{ "version key not a version, digit-start",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.x\":\"" HASH_26 "\"}]}" },
		{ "version key missing patch",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0\":\"" HASH_26 "\"}]}" },
		{ "version key extra component",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0.0.0\":\"" HASH_26 "\"}]}" },
		{ "version key beyond INT_MAX",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"2147483648.0.0\":\"" HASH_26 "\"}]}" },
		{ "version key too long",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"123456789.123456789.1\":\"" HASH_26 "\"}]}" },
		{ "version key trailing dot",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0.\":\"" HASH_26 "\"}]}" },
		{ "version value not a string",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":123}]}" },

		{ "invalid escape in abi",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64\\qv8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "bad unicode escape",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64\\uZZZZv8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "lone surrogate in abi",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64\\ud800v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "non-ascii unicode escape in abi",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64\\u00e9v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "embedded NUL escape in name",
			"{\"libraries\":[{\"name\":\"libc\\u0000_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "name value overflows buffer",
			"{\"libraries\":[{\"name\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "empty name value",
			"{\"libraries\":[{\"name\":\"\",\"abi\":\"arm64-v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
		{ "version key single component",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
			"\"26\":\"" HASH_26 "\"}]}" },
		{ "library key not a string",
			"{\"libraries\":[{123:1}]}" },
		{ "truncated true keyword",
			"{\"x\":tru,\"libraries\":[]}" },
		{ "number dash then bracket",
			"{\"x\":-}" },
		{ "number dash at end of file",
			"{\"x\":-" },
		{ "fraction digits missing",
			"{\"x\":1.}" },
		{ "exponent digits missing",
			"{\"x\":1e}" },
		{ "array truncated at end of file",
			"{\"x\":[" },
		{ "object value truncated",
			"{\"x\":{\"k\":" },
		{ "lowercase non-hex unicode escape",
			"{\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64\\u00g0v8a\","
			"\"26.0.0\":\"" HASH_26 "\"}]}" },
	};

static void run_malformed_json_table(void)
{
	for (size_t i = 0; i < sizeof malformed_cases / sizeof malformed_cases[0]; i++) {
		write_json(malformed_cases[i].json);
		EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);
	}
}

static void run_malformed_json_special(void)
{
	write_json("{\"libraries\":[{\"name\":\"libc++\n_shared.so\",\"abi\":\"arm64-v8a\","
		"\"26.0.0\":\"" HASH_26 "\"}]}");
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);

	write_bytes(LIST_PATH, "{\"libraries\":[{\"name\":\"libc\\u12",
		sizeof "{\"libraries\":[{\"name\":\"libc\\u12" - 1);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);

	{
		char deep[128];
		char buffer[STR_BUF_MAX];
		build_deep_array(deep, sizeof deep, 33);
		int n = snprintf_s(buffer, sizeof buffer, sizeof buffer - 1, "{\"x\":%s,\"libraries\":[]}", deep);
		if (n < 0 || (size_t)n >= sizeof buffer) {
			t_error("%s: buffer overflow\n", __func__);
			} else {
			write_json(buffer);
			EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);
		}
	}

	{
		char deep_obj[STR_BUF_MAX];
		char buffer[STR_BUF_MAX];
		build_deep_object(deep_obj, sizeof deep_obj, 33);
		int n = snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
			"{\"x\":%s,\"libraries\":[]}", deep_obj);
		if (n < 0 || (size_t)n >= sizeof buffer) {
			t_error("%s: buffer overflow\n", __func__);
			} else {
			write_json(buffer);
			EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);
		}
	}

	write_json("{\"x\":[1 2],\"libraries\":[]}");
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);

	write_json("{\"x\":{\"k\":1 2},\"libraries\":[]}");
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);

	write_baseline();
}

/**
 * @tc.name     : so_dedup_malformed_json_0200
 * @tc.desc     : malformed deduplist.json variants return ERROR
 * @tc.level    : Level 1
 */
static void so_dedup_malformed_json_0200(void)
{
	run_malformed_json_table();
	run_malformed_json_special();
}

/**
 * @tc.name     : so_dedup_unknown_properties_skipped_0210
 * @tc.desc     : unknown root/library properties of every JSON type are skipped
 * @tc.level    : Level 1
 */
static void so_dedup_unknown_properties_skipped_0210(void)
{
	char buffer[STR_BUF_MAX];
	char deep[STR_BUF_MAX];
	char longkey[STR_BUF_MAX];
	build_deep_array(deep, sizeof deep, 32);
	fill_char(longkey, 'k', 100);
	longkey[100] = '\0';
	int n = snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
		"{\"dedupMinApiVersion\":26,\"deviceType\":[\"phone\",\"tablet\",\"2in1\"],"
		"\"zero\":0,\"count\":5,\"flag_true\":true,\"flag_false\":false,"
		"\"nothing\":null,\"empty_obj\":{},\"empty_arr\":[],\"deep\":%s,"
		"\"%s\":\"long key gets truncated but skipped\","
		"\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
		"\"extra_key\":\"ignored\",\"24.0.0\":\"" HASH_24 "\","
		"\"26.0.0\":\"" HASH_26 "\","
		"\"27.0.0\":\"" HASH_27 "\"}]}",
		deep, longkey);
	if (n < 0 || (size_t)n >= sizeof buffer) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	write_json(buffer);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC26));
	unlink(TARGET_SO);
	write_baseline();
}

/**
 * @tc.name     : so_dedup_json_depth_exceeded_0220
 * @tc.desc     : unknown value nested deeper than DEDUP_JSON_MAX_DEPTH is rejected
 * @tc.level    : Level 2
 */
static void so_dedup_json_depth_exceeded_0220(void)
{
	char buffer[STR_BUF_MAX];
	char deep[128];
	build_deep_array(deep, sizeof deep, 33);
	int n = snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
		"{\"deep\":%s,\"libraries\":[{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\","
		"\"26.0.0\":\"" HASH_26 "\"}]}", deep);
	if (n < 0 || (size_t)n >= sizeof buffer) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	write_json(buffer);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_ERROR);
	check_hilog_keyword("nesting depth exceeds");
	write_baseline();
}

/**
 * @tc.name     : so_dedup_source_symlink_falls_to_latest_0230
 * @tc.desc     : preset path occupied by a symlink is not a valid source, latest wins
 * @tc.level    : Level 2
 */
static void so_dedup_source_symlink_falls_to_latest_0230(void)
{
	unlink(SRC26);
	EXPECT_EQ(__func__, symlink(SRC24, SRC26), 0);
	/* 26.0.0 matches the hash but its path is not a regular file: 27.0.0 takes over */
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_VERSION_MISMATCH);
	check_hilog_keyword("layer2 fallback");
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC27));
	unlink(TARGET_SO);
	unlink(SRC26);
	write_file(SRC26, "preset-so-placeholder-26");
}

/**
 * @tc.name     : so_dedup_multiple_libraries_0250
 * @tc.desc     : each configured library links its own target independently
 * @tc.level    : Level 1
 */
static void so_dedup_multiple_libraries_0250(void)
{
	char buffer[STR_BUF_MAX];
	write_file(SRC_HILOG, "preset-so-placeholder-hilog");
	if (snprintf_s(buffer, sizeof buffer, sizeof buffer - 1,
		"{\"libraries\":["
		"{\"name\":\"libc++_shared.so\",\"abi\":\"arm64-v8a\",\"26.0.0\":\"" HASH_26 "\"},"
		"{\"name\":\"libhilog.so\",\"abi\":\"arm64-v8a\",\"26.0.0\":\"" HASH_HILOG "\"}"
		"]}") < 0) {
		t_error("%s: buffer overflow\n", __func__);
		return;
	}
	write_json(buffer);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC26));
	unlink(TARGET_SO);
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_HILOG, HASH_HILOG), SO_DEDUP_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_HILOG, SRC_HILOG));
	unlink(TARGET_HILOG);
	write_baseline();
}


/**
 * @tc.name     : so_dedup_app_upgrade_0280
 * @tc.desc     : upgrade with changed/unchanged sha256 links to new/same path
 * @tc.level    : Level 1
 */
static void so_dedup_app_upgrade_0280(void)
{
	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_26), SO_DEDUP_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC26));
	unlink(TARGET_SO);

	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_27), SO_DEDUP_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC27));
	unlink(TARGET_SO);

	EXPECT_EQ(__func__, OH_LinkDedupedSo(TARGET_SO, HASH_27), SO_DEDUP_LINKED);
	EXPECT_TRUE(__func__, readlink_is(TARGET_SO, SRC27));
	unlink(TARGET_SO);
}


/**
 * @tc.name     : so_dedup_caller_not_permitted_0290
 * @tc.desc     : app-range caller uid is rejected with ERROR/EACCES and no side effects
 * @tc.level    : Level 1
 */
static void so_dedup_caller_not_permitted_0290(void)
{
	if (getuid() != 0) {
		t_printf("%s: skipped, needs root to drop to an app uid\n", __func__);
		return;
	}
	write_baseline();
	pid_t child = fork();
	if (child < 0) {
		t_error("%s: fork errno=%d\n", __func__, errno);
		return;
	}
	if (child == 0) {
		if (setgid(10000) != 0 || setuid(10000) != 0) {
			_exit(98);
		}
		int failed = 0;
		if (OH_LinkDedupedSo(TARGET_SO, HASH_26) != SO_DEDUP_ERROR) {
			failed |= 1;
		}
		if (errno != EACCES) {
			failed |= 2;
		}
		struct stat st;
		if (lstat(TARGET_SO, &st) == 0) {
			failed |= 4;
		}
		_exit(failed);
	}
	int status = 0;
	if (waitpid(child, &status, 0) != child) {
		t_error("%s: waitpid errno=%d\n", __func__, errno);
		return;
	}
	EXPECT_EQ(__func__, WIFEXITED(status) ? WEXITSTATUS(status) : -1, 0);
	check_hilog_keyword("is not permitted");
}


/* switch the musl log path on for this process: lift hilog flow control,
 * enable musl logs and raise the level to INFO so both the ERROR and INFO
 * messages of the scenarios pass the filter, then reload the parameters
 * into this binary's copy of the hilog adapter state (same recipe as the
 * hilog adapter unittest)
 */
static void enable_musl_logs(void)
{
	(void)system("/bin/hilog -Q pidoff");
	(void)system("/bin/hilog -Q domainoff");
	(void)system("param set musl.log.enable true");
	(void)system("param set musl.log.level INFO");
	musl_log_reset();
}

static void restore_musl_logs(void)
{
	(void)system("param set musl.log.enable false");
	(void)system("param set musl.log.level WARN");
	(void)system("/bin/hilog -Q pidon");
	(void)system("/bin/hilog -Q domainon");
}

int main(void)
{
	t_printf("so_dedup_test start\n");
	enable_musl_logs();
	setup_sandbox();

	so_dedup_layer1_hash_match_0010();
	so_dedup_layer2_latest_fallback_0020();
	so_dedup_no_match_unknown_library_0030();
	so_dedup_no_match_all_sources_missing_0040();
	so_dedup_layer1_missing_source_falls_to_latest_0050();
	so_dedup_latest_version_compare_0060();
	so_dedup_already_linked_0080();
	so_dedup_existing_regular_target_0090();
	so_dedup_existing_foreign_symlink_0100();
	so_dedup_config_unavailable_0110();
	so_dedup_config_is_directory_0120();
	so_dedup_config_empty_file_0140();
	so_dedup_errno_preserved_0180();
	so_dedup_malformed_json_0200();
	so_dedup_unknown_properties_skipped_0210();
	so_dedup_json_depth_exceeded_0220();
	so_dedup_source_symlink_falls_to_latest_0230();
	so_dedup_multiple_libraries_0250();
	so_dedup_app_upgrade_0280();
	so_dedup_caller_not_permitted_0290();

	remove_tree(TEST_ROOT);
	restore_musl_logs();
	return 0;
}
