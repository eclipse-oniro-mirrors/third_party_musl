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

#ifndef IO_GUARD_IMPL_H
#define IO_GUARD_IMPL_H
#ifdef IO_GUARD_ENABLE
#include <stdint.h>
#include "../../interfaces/inner_api/io_guard.h"
__attribute__((visibility("hidden"))) void __init_io_guard(void);
__attribute__((visibility("hidden"))) int __io_guard_check(int fd, uint64_t expected);
__attribute__((visibility("hidden"))) uint64_t __io_guard_get_token(int fd);
#endif
#endif
