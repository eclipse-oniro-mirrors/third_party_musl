// commit 39dfd58417ef642307d90306e1c7e50aaec5a35c 2015-03-20
// backslash followed by high byte should be parsed as a normal sequence
#include <locale.h>
#include <regex.h>
#include "test.h"

int main(void)
{
	char buf[200];
	regex_t r;
	int n;

	setlocale(LC_CTYPE, "C.UTF-8");

	n = regcomp(&r, "\\\xfc", 0);
	if (n != 0) {
		regerror(n, &r, buf, sizeof buf);
		t_error("regcomp(\\\\\\xfc) returned %d (%s) wanted 0\n", n, buf);
	}

	return t_status;
}
