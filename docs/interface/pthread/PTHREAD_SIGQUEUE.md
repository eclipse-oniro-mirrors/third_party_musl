## PTHREAD_SIGQUEUE &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       pthread_sigqueue - queue a signal with data to a thread.

#### **SYNOPSIS**

```c
#include <signal.h>

int pthread_sigqueue(pthread_t t, int sig, const union sigval value);
```

#### **DESCRIPTION**

​The pthread_sigqueue() function sends the signal sig to the thread identified by t, accompanied by the data carried in value.

The function constructs a siginfo_t structure with si_code set to SI_QUEUE, si_pid set to the caller's process ID, si_uid set to the caller's user ID, and si_value set to the supplied value. This structure is passed to the kernel via the rt_tgsigqueueinfo system call, which delivers the signal to the specific thread identified by its thread ID (tid) within the calling process.

Access to the target thread's tid is protected by the thread's internal killlock to ensure that the tid is valid and stable during the system call. Additionally, application-level signals are temporarily blocked during the operation to prevent interference from signal handlers that might alter thread state.

The following signals are reserved for internal C library use and must not be sent via this interface:

* SIGCANCEL - used for thread cancellation
* SIGTIMER - used for timer delivery
* SIGSYNCCALL - used for synchronized cross-thread calls

Attempting to send any of these reserved signals results in an EINVAL error.

If the target thread's tid is zero (indicating the thread has already exited or was never fully initialized), the function returns ESRCH without making a system call.

This interface is available when the target architecture provides the SYS_rt_tgsigqueueinfo system call. When this system call is not available, the function returns ENOSYS.

#### **RETURN VALUE**

​On success, pthread_sigqueue() returns 0.

On failure, it returns a positive errno value to indicate the error. It does not set errno.

#### **ERRORS**

**EINVAL**: sig is one of the reserved internal signals (SIGCANCEL, SIGTIMER, or SIGSYNCCALL), or sig is not a valid signal number.

**ESRCH**: The target thread t has a tid of zero, indicating it has exited or was not initialized.

**EPERM**: The caller does not have permission to send the signal to the target thread.

**ENOSYS**: The SYS_rt_tgsigqueueinfo system call is not available on the target architecture.

Other errno values may be returned by the kernel's rt_tgsigqueueinfo system call to indicate delivery failures.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| pthread_sigqueue() | Thread safety | MT-Safe        |
|             | Signal safety | Not Safe       |

The function acquires the target thread's killlock to safely access the tid, and temporarily blocks application signals during the operation.

#### EXAMPLES

```c
#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static void handler(int sig, siginfo_t *si, void *ctx)
{
    if (si->si_code == SI_QUEUE) {
        printf("received queued signal, value: %d\n",
               si->si_value.sival_int);
    }
}

static void *worker(void *arg)
{
    sleep(2);
    return NULL;
}

int main(void)
{
    struct sigaction sa = {0};
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    pthread_t t;
    pthread_create(&t, NULL, worker, NULL);

    union sigval val;
    val.sival_int = 42;
    int rv = pthread_sigqueue(t, SIGUSR1, val);
    if (rv != 0) {
        printf("pthread_sigqueue failed: %d\n", rv);
    }

    pthread_join(t, NULL);
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
