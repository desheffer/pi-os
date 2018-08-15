#include <assert.h>
#include <bcm2837.h>
#include <interrupt.h>
#include <scheduler.h>

static interrupt_handler_t* _handlers[NUM_IRQS];
static void* _handlers_data[NUM_IRQS];

static void interrupt_disable(irq_number_t num)
{
    assert(num < NUM_IRQS);

    *core0_timers_cntl ^= (1 << num);
}

static void interrupt_enable(irq_number_t num)
{
    assert(num < NUM_IRQS);

    *core0_timers_cntl |= (1 << num);
}

static int interrupt_pending(unsigned num)
{
    assert(num < NUM_IRQS);

    return *core0_irq & (1 << num);
}

void interrupt_init()
{
    for (unsigned i = 0; i < NUM_IRQS; ++i) {
        _handlers[i] = 0;
        _handlers_data[i] = 0;
    }

    enable_interrupts();
}

void interrupt_connect(irq_number_t num, interrupt_handler_t* handler, void* handlerData)
{
    assert(num < NUM_IRQS);

    _handlers[num] = handler;
    _handlers_data[num] = handlerData;

    interrupt_enable(num);
}

void interrupt_disconnect(irq_number_t num)
{
    assert(num < NUM_IRQS);

    interrupt_disable(num);

    _handlers[num] = 0;
    _handlers_data[num] = 0;
}

process_regs_t* interrupt_handler(process_regs_t* state)
{
    for (unsigned num = 0; num < NUM_IRQS; ++num) {
        if (interrupt_pending(num) && _handlers[num]) {
            _handlers[num](_handlers_data[num]);
            break;
        }
    }

    return scheduler_schedule(state);
}