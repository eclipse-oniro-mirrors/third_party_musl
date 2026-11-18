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

#ifndef PASSWD_FACTORY_TEST_COMMON_H
#define PASSWD_FACTORY_TEST_COMMON_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test.h"

#define ROOT_TEMPLATE "/data/local/tmp/passwd-factory-XXXXXX"
#define STRESS_ROUNDS 2000
#define ITER_LIMIT 8

/* 第一路径 /etc/passwd、/etc/group；第二路径为对应的 _factory 文件 */
enum fixture {
    FIX_BOTH,
    FIX_SECOND_ONLY,
    FIX_NONE,
    FIX_FIRST_LOOP, /* 第一路径为自环符号链接，open 返回 ELOOP */
};

enum op {
    OP_PWUID,
    OP_PWNAM,
    OP_PWUIDR,
    OP_PWNAMR,
    OP_PWENT,
    OP_PWENTR,
    OP_GRGID,
    OP_GRNAM,
    OP_GRGIDR,
    OP_GRNAMR,
    OP_GRENT,
    OP_GRENTR,
    OP_GROUPLIST,
    OP_STRESS_FALLBACK,
    OP_STRESS_FIRST,
    OP_STRESS_DISABLED,
};

enum {
    NORMAL_UID = 1000,
    FALLBACK_UID = 2000,
    NORMAL_GID = 1000,
    FALLBACK_GID = 2001,
    BASE_GID = 999,
};

static const char kPasswdFile[] = "normal:x:1000:1000:Normal:/home/normal:/bin/sh\n";
static const char kPasswdFactoryFile[] =
    "fallback:x:2000:2000:Fallback:/home/fallback:/bin/sh\n"
    "second:x:2001:2001:Second:/home/second:/bin/sh\n";
static const char kGroupFile[] = "gnormal:x:1000:guser\n";
static const char kGroupFactoryFile[] = "gfallback:x:2001:guser\n";

struct outcome {
    int ran;     /* 子进程完整返回 */
    int env;     /* chroot 成功 */
    int found;   /* 查询是否有结果 */
    int count;   /* 迭代条数或压测成功轮数 */
    int rc;      /* _r 接口返回值或 getgrouplist 返回值 */
    int err;     /* 调用后 errno */
    unsigned id; /* 条目 uid/gid */
    char name[32];
    int groups[4];
    int ngroups;
};

static int g_env_unsupported;

static int WriteFile(const char *root, const char *name, const char *content)
{
    char path[128];
    size_t len = strlen(content);
    size_t done = 0;

    (void)snprintf(path, sizeof path, "%s/etc/%s", root, name);
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }
    while (done < len) {
        ssize_t n = write(fd, content + done, len - done);
        if (n <= 0) {
            (void)close(fd);
            return -1;
        }
        done += (size_t)n;
    }
    return close(fd) == 0 ? 0 : -1;
}

static int MakeSymlink(const char *root, const char *name, const char *target)
{
    char path[128];

    (void)snprintf(path, sizeof path, "%s/etc/%s", root, name);
    return symlink(target, path) == 0 ? 0 : -1;
}

static int SandboxCreate(char *root, size_t size, enum fixture fixture)
{
    char dir[128];

    (void)snprintf(root, size, "%s", ROOT_TEMPLATE);
    if (mkdtemp(root) == NULL) {
        return -1;
    }
    (void)snprintf(dir, sizeof dir, "%s/etc", root);
    if (mkdir(dir, 0755)) {
        return -1;
    }
    switch (fixture) {
        case FIX_BOTH:
            return WriteFile(root, "passwd", kPasswdFile) ||
                   WriteFile(root, "passwd_factory", kPasswdFactoryFile) ||
                   WriteFile(root, "group", kGroupFile) ||
                   WriteFile(root, "group_factory", kGroupFactoryFile) ? -1 : 0;
        case FIX_SECOND_ONLY:
            return WriteFile(root, "passwd_factory", kPasswdFactoryFile) ||
                   WriteFile(root, "group_factory", kGroupFactoryFile) ? -1 : 0;
        case FIX_NONE:
            return 0;
        case FIX_FIRST_LOOP:
            return MakeSymlink(root, "passwd", "passwd") ||
                   MakeSymlink(root, "group", "group") ? -1 : 0;
    }
    return -1;
}

static void SandboxDestroy(const char *root)
{
    static const char *const files[] = {"passwd", "passwd_factory", "group", "group_factory"};
    char path[128];
    size_t i;

    for (i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        (void)snprintf(path, sizeof path, "%s/etc/%s", root, files[i]);
        (void)unlink(path);
    }
    (void)snprintf(path, sizeof path, "%s/etc", root);
    (void)rmdir(path);
    (void)rmdir(root);
}

static void CopyName(char *dst, size_t size, const char *src)
{
    (void)snprintf(dst, size, "%s", src == NULL ? "" : src);
}

static void Lookup(enum op op, const char *name, unsigned id, struct outcome *out)
{
    struct passwd pw;
    struct passwd *pwr = NULL;
    struct group gr;
    struct group *grr = NULL;
    char buf[512];
    gid_t groups[4];
    int ngroups;
    int i;

    errno = 0;
    switch (op) {
        case OP_PWUID: {
            struct passwd *res = getpwuid((uid_t)id);
            out->found = res != NULL;
            if (res != NULL) {
                out->id = res->pw_uid;
                CopyName(out->name, sizeof out->name, res->pw_name);
            }
            break;
        }
        case OP_PWNAM: {
            struct passwd *res = getpwnam(name);
            out->found = res != NULL;
            if (res != NULL) {
                out->id = res->pw_uid;
                CopyName(out->name, sizeof out->name, res->pw_name);
            }
            break;
        }
        case OP_PWUIDR:
            out->rc = getpwuid_r((uid_t)id, &pw, buf, sizeof buf, &pwr);
            out->found = out->rc == 0 && pwr != NULL;
            if (pwr != NULL) {
                out->id = pwr->pw_uid;
                CopyName(out->name, sizeof out->name, pwr->pw_name);
            }
            break;
        case OP_PWNAMR:
            out->rc = getpwnam_r(name, &pw, buf, sizeof buf, &pwr);
            out->found = out->rc == 0 && pwr != NULL;
            if (pwr != NULL) {
                out->id = pwr->pw_uid;
                CopyName(out->name, sizeof out->name, pwr->pw_name);
            }
            break;
        case OP_PWENT:
            setpwent();
            while (getpwent() != NULL) {
                if (++out->count > ITER_LIMIT) {
                    break;
                }
            }
            endpwent();
            break;
        case OP_PWENTR:
            setpwent();
            for (;;) {
                out->rc = getpwent_r(&pw, buf, sizeof buf, &pwr);
                if (out->rc != 0 || pwr == NULL) {
                    break;
                }
                if (++out->count > ITER_LIMIT) {
                    break;
                }
            }
            endpwent();
            break;
        case OP_GRGID: {
            struct group *res = getgrgid((gid_t)id);
            out->found = res != NULL;
            if (res != NULL) {
                out->id = res->gr_gid;
                CopyName(out->name, sizeof out->name, res->gr_name);
            }
            break;
        }
        case OP_GRNAM: {
            struct group *res = getgrnam(name);
            out->found = res != NULL;
            if (res != NULL) {
                out->id = res->gr_gid;
                CopyName(out->name, sizeof out->name, res->gr_name);
            }
            break;
        }
        case OP_GRGIDR:
            out->rc = getgrgid_r((gid_t)id, &gr, buf, sizeof buf, &grr);
            out->found = out->rc == 0 && grr != NULL;
            if (grr != NULL) {
                out->id = grr->gr_gid;
                CopyName(out->name, sizeof out->name, grr->gr_name);
            }
            break;
        case OP_GRNAMR:
            out->rc = getgrnam_r(name, &gr, buf, sizeof buf, &grr);
            out->found = out->rc == 0 && grr != NULL;
            if (grr != NULL) {
                out->id = grr->gr_gid;
                CopyName(out->name, sizeof out->name, grr->gr_name);
            }
            break;
        case OP_GRENT:
            setgrent();
            while (getgrent() != NULL) {
                if (++out->count > ITER_LIMIT) {
                    break;
                }
            }
            endgrent();
            break;
        case OP_GRENTR:
            setgrent();
            for (;;) {
                out->rc = getgrent_r(&gr, buf, sizeof buf, &grr);
                if (out->rc != 0 || grr == NULL) {
                    break;
                }
                if (++out->count > ITER_LIMIT) {
                    break;
                }
            }
            endgrent();
            break;
        case OP_GROUPLIST:
            ngroups = 4;
            out->rc = getgrouplist(name, (gid_t)id, groups, &ngroups);
            out->ngroups = ngroups < 4 ? ngroups : 4;
            for (i = 0; i < out->ngroups; i++) {
                out->groups[i] = (int)groups[i];
            }
            break;
        case OP_STRESS_FALLBACK:
        case OP_STRESS_FIRST: {
            int second = op == OP_STRESS_FALLBACK;
            uid_t uid = second ? FALLBACK_UID : NORMAL_UID;
            gid_t gid = second ? FALLBACK_GID : NORMAL_GID;
            const char *uname = second ? "fallback" : "normal";
            const char *gname = second ? "gfallback" : "gnormal";
            for (i = 0; i < STRESS_ROUNDS; i++) {
                struct passwd *p = getpwuid(uid);
                struct group *g = getgrgid(gid);
                if (p == NULL || g == NULL || strcmp(p->pw_name, uname) != 0 ||
                    strcmp(g->gr_name, gname) != 0) {
                    break;
                }
                int n = 0;
                setpwent();
                while (getpwent() != NULL) {
                    if (++n > ITER_LIMIT) {
                        break;
                    }
                }
                endpwent();
                if (n != (second ? 2 : 1)) {
                    break;
                }
                n = 0;
                setgrent();
                while (getgrent() != NULL) {
                    if (++n > ITER_LIMIT) {
                        break;
                    }
                }
                endgrent();
                if (n != 1) {
                    break;
                }
                ngroups = 4;
                if (getgrouplist("guser", BASE_GID, groups, &ngroups) != 2) {
                    break;
                }
                out->count++;
            }
            break;
        }
        case OP_STRESS_DISABLED: {
            for (i = 0; i < STRESS_ROUNDS; i++) {
                int n = 0;
                if (getpwuid(FALLBACK_UID) != NULL || getpwnam("fallback") != NULL ||
                    getgrgid(FALLBACK_GID) != NULL || getgrnam("gfallback") != NULL) {
                    break;
                }
                setpwent();
                while (getpwent() != NULL) {
                    if (++n > ITER_LIMIT) {
                        break;
                    }
                }
                endpwent();
                if (n != 0) {
                    break;
                }
                n = 0;
                setgrent();
                while (getgrent() != NULL) {
                    if (++n > ITER_LIMIT) {
                        break;
                    }
                }
                endgrent();
                if (n != 0) {
                    break;
                }
                ngroups = 4;
                if (getgrouplist("guser", BASE_GID, groups, &ngroups) != 1) {
                    break;
                }
                out->count++;
            }
            break;
        }
    }
    out->err = errno;
}

static struct outcome Run(const char *root, enum op op, const char *name, unsigned id)
{
    struct outcome out = {0};
    int pfd[2];
    int status = 0;
    ssize_t n;
    pid_t pid;

    if (pipe(pfd)) {
        return out;
    }
    pid = fork();
    if (pid < 0) {
        (void)close(pfd[0]);
        (void)close(pfd[1]);
        return out;
    }
    if (pid == 0) {
        (void)close(pfd[0]);
        if (chroot(root) || chdir("/")) {
            out.env = 0;
            (void)write(pfd[1], &out, sizeof out);
            _exit(0);
        }
        out.env = 1;
        Lookup(op, name, id, &out);
        n = write(pfd[1], &out, sizeof out);
        _exit(n == (ssize_t)sizeof out ? 0 : 1);
    }
    (void)close(pfd[1]);
    n = read(pfd[0], &out, sizeof out);
    (void)close(pfd[0]);
    (void)waitpid(pid, &status, 0);
    out.ran = n == (ssize_t)sizeof out && WIFEXITED(status) && WEXITSTATUS(status) == 0;
    return out;
}

static int Prepare(const char *test, const struct outcome *o)
{
    if (!o->ran) {
        t_error("[%s] sandbox child did not run\n", test);
        return 0;
    }
    if (!o->env) {
        g_env_unsupported = 1;
        return 0;
    }
    return 1;
}

static void CheckUser(const char *test, const struct outcome *o, unsigned uid, const char *name)
{
    if (!o->found) {
        t_error("[%s] expected user %s, got none (errno=%d)\n", test, name, o->err);
        return;
    }
    if (o->id != uid) {
        t_error("[%s] expected uid %u, got %u\n", test, uid, o->id);
    }
    if (strcmp(o->name, name)) {
        t_error("[%s] expected name %s, got %s\n", test, name, o->name);
    }
}

static void CheckGroup(const char *test, const struct outcome *o, unsigned gid, const char *name)
{
    if (!o->found) {
        t_error("[%s] expected group %s, got none (errno=%d)\n", test, name, o->err);
        return;
    }
    if (o->id != gid) {
        t_error("[%s] expected gid %u, got %u\n", test, gid, o->id);
    }
    if (strcmp(o->name, name)) {
        t_error("[%s] expected name %s, got %s\n", test, name, o->name);
    }
}

static void CheckMissing(const char *test, const struct outcome *o, int err)
{
    if (o->found) {
        t_error("[%s] expected no entry, got %s\n", test, o->name);
    }
    if (err && o->err != err) {
        t_error("[%s] expected errno %d, got %d\n", test, err, o->err);
    }
}

static void CheckRc(const char *test, const struct outcome *o, int rc)
{
    if (o->rc != rc) {
        t_error("[%s] expected rc %d, got %d\n", test, rc, o->rc);
    }
}

static void CheckCount(const char *test, const struct outcome *o, int count)
{
    if (o->count != count) {
        t_error("[%s] expected %d entries, got %d\n", test, count, o->count);
    }
}

static void CheckList(const char *test, const struct outcome *o, int rc, int ngroups, int g0, int g1)
{
    if (o->rc != rc) {
        t_error("[%s] expected rc %d, got %d\n", test, rc, o->rc);
    }
    if (o->ngroups != ngroups) {
        t_error("[%s] expected ngroups %d, got %d\n", test, ngroups, o->ngroups);
    }
    if (ngroups >= 1 && o->groups[0] != g0) {
        t_error("[%s] expected groups[0] %d, got %d\n", test, g0, o->groups[0]);
    }
    if (ngroups >= 2 && o->groups[1] != g1) {
        t_error("[%s] expected groups[1] %d, got %d\n", test, g1, o->groups[1]);
    }
}

#define MAKE_SANDBOX(test, fixture)                         \
    char root[64];                                          \
    if (SandboxCreate(root, sizeof root, (fixture))) {      \
        t_error("[%s] failed to create sandbox\n", (test)); \
        return;                                             \
    }

#endif
