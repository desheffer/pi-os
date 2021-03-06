#include <fcntl.h>

#include "abi.h"
#include "mm_context.h"
#include "page.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE4(openat, int, dirfd, const char *, pathname, int, flags, unsigned, mode)
{
    (void) flags;
    (void) mode;

    struct task * task = scheduler_current_task();

    struct path * path = vfs_path_create();

    // Copy `pathname` to kernel memory.
    char * kpathname = 0;
    if (pathname != 0) {
        size_t length = strnlen(pathname, page_size());
        kpathname = kmalloc(strnlen(pathname, page_size()));
        mm_copy_from_user(task->mm_context, kpathname, pathname, length);
    }

    struct dentry * pwd = 0;
    if (dirfd == AT_FDCWD) {
        pwd = task->vfs_context->pwd;
    } else {
        struct file * file = vfs_context_file(task->vfs_context, dirfd);
        if (file == 0) {
            return -EBADF;
        }

        pwd = list_peek_front(file->inode->dentries);
    }

    vfs_resolve_path(path, pwd, kpathname);

    if (path->child == 0) {
        vfs_path_destroy(path);
        kfree(kpathname);

        return -ENOENT;
    }

    int fd = vfs_context_open(task->vfs_context, path);

    vfs_path_destroy(path);

    return fd;
}
