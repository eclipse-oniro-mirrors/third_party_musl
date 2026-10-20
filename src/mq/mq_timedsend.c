#include <mqueue.h>
#include <errno.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

#define IS32BIT(x) !((x)+0x80000000ULL>>32)
#define CLAMP(x) (int)(IS32BIT(x) ? (x) : 0x7fffffffU+((0ULL+(x))>>63))

int mq_timedsend(mqd_t mqd, const char *msg, size_t len, unsigned prio, const struct timespec *at)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(mqd, 0) < 0) {
		return -1;
	}
#endif
#ifdef SYS_mq_timedsend_time64
	time_t s = at ? at->tv_sec : 0;
	long ns = at ? at->tv_nsec : 0;
	long r = -ENOSYS;
	if (SYS_mq_timedsend == SYS_mq_timedsend_time64 || !IS32BIT(s))
		r = __syscall_cp(SYS_mq_timedsend_time64, mqd, msg, len, prio,
			at ? ((long long []){at->tv_sec, at->tv_nsec}) : 0);
	if (SYS_mq_timedsend == SYS_mq_timedsend_time64 || r != -ENOSYS)
		return __syscall_ret(r);
	return syscall_cp(SYS_mq_timedsend, mqd, msg, len, prio,
		at ? ((long[]){CLAMP(s), ns}) : 0);
#else
	return syscall_cp(SYS_mq_timedsend, mqd, msg, len, prio, at);
#endif
}
