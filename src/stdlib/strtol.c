#include "stdio_impl.h"
#include "intscan.h"
#include "shgetc.h"
#include <inttypes.h>
#include <limits.h>
#include <ctype.h>

static unsigned long long strtox(const char *s, char **p, int base, unsigned long long lim, int bin_prefix)
{
	FILE f;
	sh_fromstring(&f, s);
	shlim(&f, 0);
	unsigned long long y = __intscan(&f, base, 1, lim, bin_prefix);
	if (p) {
		size_t cnt = shcnt(&f);
		*p = (char *)s + cnt;
	}
	return y;
}

unsigned long long strtoull(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, ULLONG_MAX, 0);
}

long long strtoll(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, LLONG_MIN, 0);
}

#ifndef __LITEOS__
unsigned long strtoul_weak(const char *restrict s, char **restrict p, int base)
#else
unsigned long strtoul(const char *restrict s, char **restrict p, int base)
#endif
{
	return strtox(s, p, base, ULONG_MAX, 0);
}

long strtol(const char *restrict s, char **restrict p, int base)
{
	return (long)strtox(s, p, base, 0UL+LONG_MIN, 0);
}

long __isoc23_strtol(const char *restrict s, char **restrict p, int base)
{
	return (long)strtox(s, p, base, 0UL+LONG_MIN, 1);
}

unsigned long __isoc23_strtoul(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, ULONG_MAX, 1);
}

long long __isoc23_strtoll(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, LLONG_MIN, 1);
}

unsigned long long __isoc23_strtoull(const char *restrict s, char **restrict p, int base)
{
	return strtox(s, p, base, ULLONG_MAX, 1);
}

#ifndef __LITEOS__
intmax_t strtoimax_weak(const char *restrict s, char **restrict p, int base)
#else
intmax_t strtoimax(const char *restrict s, char **restrict p, int base)
#endif
{
	return strtoll(s, p, base);
}

intmax_t __isoc23_strtoimax(const char *restrict s, char **restrict p, int base)
{
	return __isoc23_strtoll(s, p, base);
}

#ifndef __LITEOS__
uintmax_t strtoumax_weak(const char *restrict s, char **restrict p, int base)
#else
uintmax_t strtoumax(const char *restrict s, char **restrict p, int base)
#endif
{
	return strtoull(s, p, base);
}

uintmax_t __isoc23_strtoumax(const char *restrict s, char **restrict p, int base)
{
	return __isoc23_strtoull(s, p, base);
}

weak_alias(strtol, __strtol_internal);
#ifndef __LITEOS__
weak_alias(strtoul_weak, strtoul);
#endif
weak_alias(strtoul, __strtoul_internal);
weak_alias(strtoll, __strtoll_internal);
weak_alias(strtoull, __strtoull_internal);
#ifndef __LITEOS__
weak_alias(strtoimax_weak, strtoimax);
weak_alias(strtoumax_weak, strtoumax);
#endif
weak_alias(strtoimax, __strtoimax_internal);
weak_alias(strtoumax, __strtoumax_internal);
