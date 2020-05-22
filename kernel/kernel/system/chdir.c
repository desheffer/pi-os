#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE1(chdir, const char *, path)
{
    struct process * process = scheduler_current();

    struct dentry * dir = fs_dentry(path, process->fs_context->cwd);

    if (dir == 0) {
        return -ENOENT;
    }

    if ((dir->inode->mode & S_IFDIR) == 0) {
        return -ENOTDIR;
    }

    process->fs_context->cwd = dir;

    return 0;
}
