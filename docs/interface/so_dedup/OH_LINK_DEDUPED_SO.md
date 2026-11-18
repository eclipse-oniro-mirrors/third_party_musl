## OH_LINK_DEDUPED_SO &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

       OH_LinkDedupedSo - create a dedup symbolic link to a preset system so

#### **SYNOPSIS**

       #include <so_dedup/so_dedup.h>

       uint32_t OH_LinkDedupedSo(const char *library_path, const char *original_sha256);

#### **DESCRIPTION**

The function `OH_LinkDedupedSo()` creates a symbolic link at `library_path` pointing to a
preset so file under the dedup source directory. The preset so is selected from the dedup
configuration file `deduplist.json`: the version whose sha256 matches `original_sha256` is
preferred; otherwise the newest configured version is used.

`library_path` must be an absolute path, and `original_sha256` must be the sha256 of the
library file shipped inside the application package.

Only system callers are permitted to use this function. The interface is reserved for the
system installation path (Bundle Manager Service and installd); the check happens before
any configuration or filesystem access, so a rejected call produces no side effects.

#### **RETURN VALUE**

​       On success, OH_LinkDedupedSo() returns one of the following values:

​       `SO_DEDUP_LINKED`　　A symbolic link was created and verified.

​       `SO_DEDUP_NO_MATCH`　　No configured preset so can be selected for the library.

​       `SO_DEDUP_ALREADY_LINKED`　　The target already links to the selected preset so
　　(idempotent relink).

​       `SO_DEDUP_CONFIG_UNAVAILABLE`　　`deduplist.json` cannot be found.

​       `SO_DEDUP_VERSION_MISMATCH`　　Layer 2 fallback: the link was created for the newest
　　version, whose sha256 differs from `original_sha256`.

​       On error, it returns `SO_DEDUP_ERROR` (UINT32_MAX), errno is set to indicate the
error. For `SO_DEDUP_LINKED`, `SO_DEDUP_NO_MATCH`, `SO_DEDUP_ALREADY_LINKED`,
`SO_DEDUP_CONFIG_UNAVAILABLE` and `SO_DEDUP_VERSION_MISMATCH`, the caller's `errno` is
left untouched.

#### **ERRORS**

The following error codes may be set in errno when `SO_DEDUP_ERROR` is returned:

1. **EACCES**: the calling process is not a system caller (uid is in the application domain),
   or the underlying `symlink()`/`lsetxattr()` was denied by the filesystem or SELinux.
2. **EINVAL**: `library_path` is NULL or does not start with `/`, or `original_sha256` is NULL.
3. **EBADMSG**: `deduplist.json` is malformed, empty, or not a regular file.
4. **EEXIST**: the target path exists but is not a symlink created by this interface
   (regular file, directory or foreign symlink).
5. **EBUSY**: the created link failed readback verification (target replaced concurrently,
   content or inode mismatch).
6. **ENOENT, ENOSPC, ENOMEM, ...**: propagated from the underlying filesystem operations.

#### **ATTRIBUTES**

| Interface               | Attribute     | Value    |
| ----------------------- | ------------- | -------- |
| uint32_t OH_LinkDedupedSo(const char *library_path, const char *original_sha256) | Thread safety | MT-Safe |
|                         | Signal safety | Not Safe |

#### **HISTORY**

       -- 2026

#### **NOTES**

When readback verification fails after the link has been created, the link is kept on disk
and `SO_DEDUP_ERROR` is returned: the path may have been replaced concurrently, so the
interface never deletes it. The directory owner (the installation service) cleans the
staging directory up during rollback.

`SO_DEDUP_VERSION_MISMATCH` still leaves a valid link to the newest preset so; the caller
decides whether to accept it or to roll back the installation.

#### **CONFORMING TO**

       This is a platform-specific extension and is not part of any POSIX standard.

#### **EXAMPLES**

```c
// This interface is reserved for the system installation path (installd);
// the sample must be compiled and executed system-side.
#include <errno.h>
#include <stdio.h>
#include <so_dedup/so_dedup.h>

int main(void)
{
    const char *library_path =
        "/data/app/el1/bundle/public/com.example.demo/libs/arm64/libc++_shared.so";
    const char *original_sha256 =
        "b57bda08e7b85d1921adbb571bcf283e0ca114075b06d9719cb4d81fa7a3d4df";

    uint32_t result = OH_LinkDedupedSo(library_path, original_sha256);
    switch (result) {
    case SO_DEDUP_LINKED:
    case SO_DEDUP_ALREADY_LINKED:
        /* the library is deduped, continue the installation */
        break;
    case SO_DEDUP_VERSION_MISMATCH:
        /* linked to the newest preset version, accept or roll back */
        break;
    case SO_DEDUP_NO_MATCH:
    case SO_DEDUP_CONFIG_UNAVAILABLE:
    case SO_DEDUP_ERROR:
    default:
        /* abort the installation; for SO_DEDUP_ERROR errno carries the
         * reason, EACCES for an unpermitted caller */
        break;
    }
    return 0;
}
```

#### **COLOPHON**

       this page is part of the C library user-space interface documentation.
       Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
