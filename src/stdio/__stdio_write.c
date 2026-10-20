#include "stdio_impl.h"
#include <sys/uio.h>
#include <string.h>
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

/* Shared by buffered flushes and direct stdio output. Keep non-cancelling I/O. */
static ssize_t write_fd(int fd, const void *buf, size_t len)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return -1;
	}
#endif
	return syscall(SYS_write, fd, buf, len);
}

ssize_t __flush_buffer(FILE *f)
{
	ssize_t cnt = 0;
	char *wbase = (char *)f->wbase;
	size_t rem = f->wpos - f->wbase;
	while (rem > 0) {
		cnt = write_fd(f->fd, wbase, rem);
		if (cnt < 0) {
			f->wpos = f->wbase = f->wend = 0;
			f->flags |= F_ERR;
			return cnt;
		}
		wbase += cnt;
		rem -= cnt;
	}

	/* reset file buffer */
	f->wend = f->buf + f->buf_size;
	f->wpos = f->wbase = f->buf;
	return cnt;
}

size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len)
{
	size_t rem = len;
	unsigned char *wbuf = (unsigned char *)buf;

	/* flush buffer first */
	ssize_t cnt = __flush_buffer(f);
	if (cnt < 0) {
		return 0;
	}

	for (;;) {
		if (f->lbf < 0 && rem <= f->wend - f->wpos) {
			memcpy(f->wpos, wbuf, rem);
			f->wpos += rem;
			return len;
		}

		/* write directly if
		 * 1. file buffer < rem
		 * 2. line buffer mode
		 */
		cnt = write_fd(f->fd, wbuf, rem);
		if (cnt < 0) {
			f->wpos = f->wbase = f->wend = 0;
			f->flags |= F_ERR;
			return len - rem;
		}

		rem -= cnt;
		wbuf += cnt;
		if (rem == 0) {
			break;
		}
	}

	return len;
}
