#include "slots.h"

__attribute__((naked)) void jump(unsigned long addr) {
    __asm volatile (
        "ldrd r3, r2, [r0] \n"  // r3 = stack pointer, r2 = start address
        "msr  MSP, r3      \n"  // set stack pointer
        "bx   r2           \n"  // jump to start address
    );
}

slot_t current_slot(void) {
    unsigned long here = (unsigned long)&current_slot;

    if (here >= SLOT_A_START && here < SLOT_A_START + SLOT_SIZE) return SLOT_A;
    if (here >= SLOT_B_START && here < SLOT_B_START + SLOT_SIZE) return SLOT_B;

    return SLOT_UNKNOWN;
}