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

#define _GNU_SOURCE
#include <stdlib.h>
#include <langinfo.h>
#include <regex.h>

static int match_pattern(const char *response, int item,
                         int on_match, int on_mismatch)
{
	regex_t re;
	int matched;

	if (regcomp(&re, nl_langinfo(item), REG_EXTENDED | REG_NOSUB) != 0) {
		return -1;
	}
	matched = regexec(&re, response, 0, 0, 0) == 0;
	regfree(&re);
	return matched ? on_match : on_mismatch;
}

int rpmatch(const char *response)
{
	int r;
	r = match_pattern(response, YESEXPR, 1, 0);
	if (r == 0) {
		r = match_pattern(response, NOEXPR, 0, -1);
	}
	return r;
}