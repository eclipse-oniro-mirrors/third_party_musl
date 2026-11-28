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
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdbool.h>
#include <dlfcn.h>
#include <dlfcn_ext.h>
#include <string.h>

static int nested_value = 42;

__attribute__((constructor)) static void relative_path_init(void)
{
    Dl_info info;
    if (!dladdr((void *)relative_path_init, &info) || !info.dli_fname) {
        nested_value = -1;
        return;
    }
    const char *name = strrchr(info.dli_fname, '/');
    if (!name || strcmp(name + 1, "librelative_path_cold.so")) {
        return;
    }

    /* Loading the cold copy reenters dlopen with a different relative path.
     * Check that each DSO retains its own path after the outer loader unlocks. */
    nested_value = -1;
    void *handle = dlopen("./allowed/librelative_path_fixture.so", RTLD_NOW);
    if (!handle) {
        return;
    }
    int (*value)(void) = dlsym(handle, "relative_path_value");
    if (value) {
        nested_value = value();
    }
    if (dlclose(handle)) {
        nested_value = -1;
    }
}

int relative_path_nested_value(void) { return nested_value; }

int relative_path_value(void) { return 42; }

/* Exercise caller-namespace selection for dlopen and dlopen_ext. The volatile
 * local prevents a tail call from substituting the executable's caller address. */
void *relative_path_open(const char *path, int mode, int extended)
{
    const dl_extinfo info = {0};
    void *volatile handle = extended ? dlopen_ext(path, mode, &info) : dlopen(path, mode);
    return handle;
}

/* Pass caller-provided extinfo through dlopen_ext for extinfo coverage. */
void *relative_path_open_ext_info(const char *path, int mode, const dl_extinfo *info)
{
    void *volatile handle = dlopen_ext(path, mode, info);
    return handle;
}
