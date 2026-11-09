#include <unistd.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif
#ifdef OHOS_FDTRACK_HOOK_ENABLE
#include "memory_trace.h"
#include "musl_fdtrack_hook.h"
#endif

int dup(int fd)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return -1;
	}
#endif
#ifdef OHOS_FDTRACK_HOOK_ENABLE
	int fd_new = syscall(SYS_dup, fd);
	int res = FDTRACK_START_HOOK(fd_new);
	restraceFd(RES_FD_DUP, fd_new, TAG_RES_FD_DUP, true);
	restrace(RES_DMABUF_MASK, res, FD_SIZE, TAG_RES_DMABUF_MASK, true);
	return res;
#endif
	return syscall(SYS_dup, fd);
}
