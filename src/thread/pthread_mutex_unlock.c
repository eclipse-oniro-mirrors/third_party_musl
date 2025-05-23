#include "pthread_impl.h"

// Set a recursive lock to init status, memory is "01 00 ... 00"
void __pthread_mutex_unlock_recursive_inner(pthread_mutex_t *m)
{
	char *p = (char *)m;
	for (size_t i = 0;i < sizeof(pthread_mutex_t);i++) {
		*(p + i) = 0;
	}
	m->_m_type = PTHREAD_MUTEX_RECURSIVE; // Set recursive flag
}

int __pthread_mutex_unlock(pthread_mutex_t *m)
{
	pthread_t self;
	int waiters = m->_m_waiters;
	int cont;
	int type = m->_m_type & 15;
	int priv = (m->_m_type & 128) ^ 128;
	int new = 0;
	int old;

	if (type != PTHREAD_MUTEX_NORMAL) {
		self = __pthread_self();
		old = m->_m_lock;
		int own = old & 0x3fffffff;
		if (own != self->tid)
			return EPERM;
#ifdef __LITEOS_A__
		if ((type & PTHREAD_MUTEX_TYPE_MASK) == PTHREAD_MUTEX_RECURSIVE && m->_m_count)
#else
		if (__is_mutex_destroyed(m->_m_type))
			__handle_using_destroyed_mutex(__FUNCTION__);
		if ((type&3) == PTHREAD_MUTEX_RECURSIVE && m->_m_count)
#endif
			return m->_m_count--, 0;
		if ((type&4) && (old&0x40000000))
			new = 0x7fffffff;
		if (!priv) {
			self->robust_list.pending = &m->_m_next;
			__vm_lock();
		}
		volatile void *prev = m->_m_prev;
		volatile void *next = m->_m_next;
		*(volatile void *volatile *)prev = next;
		if (next != &self->robust_list.head) *(volatile void *volatile *)
			((char *)next - sizeof(void *)) = prev;
	}
#ifdef __LITEOS_A__
	cont = a_swap(&m->_m_lock, new);
#else
	if (type&8) {
		if (old<0 || a_cas(&m->_m_lock, old, new)!=old) {
			if (new) a_store(&m->_m_waiters, -1);
			__syscall(SYS_futex, &m->_m_lock, FUTEX_UNLOCK_PI|priv);
		}
		cont = 0;
		waiters = 0;
	} else {
		cont = a_swap(&m->_m_lock, new);
	}
#endif

#ifdef USE_MUTEX_WAIT_OPT
	__asm__ volatile ("sev" : : : "memory");
#endif

	if (type != PTHREAD_MUTEX_NORMAL && !priv) {
		self->robust_list.pending = 0;
		__vm_unlock();
	}
	if (waiters || cont<0)
		__wake(&m->_m_lock, 1, priv);
	return 0;
}

weak_alias(__pthread_mutex_unlock, pthread_mutex_unlock);
