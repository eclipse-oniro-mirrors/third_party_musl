#ifndef HOOK_ENABLE
#include <stdlib.h>

#ifdef USE_JEMALLOC
extern void oh_free(void* p);
#endif

void free(void *p)
{
#ifdef USE_JEMALLOC
	return oh_free(p);
#endif
	__libc_free(p);
}
#endif
