#include <sys/sendfile.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

ssize_t sendfile(int out_fd, int in_fd, off_t *ofs, size_t count)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(out_fd, 0) < 0) {
		return -1;
	}
#endif
	return syscall(SYS_sendfile, out_fd, in_fd, ofs, count);
}

weak_alias(sendfile, sendfile64);
