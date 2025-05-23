#include "pthread_impl.h"

#ifdef ENABLE_HWASAN
__attribute__((no_sanitize("hwaddress")))
#endif
void __wait(volatile int *addr, volatile int *waiters, int val, int priv)
{
	int spins=100;
	if (priv) priv = FUTEX_PRIVATE;
	while (spins-- && (!waiters || !*waiters)) {
		if (*addr==val) a_spin();
		else return;
	}
	if (waiters) a_inc(waiters);
	while (*addr==val) {
#ifdef __LITEOS_A__
		__syscall(SYS_futex, addr, FUTEX_WAIT|priv, val, 0xffffffffu) != -ENOSYS
		|| __syscall(SYS_futex, addr, FUTEX_WAIT, val, 0xffffffffu);
#else
		__syscall(SYS_futex, addr, FUTEX_WAIT|priv, val, 0) != -ENOSYS
		|| __syscall(SYS_futex, addr, FUTEX_WAIT, val, 0);
#endif
	}
	if (waiters) a_dec(waiters);
}
