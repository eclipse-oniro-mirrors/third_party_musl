#include "stdio_impl.h"
#include <wchar.h>

/* Numeric conversions accept ASCII input. Map other wide characters to a
 * non-numeric sentinel, without requiring them to be encodable in LC_CTYPE. */
static size_t c23_wstring_read(FILE *f, unsigned char *buf, size_t len)
{
	const wchar_t *src = f->cookie;
	size_t k;

	for (k=0; k<f->buf_size && src[k]; k++) {
		f->buf[k] = (unsigned)src[k] < 128 ? src[k] : '@';
	}
	f->rpos = f->buf;
	f->rend = f->buf + k;
	f->cookie = (void *)(src+k);

	if (!len || !k) {
		return 0;
	}

	*buf = *f->rpos++;
	return 1;
}

int __vswscanf(const wchar_t *restrict s, const wchar_t *restrict fmt, va_list ap, int isoc23)
{
	if (!isoc23) {
		return vswscanf(s, fmt, ap);
	}
	unsigned char buf[UNGET+256];
	FILE f = {
		.buf = buf+UNGET, .buf_size = sizeof buf-UNGET,
		.read = c23_wstring_read, .lock = -1
	};
	return __vfwscanf(&f, fmt, ap, 1, s);
}

static size_t wstring_read(FILE *f, unsigned char *buf, size_t len)
{
	const wchar_t *src = f->cookie;
	size_t k;

	if (!src) return 0;

	k = wcsrtombs((void *)f->buf, &src, f->buf_size, 0);
	if (k==(size_t)-1) {
		f->rpos = f->rend = 0;
		return 0;
	}

	f->rpos = f->buf;
	f->rend = f->buf + k;
	f->cookie = (void *)src;

	if (!len || !k) return 0;

	*buf = *f->rpos++;
	return 1;
}

int vswscanf(const wchar_t *restrict s, const wchar_t *restrict fmt, va_list ap)
{
	unsigned char buf[256];
	FILE f = {
		.buf = buf, .buf_size = sizeof buf,
		.cookie = (void *)s,
		.read = wstring_read, .lock = -1
	};
	return vfwscanf(&f, fmt, ap);
}

weak_alias(vswscanf,__isoc99_vswscanf);
