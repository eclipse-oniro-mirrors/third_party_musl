#ifndef HOOK_ENABLE
#include <stdlib.h>
#ifdef USE_JEMALLOC
extern void* oh_realloc(void* p, size_t newsize);
#endif

void *realloc(void *p, size_t n)
{
#ifdef USE_JEMALLOC
	return oh_realloc(p, n);
#endif
	return __libc_realloc(p, n);
}
#endif
