#include "pthread_impl.h"
#ifdef USE_MUTEX_WAIT_OPT
#include "musl_opt.h"
#endif

#define IS32BIT(x) !((x)+0x80000000ULL>>32)
#define CLAMP(x) (int)(IS32BIT(x) ? (x) : 0x7fffffffU+((0ULL+(x))>>63))

static int __futex4(volatile void *addr, int op, int val, const struct timespec *to)
{
#ifdef SYS_futex_time64
	time_t s = to ? to->tv_sec : 0;
	long ns = to ? to->tv_nsec : 0;
	int r = -ENOSYS;
	if (SYS_futex == SYS_futex_time64 || !IS32BIT(s))
		r = __syscall(SYS_futex_time64, addr, op, val,
			to ? ((long long[]){s, ns}) : 0);
	if (SYS_futex == SYS_futex_time64 || r!=-ENOSYS) return r;
	to = to ? (void *)(long[]){CLAMP(s), ns} : 0;
#endif
	return __syscall(SYS_futex, addr, op, val, to);
}

static int pthread_mutex_timedlock_pi(pthread_mutex_t *restrict m, const struct timespec *restrict at)
{
	int clock = m->_m_clock;
	int type = m->_m_type;
	int priv = (type & 128) ^ 128;
	pthread_t self = __pthread_self();
	int e;

	if (!priv) self->robust_list.pending = &m->_m_next;

	do e = -__futex4(&m->_m_lock, FUTEX_LOCK_PI|priv, 0, at);
	while (e==EINTR);
	if (e) self->robust_list.pending = 0;

	switch (e) {
	case 0:
		/* Catch spurious success for non-robust mutexes. */
		if (!(type&4) && ((m->_m_lock & 0x40000000) || m->_m_waiters)) {
			a_store(&m->_m_waiters, -1);
			__syscall(SYS_futex, &m->_m_lock, FUTEX_UNLOCK_PI|priv);
			self->robust_list.pending = 0;
			break;
		}
		/* Signal to trylock that we already have the lock. */
		m->_m_count = -1;
		return __pthread_mutex_trylock_owner(m);
	case ETIMEDOUT:
		return e;
	case EDEADLK:
		if ((type&3) == PTHREAD_MUTEX_ERRORCHECK) return e;
	}
	do e = __timedwait(&(int){0}, 0, clock, at, 1);
	while (e != ETIMEDOUT);
	return e;
}

int __pthread_mutex_timedlock_inner(pthread_mutex_t *restrict m, const struct timespec *restrict at)
{
	int type = m->_m_type;
	int r;
	// PI
#ifndef __LITEOS_A__
	if (type & 8) {
		r = __pthread_mutex_trylock_owner(m);
		if (r != EBUSY) return r;
		return pthread_mutex_timedlock_pi(m, at);
	}
#endif
	r = __pthread_mutex_trylock(m);
	if (r != EBUSY) return r;
	int clock = (m->_m_clock == CLOCK_MONOTONIC) ? CLOCK_MONOTONIC : CLOCK_REALTIME;
	int t, priv = (type & 128) ^ 128;

	/* Marco: Optimazation for mutex lock wait. 
	   When enabled, call function implemented in libkccl_lockopt.so.
	   Otherwise, use the default spin.
	*/
#ifdef USE_MUTEX_WAIT_OPT
	lock_func(m);
#else
	int spins = 100;
	while (spins-- && m->_m_lock && !m->_m_waiters) a_spin();
#endif

	while ((r=__pthread_mutex_trylock(m)) == EBUSY) {
		r = m->_m_lock;
		int own = r & 0x3fffffff;
		if (!own && (!r || (type&4)))
			continue;
		if ((type&3) == PTHREAD_MUTEX_ERRORCHECK
		    && own == __pthread_self()->tid)
			return EDEADLK;

		a_inc(&m->_m_waiters);
		t = r | 0x80000000;
		a_cas(&m->_m_lock, r, t);
		r = __timedwait(&m->_m_lock, t, clock, at, priv);
		a_dec(&m->_m_waiters);
		if (r && r != EINTR) break;
	}
	return r;
}

int __pthread_mutex_timedlock(pthread_mutex_t *restrict m, const struct timespec *restrict at)
{
	if ((m->_m_type&15) == PTHREAD_MUTEX_NORMAL
		&& !a_cas(&m->_m_lock, 0, EBUSY))
		return 0;
	return __pthread_mutex_timedlock_inner(m, at);
}

weak_alias(__pthread_mutex_timedlock, pthread_mutex_timedlock);
