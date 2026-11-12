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
 
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <errno.h>
#include <syscall.h>
#include "libc.h"
#ifdef MUSL_EXTERNAL_FUNCTION
#include "musl_log.h"
#endif

#define HM_ACCESS_TOKEN_ID_IOCTL_BASE 'A'

enum {
	HM_GET_TOKEN_ID = 1,
	HM_SET_TOKEN_ID,
	HM_GET_FTOKEN_ID,
	HM_SET_FTOKEN_ID,
	HM_ADD_PERMISSION,
	HM_REMOVE_PERMISSION,
	HM_GET_PERMISSION,
	HM_SET_PERMISSION,
	HM_GET_HAP_PTOKENID,
	HM_GET_FAMILY_TOKENIDS,
	HM_GET_ALL_PERMISSIONS,
	HM_SET_USERID,
	HM_GET_USERID,
	HM_SET_SA_TOKEN_ID,
	HM_GET_USERINFO,
	HM_ADD_SPM_ENTRIES,
	HM_SET_SPM_ENTRIES,
	HM_GET_SPM_ENTRY,
	HM_REMOVE_SPM_ENTRY,
	HM_SET_REFCNT_UID,
	HM_GET_REFCNT_UID,
	HM_SET_REFCNT_TOKENID,
	HM_GET_REFCNT_TOKENID,
	HM_SPM_CLEAN_REFCNT,
	HM_SPM_GET_VERSION,
	HM_SET_HAP_PTOKENID,
	HM_SET_BIN_DECLARED_PERM,
	HM_GET_BIN_DECLARED_PERM,
	HM_GET_ENTERPRISE_SPACE_ID,
	HM_SET_ENTERPRISE_SPACE_ID,
	HM_SET_APPUID,
	HM_GET_APPUID,
	HM_ACCESS_TOKENID_MAX_NR
};

#define ACCESS_TOKENID_SET_APPUID _IOW(HM_ACCESS_TOKEN_ID_IOCTL_BASE, HM_SET_APPUID, uint32_t)
#define ACCESS_TOKENID_GET_APPUID _IOR(HM_ACCESS_TOKEN_ID_IOCTL_BASE, HM_GET_APPUID, uint32_t)

#define HDAC_DEV "/dev/access_token_id"

uid_t getappuid(void)
{
#ifdef MUSL_EXTERNAL_FUNCTION
    int saved_errno = errno;
    int fd = open(HDAC_DEV, O_RDONLY);
    if (fd >= 0) {
        uid_t userid;
        int ret = ioctl(fd, ACCESS_TOKENID_GET_APPUID, &userid);
        close(fd);
        if (ret == 0) {
            errno = saved_errno;
            return userid;
        }
    }
    /* ioctl failed, fall back to getuid*/
    errno = ENOTSUP;
#endif
    return getuid();
}

struct setappuid_ctx {
    uid_t uid;
    int ret;
};

#ifdef MUSL_EXTERNAL_FUNCTION
static void do_setappuid(void *p)
{
    struct setappuid_ctx *c = p;
    if (c->ret < 0) {
        return;
    }
    int fd = __syscall(SYS_openat, AT_FDCWD, HDAC_DEV, O_RDONLY);
    if (fd < 0) {
        if (!c->ret) {
            __block_all_sigs(0);
            __syscall(SYS_kill, __syscall(SYS_getpid), SIGKILL);
        }
        c->ret = fd;
        return;
    }
    long ret = __syscall(SYS_ioctl, fd, ACCESS_TOKENID_SET_APPUID, &c->uid);
    if (__syscall(SYS_close, fd) && !ret) {
        MUSL_LOGE("setappuid: close fd failed after successful ioctl, uid: %{public}d", c->uid);
    }
    if (ret && !c->ret) {
        __block_all_sigs(0);
        __syscall(SYS_kill, __syscall(SYS_getpid), SIGKILL);
    }
    c->ret = ret;
}
#endif

int setappuid(uid_t uid)
{
#ifdef MUSL_EXTERNAL_FUNCTION
    struct setappuid_ctx c = { .uid = uid, .ret = 1 };
    __synccall(do_setappuid, &c);
    if (c.ret != 0) {
        /* ioctl failed or no thread executed, fall back to setuid*/
        errno = ENOTSUP;
        return setuid(uid);
    }
    return 0;
#else
    return setuid(uid);
#endif
}
