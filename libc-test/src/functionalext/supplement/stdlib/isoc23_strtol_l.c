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
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

static void check_c23_cases(locale_t loc)
{
	static const struct {
		const char *text;
		int base;
		long long value;
		int count;
	} cases[] = {
		{ "0b101Z", 0, 5, 5 },
		{ "0B101Z", 2, 5, 5 },
		{ " \t-0b111Z", 0, -7, 8 },
		{ "+0B101Z", 2, 5, 6 },
		{ "0b", 0, 0, 1 },
		{ "-0B2Z", 2, 0, 2 },
		{ "0b101", 10, 0, 1 },
		{ "0b101", 16, 45313, 5 },
		{ "077Z", 0, 63, 3 },
		{ "0x2aZ", 0, 42, 4 },
		{ "9223372036854775807Z", 10, LLONG_MAX, 19 },
		{ "-9223372036854775808Z", 10, LLONG_MIN, 20 },
	};
	for (size_t i=0; i<sizeof cases/sizeof *cases; i++) {
		char *end = 0;
		errno = EDOM;
		long long s = __isoc23_strtoll_l(cases[i].text, &end, cases[i].base, loc);
		if (s != cases[i].value || end != cases[i].text+cases[i].count || errno != EDOM) {
			t_error("signed locale case %zu failed\n", i);
		}
		errno = EDOM;
		unsigned long long u = __isoc23_strtoull_l(cases[i].text, &end, cases[i].base, loc);
		if (u != (unsigned long long)cases[i].value ||
		    end != cases[i].text+cases[i].count || errno != EDOM) {
			t_error("unsigned locale case %zu failed\n", i);
		}
	}
}

static void check_c23(locale_t loc)
{
	check_c23_cases(loc);
	if (__isoc23_strtoll_l("0b101", 0, 0, loc) != 5 ||
	    __isoc23_strtoull_l("0B101", 0, 2, loc) != 5) {
		t_error("null end pointer failed\n");
	}
	char *end;
	const char *text = "18446744073709551615Z";
	errno = 0;
	if (__isoc23_strtoull_l(text, &end, 10, loc) != ULLONG_MAX || end != text+20 || errno) {
		t_error("unsigned decimal limit failed\n");
	}
	text = "18446744073709551616Z";
	errno = 0;
	if (__isoc23_strtoull_l(text, &end, 10, loc) != ULLONG_MAX || end != text+20 || errno != ERANGE) {
		t_error("unsigned decimal overflow failed\n");
	}
	errno = 0;
	if (__isoc23_strtoll_l("0b1", &end, 1, loc) != 0 || errno != EINVAL) {
		t_error("invalid signed base failed\n");
	}
	errno = 0;
	if (__isoc23_strtoull_l("0b1", &end, 37, loc) != 0 || errno != EINVAL) {
		t_error("invalid unsigned base failed\n");
	}
}

static void check_binary_limits(locale_t loc)
{
	char buf[70], *end;
	strcpy(buf, "0b");
	memset(buf+2, '1', 63);
	strcpy(buf+65, "Z");
	errno = 0;
	if (__isoc23_strtoll_l(buf, &end, 0, loc) != LLONG_MAX || end != buf+65 || errno) {
		t_error("signed binary maximum failed\n");
	}
	buf[65] = '1';
	strcpy(buf+66, "Z");
	errno = 0;
	if (__isoc23_strtoull_l(buf, &end, 2, loc) != ULLONG_MAX || end != buf+66 || errno) {
		t_error("unsigned binary maximum failed\n");
	}
	errno = 0;
	if (__isoc23_strtoll_l(buf, &end, 0, loc) != LLONG_MAX || end != buf+66 || errno != ERANGE) {
		t_error("signed binary overflow failed\n");
	}
	buf[66] = '1';
	strcpy(buf+67, "Z");
	errno = 0;
	if (__isoc23_strtoull_l(buf, &end, 2, loc) != ULLONG_MAX || end != buf+67 || errno != ERANGE) {
		t_error("unsigned binary overflow failed\n");
	}
	strcpy(buf, "-0b1");
	memset(buf+4, '0', 63);
	strcpy(buf+67, "Z");
	errno = 0;
	if (__isoc23_strtoll_l(buf, &end, 0, loc) != LLONG_MIN || end != buf+67 || errno) {
		t_error("signed binary minimum failed\n");
	}
	buf[66] = '1';
	errno = 0;
	if (__isoc23_strtoll_l(buf, &end, 0, loc) != LLONG_MIN || end != buf+67 || errno != ERANGE) {
		t_error("signed negative binary overflow failed\n");
	}
}

static void check_legacy(locale_t loc)
{
	char *end;
	const char *text = "0b101";
	errno = EDOM;
	if (strtol_l(text, &end, 0, loc) != 0 || end != text+1 || errno != EDOM) {
		t_error("legacy locale binary prefix behavior changed\n");
	}
	text = " \t-077Z";
	if (strtol_l(text, &end, 0, loc) != -63 || end != text+6) {
		t_error("legacy locale octal failed\n");
	}
	char buf[64];
	int len = snprintf(buf, sizeof buf, "%ldZ", LONG_MIN);
	if (len < 0 || (size_t)len >= sizeof buf) {
		t_error("cannot format legacy locale minimum\n");
		return;
	}
	errno = 0;
	if (strtol_l(buf, &end, 10, loc) != LONG_MIN || *end != 'Z' || errno) {
		t_error("legacy locale minimum failed\n");
	}
	text = "999999999999999999999999999999Z";
	errno = 0;
	if (strtol_l(text, &end, 10, loc) != LONG_MAX || end != text+30 || errno != ERANGE) {
		t_error("legacy locale overflow failed\n");
	}
}

int main(void)
{
	const char *names[] = { "C", "C.UTF-8" };
	locale_t current = uselocale(0);
	for (size_t i=0; i<sizeof names/sizeof *names; i++) {
		locale_t loc = newlocale(LC_ALL_MASK, names[i], 0);
		if (!loc) {
			t_error("cannot create locale %s\n", names[i]);
			continue;
		}
		check_c23(loc);
		check_binary_limits(loc);
		check_legacy(loc);
		if (uselocale(0) != current) {
			t_error("locale conversion changed thread locale\n");
		}
		freelocale(loc);
	}
	return t_status;
}
