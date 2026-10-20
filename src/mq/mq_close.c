#include <mqueue.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

int mq_close(mqd_t mqd)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(mqd, 0) < 0) {
		return -1;
	}
#endif
#ifdef __LITEOS_A__
	return syscall(SYS_mqclose, mqd);
#else
	return syscall(SYS_close, mqd);
#endif
}
