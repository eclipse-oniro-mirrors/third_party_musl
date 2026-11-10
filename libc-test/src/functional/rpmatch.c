/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include "test.h"

static void check_rpmatch(const char *s, int expected)
{
	int r = rpmatch(s);
	if (r != expected)
		t_error("rpmatch(\"%s\") expected %d, got %d\n", s, expected, r);
}

int main(void)
{
	/* C locale: YESEXPR=^[yY], NOEXPR=^[nN] (glibc tst-rpmatch case table) */
	check_rpmatch("Yes", 1);
	check_rpmatch("yes", 1);
	check_rpmatch("YES", 1);
	check_rpmatch("y", 1);
	check_rpmatch("No", 0);
	check_rpmatch("no", 0);
	check_rpmatch("n", 0);
	check_rpmatch("nonsens", 0);
	check_rpmatch("Error", -1);

	/* neither affirmative nor negative */
	check_rpmatch("q", -1);
	check_rpmatch("", -1);

	return t_status;
}
