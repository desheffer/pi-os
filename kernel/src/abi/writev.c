#include <sys/uio.h>

#include "abi.h"
#include "mm_context.h"
#include "page.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE3(writev, int, fd, const struct iovec *, iov, int, iovcnt)
{
    long res = 0;

    struct task * task = scheduler_current_task();

    struct file * file = vfs_context_file(task->vfs_context, fd);
    if (file == 0) {
        return -EBADF;
    }

    unsigned p_size = page_size();
    struct page * page_buf = page_alloc();

    while (iovcnt--) {
        size_t count = iov->iov_len < p_size ? iov->iov_len : p_size;
        count = mm_copy_from_user(task->mm_context, page_buf->vaddr, iov->iov_base, count);

        off_t off = 0;
        res += vfs_write(file, page_buf->vaddr, count, &off);

        ++iov;
    }

    kfree(page_buf);

    return res;
}
