#include <stdio.h>
#include <stdarg.h>
#ifndef __LITEOS__
#include "param_check.h"
#endif

int fscanf(FILE *restrict f, const char *restrict fmt, ...)
{
#ifndef __LITEOS__
	PARAM_CHECK(f);
#endif
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vfscanf(f, fmt, ap);
	va_end(ap);
	return ret;
}

int __isoc23_fscanf(FILE *restrict f, const char *restrict fmt, ...)
{
#ifndef __LITEOS__
	PARAM_CHECK(f);
#endif
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = __isoc23_vfscanf(f, fmt, ap);
	va_end(ap);
	return ret;
}

weak_alias(fscanf, __isoc99_fscanf);
