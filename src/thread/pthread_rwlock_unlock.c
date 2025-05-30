#include "pthread_impl.h"

// Set a rw lock to init status: PTHREAD_RWLOCK_INITIALIZER
void __pthread__rwlock_unlock_inner(pthread_rwlock_t *m)
{
	char *p = (char *)m;
	for (size_t i = 0;i < sizeof(pthread_rwlock_t);i++) {
		*(p + i) = 0;
	}
}

#ifdef ENABLE_HWASAN
__attribute__((no_sanitize("hwaddress")))
#endif
int __pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
	int val, cnt, waiters, new, priv = rw->_rw_shared^128;

	do {
		val = rw->_rw_lock;
		cnt = val & 0x7fffffff;
		waiters = rw->_rw_waiters;
		new = (cnt == 0x7fffffff || cnt == 1) ? 0 : val-1;
	} while (a_cas(&rw->_rw_lock, val, new) != val);

	if (!new && (waiters || val<0))
		__wake(&rw->_rw_lock, cnt, priv);

	return 0;
}

weak_alias(__pthread_rwlock_unlock, pthread_rwlock_unlock);
