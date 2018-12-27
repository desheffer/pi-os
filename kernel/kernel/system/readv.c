#include <assert.h>
#include <serial.h>
#include <sys/uio.h>
#include <system.h>

registers_t* sys_readv(registers_t* regs)
{
    int fd = (int) regs->regs[0];
    struct iovec* iov = (struct iovec*) regs->regs[1];
    size_t iovcnt = (size_t) regs->regs[2];

    ssize_t ret = 0;

    assert(fd == 0);

    while (iovcnt--) {
        char* buf = (char*) iov->iov_base;
        size_t count = iov->iov_len;

        while (count--) {
            char c = serial_getc();
            *(buf++) = c;
            ++ret;

            if (c == '\n') {
                break;
            }
        }

        ++iov;
    }

    regs->regs[0] = ret;
    return regs;
}