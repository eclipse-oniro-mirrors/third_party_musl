#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

int dup2(int old, int new)
{
	int r;
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(old, 0) < 0) {
		return -1;
	}
	if (old != new && __io_guard_check(new, 0) < 0) {
		return -1;
	}
#endif
#ifdef SYS_dup2
	while ((r=__syscall(SYS_dup2, old, new))==-EBUSY);
#else
	if (old==new) {
		r = __syscall(SYS_fcntl, old, F_GETFD);
		if (r >= 0) return old;
	} else {
		while ((r=__syscall(SYS_dup3, old, new, 0))==-EBUSY);
	}
#endif
	return __syscall_ret(r);
}
