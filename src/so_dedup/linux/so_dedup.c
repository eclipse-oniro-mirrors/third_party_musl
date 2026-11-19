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

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <unistd.h>

#include "../../../include/so_dedup/so_dedup.h"
#include "musl_log.h"

#ifndef DEDUP_ROOT
#define DEDUP_ROOT "/system/lib64/dedupSource"
#endif
#define DEDUP_LIST_PATH DEDUP_ROOT "/deduplist.json"
#ifndef DEDUP_SOURCE_ROOT
#define DEDUP_SOURCE_ROOT DEDUP_ROOT "/deduplist"
#endif
#define DEDUP_JSON_MAX_DEPTH 32
#define DEDUP_VERSION_KEY_LEN 16
#define DEDUP_SHA256_LEN 65
#define SO_DEDUP_MIN_APP_UID 10000


struct json_reader {
	const char *cur;
	const char *end;
	unsigned int depth;
};

struct dedup_so_version {
	char key[DEDUP_VERSION_KEY_LEN];
	int major;
	int minor;
	int patch;
	char sha256[DEDUP_SHA256_LEN];
	struct dedup_so_version *next;
};

struct dedup_library {
	char *name;
	char *abi;
	struct dedup_so_version *versions;
	struct dedup_library *next;
};

struct dedup_config {
	struct dedup_library *libraries;
};

static void json_skip_space(struct json_reader *reader)
{
	while (reader->cur < reader->end && isspace((unsigned char)*reader->cur) != 0) {
		reader->cur++;
	}
}

static int json_consume(struct json_reader *reader, char expected)
{
	json_skip_space(reader);
	if (reader->cur == reader->end || *reader->cur != expected) {
		MUSL_LOGE("json: expected '%{public}c' not found", expected);
		errno = EBADMSG;
		return -1;
	}
	reader->cur++;
	return 0;
}

static int json_decode_escape(struct json_reader *reader, unsigned char *value)
{
	if (reader->cur == reader->end) {
		MUSL_LOGE("json: escape cut at end of input");
		errno = EBADMSG;
		return -1;
	}
	*value = (unsigned char)*reader->cur++;
	switch (*value) {
	case '"': case '\\': case '/':
		return 0;
	case 'b': *value = '\b'; return 0;
	case 'f': *value = '\f'; return 0;
	case 'n': *value = '\n'; return 0;
	case 'r': *value = '\r'; return 0;
	case 't': *value = '\t'; return 0;
	default:
		MUSL_LOGE("json: invalid escape '\\%{public}c'", *value);
		errno = EBADMSG;
		return -1;
	}
}

static void json_store_character(char *output, size_t output_size, size_t length,
	unsigned char value, bool *overflow)
{
	if (output == NULL) {
		return;
	}
	if (output_size == 0 || length + 1 >= output_size) {
		*overflow = true;
		return;
	}
	output[length] = (char)value;
}

static int json_read_string(struct json_reader *reader,
	char *output, size_t output_size, bool *truncated)
{
	size_t length = 0;
	bool overflow = false;

	json_skip_space(reader);
	if (reader->cur == reader->end || *reader->cur++ != '"') {
		MUSL_LOGE("json: string must start with quote");
		errno = EBADMSG;
		return -1;
	}
	while (reader->cur < reader->end) {
		unsigned char value = (unsigned char)*reader->cur++;
		if (value == '"') {
			if (output && output_size) {
				output[length < output_size ? length : output_size - 1] = '\0';
			}
			if (truncated) {
				*truncated = overflow;
			}
			return 0;
		}
		if (value < 0x20) {
			MUSL_LOGE("json: control character in string");
			errno = EBADMSG;
			return -1;
		}
		if (value == '\\') {
			if (json_decode_escape(reader, &value) < 0) {
				return -1;
			}
		}
		json_store_character(output, output_size, length, value, &overflow);
		length++;
	}
	MUSL_LOGE("json: string not closed");
	errno = EBADMSG;
	return -1;
}

static int json_skip_value(struct json_reader *reader);

static int json_skip_number(struct json_reader *reader)
{
	if (reader->cur < reader->end && *reader->cur == '-') {
		reader->cur++;
	}
	if (reader->cur == reader->end || isdigit((unsigned char)*reader->cur) == 0) {
		MUSL_LOGE("json: invalid number");
		errno = EBADMSG;
		return -1;
	}
	while (reader->cur < reader->end && isdigit((unsigned char)*reader->cur) != 0) {
		reader->cur++;
	}
	return 0;
}

static int json_skip_array(struct json_reader *reader)
{
	if (++reader->depth > DEDUP_JSON_MAX_DEPTH) {
		MUSL_LOGE("json: nesting depth exceeds %{public}d", DEDUP_JSON_MAX_DEPTH);
		errno = EBADMSG;
		return -1;
	}
	if (json_consume(reader, '[') < 0) {
		return -1;
	}
	json_skip_space(reader);
	if (reader->cur < reader->end && *reader->cur == ']') {
		reader->cur++;
		reader->depth--;
		return 0;
	}
	for (;;) {
		if (json_skip_value(reader) < 0) {
			return -1;
		}
		json_skip_space(reader);
		if (reader->cur == reader->end) {
			MUSL_LOGE("json: array not closed");
			errno = EBADMSG;
			return -1;
		}
		if (*reader->cur == ']') {
			reader->cur++;
			reader->depth--;
			return 0;
		}
		if (*reader->cur++ != ',') {
			MUSL_LOGE("json: expected ',' in array");
			errno = EBADMSG;
			return -1;
		}
	}
}

static int json_skip_object(struct json_reader *reader)
{
	if (++reader->depth > DEDUP_JSON_MAX_DEPTH) {
		MUSL_LOGE("json: nesting depth exceeds %{public}d", DEDUP_JSON_MAX_DEPTH);
		errno = EBADMSG;
		return -1;
	}
	if (json_consume(reader, '{') < 0) {
		return -1;
	}
	json_skip_space(reader);
	if (reader->cur < reader->end && *reader->cur == '}') {
		reader->cur++;
		reader->depth--;
		return 0;
	}
	for (;;) {
		if (json_read_string(reader, NULL, 0, NULL) < 0 || json_consume(reader, ':') < 0 ||
			json_skip_value(reader) < 0) {
			return -1;
		}
		json_skip_space(reader);
		if (reader->cur == reader->end) {
			MUSL_LOGE("json: object not closed");
			errno = EBADMSG;
			return -1;
		}
		if (*reader->cur == '}') {
			reader->cur++;
			reader->depth--;
			return 0;
		}
		if (*reader->cur++ != ',') {
			MUSL_LOGE("json: expected ',' in object");
			errno = EBADMSG;
			return -1;
		}
	}
}

static int json_skip_value(struct json_reader *reader)
{
	json_skip_space(reader);
	if (reader->cur == reader->end) {
		MUSL_LOGE("json: unexpected end of input");
		errno = EBADMSG;
		return -1;
	}
	switch (*reader->cur) {
	case '"':
		return json_read_string(reader, NULL, 0, NULL);
	case '{':
		return json_skip_object(reader);
	case '[':
		return json_skip_array(reader);
	case 't':
		if ((size_t)(reader->end - reader->cur) < 4 || memcmp(reader->cur, "true", 4) != 0) {
			MUSL_LOGE("json: invalid literal");
			errno = EBADMSG;
			return -1;
		}
		reader->cur += 4;
		return 0;
	case 'f':
		if ((size_t)(reader->end - reader->cur) < 5 || memcmp(reader->cur, "false", 5) != 0) {
			MUSL_LOGE("json: invalid literal");
			errno = EBADMSG;
			return -1;
		}
		reader->cur += 5;
		return 0;
	case 'n':
		if ((size_t)(reader->end - reader->cur) < 4 || memcmp(reader->cur, "null", 4) != 0) {
			MUSL_LOGE("json: invalid literal");
			errno = EBADMSG;
			return -1;
		}
		reader->cur += 4;
		return 0;
	default:
		return json_skip_number(reader);
	}
}

static const char *path_basename(const char *path)
{
	const char *base = strrchr(path, '/');
	return base ? base + 1 : path;
}

static void free_versions(struct dedup_so_version *version)
{
	while (version) {
		struct dedup_so_version *next = version->next;
		free(version);
		version = next;
	}
}

static void free_config(struct dedup_config *config)
{
	struct dedup_library *library = config->libraries;
	while (library) {
		struct dedup_library *next = library->next;
		free(library->name);
		free(library->abi);
		free_versions(library->versions);
		free(library);
		library = next;
	}
	*config = (struct dedup_config){0};
}

static bool parse_version_key(const char *key, int *major, int *minor, int *patch)
{
	int components[3] = { 0, 0, 0 };
	int count = 0;
	const char *cursor = key;
	if (*cursor == '\0') {
		return false;
	}
	for (;;) {
		if (count >= 3) {
			return false;
		}
		long value = 0;
		int digits = 0;
		while (*cursor >= '0' && *cursor <= '9') {
			value = value * 10 + (*cursor - '0');
			if (value > INT_MAX) {
				return false;
			}
			cursor++;
			digits++;
		}
		if (digits == 0) {
			return false;
		}
		components[count++] = (int)value;
		if (*cursor == '\0') {
			break;
		}
		if (*cursor != '.') {
			return false;
		}
		cursor++;
		if (*cursor == '\0') {
			return false;
		}
	}
	if (count != 3) {
		return false;
	}
	*major = components[0];
	*minor = components[1];
	*patch = components[2];
	return true;
}

static int parse_version_property(struct json_reader *reader,
	const char *key, bool key_truncated, struct dedup_so_version ***tail)
{
	int major;
	int minor;
	int patch;
	if (key_truncated || !parse_version_key(key, &major, &minor, &patch)) {
		if (isdigit((unsigned char)key[0]) != 0) {
			MUSL_LOGE("invalid version key '%{public}s' in deduplist.json", key);
			errno = EBADMSG;
			return -1;
		}
		return json_skip_value(reader);
	}
	if (strlen(key) >= DEDUP_VERSION_KEY_LEN) {
		MUSL_LOGE("version key '%{public}s' too long in deduplist.json", key);
		errno = EBADMSG;
		return -1;
	}
	struct dedup_so_version *version = calloc(1, sizeof *version);
	if (version == NULL) {
		return -1;
	}
	char hash[DEDUP_SHA256_LEN];
	bool truncated;
	if (json_read_string(reader, hash, sizeof hash, &truncated) < 0) {
		goto fail;
	}
	if (truncated) {
		MUSL_LOGE("sha256 too long in version '%{public}s'", key);
		goto invalid;
	}
	(void)snprintf(version->sha256, sizeof version->sha256, "%s", hash);
	(void)snprintf(version->key, sizeof version->key, "%s", key);
	version->major = major;
	version->minor = minor;
	version->patch = patch;
	**tail = version;
	*tail = &version->next;
	return 0;
invalid:
	errno = EBADMSG;
fail:
	free_versions(version);
	return -1;
}

static int parse_name_abi(struct json_reader *reader, const char *key, bool key_truncated,
	struct dedup_library *library, bool *has_name, bool *has_abi)
{
	if (!key_truncated && strcmp(key, "name") == 0) {
		if (*has_name) {
			return 0;
		}
		char name[NAME_MAX + 1];
		bool name_truncated;
		if (json_read_string(reader, name, sizeof name, &name_truncated) < 0 ||
			name_truncated || name[0] == '\0') {
			return 0;
		}
		library->name = strdup(name);
		if (library->name == NULL) {
			return -1;
		}
		*has_name = true;
		return 2;
	}
	if (!key_truncated && strcmp(key, "abi") == 0) {
		if (*has_abi) {
			return 0;
		}
		char abi[64];
		bool abi_truncated;
		if (json_read_string(reader, abi, sizeof abi, &abi_truncated) < 0 ||
			abi_truncated || abi[0] == '\0') {
			return 0;
		}
		library->abi = strdup(abi);
		if (library->abi == NULL) {
			return -1;
		}
		*has_abi = true;
		return 2;
	}
	return 1;
}

static int parse_library_properties(struct json_reader *reader,
	struct dedup_library *library, struct dedup_so_version ***tail, bool *has_name, bool *has_abi)
{
	for (;;) {
		char key[64];
		bool key_truncated;
		if (json_read_string(reader, key, sizeof key, &key_truncated) < 0 ||
			json_consume(reader, ':') < 0) {
			return -1;
		}
		int prop = parse_name_abi(reader, key, key_truncated, library, has_name, has_abi);
		if (prop < 0) {
			return -1;
		}
		if (prop == 0) {
			return 0;
		}
		if (prop != 2) {
			if (parse_version_property(reader, key, key_truncated, tail) < 0) {
				return -1;
			}
		}
		json_skip_space(reader);
		if (reader->cur == reader->end) {
			return 0;
		}
		if (*reader->cur == '}') {
			reader->cur++;
			return 1;
		}
		if (*reader->cur++ != ',') {
			return 0;
		}
	}
}

static int parse_library(struct json_reader *reader, struct dedup_library **result)
{
	struct dedup_library *library = calloc(1, sizeof *library);
	if (library == NULL) {
		return -1;
	}
	if (json_consume(reader, '{') < 0) {
		goto fail;
	}
	json_skip_space(reader);
	if (reader->cur < reader->end && *reader->cur == '}') {
		goto invalid;
	}
	struct dedup_so_version **tail = &library->versions;
	bool has_name = false;
	bool has_abi = false;
	int prop_result = parse_library_properties(reader, library, &tail, &has_name, &has_abi);
	if (prop_result < 0) {
		goto fail;
	}
	if (prop_result == 0) {
		goto invalid;
	}
	if (!has_name || !has_abi || library->versions == NULL) {
		MUSL_LOGE("library missing name/abi/version in deduplist.json");
		goto invalid;
	}
	*result = library;
	return 0;
invalid:
	errno = EBADMSG;
fail:
	free_config(&(struct dedup_config){.libraries = library});
	return -1;
}

static bool config_has_library(const struct dedup_config *config, const char *name)
{
	for (const struct dedup_library *library = config->libraries; library; library = library->next) {
		if (strcmp(library->name, name) == 0) {
			return true;
		}
	}
	return false;
}

static int parse_libraries(struct json_reader *reader, struct dedup_config *config)
{
	struct dedup_library **tail = &config->libraries;
	if (json_consume(reader, '[') < 0) {
		return -1;
	}
	json_skip_space(reader);
	if (reader->cur < reader->end && *reader->cur == ']') {
		MUSL_LOGE("deduplist.json: libraries array is empty");
		errno = EBADMSG;
		return -1;
	}
	for (;;) {
		struct dedup_library *library = NULL;
		if (parse_library(reader, &library) < 0) {
			return -1;
		}
		if (library->name == NULL || config_has_library(config, library->name)) {
			MUSL_LOGE("duplicate library '%{public}s'", library->name ? library->name : "");
			free_config(&(struct dedup_config){.libraries = library});
			errno = EBADMSG;
			return -1;
		}
		*tail = library;
		tail = &library->next;
		json_skip_space(reader);
		if (reader->cur == reader->end) {
			MUSL_LOGE("json: libraries array not closed");
			errno = EBADMSG;
			return -1;
		}
		if (*reader->cur == ']') {
			reader->cur++;
			return 0;
		}
		if (*reader->cur++ != ',') {
			MUSL_LOGE("json: expected ',' between libraries");
			errno = EBADMSG;
			return -1;
		}
	}
}

static int parse_root_property(struct json_reader *reader, struct dedup_config *config,
	const char *key, bool key_truncated, bool *has_libraries)
{
	if (!key_truncated && strcmp(key, "libraries") == 0) {
		if (*has_libraries || parse_libraries(reader, config) < 0) {
			errno = EBADMSG;
			return -1;
		}
		*has_libraries = true;
		return 0;
	}
	return json_skip_value(reader);
}

static int parse_dedup_list(const char *json, size_t size, struct dedup_config *config)
{
	struct json_reader reader = { .cur = json, .end = json + size };
	bool has_libraries = false;

	if (json_consume(&reader, '{') < 0) {
		return -1;
	}
	json_skip_space(&reader);
	if (reader.cur < reader.end && *reader.cur == '}') {
		MUSL_LOGE("deduplist.json: empty object");
		errno = EBADMSG;
		return -1;
	}
	for (;;) {
		char key[64];
		bool key_truncated;
		if (json_read_string(&reader, key, sizeof key, &key_truncated) < 0 ||
			json_consume(&reader, ':') < 0) {
			return -1;
		}
		if (parse_root_property(&reader, config, key, key_truncated, &has_libraries) < 0) {
			return -1;
		}
		json_skip_space(&reader);
		if (reader.cur == reader.end) {
			MUSL_LOGE("json: root object not closed");
			errno = EBADMSG;
			return -1;
		}
		if (*reader.cur == '}') {
			reader.cur++;
			break;
		}
		if (*reader.cur++ != ',') {
			MUSL_LOGE("json: expected ',' in root object");
			errno = EBADMSG;
			return -1;
		}
	}
	json_skip_space(&reader);
	if (!has_libraries || !config->libraries || reader.cur != reader.end) {
		MUSL_LOGE("deduplist.json: missing libraries or trailing garbage");
		errno = EBADMSG;
		return -1;
	}
	return 0;
}

static int find_dedup_list(void)
{
	struct stat status;
	if (lstat(DEDUP_LIST_PATH, &status) != 0) {
		if (errno == ENOENT) {
			MUSL_LOGE("deduplist.json not found at %{public}s", DEDUP_LIST_PATH);
			return 0;
		}
		MUSL_LOGE("lstat deduplist.json failed: %{public}d", errno);
		return -1;
	}
	if (!S_ISREG(status.st_mode)) {
		MUSL_LOGE("deduplist.json is not a regular file");
		errno = EBADMSG;
		return -1;
	}
	return 1;
}

static int read_file(const char *path, char **content, size_t *size)
{
	int fd = open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
	if (fd < 0) {
		MUSL_LOGE("open %{public}s failed: %{public}d", path, errno);
		return -1;
	}
	struct stat status;
	if (fstat(fd, &status) != 0) {
		MUSL_LOGE("fstat %{public}s failed: %{public}d", path, errno);
		close(fd);
		return -1;
	}
	if (status.st_size <= 0) {
		MUSL_LOGE("deduplist.json is empty");
		close(fd);
		errno = EBADMSG;
		return -1;
	}
	size_t file_size = (size_t)status.st_size;
	char *buffer = malloc(file_size + 1);
	if (buffer == NULL) {
		close(fd);
		return -1;
	}
	size_t offset = 0;
	while (offset < file_size) {
		ssize_t count = read(fd, buffer + offset, file_size - offset);
		if (count < 0 && errno == EINTR) {
			continue;
		}
		if (count <= 0) {
			MUSL_LOGE("read %{public}s failed: %{public}d", path, count < 0 ? errno : EIO);
			free(buffer);
			close(fd);
			errno = count < 0 ? errno : EIO;
			return -1;
		}
		offset += (size_t)count;
	}
	close(fd);
	buffer[file_size] = '\0';
	*content = buffer;
	*size = file_size;
	return 0;
}

static int load_config(struct dedup_config *config)
{
	int found = find_dedup_list();
	if (found <= 0) {
		return found;
	}
	char *json = NULL;
	size_t json_size;
	if (read_file(DEDUP_LIST_PATH, &json, &json_size) < 0) {
		return -1;
	}
	int parse_result = parse_dedup_list(json, json_size, config);
	free(json);
	if (parse_result < 0) {
		free_config(config);
		return -1;
	}
	return 1;
}

static int derive_source_path(const struct dedup_library *library,
	const char *version_dir, char *path, size_t path_size)
{
	int n = snprintf(path, path_size, "%s/%s/%s/%s",
		DEDUP_SOURCE_ROOT, library->abi, version_dir, library->name);
	if (n < 0 || (size_t)n >= path_size) {
		MUSL_LOGE("derive_source_path: path too long");
		errno = ENAMETOOLONG;
		return -1;
	}
	return 0;
}

static bool source_file_exists(const char *path)
{
	struct stat status;
	return lstat(path, &status) == 0 && S_ISREG(status.st_mode);
}

static const struct dedup_library *find_library(const struct dedup_config *config, const char *name)
{
	for (const struct dedup_library *library = config->libraries; library; library = library->next) {
		if (strcmp(library->name, name) == 0) {
			return library;
		}
	}
	return NULL;
}

static bool version_greater(const struct dedup_so_version *first,
	const struct dedup_so_version *second)
{
	if (first->major != second->major) {
		return first->major > second->major;
	}
	if (first->minor != second->minor) {
		return first->minor > second->minor;
	}
	return first->patch > second->patch;
}

static const struct dedup_so_version *find_version_by_sha256(
	const struct dedup_library *library, const char *hash)
{
	for (const struct dedup_so_version *v = library->versions; v; v = v->next) {
		if (strcmp(v->sha256, hash) == 0) {
			return v;
		}
	}
	return NULL;
}

static const struct dedup_so_version *find_latest_version(
	const struct dedup_library *library)
{
	const struct dedup_so_version *latest = NULL;
	for (const struct dedup_so_version *v = library->versions; v; v = v->next) {
		if (!latest || version_greater(v, latest)) {
			latest = v;
		}
	}
	return latest;
}

static int lookup_link_source(const struct dedup_config *config, const char *library_name,
	const char *original_sha256, char *install_path)
{
	char derived[PATH_MAX];
	const struct dedup_library *library = find_library(config, library_name);
	if (library == NULL) {
		MUSL_LOGE("library '%{public}s' not found in deduplist.json", library_name);
		return 0;
	}
	const struct dedup_so_version *matched = find_version_by_sha256(library, original_sha256);
	if (matched != NULL) {
		if (derive_source_path(library, matched->key, derived, sizeof derived) < 0) {
			return -1;
		}
		if (source_file_exists(derived)) {
			(void)snprintf(install_path, PATH_MAX, "%s", derived);
			return 1;
		}
		MUSL_LOGE("layer1: sha256 matched version '%{public}s' but preset file missing: %{public}s",
			matched->key, derived);
	}
	const struct dedup_so_version *latest = find_latest_version(library);
	if (latest != NULL) {
		if (derive_source_path(library, latest->key, derived, sizeof derived) < 0) {
			return -1;
		}
		if (source_file_exists(derived)) {
			MUSL_LOGE("layer2 fallback: sha256 mismatch, latest '%{public}s'", latest->key);
			(void)snprintf(install_path, PATH_MAX, "%s", derived);
			return 2;
		}
	}
	MUSL_LOGE("no available preset so for '%{public}s'", library_name);
	return 0;
}

static int lookup_link_install_path(const char *library_name, const char *original_sha256,
	char *install_path)
{
	struct dedup_config config = {0};
	int result = load_config(&config);
	if (result == 0) {
		return SO_DEDUP_CONFIG_UNAVAILABLE;
	}
	if (result < 0) {
		return -1;
	}
	result = lookup_link_source(&config, library_name, original_sha256, install_path);
	free_config(&config);
	return result;
}

static int resolve_source(const char *source_path, struct stat *status)
{
	if (stat(source_path, status) != 0) {
		MUSL_LOGE("stat source '%{public}s' failed: %{public}d", source_path, errno);
		return -1;
	}
	return 0;
}

static bool same_inode(const struct stat *first, const struct stat *second)
{
	return first->st_dev == second->st_dev && first->st_ino == second->st_ino;
}

static uint32_t preserve_errno_result(uint32_t result, int caller_errno)
{
	errno = caller_errno;
	return result;
}

static int verify_symlinked_file(const char *target, const char *source,
	const struct stat *source_status)
{
	char linked_source[PATH_MAX];
	ssize_t length = readlink(target, linked_source, sizeof linked_source - 1);
	if (length < 0) {
		MUSL_LOGE("verify: readlink '%{public}s' failed: %{public}d", target, errno);
		return -1;
	}
	linked_source[length] = '\0';
	if (strcmp(linked_source, source) != 0) {
		MUSL_LOGE("verify: symlink points to '%{public}s', expected '%{public}s'", linked_source, source);
		errno = EBUSY;
		return -1;
	}
	struct stat followed_status;
	if (stat(target, &followed_status) != 0) {
		MUSL_LOGE("verify: stat '%{public}s' failed: %{public}d", target, errno);
		return -1;
	}
	if (!S_ISREG(followed_status.st_mode) || !same_inode(source_status, &followed_status)) {
		MUSL_LOGE("verify: inode mismatch for '%{public}s'", target);
		errno = EBUSY;
		return -1;
	}
	return 0;
}

static int align_symlink_attributes(const char *target, const char *source)
{
	struct stat source_status;
	if (stat(source, &source_status) != 0) {
		MUSL_LOGE("align: stat source '%{public}s' failed: %{public}d", source, errno);
		return -1;
	}
	if (lchown(target, source_status.st_uid, source_status.st_gid) != 0) {
		MUSL_LOGE("align: lchown '%{public}s' failed: %{public}d", target, errno);
		return -1;
	}
	char context[256];
	ssize_t length = lgetxattr(source, "security.selinux", context, sizeof(context) - 1);
	if (length < 0) {
		if (errno == ENOTSUP || errno == ENODATA) {
			return 0;
		}
		MUSL_LOGE("align: lgetxattr '%{public}s' failed: %{public}d", source, errno);
		return -1;
	}
	while (length > 0 && context[length - 1] == '\0') {
		length--;
	}
	context[length] = '\0';
	if (lsetxattr(target, "security.selinux", context, length, 0) != 0) {
		MUSL_LOGE("align: lsetxattr '%{public}s' failed: %{public}d", target, errno);
		return -1;
	}
	return 0;
}

static uint32_t symlink_new_file(const char *source, const char *target,
	const struct stat *source_status)
{
	if (symlink(source, target) == 0) {
		if (align_symlink_attributes(target, source) == 0 &&
			verify_symlinked_file(target, source, source_status) == 0) {
			MUSL_LOGI("symlink created: %{public}s -> %{public}s", target, source);
			return SO_DEDUP_LINKED;
		}
		MUSL_LOGE("post-process failed, keeping link: %{public}s, errno=%{public}d", target, errno);
		return SO_DEDUP_ERROR;
	}
	if (errno != EEXIST) {
		MUSL_LOGE("symlink '%{public}s' failed: %{public}d", target, errno);
		return SO_DEDUP_ERROR;
	}
	if (verify_symlinked_file(target, source, source_status) == 0) {
		return SO_DEDUP_ALREADY_LINKED;
	}
	MUSL_LOGE("target exists but is not our symlink: %{public}s", target);
	errno = EEXIST;
	return SO_DEDUP_ERROR;
}

/*
 * Only system callers may create dedup symlinks
 */
static bool caller_is_system(void)
{
	uid_t uid = getuid();
	return uid < SO_DEDUP_MIN_APP_UID;
}

uint32_t OH_LinkDedupedSo(const char *library_path, const char *original_sha256)
{
	int caller_errno = errno;
	if (!caller_is_system()) {
		MUSL_LOGE("OH_LinkDedupedSo: caller uid %{public}d is not permitted", getuid());
		errno = EACCES;
		return SO_DEDUP_ERROR;
	}
	if (library_path == NULL || library_path[0] != '/' || original_sha256 == NULL) {
		MUSL_LOGE("OH_LinkDedupedSo: invalid input");
		errno = EINVAL;
		return SO_DEDUP_ERROR;
	}
	const char *library_name = path_basename(library_path);
	char install_path[PATH_MAX];
	install_path[0] = '\0';
	int result = lookup_link_install_path(library_name, original_sha256, install_path);
	if (result == SO_DEDUP_CONFIG_UNAVAILABLE) {
		return preserve_errno_result(SO_DEDUP_CONFIG_UNAVAILABLE, caller_errno);
	}
	if (result < 0) {
		return SO_DEDUP_ERROR;
	}
	if (result == 0) {
		return preserve_errno_result(SO_DEDUP_NO_MATCH, caller_errno);
	}
	struct stat source_status;
	if (resolve_source(install_path, &source_status) < 0) {
		return SO_DEDUP_ERROR;
	}
	uint32_t link_result = symlink_new_file(install_path, library_path, &source_status);
	if (link_result == SO_DEDUP_ERROR) {
		return link_result;
	}
	if (result == 2) {
		return preserve_errno_result(SO_DEDUP_VERSION_MISMATCH, caller_errno);
	}
	return preserve_errno_result(link_result, caller_errno);
}
