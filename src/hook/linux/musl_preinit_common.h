/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef _MUSL_PREINIT_COMMON_H
#define _MUSL_PREINIT_COMMON_H

#include <stdatomic.h>
#include "musl_malloc_dispatch_table.h"
#include "musl_malloc_dispatch.h"

extern struct musl_libc_globals __musl_libc_globals;

extern struct MallocDispatchType __libc_malloc_default_dispatch;

extern volatile atomic_bool __hook_enable_hook_flag;

extern volatile atomic_bool __memleak_hook_flag;
extern bool checkLoadMallocMemTrack;

enum EnumFunc {
	INITIALIZE_FUNCTION,
	FINALIZE_FUNCTION,
	GET_HOOK_FLAG_FUNCTION,
	SET_HOOK_FLAG_FUNCTION,
	ON_START_FUNCTION,
	ON_END_FUNCTION,
	SEND_HOOK_JS_STACK,
	GET_HOOK_JS_CONFIG,
	LAST_FUNCTION,
};

enum EnumHookMode {
	STARTUP_HOOK_MODE,
	DIRECT_HOOK_MODE,
	STEP_HOOK_MODE,
};

#ifdef HOOK_ENABLE
extern void* function_of_shared_lib[];
extern volatile atomic_llong ohos_malloc_hook_shared_library;
#endif // HOOK_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
inline bool __get_global_hook_flag()
{
#ifdef HOOK_ENABLE
	volatile bool g_flag = atomic_load_explicit(&__hook_enable_hook_flag, memory_order_acquire);
	return g_flag;
#else
	return false;
#endif // HOOK_ENABLE
}

__attribute__((always_inline))
inline bool __get_memleak_hook_flag()
{
#ifdef HOOK_ENABLE
	volatile bool memleak_flag = atomic_load_explicit(&__memleak_hook_flag, memory_order_acquire);
	return memleak_flag;
#else
	return false;
#endif // HOOK_ENABLE
}


__attribute__((always_inline))
inline bool __get_hook_flag()
{
#ifdef HOOK_ENABLE
	volatile void* impl_handle = (void *)atomic_load_explicit(&ohos_malloc_hook_shared_library, memory_order_acquire);
	if (impl_handle == NULL) {
		return false;
	}
	else if (impl_handle == (void *)-1) {
		return true;
	}
	else {
		GetHookFlagType get_hook_func_ptr = (GetHookFlagType)(function_of_shared_lib[GET_HOOK_FLAG_FUNCTION]);
		bool flag = get_hook_func_ptr();
		return flag;
	}
#else
	return false;
#endif // HOOK_ENABLE
}

__attribute__((always_inline))
inline bool __set_hook_flag(bool flag)
{
#ifdef HOOK_ENABLE
	volatile void* impl_handle = (void *)atomic_load_explicit(&ohos_malloc_hook_shared_library, memory_order_acquire);
	if (impl_handle == NULL) {
		return false;
	}
	else if (impl_handle == (void *)-1) {
		return true;
	}
	else {
		SetHookFlagType set_hook_func_ptr = (SetHookFlagType)(function_of_shared_lib[SET_HOOK_FLAG_FUNCTION]);
		return set_hook_func_ptr(flag);
	}
#else
	return false;
#endif // HOOK_ENABLE
}

__attribute__((always_inline))
inline volatile const struct MallocDispatchType* get_current_dispatch_table()
{
#ifdef HOOK_ENABLE
	volatile const struct MallocDispatchType* ret = (struct MallocDispatchType *)atomic_load_explicit(&__musl_libc_globals.current_dispatch_table, memory_order_acquire);
	if (ret != NULL) {
		if (__get_memleak_hook_flag()) {
			return ret;
		}
		if (!__get_global_hook_flag()) {
			ret = NULL;
		}
		else if (!__get_hook_flag()) {
			ret = NULL;
		}
	}
	return ret;
#else
	return NULL;
#endif // HOOK_ENABLE
}

__attribute__((always_inline))
inline bool __send_hook_js_rawstack(uint64_t id, const char* jsStackptr, size_t jsStackSize)
{
#ifdef HOOK_ENABLE
	volatile void* impl_handle = (void*)atomic_load_explicit(&ohos_malloc_hook_shared_library, memory_order_acquire);
	if (impl_handle == NULL) {
		return false;
	}
	else if (impl_handle == (void*)-1) {
		return false;
	}
	else {
		SendHookJsStack send_hook_func_ptr = (SendHookJsStack)(function_of_shared_lib[SEND_HOOK_JS_STACK]);
		return send_hook_func_ptr(id, jsStackptr, jsStackSize);
	}
#else
	return false;
#endif // HOOK_ENABLE
}

__attribute__((always_inline))
inline void* __get_hook_js_config()
{
#ifdef HOOK_ENABLE
	volatile void* impl_handle = (void*)atomic_load_explicit(&ohos_malloc_hook_shared_library, memory_order_acquire);
	if (impl_handle == NULL) {
		return NULL;
	}
	else if (impl_handle == (void*)-1) {
		return NULL;
	}
	else {
		GetHookJsConfig get_hook_func_ptr = (GetHookJsConfig)(function_of_shared_lib[GET_HOOK_JS_CONFIG]);
		return get_hook_func_ptr();
	}
#else
	return NULL;
#endif // HOOK_ENABLE
}

#define MUSL_HOOK_PARAM_NAME "libc.hook_mode"
#define OHOS_PARAM_MAX_SIZE 96
#define FILE_NAME_MAX_SIZE 40

#ifdef __cplusplus
}
#endif

#endif