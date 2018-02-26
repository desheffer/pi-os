#include <exception.h>
#include <stdio.h>
#include <sys.h>

debug_state_t* debug_state = 0;

void exception_handler()
{
    const char* type = "Unknown";
    const char* ifs = "Unknown";
    const char* dfs = "Unknown";
    const char* level = "Unknown";

    printf("\n\n!!! Kernel Exception! !!!\n\n");

    switch (debug_state->index) {
        case 0: type = "Synchronous"; break;
        case 1: type = "IRQ"; break;
        case 2: type = "FIQ"; break;
        case 3: type = "SError"; break;
    }

    switch (debug_state->esr >> 26) {
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

    if (debug_state->esr >> 26 == 0b100100 || debug_state->esr >> 26 == 0b100101) {
        switch (debug_state->esr & 0b01100) {
            case 0b00000: dfs = "Address size fault"; break;
            case 0b00100: dfs = "Translation fault"; break;
            case 0b01000: dfs = "Access flag fault"; break;
            case 0b01100: dfs = "Permission fault"; break;
        }

        switch (debug_state->esr & 0b11) {
            case 0b00: level = "0"; break;
            case 0b01: level = "1"; break;
            case 0b10: level = "2"; break;
            case 0b11: level = "3"; break;
        }
    }

    printf("Exception Type = %s\n", type);
    printf("Instruction Fault Status = %s\n", ifs);
    printf("Data Fault Status = %s, level %s\n", dfs, level);

    printf("\nRegisters:\n\n");
    printf("   spsr = %08s %08x", debug_state->spsr >> 32, debug_state->spsr);
    printf("    elr = %08x %08x\n", debug_state->elr >> 32, debug_state->elr);
    printf("    esr = %08x %08x", debug_state->esr >> 32, debug_state->esr);
    printf("    far = %08x %08x", debug_state->far >> 32, debug_state->far);
    printf("  sctlr = %08x %08x", debug_state->sctlr >> 32, debug_state->sctlr);
    printf("    tcr = %08x %08x\n\n", debug_state->tcr >> 32, debug_state->tcr);
    for (unsigned i = 0; i < NUM_REGS; i++) {
        printf("  %5u = %08x %08x", i, debug_state->x[i] >> 32, debug_state->x[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
    }
    printf("\n\n");

    halt();
}