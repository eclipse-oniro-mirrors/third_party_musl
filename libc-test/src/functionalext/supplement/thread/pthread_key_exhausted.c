/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "functionalext.h"

#define STRINGIFY_IMPL(value) #value
#define STRINGIFY(value) STRINGIFY_IMPL(value)

static int build_expected_event_data(const char *program, char *expected_str, size_t size)
{
    char process[256] = {0};
    if (program != NULL) {
        for (size_t i = 0; i < sizeof(process) - 1 && program[i] != '\0'; ++i) {
            process[i] = program[i];
        }
    }
    char *app = strrchr(process, '/');
    if (app == NULL) {
        app = process;
    } else {
        ++app;
    }
    char *suffix = strchr(app, ':');
    if (suffix) {
        *suffix = '\0';
    }
    if (strlcat(expected_str, process[0] ? process : "unknown", size) >= size ||
        strlcat(expected_str, ",keys_max=" STRINGIFY(PTHREAD_KEYS_MAX),
            size) >= size) {
        t_error("Expected event data exceeds buffer size\n");
        return -1;
    }
    return 0;
}

static void collect_hisysevent_usage(FILE *fp, const char *expected_str, int *event_found)
{
    const char *usage[] = {
        ",keys_used=100", ",keys_used=110", ",keys_used=120", (",keys_used=" STRINGIFY(PTHREAD_KEYS_MAX))
    };
    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, expected_str) == NULL) {
            continue;
        }
        for (size_t i = 0; i < sizeof(usage) / sizeof(usage[0]); ++i) {
            if (strstr(line, usage[i]) != NULL) {
                event_found[i] = 1;
            }
        }
    }
}

/**
 * @tc.name:      pthread_key_exhausted_hisysevent
 * @tc.desc:      Query all four events only in a controlled all-samples-hit run.
 * @tc.level:     Level 1
 */
static void check_hisysevent(int argc, char *argv[])
{
    if (argc == 1) {
        printf("HiSysEvent checks skipped: each level is sampled independently (1/10).\n");
        return;
    }
    if (argc != 2 || strcmp(argv[1], "--check-events") != 0) {
        t_error("Usage: %s [--check-events]\n", argv[0]);
        return;
    }
    char expected_str[320] = "process=";
    if (build_expected_event_data(argv[0], expected_str, sizeof(expected_str)) != 0) {
        return;
    }
    usleep(100000);
    FILE *fp = popen("hisysevent -l -n PTHREAD_KEY_EXHAUSTED 2>/dev/null", "r");
    if (fp == NULL) {
        t_error("Failed to run hisysevent command\n");
        return;
    }
    int event_found[4] = {0};
    collect_hisysevent_usage(fp, expected_str, event_found);
    int read_error = ferror(fp);
    int status = pclose(fp);
    if (read_error || status != 0) {
        t_error("Failed to query HiSysEvent (status=%d)\n", status);
        return;
    }
    EXPECT_EQ("pthread_key_usage_100_event_found", event_found[0], 1);
    EXPECT_EQ("pthread_key_usage_110_event_found", event_found[1], 1);
    EXPECT_EQ("pthread_key_usage_120_event_found", event_found[2], 1);
    EXPECT_EQ("pthread_key_exhausted_event_found", event_found[3], 1);
}

static void expect_exhausted(void)
{
    pthread_key_t key = PTHREAD_KEYS_MAX;
    errno = EDOM;
    int ret = pthread_key_create(&key, NULL);
    int saved_errno = errno;
    EXPECT_EQ("pthread_key_exhausted_return", ret, EAGAIN);
    EXPECT_EQ("pthread_key_exhausted_output", (int)key, PTHREAD_KEYS_MAX);
    EXPECT_EQ("pthread_key_exhausted_errno", saved_errno, EDOM);
    if (ret == 0) {
        pthread_key_delete(key);
    }
}

static void cleanup_keys(pthread_key_t *keys, int created)
{
    while (created > 0) {
        --created;
        int ret = pthread_key_delete(keys[created]);
        EXPECT_EQ("pthread_key_exhausted_cleanup", ret, 0);
    }
}

/**
 * @tc.name:      pthread_key_exhausted
 * @tc.desc:      Exhaust the process key table and verify recovery after deletion.
 * @tc.level:     Level 1
 */
int main(int argc, char *argv[])
{
    pthread_key_t keys[PTHREAD_KEYS_MAX];
    int created = 0;

    while (created < PTHREAD_KEYS_MAX) {
        keys[created] = PTHREAD_KEYS_MAX;
        errno = EDOM;
        int ret = pthread_key_create(&keys[created], NULL);
        int saved_errno = errno;
        if (ret == EAGAIN) {
            EXPECT_EQ("pthread_key_first_output", (int)keys[created], PTHREAD_KEYS_MAX);
            EXPECT_EQ("pthread_key_first_errno", saved_errno, EDOM);
            break;
        }
        if (ret != 0) {
            t_error("pthread_key_create failed unexpectedly: %d\n", ret);
            cleanup_keys(keys, created);
            return t_status;
        }
        ++created;
    }

    for (int i = 0; i < 8; ++i) {
        expect_exhausted();
    }

    if (created == 0) {
        t_error("No free pthread key was available at test startup\n");
        cleanup_keys(keys, created);
        return t_status;
    }

    check_hisysevent(argc, argv);

    int ret = pthread_key_delete(keys[created - 1]);
    EXPECT_EQ("pthread_key_exhausted_delete", ret, 0);
    if (ret != 0) {
        cleanup_keys(keys, created);
        return t_status;
    }
    --created;
    ret = pthread_key_create(&keys[created], NULL);
    EXPECT_EQ("pthread_key_exhausted_reuse", ret, 0);
    if (ret == 0) {
        ++created;
        expect_exhausted();
    }

    cleanup_keys(keys, created);
    return t_status;
}
