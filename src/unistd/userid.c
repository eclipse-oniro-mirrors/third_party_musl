#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdint.h>
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
	HM_GET_CLOSEST_HAP_TOKENID,
	HM_GET_FAMILY_TOKENIDS,
	HM_GET_BINARY_PERMISSIONS,
	HM_SET_USERID,
	HM_GET_USERID,
	HM_ACCESS_TOKENID_MAX_NR
};

#define ACCESS_TOKENID_SET_USERID _IOW(HM_ACCESS_TOKEN_ID_IOCTL_BASE, HM_SET_USERID, uint32_t)
#define ACCESS_TOKENID_GET_USERID _IOR(HM_ACCESS_TOKEN_ID_IOCTL_BASE, HM_GET_USERID, uint32_t)

#define HDAC_DEV "/dev/access_token_id"

uid_t getappuid(void)
{
#ifdef MUSL_EXTERNAL_FUNCTION
    int fd = open(HDAC_DEV, O_RDONLY);
    if (fd >=0) {
        uid_t userid;
        int ret = ioctl(fd, ACCESS_TOKENID_GET_USERID, &userid);
        close(fd);
        if (ret == 0) {
            return userid;
        }
    }
    /* ioctl failed, fall back to getuid*/
#endif
    return getuid();
}

struct setappuid_ctx {
    uid_t uid;
    int ret;
};

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
    long ret = __syscall(SYS_ioctl, fd, ACCESS_TOKENID_SET_USERID, &c->uid);
    if (__syscall(SYS_close, fd) && !ret) {
#ifdef MUSL_EXTERNAL_FUNCTION
        MUSL_LOGE("setappuid: close fd failed after successful ioctl, uid: %{public}d", c->uid);
#endif
    }
    if (ret && !c->ret) {
        __block_all_sigs(0);
        __syscall(SYS_kill, __syscall(SYS_getpid), SIGKILL);
    }
    c->ret = ret;
}

int setappuid(uid_t uid)
{
#ifdef MUSL_EXTERNAL_FUNCTION
    struct setappuid_ctx c = { .uid = uid, .ret = 1 };
    __synccall(do_setappuid, &c);
    if (c.ret != 0) {
        /* ioctl failed or no thread executed, fall back to setuid*/
        return setuid(uid);
    }
    return 0;
#else
    return setuid(uid);
#endif
}
