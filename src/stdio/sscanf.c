#include <stdio.h>
#include <stdarg.h>

int sscanf(const char *restrict s, const char *restrict fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vsscanf(s, fmt, ap);
	va_end(ap);
	return ret;
}

int __isoc23_sscanf(const char *restrict s, const char *restrict fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = __isoc23_vsscanf(s, fmt, ap);
	va_end(ap);
	return ret;
}

weak_alias(sscanf,__isoc99_sscanf);
