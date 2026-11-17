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

#include <stdbool.h>
#include <dlfcn.h>
#include "test.h"

/* This is deliberately a consumer declaration, not a public header change. */
extern void dlopen_enable_relative_path(void);

int main(void)
{
    void (*enable)(void) = dlsym(RTLD_DEFAULT, "dlopen_enable_relative_path");
    if (!enable) {
        t_error("dlopen_enable_relative_path is not exported: %s\n", dlerror());
        return t_status;
    }
    dlopen_enable_relative_path();
    enable();
    return t_status;
}
