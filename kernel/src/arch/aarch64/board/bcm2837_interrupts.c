#include "asm/entry.h"
#include "board/armv8_timer.h"
#include "board/bcm2837_interrupts.h"
#include "pimento.h"

static interrupt_handler_t _handlers[BCM2837_NUM_IRQS] = {0};
static void * _handlers_data[BCM2837_NUM_IRQS] = {0};

/**
 * Enable an interrupt.
 */
inline void _interrupts_enable(unsigned num)
{
    *BCM2837_CORE0_TIMERS_CNTL |= (0b1 << num);
}

/**
 * Disable an interrupt.
 */
inline void _interrupts_disable(unsigned num)
{
    *BCM2837_CORE0_TIMERS_CNTL ^= (0b1 << num);
}

/**
 * Determine if an interrupt is pending.
 */
inline int _interrupts_pending(unsigned num)
{
    return *BCM2837_CORE0_IRQ & (0b1 << num);
}

/**
 * Initialize timer for the BCM2837.
 */
static void _interrupts_timer_init(void)
{
    // Use Crystal clock and increment by 1.
    *BCM2837_CORE_TIMERS_CONTROL = 0;

    // Set timer prescaler 1:1 (timer freq = 2^31 / prescaler * input).
    *BCM2837_CORE_TIMERS_PRESCALER = 0x80000000;

    bcm2837_interrupts_connect(BCM2837_LOCAL_IRQ_CNTPNSIRQ, armv8_timer_tick, 0);
}

/**
 * Initialize interrupt handling for the BCM2837.
 */
void bcm2837_interrupts_init(void)
{
    for (unsigned i = 0; i < BCM2837_NUM_IRQS; ++i) {
        _handlers[i] = 0;
        _handlers_data[i] = 0;
    }

    set_interrupts_handler(bcm2837_interrupts_handler);

    _interrupts_timer_init();
}

/**
 * Connect an interrupt handler.
 */
void bcm2837_interrupts_connect(unsigned num, interrupt_handler_t handler,
                                void * handler_data)
{
    if (num < BCM2837_NUM_IRQS) {
        _handlers[num] = handler;
        _handlers_data[num] = handler_data;

        _interrupts_enable(num);
    }
}

/**
 * Disconnect an interrupt handler.
 */
void bcm2837_interrupts_disconnect(unsigned num)
{
    if (num < BCM2837_NUM_IRQS) {
        _interrupts_disable(num);

        _handlers[num] = 0;
        _handlers_data[num] = 0;
    }
}

/**
 * Listen for interrupts and delegate to the connected interrupt handlers.
 */
void bcm2837_interrupts_handler(void)
{
    for (unsigned num = 0; num < BCM2837_NUM_IRQS; ++num) {
        if (_handlers[num] && _interrupts_pending(num)) {
            _handlers[num](_handlers_data[num]);
            break;
        }
    }
}
