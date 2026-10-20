#define _BSD_SOURCE
#include <sys/uio.h>
#include <unistd.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

ssize_t pwritev(int fd, const struct iovec *iov, int count, off_t ofs)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return -1;
	}
#endif
	return syscall_cp(SYS_pwritev, fd, iov, count,
		(long)(ofs), (long)(ofs>>32));
}

weak_alias(pwritev, pwritev64);
