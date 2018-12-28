#include <scheduler.h>
#include <synchronize.h>
#include <system.h>

void sys_exit_group(int status)
{
    // @TODO
    (void) status;

    enter_critical();

    process_t* process = scheduler_current();
    process->state = zombie;

    leave_critical();

    scheduler_context_switch();
}
