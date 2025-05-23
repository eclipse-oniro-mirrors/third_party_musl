#include "time_impl.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <ctype.h>
#include <unistd.h>
#include "libc.h"
#include "lock.h"
#include "fork_impl.h"

#include "time_impl.h"
#ifdef OHOS_ENABLE_PARAMETER
#include "sys_param.h"
#define SYSPARAM_LENGTH 40
#endif
#define __TZ_VERSION__ '2'

#define malloc __libc_malloc
#define calloc undef
#define realloc undef
#define free undef

long  __timezone = 0;
int   __daylight = 0;
char *__tzname[2] = { 0, 0 };

weak_alias(__timezone, timezone);
weak_alias(__daylight, daylight);
weak_alias(__tzname, tzname);

static char std_name[TZNAME_MAX+1];
static char dst_name[TZNAME_MAX+1];
const char __utc[] = "UTC";
const char __gmt[] = "GMT";

static int dst_off;
static int r0[5], r1[5];

static const unsigned char *zi, *trans, *index, *types, *abbrevs, *abbrevs_end, *tzdata_map;
static size_t map_size, tzdata_map_size, map_offset;

static char old_tz_buf[32];
static char *old_tz = old_tz_buf;
static size_t old_tz_size = sizeof old_tz_buf;
static int cota_accessable = 0;

#if defined(OHOS_ENABLE_PARAMETER) && (!defined(__LITEOS__))
static CachedHandle cota_param_handle = NULL;
static CachedHandle tz_param_handle = NULL;
#endif

static volatile int lock[1];
volatile int *const __timezone_lockptr = lock;

void InitTimeZoneParam(void)
{
#if defined(OHOS_ENABLE_PARAMETER) && (!defined(__LITEOS__))
	if (cota_param_handle == NULL) {
		cota_param_handle = CachedParameterCreate("persist.global.tz_override", "false");
	}
	if (tz_param_handle == NULL) {
		tz_param_handle = CachedParameterCreate("persist.time.timezone", "/etc/localtime");
	}
#endif
}

static int getint(const char **p)
{
	unsigned x;
	for (x=0; **p-'0'<10U; (*p)++) x = **p-'0' + 10*x;
	return x;
}

static int getoff(const char **p)
{
	int neg = 0;
	if (**p == '-') {
		++*p;
		neg = 1;
	} else if (**p == '+') {
		++*p;
	}
	int off = 3600*getint(p);
	if (**p == ':') {
		++*p;
		off += 60*getint(p);
		if (**p == ':') {
			++*p;
			off += getint(p);
		}
	}
	return neg ? -off : off;
}

static void getrule(const char **p, int rule[5])
{
	int r = rule[0] = **p;

	if (r!='M') {
		if (r=='J') ++*p;
		else rule[0] = 0;
		rule[1] = getint(p);
	} else {
		++*p; rule[1] = getint(p);
		++*p; rule[2] = getint(p);
		++*p; rule[3] = getint(p);
	}

	if (**p=='/') {
		++*p;
		rule[4] = getoff(p);
	} else {
		rule[4] = 7200;
	}
}

static void getname(char *d, const char **p)
{
	int i;
	if (**p == '<') {
		++*p;
		for (i=0; (*p)[i] && (*p)[i]!='>'; i++)
			if (i<TZNAME_MAX) d[i] = (*p)[i];
		if ((*p)[i]) ++*p;
	} else {
		for (i=0; ((*p)[i]|32)-'a'<26U; i++)
			if (i<TZNAME_MAX) d[i] = (*p)[i];
	}
	*p += i;
	d[i<TZNAME_MAX?i:TZNAME_MAX] = 0;
}

#define VEC(...) ((const unsigned char[]){__VA_ARGS__})

static uint32_t zi_read32(const unsigned char *z)
{
	return (unsigned)z[0]<<24 | z[1]<<16 | z[2]<<8 | z[3];
}

static size_t zi_dotprod(const unsigned char *z, const unsigned char *v, size_t n)
{
	size_t y;
	uint32_t x;
	for (y=0; n; n--, z+=4, v++) {
		x = zi_read32(z);
		y += x * *v;
	}
	return y;
}

static int check_cota(void)
{
#if defined(OHOS_ENABLE_PARAMETER) && (!defined(__LITEOS__))
	static int cota_exist = 0;
	if (cota_exist) return 1;
	if (cota_param_handle == NULL) {
		cota_param_handle = CachedParameterCreate("persist.global.tz_override", "false");
	}
	const char *cota_param_value = CachedParameterGet(cota_param_handle);
	if (cota_param_value != NULL && !strcmp(cota_param_value, "true")) {
		cota_exist = 1;
		return 1;
	}
	return 0;
#else
	return 0;
#endif
}

static void do_tzset(int use_env)
{
	char buf[NAME_MAX+25], *pathname=buf+24;
	const char *try, *p;
	const char *s = NULL;
	const unsigned char *map = 0;
	size_t i;
	static const char cota_path[] = "/etc/tzdata_distro/tzdata\0";

	/* Cota timezone data needs to be mounted at startup, some app's
	 * startup timezone is before cota timezone data mounted time.
	 * In addition, because of sandbox mechanism, app can't read cota
	 * timezone data at startup. Therefore, it is necessary to read
	 * System param and cota file reading attempt to ensure cota
	 * timezone data is loaded.*/
	int use_cota = 0;
	int keep_old_tzid = 1;
	int old_errno = errno;
	if (!cota_accessable) {
		if (check_cota()) {
			if (access(cota_path, R_OK) == 0) {
				use_cota = 1;
				keep_old_tzid = 0;
				cota_accessable = 1;
			}
		}
		errno = old_errno;
	} else {
		use_cota = 1;
	}

#ifndef __LITEOS__
	static const char search[] =
		"/etc/zoneinfo/tzdata\0/usr/share/zoneinfo/tzdata\0/share/zoneinfo/tzdata\0";
#else
	static const char search[] =
		"/usr/share/zoneinfo/tzdata\0/share/zoneinfo/tzdata\0/etc/zoneinfo/tzdata\0";
#endif

	if (use_env == 1) {
		s = getenv("TZ");
	}

	if (!s) {
#if defined(OHOS_ENABLE_PARAMETER) && (!defined(__LITEOS__))
		if (tz_param_handle == NULL) {
			tz_param_handle = CachedParameterCreate("persist.time.timezone", "/etc/localtime");
		}
		const char *tz_param_value = CachedParameterGet(tz_param_handle);
		if (tz_param_value != NULL) {
			s = tz_param_value;
		} else {
			s = "/etc/localtime";
		}
#else
		s = "/etc/localtime";
#endif
	}
	if (!*s) s = __utc;

	if (keep_old_tzid && old_tz && !strcmp(s, old_tz)) return;

	for (i=0; i<5; i++) r0[i] = r1[i] = 0;

	if (tzdata_map && zi) {
		if (tzdata_map + map_offset == zi) {
			__munmap((void *)tzdata_map, tzdata_map_size);
		} else {
			__munmap((void *)zi, map_size);
		}
	}

	/* Cache the old value of TZ to check if it has changed. Avoid
	 * free so as not to pull it into static programs. Growth
	 * strategy makes it so free would have minimal benefit anyway. */
	i = strlen(s);
	if (i > PATH_MAX+1) s = __utc, i = 3;
	if (i >= old_tz_size) {
		old_tz_size *= 2;
		if (i >= old_tz_size) old_tz_size = i+1;
		if (old_tz_size > PATH_MAX+2) old_tz_size = PATH_MAX+2;
		old_tz = malloc(old_tz_size);
	}
	if (old_tz) memcpy(old_tz, s, i+1);

	int posix_form = 0;
	if (*s != ':') {
		p = s;
		char dummy_name[TZNAME_MAX+1];
		getname(dummy_name, &p);
		if (p!=s && (*p == '+' || *p == '-' || isdigit(*p)
					|| !strcmp(dummy_name, "UTC")
					|| !strcmp(dummy_name, "GMT")))
			posix_form = 1;
	}	

	/* Non-suid can use an absolute tzfile pathname or a relative
	 * pathame beginning with "."; in secure mode, only the
	 * standard path will be searched. */
#ifndef __LITEOS__
	int flag = 1;
	if (!posix_form) {
		if (*s == ':') s++;
		if (*s == '/' || *s == '.') {
			/* The path is invalid, use the default value. */
			flag = 0;
			if (!libc.secure || !strcmp(s, "/etc/localtime")) {
				map = __map_file(s, &map_size);
				map_offset = 0;
			}
		}
	}

	if (flag) {
		/* Adapt to time zone names, such as Asia/Shanghai or Shanghai*/
		size_t l = strlen(s);
		if (l <= NAME_MAX && !strchr(s, '.')) {
			memcpy(pathname, s, l+1);
			pathname[l] = 0;
			/* Try to load distro timezone data first*/
			if (use_cota) {
				tzdata_map = __map_tzdata_file(cota_path, pathname, &tzdata_map_size, &map_offset, &map_size);
				if (tzdata_map != NULL) {
					map = tzdata_map + map_offset;
				}
			}
			try = search;
			while (!map && *try) {
				tzdata_map = __map_tzdata_file(try, pathname, &tzdata_map_size, &map_offset, &map_size);
				if (tzdata_map != NULL) {
					map = tzdata_map + map_offset;
				}
				try += strlen(try) + 1;
			}
		}
	}
#else
	if (!posix_form) {
		if (*s == ':') s++;
		if (*s == '/' || *s == '.') {
			if (!libc.secure || !strcmp(s, "/etc/localtime"))
				map = __map_file(s, &map_size);
				map_offset = 0;
		} else {
			size_t l = strlen(s);
			if (l <= NAME_MAX && !strchr(s, '.')) {
				memcpy(pathname, s, l+1);
				pathname[l] = 0;
				for (try=search; !map && *try; try+=l+1) {
					l = strlen(try);
					memcpy(pathname-l, try, l);
					map = __map_file(pathname-l, &map_size);
					map_offset = 0;
				}
			}
		}
		if (!map) s = __utc;
	}
#endif

	if (map && (map_size < 44 || memcmp(map, "TZif", 4))) {
		if (tzdata_map + map_offset == map) {
			__munmap((void *)tzdata_map, tzdata_map_size);
			tzdata_map = 0;
		} else {
			__munmap((void *)map, map_size);
		}
		map = 0;
		s = __utc;
	}

	zi = map;
	if (map) {
		int scale = 2;
		/*
         * map[0]-map[3]: magic, it is TZif
         * map[4]:        version, '\0' or '2' or '3' as of 2013
         * map[5]-map[19]: reserved; must be zero
         * map[20]-map[23]: The number of UT/local indicators stored in the file.
         * map[24]-map[27]: The number of standard/wall indicators stored in the file.
         * map[24]-map[31]: The number of leap seconds for which data entries are stored in the file.
         * map[32]-map[35]: The number of transition times for which data entries are stored in the file.
         * map[36]-map[39]: The number of local time types for which data entries are
         *                  stored in the file (must not be zero).
         * map[40]-map[43]: The number of bytes of time zone abbreviation strings stored in the file.

         * If map[4] is '2' or greater, the above is followed by a second instance
         * of tzhead and a second instance of the data in which each coded transition
         * time uses 8 rather than 4 chars,
         * then a POSIX-TZ-environment-variable-style string for use in handling
         * instants after the last transition time stored in the file
         * (with nothing between the newlines if there is no POSIX representation for
         * such instants).

         * If map[4] is '3' or greater, the above is extended as follows.
         * First, the POSIX TZ string's hour offset may range from -167
         * through 167 as compared to the POSIX-required 0 through 24.
         * Second, its DST start time may be January 1 at 00:00 and its stop
         * time December 31 at 24:00 plus the difference between DST and
         * standard time, indicating DST all year. */
		if (map[4]!='1') {
			size_t skip = zi_dotprod(zi+20, VEC(1,1,8,5,6,1), 6);
			trans = zi+skip+44+44;
			scale++;
		} else {
			trans = zi+44;
		}
		index = trans + (zi_read32(trans-12) << scale);
		types = index + zi_read32(trans-12);
		abbrevs = types + 6*zi_read32(trans-8);
		abbrevs_end = abbrevs + zi_read32(trans-4);
		if (zi[map_size-1] == '\n') {
			for (s = (const char *)zi+map_size-2; *s!='\n'; s--);
			s++;
		} else {
			const unsigned char *p;
			__tzname[0] = __tzname[1] = 0;
			__daylight = __timezone = dst_off = 0;
			for (p=types; p<abbrevs; p+=6) {
				if (!p[4] && !__tzname[0]) {
					__tzname[0] = (char *)abbrevs + p[5];
					__timezone = -zi_read32(p);
				}
				if (p[4] && !__tzname[1]) {
					__tzname[1] = (char *)abbrevs + p[5];
					dst_off = -zi_read32(p);
					__daylight = 1;
				}
			}
			if (!__tzname[0]) __tzname[0] = __tzname[1];
			if (!__tzname[0]) __tzname[0] = (char *)__utc;
			if (!__daylight) {
				__tzname[1] = __tzname[0];
				dst_off = __timezone;
			}
			return;
		}
	}

	if (!s) s = __utc;
	getname(std_name, &s);
	__tzname[0] = std_name;
	__timezone = getoff(&s);
	getname(dst_name, &s);
	__tzname[1] = dst_name;
	if (dst_name[0]) {
		__daylight = 1;
		if (*s == '+' || *s=='-' || *s-'0'<10U)
			dst_off = getoff(&s);
		else
			dst_off = __timezone - 3600;
	} else {
		__daylight = 0;
		dst_off = __timezone;
	}

	if (*s == ',') s++, getrule(&s, r0);
	if (*s == ',') s++, getrule(&s, r1);
}

/* Search zoneinfo rules to find the one that applies to the given time,
 * and determine alternate opposite-DST-status rule that may be needed. */

static size_t scan_trans(long long t, int local, size_t *alt)
{
	int scale = 3 - (trans == zi+44);
	uint64_t x;
	int off = 0;

	size_t a = 0, n = (index-trans)>>scale, m;

	if (!n) {
		if (alt) *alt = 0;
		return 0;
	}

	/* Binary search for 'most-recent rule before t'. */
	while (n > 1) {
		m = a + n/2;
		x = zi_read32(trans + (m<<scale));
		if (scale == 3) x = x<<32 | zi_read32(trans + (m<<scale) + 4);
		else x = (int32_t)x;
		if (local) off = (int32_t)zi_read32(types + 6 * index[m-1]);
		if (t - off < (int64_t)x) {
			n /= 2;
		} else {
			a = m;
			n -= n/2;
		}
	}

	/* First and last entry are special. First means to use lowest-index
	 * non-DST type. Last means to apply POSIX-style rule if available. */
	n = (index-trans)>>scale;
	if (a == n-1) return -1;
	if (a == 0) {
		x = zi_read32(trans);
		if (scale == 3) x = x<<32 | zi_read32(trans + 4);
		else x = (int32_t)x;
		/* Find the lowest non-DST type, or 0 if none. */
		size_t j = 0;
		for (size_t i=abbrevs-types; i; i-=6) {
			if (!types[i-6+4]) j = i-6;
		}
		if (local) off = (int32_t)zi_read32(types + j);
		/* If t is before first transition, use the above-found type
		 * and the index-zero (after transition) type as the alt. */
		if (t - off < (int64_t)x) {
			if (alt) *alt = index[0];
			return j/6;
		}
	}

	/* Try to find a neighboring opposite-DST-status rule. */
	if (alt) {
		if (a && types[6*index[a-1]+4] != types[6*index[a]+4])
			*alt = index[a-1];
		else if (a+1<n && types[6*index[a+1]+4] != types[6*index[a]+4])
			*alt = index[a+1];
		else
			*alt = index[a];
	}

	return index[a];
}

static int days_in_month(int m, int is_leap)
{
	if (m==2) return 28+is_leap;
	else return 30+((0xad5>>(m-1))&1);
}

/* Convert a POSIX DST rule plus year to seconds since epoch. */

static long long rule_to_secs(const int *rule, int year)
{
	int is_leap;
	long long t = __year_to_secs(year, &is_leap);
	int x, m, n, d;
	if (rule[0]!='M') {
		x = rule[1];
		if (rule[0]=='J' && (x < 60 || !is_leap)) x--;
		t += 86400 * x;
	} else {
		m = rule[1];
		n = rule[2];
		d = rule[3];
		t += __month_to_secs(m-1, is_leap);
		int wday = (int)((t + 4*86400) % (7*86400)) / 86400;
		int days = d - wday;
		if (days < 0) days += 7;
		if (n == 5 && days+28 >= days_in_month(m, is_leap)) n = 4;
		t += 86400 * (days + 7*(n-1));
	}
	t += rule[4];
	return t;
}

/* Determine the time zone in effect for a given time in seconds since the
 * epoch. It can be given in local or universal time. The results will
 * indicate whether DST is in effect at the queried time, and will give both
 * the GMT offset for the active zone/DST rule and the opposite DST. This
 * enables a caller to efficiently adjust for the case where an explicit
 * DST specification mismatches what would be in effect at the time. */

void __secs_to_zone(long long t, int local, int *isdst, long *offset, long *oppoff, const char **zonename, int use_env)
{
	LOCK(lock);
	if (use_env == 1) {
		do_tzset(TZ_USE_ENV);
	} else {
		do_tzset(TZ_NO_USE_ENV);
	}

	if (zi) {
		size_t alt, i = scan_trans(t, local, &alt);
		if (i != -1) {
			*isdst = types[6*i+4];
			*offset = (int32_t)zi_read32(types+6*i);
			*zonename = (const char *)abbrevs + types[6*i+5];
			if (oppoff) *oppoff = (int32_t)zi_read32(types+6*alt);
			UNLOCK(lock);
			return;
		}
	}

	if (!__daylight) goto std;

	/* FIXME: may be broken if DST changes right at year boundary?
	 * Also, this could be more efficient.*/
	long long y = t / 31556952 + 70;
	while (__year_to_secs(y, 0) > t) y--;
	while (__year_to_secs(y+1, 0) < t) y++;

	long long t0 = rule_to_secs(r0, y);
	long long t1 = rule_to_secs(r1, y);

	if (!local) {
		t0 += __timezone;
		t1 += dst_off;
	}
	if (t0 < t1) {
		if (t >= t0 && t < t1) goto dst;
		goto std;
	} else {
		if (t >= t1 && t < t0) goto std;
		goto dst;
	}
std:
	*isdst = 0;
	*offset = -__timezone;
	if (oppoff) *oppoff = -dst_off;
	*zonename = __tzname[0];
	UNLOCK(lock);
	return;
dst:
	*isdst = 1;
	*offset = -dst_off;
	if (oppoff) *oppoff = -__timezone;
	*zonename = __tzname[1];
	UNLOCK(lock);
}

static void __tzset()
{
	LOCK(lock);
	do_tzset(TZ_USE_ENV);
	UNLOCK(lock);
}

weak_alias(__tzset, tzset);

const char *__tm_to_tzname(const struct tm *tm)
{
	const void *p = tm->__tm_zone;
	LOCK(lock);
	do_tzset(TZ_USE_ENV);
	if (p != __utc && p != __tzname[0] && p != __tzname[1] &&
	    (!zi || (uintptr_t)p-(uintptr_t)abbrevs >= abbrevs_end - abbrevs))
		p = "";
	UNLOCK(lock);
	return p;
}
