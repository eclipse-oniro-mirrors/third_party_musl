#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>

#include "stdio_impl.h"
#include "shgetc.h"
#include "intscan.h"
#include "floatscan.h"
#include "scanf_impl.h"

#define BUF_LEN 513

#if (defined(MUSL_AARCH64_ARCH)) || (defined(MUSL_ARM_ARCH))
extern int parsefloat(FILE *f, char *buf, char *end);
#endif

static void store_int(void *dest, int size, unsigned long long i)
{
	if (!dest) return;
	switch (size) {
	case SIZE_hh:
		*(char *)dest = i;
		break;
	case SIZE_h:
		*(short *)dest = i;
		break;
	case SIZE_def:
		*(int *)dest = i;
		break;
	case SIZE_l:
		*(long *)dest = i;
		break;
	case SIZE_ll:
		*(long long *)dest = i;
		break;
	}
}

static void *arg_n(va_list ap, unsigned int n)
{
	void *p;
	unsigned int i;
	va_list ap2;
	va_copy(ap2, ap);
	for (i=n; i>1; i--) va_arg(ap2, void *);
	p = va_arg(ap2, void *);
	va_end(ap2);
	return p;
}

static int vfscanf_internal(FILE *restrict f, const char *restrict fmt, va_list ap, int isoc23)
{
#if (defined(MUSL_AARCH64_ARCH)) || (defined(MUSL_ARM_ARCH))
	char buf[BUF_LEN];
	char *endptr;
#endif
	int width;
	int size;
	int alloc = 0;
	int base;
	const unsigned char *p;
	int c, t;
	int saved_errno;
	char *s;
	wchar_t *wcs;
	mbstate_t st;
	void *dest=NULL;
	int invert;
	int matches=0;
	unsigned long long x;
	long double y;
	off_t pos = 0;
	unsigned char scanset[257];
	size_t i, k;
	wchar_t wc;

	FLOCK(f);

	if (!f->rpos) __toread(f);
	if (!f->rpos) goto input_fail;

	for (p=(const unsigned char *)fmt; *p; p++) {

		alloc = 0;

		if (isspace(*p)) {
			while (isspace(p[1])) p++;
			shlim(f, 0);
			while (isspace(shgetc(f)));
			shunget(f);
			pos += shcnt(f);
			continue;
		}
		if (*p != '%' || p[1] == '%') {
			shlim(f, 0);
			if (*p == '%') {
				p++;
				while (isspace((c=shgetc(f))));
			} else {
				c = shgetc(f);
			}
			if (c!=*p) {
				shunget(f);
				if (c<0) goto input_fail;
				goto match_fail;
			}
			pos += shcnt(f);
			continue;
		}

		p++;
		if (*p=='*') {
			dest = 0; p++;
		} else if (isdigit(*p) && p[1]=='$') {
			dest = arg_n(ap, *p-'0'); p+=2;
		} else {
			dest = va_arg(ap, void *);
		}

		for (width=0; isdigit(*p); p++) {
			if (isoc23 && width > (INT_MAX - (*p-'0')) / 10) {
				goto width_fail;
			}
			width = 10*width + *p - '0';
		}

		if (*p=='m') {
			wcs = 0;
			s = 0;
			alloc = !!dest;
			p++;
		} else {
			alloc = 0;
		}

		size = SIZE_def;
		switch (*p++) {
		case 'h':
			if (*p == 'h') p++, size = SIZE_hh;
			else size = SIZE_h;
			break;
		case 'l':
			if (*p == 'l') p++, size = SIZE_ll;
			else size = SIZE_l;
			break;
		case 'j':
			size = SIZE_ll;
			break;
		case 'z':
		case 't':
			size = SIZE_l;
			break;
		case 'L':
			size = SIZE_L;
			break;
		case 'w': {
			int fast = 0, bits = 0;
			if (!isoc23) {
				goto fmt_fail;
			}
			if (*p == 'f') {
				fast = 1, p++;
			}
			if (*p < '1' || *p > '9') {
				goto width_fail;
			}
			for (; *p >= '0' && *p <= '9'; p++) {
				/* Only widths up to 64 are supported; avoid overflow. */
				if (bits > 64) {
					goto width_fail;
				}
				bits = 10*bits + *p - '0';
			}
			size = scanf_wsize(bits, fast);
			if (size == SIZE_invalid) {
				goto width_fail;
			}
			switch (*p) {
			case 'b': case 'd': case 'i': case 'o':
			case 'u': case 'x': case 'X': case 'n':
				break;
			default:
				goto width_fail;
			}
			break;
		}
		case 'b':
			if (!isoc23) {
				goto fmt_fail;
			}
			p--;
			break;
		case 'd': case 'i': case 'o': case 'u': case 'x':
		case 'a': case 'e': case 'f': case 'g':
		case 'A': case 'E': case 'F': case 'G': case 'X':
		case 's': case 'c': case '[':
		case 'S': case 'C':
		case 'p': case 'n':
			p--;
			break;
		default:
			goto fmt_fail;
		}

		t = *p;

		/* C or S */
		if ((t&0x2f) == 3) {
			t |= 32;
			size = SIZE_l;
		}

		switch (t) {
		case 'c':
			if (width < 1) width = 1;
		case '[':
			break;
		case 'n':
			store_int(dest, size, pos);
			/* do not increment match count, etc! */
			continue;
		default:
			shlim(f, 0);
			while (isspace(shgetc(f)));
			shunget(f);
			pos += shcnt(f);
		}

		/* Only the C23 path counts wide output characters instead of bytes. */
		shlim(f, isoc23 && size == SIZE_l && (t == 'c' || t == 's' || t == '[') ? 0 : width);
		if (shgetc(f) < 0) goto input_fail;
		shunget(f);

		switch (t) {
		case 's':
		case 'c':
		case '[':
			if (t == 'c' || t == 's') {
				memset(scanset, -1, sizeof scanset);
				scanset[0] = 0;
				if (t == 's') {
					scanset[1+'\t'] = 0;
					scanset[1+'\n'] = 0;
					scanset[1+'\v'] = 0;
					scanset[1+'\f'] = 0;
					scanset[1+'\r'] = 0;
					scanset[1+' '] = 0;
				}
			} else {
				if (*++p == '^') p++, invert = 1;
				else invert = 0;
				memset(scanset, invert, sizeof scanset);
				scanset[0] = 0;
				if (*p == '-') p++, scanset[1+'-'] = 1-invert;
				else if (*p == ']') p++, scanset[1+']'] = 1-invert;
				for (; *p != ']'; p++) {
					if (!*p) goto fmt_fail;
					if (*p=='-' && p[1] && p[1] != ']')
						for (c=p++[-1]; c<*p; c++)
							scanset[1+c] = 1-invert;
					scanset[1+*p] = 1-invert;
				}
			}
			wcs = 0;
			s = 0;
			i = 0;
			k = t=='c' ? width+1U : 31;
			if (size == SIZE_l) {
				if (alloc) {
					if (isoc23 && k > SIZE_MAX/sizeof(wchar_t)) {
						errno = ENOMEM;
						goto alloc_fail;
					}
					wcs = malloc(k*sizeof(wchar_t));
					if (!wcs) {
						goto alloc_fail;
					}
				} else {
					wcs = dest;
				}
				st = (mbstate_t){0};
				while ((!isoc23 || !width || i < width) && scanset[(c=shgetc(f))+1]) {
					switch (mbrtowc(&wc, &(char){c}, 1, &st)) {
					case -1:
						goto input_fail;
					case -2:
						continue;
					}
					if (wcs) {
						wcs[i++] = wc;
					} else if (isoc23) {
						i++;
					}
					if (alloc && i==k) {
						if (isoc23 && k > (SIZE_MAX/sizeof(wchar_t)-1)/2) {
							errno = ENOMEM;
							goto alloc_fail;
						}
						k+=k+1;
						wchar_t *tmp = realloc(wcs, k*sizeof(wchar_t));
						if (!tmp) {
							goto alloc_fail;
						}
						wcs = tmp;
					}
				}
				if (!mbsinit(&st)) {
					if (isoc23) {
						errno = EILSEQ;
					}
					goto input_fail;
				}
			} else if (alloc) {
				s = malloc(k);
				if (!s) {
					goto alloc_fail;
				}
				while (scanset[(c=shgetc(f))+1]) {
					s[i++] = c;
					if (i==k) {
						if (isoc23 && k > (SIZE_MAX-1)/2) {
							errno = ENOMEM;
							goto alloc_fail;
						}
						k+=k+1;
						char *tmp = realloc(s, k);
						if (!tmp) {
							goto alloc_fail;
						}
						s = tmp;
					}
				}
			} else if ((s = dest)) {
				while (scanset[(c=shgetc(f))+1])
					s[i++] = c;
			} else {
				while (scanset[(c=shgetc(f))+1]);
			}
			if (!isoc23 || size != SIZE_l || !width || i < width) {
				shunget(f);
			}
			if (!shcnt(f)) goto match_fail;
			if (t == 'c' && !isoc23 && shcnt(f) != width) {
				goto match_fail;
			}
			if (alloc) {
				if (size == SIZE_l) *(wchar_t **)dest = wcs;
				else *(char **)dest = s;
			}
			if (t != 'c') {
				if (wcs) wcs[i] = 0;
				if (s) s[i] = 0;
			}
			break;
		case 'p':
			if (isoc23 && (!width || width >= 5)) {
				if (shgetc(f) == '(') {
					for (i=0; i<4; i++) {
						c = shgetc(f);
						if (tolower(c) != "nil)"[i]) {
							shunget(f);
							goto match_fail;
						}
					}
					x = 0;
					goto store_integer;
				}
				shunget(f);
			}
		case 'X':
		case 'x':
			base = 16;
			goto int_common;
		case 'o':
			base = 8;
			goto int_common;
		case 'b':
			/* Preserve legacy handling of length-modified unsupported formats. */
			if (!isoc23) {
				break;
			}
			base = 2;
			goto int_common;
		case 'd':
		case 'u':
			base = 10;
			goto int_common;
		case 'i':
			base = 0;
int_common:
			saved_errno = errno;
			x = isoc23 ? __intscan_c23(f, base, ULLONG_MAX)
				: __intscan(f, base, 0, ULLONG_MAX, 0);
			if (!shcnt(f)) {
				if (isoc23 && errno == EINVAL && !(f->flags & F_ERR)) {
					errno = saved_errno;
				}
				goto match_fail;
			}
store_integer:
			if (t=='p' && dest) *(void **)dest = (void *)(uintptr_t)x;
			else store_int(dest, size, x);
			break;
		case 'a': case 'A':
		case 'e': case 'E':
		case 'f': case 'F':
		case 'g': case 'G':
#if (defined(MUSL_AARCH64_ARCH)) || (defined(MUSL_ARM_ARCH))
			if (!isoc23) {
				if (width == 0 || width > sizeof(buf) - 1)
					width = sizeof(buf) - 1;
				int count = parsefloat(f, buf, buf + width);
				if (count == 0)
					goto match_fail;
				if (dest) switch (size) {
				case SIZE_def:
					y = strtof(buf, &endptr);
					*(float *)dest = y;
					break;
				case SIZE_l:
					y = strtod(buf, &endptr);
					*(double *)dest = y;
					break;
				case SIZE_L:
					y = strtold(buf, &endptr);
					*(long double *)dest = y;
					break;
				}
				break;
			}
#endif
			saved_errno = errno;
			y = isoc23 ? __floatscan_c23(f, size) : __floatscan(f, size, 0);
			if (!shcnt(f)) {
				if (isoc23 && errno == EINVAL && !(f->flags & F_ERR)) {
					errno = saved_errno;
				}
				goto match_fail;
			}
			if (dest) switch (size) {
			case SIZE_def:
				*(float *)dest = y;
				break;
			case SIZE_l:
				*(double *)dest = y;
				break;
			case SIZE_L:
				*(long double *)dest = y;
				break;
			}
			break;
		}
		pos += shcnt(f);
		if (dest) {
			matches++;
		}
	}
	if (0) {
width_fail:
		errno = EINVAL;
		goto match_fail;
fmt_fail:
alloc_fail:
input_fail:
		if (!matches) {
			matches--;
		}
match_fail:
		if (alloc) {
			free(s);
			free(wcs);
		}
	}
	FUNLOCK(f);
	return matches;
}

int vfscanf(FILE *restrict f, const char *restrict fmt, va_list ap)
{
	return vfscanf_internal(f, fmt, ap, 0);
}

int __isoc23_vfscanf(FILE *restrict f, const char *restrict fmt, va_list ap)
{
	return vfscanf_internal(f, fmt, ap, 1);
}

weak_alias(vfscanf, __isoc99_vfscanf);
