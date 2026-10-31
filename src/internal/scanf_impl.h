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

#ifndef SCANF_IMPL_H
#define SCANF_IMPL_H

#include <stdint.h>

#define SIZE_hh (-2)
#define SIZE_h  (-1)
#define SIZE_def 0
#define SIZE_l   1
#define SIZE_L   2
#define SIZE_ll  3
#define SIZE_invalid (-3)

/* Map the stdint types to the existing integer stores on each architecture. */
static int scanf_wsize(int bits, int fast)
{
	unsigned int bytes;

	switch (bits) {
	case 8:
		bytes = (fast != 0) ? sizeof(int_fast8_t) : sizeof(int8_t);
		break;
	case 16:
		bytes = (fast != 0) ? sizeof(int_fast16_t) : sizeof(int16_t);
		break;
	case 32:
		bytes = (fast != 0) ? sizeof(int_fast32_t) : sizeof(int32_t);
		break;
	case 64:
		bytes = (fast != 0) ? sizeof(int_fast64_t) : sizeof(int64_t);
		break;
	default:
		return SIZE_invalid;
	}

	if (bytes == sizeof(char)) {
		return SIZE_hh;
	}
	if (bytes == sizeof(short)) {
		return SIZE_h;
	}
	if (bytes == sizeof(int)) {
		return SIZE_def;
	}
	if (bytes == sizeof(long)) {
		return SIZE_l;
	}
	if (bytes == sizeof(long long)) {
		return SIZE_ll;
	}
	return SIZE_invalid;
}

#endif
