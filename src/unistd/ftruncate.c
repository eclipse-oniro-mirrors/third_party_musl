#include <unistd.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

int ftruncate(int fd, off_t length)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return -1;
	}
#endif
	return syscall(SYS_ftruncate, fd, __SYSCALL_LL_O(length));
}

weak_alias(ftruncate, ftruncate64);
