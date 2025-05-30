#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/features.h>
#include <sys/types.h>

#include <time.h>
#include <fcntl.h>
#include "los_sem_pri.h"

#define SEM_FAILED ((sem_t *)0)

typedef struct posix_sem {
  LosSemCB* sem;
} sem_t;

int    sem_close(sem_t *);
int    sem_destroy(sem_t *);
int    sem_getvalue(sem_t *__restrict, int *__restrict);
int    sem_init(sem_t *, int, unsigned);
sem_t *sem_open(const char *, int, ...);
int    sem_post(sem_t *);
int    sem_timedwait(sem_t *__restrict, const struct timespec *__restrict);
int    sem_trywait(sem_t *);
int    sem_unlink(const char *);
int    sem_wait(sem_t *);

#if _REDIR_TIME64
__REDIR(sem_timedwait, __sem_timedwait_time64);
#endif

#ifdef __cplusplus
}
#endif
#endif
