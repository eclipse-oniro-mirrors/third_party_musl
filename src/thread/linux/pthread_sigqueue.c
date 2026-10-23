/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _GNU_SOURCE
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "syscall.h"
#include "pthread_impl.h"
#include "lock.h"

int pthread_sigqueue(pthread_t t, int sig, const union sigval value)
{
#ifdef SYS_rt_tgsigqueueinfo
	siginfo_t si = {0};
	sigset_t set;
	int r;

	if (!t->tid) {
		return ESRCH;
	}

	if (sig == SIGCANCEL || sig == SIGTIMER || sig == SIGSYNCCALL) {
		return EINVAL;
	}

	si.si_signo = sig;
	si.si_code = SI_QUEUE;
	si.si_pid = getpid();
	si.si_uid = getuid();
	si.si_value = value;

	__block_app_sigs(&set);
	LOCK(t->killlock);
	r = -__syscall(SYS_rt_tgsigqueueinfo, getpid(), t->tid, sig, &si);
	UNLOCK(t->killlock);
	__restore_sigs(&set);
	return r;
#else
	(void)t;
	(void)sig;
	(void)value;
	return ENOSYS;
#endif
}
