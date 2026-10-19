#include "slots.h"

__attribute__((naked)) void jump(unsigned long addr) {
    __asm volatile (
        "ldrd r3, r2, [r0] \n"  // r3 = stack pointer, r2 = start address
        "msr  MSP, r3      \n"  // set stack pointer
        "bx   r2           \n"  // jump to start address
    );
}

slot_t slot_current(void) {
    unsigned long here = (unsigned long)&slot_current;

    if (here >= SLOT_A_START && here < SLOT_A_START + SLOT_SIZE) return SLOT_A;
    if (here >= SLOT_B_START && here < SLOT_B_START + SLOT_SIZE) return SLOT_B;

    return SLOT_UNKNOWN;
}

char *slot_name(slot_t slot) {
    switch (slot) {
        case SLOT_A: return "Slot A";
        case SLOT_B: return "Slot B";
        default: return "Slot unknown";
    }
}