#include <assert.h>
#include <exception.h>
#include <kstdio.h>
#include <service.h>
#include <sys.h>

void exception_handler(process_state_t* state, uint64_t index, uint64_t esr, uint64_t far)
{
    if (index & 0b0001 && esr >> 26 == 0b010101) {
        Service::instance()->handle(state);
    } else if (index & 0b0010) {
        Interrupt::instance()->handle();
    } else {
        debug(state, index, esr, far);
    }

    Scheduler::instance()->schedule(state);
}

void debug(process_state_t* state, uint64_t index, uint64_t esr, uint64_t far)
{
    const char* type = "Unknown";
    const char* ifs = "Unknown";
    const char* dfs = "Unknown";
    const char* level = "Unknown";

    kputs(
        "\n"
        "[41m[97m                          [0m\n"
        "[41m[97m     Kernel Exception     [0m\n"
        "[41m[97m                          [0m\n"
        "\n"
    );

    switch (index & 0b1111) {
        case 0b0001: type = "Synchronous"; break;
        case 0b0010: type = "IRQ"; break;
        case 0b0100: type = "FIQ"; break;
        case 0b1000: type = "SError"; break;
    }

    switch (esr >> 26) {
        case 0b000000: ifs = "Unknown reason"; break;
        case 0b000001: ifs = "Trapped WFI or WFE instruction"; break;
        case 0b001110: ifs = "Illegal Execution state"; break;
        case 0b010101: ifs = "SVC instruction"; break;
        case 0b100000: ifs = "Instruction Abort, lower level"; break;
        case 0b100001: ifs = "Instruction Abort, same level"; break;
        case 0b100010: ifs = "PC alignment fault"; break;
        case 0b100100: ifs = "Data Abort, lower level"; break;
        case 0b100101: ifs = "Data Abort, same level"; break;
        case 0b100110: ifs = "SP alignment fault"; break;
        case 0b101100: ifs = "Trapped floating-point"; break;
        case 0b110000: ifs = "Breakpoint, lower level"; break;
        case 0b110001: ifs = "Breakpoint, same level"; break;
        case 0b111100: ifs = "Breakpoint instruction"; break;
    }

    if (esr >> 26 == 0b100100 || esr >> 26 == 0b100101) {
        switch (esr >> 2 & 0b11) {
            case 0b00: dfs = "Address size fault"; break;
            case 0b01: dfs = "Translation fault"; break;
            case 0b10: dfs = "Access flag fault"; break;
            case 0b11: dfs = "Permission fault"; break;
        }

        switch (esr & 0b11) {
            case 0b00: level = "0"; break;
            case 0b01: level = "1"; break;
            case 0b10: level = "2"; break;
            case 0b11: level = "3"; break;
        }
    }

    kprintf("Exception Type (%#02x) = %s\n", (unsigned) index, type);
    kprintf("Instruction Fault Status = %s\n", ifs);
    kprintf("Data Fault Status = %s, level %s\n", dfs, level);

    kprintf("\nRegisters:\n\n");

    kprintf("   spsr = %08x %08x", (unsigned) (state->spsr >> 32), (unsigned) state->spsr);
    kprintf("    elr = %08x %08x", (unsigned) (state->elr >> 32), (unsigned) state->elr);
    kprintf("\n");
    kprintf("    esr = %08x %08x", (unsigned) (esr >> 32), (unsigned) esr);
    kprintf("    far = %08x %08x", (unsigned) (far >> 32), (unsigned) far);
    kprintf("\n");

    for (unsigned i = 0; i < NUM_REGS; ++i) {
        if (i % 2 == 0) {
            kprintf("\n");
        }
        kprintf("  %5u = %08x %08x", i, (unsigned) (state->x[i] >> 32), (unsigned) state->x[i]);
    }
    kprintf("\n");

    halt();
}