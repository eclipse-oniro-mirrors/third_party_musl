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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>

long strtol_l(const char *restrict s, char **restrict p, int base, locale_t loc)
{
	return strtol(s, p, base);
}

long long __isoc23_strtoll_l(const char *restrict s, char **restrict p, int base, locale_t loc)
{
	return __isoc23_strtoll(s, p, base);
}

unsigned long long __isoc23_strtoull_l(const char *restrict s, char **restrict p, int base, locale_t loc)
{
	return __isoc23_strtoull(s, p, base);
}
