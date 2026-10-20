#include <unistd.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

ssize_t pwrite(int fd, const void *buf, size_t size, off_t ofs)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return -1;
	}
#endif
	return syscall_cp(SYS_pwrite, fd, buf, size, __SYSCALL_LL_PRW(ofs));
}

weak_alias(pwrite, pwrite64);
