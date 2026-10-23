#include "pthread_impl.h"
#include "fork_impl.h"
#include "pthread_ffrt.h"
#include <stdbool.h>

#if defined(OHOS_ENABLE_PARAMETER) || defined(ENABLE_MUSL_LOG)
#include <execinfo.h>
#include <stdlib.h>
#endif

#ifdef OHOS_ENABLE_PTHREAD_KEY_HISYSEVENT
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <unistd.h>
#include "hisysevent_easy.h"
#include "vsnprintf_s_p.h"
#endif

volatile size_t __pthread_tsd_size = sizeof(void *) * PTHREAD_KEYS_MAX;
void *__pthread_tsd_main[PTHREAD_KEYS_MAX] = { 0 };

static void (*keys[PTHREAD_KEYS_MAX])(void *);

static pthread_rwlock_t key_lock = PTHREAD_RWLOCK_INITIALIZER;

static pthread_key_t next_key;

#ifdef OHOS_ENABLE_PTHREAD_KEY_HISYSEVENT
#define KEY_REPORT_LEVEL_COUNT 4
#define KEY_REPORT_SAMPLE_RATE 10U

static const unsigned int key_report_levels[KEY_REPORT_LEVEL_COUNT] = {100, 110, 120, PTHREAD_KEYS_MAX};
static unsigned int key_count;
static bool key_report_attempted[KEY_REPORT_LEVEL_COUNT];

static int reserve_key_report(unsigned int count)
{
	for (int level = 0; level < KEY_REPORT_LEVEL_COUNT; ++level) {
		if (count == key_report_levels[level] && !key_report_attempted[level]) {
			key_report_attempted[level] = true;
			return level;
		}
	}
	return -1;
}

static void read_process_name(char *name, size_t size)
{
	name[0] = 0;
	int fd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
	if (fd < 0) return;

	size_t pos = 0;
	while (pos < size - 1) {
		ssize_t len = read(fd, name + pos, size - pos - 1);
		if (len < 0) {
			if (errno == EINTR) continue;
			name[0] = 0;
			break;
		}
		if (!len) break;
		bool complete = memchr(name + pos, 0, len) != NULL;
		pos += len;
		if (complete) break;
	}
	name[pos] = 0;
	close(fd);

	char *app = strrchr(name, '/');
	if (app == NULL) {
		app = name;
	} else {
		++app;
	}
	char *app_end = strchr(app, ':');
	if (app_end) *app_end = 0;
}

static int format_key_event_data(char *data, size_t size, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vsnprintfp_s(data, size, size - 1, false, format, args);
	va_end(args);
	return ret;
}

static bool should_report_key_usage(int level)
{
	unsigned int random_value;
	ssize_t len = getrandom(&random_value, sizeof random_value, GRND_NONBLOCK);
	if (len != (ssize_t)sizeof random_value) {
		unsigned int seed = (unsigned int)__pthread_self()->pid
			^ ((unsigned int)(level + 1) * 0x9e3779b9U);
		random_value = (unsigned int)rand_r(&seed);
	}
	return random_value % KEY_REPORT_SAMPLE_RATE == 0;
}

static void report_key_usage(int level)
{
	int saved_errno = errno;
	if (!should_report_key_usage(level)) {
		errno = saved_errno;
		return;
	}
	static char process_name[KEY_REPORT_LEVEL_COUNT][256];
	static char data[KEY_REPORT_LEVEL_COUNT][320];
	read_process_name(process_name[level], sizeof process_name[level]);
	int ret = format_key_event_data(data[level], sizeof data[level],
		"process=%.255s,keys_max=%d,keys_used=%u",
		process_name[level][0] ? process_name[level] : "unknown", PTHREAD_KEYS_MAX, key_report_levels[level]);
	if (ret > 0 && (size_t)ret < sizeof data[level]) {
		(void)HiSysEventEasyWrite("MUSL", "PTHREAD_KEY_EXHAUSTED", EASY_EVENT_TYPE_STATISTIC, data[level]);
	}
	errno = saved_errno;
}
#endif

#if defined(OHOS_ENABLE_PARAMETER) || defined(ENABLE_MUSL_LOG)
#define BT_BUF_SIZE 15

static void log_key_backtrace(const char *operation,
	pthread_key_t key, int result)
{
	int saved_errno = errno;
	if (!HiLogAdapterIsLoggable(MUSL_LOG_DOMAIN, MUSL_LOG_TAG, LOG_DEBUG)) {
		errno = saved_errno;
		return;
	}

	void *buffer[BT_BUF_SIZE];
	size_t nptrs = backtrace(buffer, BT_BUF_SIZE);
	if (nptrs > BT_BUF_SIZE) nptrs = 0;
	char **strings = nptrs ? backtrace_symbols(buffer, nptrs) : NULL;
	MUSL_LOGD("musl: %{public}s key=%{public}u ret=%{public}d frames=%{public}zu",
		operation, (unsigned int)key, result, nptrs);
	for (size_t idx = 0; idx < nptrs; idx++) {
		MUSL_LOGD("musl: %{public}s pc=%{public}p %{public}s", operation,
			buffer[idx], strings ? strings[idx] : "(symbol unavailable)");
	}
	free(strings);
	errno = saved_errno;
}
#else
#define log_key_backtrace(operation, key, result) ((void)0)
#endif

static void nodtor(void *dummy)
{
}

static void dummy_0(void)
{
}

weak_alias(dummy_0, __tl_lock);
weak_alias(dummy_0, __tl_unlock);

static struct call_tl_lock *dummy_get_tl_lock_caller_count(void)
{
	return NULL;
}

weak_alias(dummy_get_tl_lock_caller_count, get_tl_lock_caller_count);

void __pthread_key_atfork(int who)
{
	if (who<0) __pthread_rwlock_rdlock(&key_lock);
	else if (!who) __pthread_rwlock_unlock(&key_lock);
	else key_lock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
}

int __pthread_key_create(pthread_key_t *k, void (*dtor)(void *))
{
	pthread_t self = __pthread_self();

	/* This can only happen in the main thread before
	 * pthread_create has been called. */
	if (!self->tsd) self->tsd = __pthread_tsd_main;

	/* Purely a sentinel value since null means slot is free. */
	if (!dtor) dtor = nodtor;

	__pthread_rwlock_wrlock(&key_lock);
	pthread_key_t j = next_key;
	do {
		if (!keys[j]) {
			keys[next_key = *k = j] = dtor;
#ifdef OHOS_ENABLE_PTHREAD_KEY_HISYSEVENT
			++key_count;
			int report_level = key_count < PTHREAD_KEYS_MAX ? reserve_key_report(key_count) : -1;
#endif
			__pthread_rwlock_unlock(&key_lock);
#ifdef OHOS_ENABLE_PTHREAD_KEY_HISYSEVENT
			if (report_level >= 0) report_key_usage(report_level);
#endif
			log_key_backtrace("pthread_key_create", j, 0);
			return 0;
		}
	} while ((j=(j+1)%PTHREAD_KEYS_MAX) != next_key);

#ifdef OHOS_ENABLE_PTHREAD_KEY_HISYSEVENT
	int report_level = reserve_key_report(key_count);
#endif
	__pthread_rwlock_unlock(&key_lock);
#ifdef OHOS_ENABLE_PTHREAD_KEY_HISYSEVENT
	if (report_level >= 0) report_key_usage(report_level);
#endif
	log_key_backtrace("pthread_key_create", PTHREAD_KEYS_MAX, EAGAIN);
	return EAGAIN;
}

int __pthread_key_delete(pthread_key_t k)
{
	sigset_t set;
	pthread_t self = __pthread_self(), td=self;

	__block_app_sigs(&set);
	__pthread_rwlock_wrlock(&key_lock);

	__tl_lock();
	if (get_tl_lock_caller_count()) {
		get_tl_lock_caller_count()->__pthread_key_delete_tl_lock++;
	}
	do td->tsd[k] = 0;
	while ((td=td->next)!=self);
	if (get_tl_lock_caller_count()) {
		get_tl_lock_caller_count()->__pthread_key_delete_tl_lock--;
	}
	__tl_unlock();

#ifdef OHOS_ENABLE_PTHREAD_KEY_HISYSEVENT
	if (keys[k]) --key_count;
#endif
	keys[k] = 0;

	__pthread_rwlock_unlock(&key_lock);
	__restore_sigs(&set);

	log_key_backtrace("pthread_key_delete", k, 0);
	return 0;
}

void __pthread_tsd_run_dtors()
{
	pthread_t self = __pthread_self();
	int i, j;
	for (j=0; self->tsd_used && j<PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
		__pthread_rwlock_rdlock(&key_lock);
		self->tsd_used = 0;
		for (i=0; i<PTHREAD_KEYS_MAX; i++) {
			void *val = self->tsd[i];
			void (*dtor)(void *) = keys[i];
			self->tsd[i] = 0;
			if (val && dtor && dtor != nodtor) {
				__pthread_rwlock_unlock(&key_lock);
				dtor(val);
				__pthread_rwlock_rdlock(&key_lock);
			}
		}
		__pthread_rwlock_unlock(&key_lock);
	}
}

void __pthread_tsd_run_dtors_ex1()
{
	pthread_t self = __pthread_self();
	int i, j;
	bool tsd_used = true;
	for (j=0; tsd_used && j<PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
		__pthread_rwlock_rdlock(&key_lock);
		tsd_used = false;
		for (i=0; i<PTHREAD_KEYS_MAX; i++) {
			void *val = self->tsd[i];
			void (*dtor)(void *) = keys[i];
			self->tsd[i] = 0;
			if (val && dtor && dtor != nodtor) {
				__pthread_rwlock_unlock(&key_lock);
				dtor(val);
				tsd_used = true;
				__pthread_rwlock_rdlock(&key_lock);
			}
		}
		__pthread_rwlock_unlock(&key_lock);
	}
}

weak_alias(__pthread_key_create, pthread_key_create);
weak_alias(__pthread_key_delete, pthread_key_delete);
weak_alias(__pthread_tsd_run_dtors_ex1, pthread_tsd_run_dtors);
