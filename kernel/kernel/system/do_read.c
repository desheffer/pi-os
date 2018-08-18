#include <assert.h>
#include <serial.h>
#include <system.h>

void do_read(process_regs_t* regs)
{
    int fd = (int) regs->x[0];
    char* buf = (char*) regs->x[1];
    size_t count = (size_t) regs->x[2];

    ssize_t ret = 0;

    assert(fd == 0);

    while (count--) {
        *(buf++) = serial_getc();
        ++ret;
    }

    regs->x[0] = ret;
}
