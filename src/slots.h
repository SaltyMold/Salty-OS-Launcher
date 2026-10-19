#ifndef SLOTS_H
#define SLOTS_H

// Check https://nwagyu.org/reference/firmware/addresses-structures.html

// 0x90400000 : start of slot B
// 0x20000    : userland header size
// 0x30       : slot info size
// 0x90420030 : target address
#define SLOT_B_START       0x90400000UL
#define SLOT_B_HEADER_SIZE 0x20000UL
#define SLOT_B_INFO_SIZE   0x30UL
#define SLOT_B_TARGET SLOT_B_START + SLOT_B_HEADER_SIZE + SLOT_B_INFO_SIZE

// 0x90000000 : start of slot A
// 0x20000    : userland headersize
// 0x30       : slot info size
// 0x90020030 : target address
#define SLOT_A_START       0x90000000UL
#define SLOT_A_HEADER_SIZE 0x20000UL
#define SLOT_A_INFO_SIZE   0x30UL
#define SLOT_A_TARGET SLOT_A_START + SLOT_A_HEADER_SIZE + SLOT_A_INFO_SIZE

#define SLOT_SIZE 0x400000UL

typedef enum { SLOT_A, SLOT_B, SLOT_UNKNOWN } slot_t;

// Set the stack pointer and jump to the address
__attribute__((naked)) void jump(unsigned long addr);

// Return the current slot
slot_t current_slot(void);

#endif