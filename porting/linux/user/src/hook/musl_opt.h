/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef _MUSL_OPT_H
#define _MUSL_OPT_H

#include "pthread_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*lock_func_type)(pthread_mutex_t *restrict);
extern lock_func_type lock_func;

#ifdef __cplusplus
}
#endif

#endif