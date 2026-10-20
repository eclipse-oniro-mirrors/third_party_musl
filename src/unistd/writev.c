#include <sys/uio.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

ssize_t writev(int fd, const struct iovec *iov, int count)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return -1;
	}
#endif
	return syscall_cp(SYS_writev, fd, iov, count);
}
