#include <scheduler.h>
#include <synchronize.h>
#include <system.h>

void do_exit_group(process_regs_t* regs)
{
    int status = (int) regs->regs[0];

    // @TODO: Use status.
    (void) status;

    enter_critical();

    process_t* process = scheduler_current();
    process->state = stopping;

    leave_critical();

    while (1) {
        asm volatile("wfi");
    }
}