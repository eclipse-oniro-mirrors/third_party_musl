#include <unistd.h>
#include <errno.h>
#include "aio_impl.h"
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif
#ifdef OHOS_FDTRACK_HOOK_ENABLE
#include "memory_trace.h"
#endif

static int dummy(int fd)
{
	return fd;
}

weak_alias(dummy, __aio_close);

int __close(int fd)
{
	/* Validation only: the caller releases its token through exchange. */
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return -1;
	}
#endif
	// Hook fd close
#ifdef OHOS_FDTRACK_HOOK_ENABLE
	restrace(RES_FD_MASK, fd, FD_SIZE, TAG_RES_FD_ALL, false);
#endif
	fd = __aio_close(fd);
	int r = __syscall_cp(SYS_close, fd);
	if (r == -EINTR) r = 0;
	return __syscall_ret(r);
}

weak_alias(__close, close);
