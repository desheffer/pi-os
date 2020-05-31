#include <exit.h>
#include <interrupts.h>
#include <scheduler.h>
#include <task.h>

/**
 * Exit from inside of a task.
 */
void exit(void)
{
    interrupts_disable();

    struct task * current_task = scheduler_current_task();

    current_task->state = stopped;

    schedule();
}
