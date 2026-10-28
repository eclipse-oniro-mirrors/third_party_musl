#include <stdarg.h>
#include <wchar.h>
#include "stdio_impl.h"

int swscanf(const wchar_t *restrict s, const wchar_t *restrict fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vswscanf(s, fmt, ap);
	va_end(ap);
	return ret;
}

weak_alias(swscanf,__isoc99_swscanf);

int __isoc23_swscanf(const wchar_t *restrict s, const wchar_t *restrict fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = __vswscanf(s, fmt, ap, 1);
	va_end(ap);
	return ret;
}
