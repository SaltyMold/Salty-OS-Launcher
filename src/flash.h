#ifndef FLASH_H
#define FLASH_H

#include "libs/eadk.h"

// Layout of the external flash
#define FLASH_BASE 0x90000000u
#define SECTOR_SIZE 0x10000u  // 64 KiB
#define PAGE_SIZE 256u        // a write must never cross a page boundary
// Epsilon numbers sectors as: 8 x 4 KiB, 1 x 32 KiB, then the 64 KiB ones.
#define FIRST_64K_SECTOR 8

// System calls (svcall.h)
#define SVC_FLASH_ERASE_SECTOR 30
#define SVC_FLASH_WRITE_MEMORY 32

typedef enum {
    NW_FLASH_OK,
    NW_FLASH_FAILED
} nw_flash_status_t;

// Erase the 64 KiB sector containing `addr`.
nw_flash_status_t nw_flash_erase_sector(uint32_t addr);

// Write `len` bytes from `src` to `dst`. `dst` and `len` must be multiples of 4.
nw_flash_status_t nw_flash_write(uint32_t dst, const void *src, uint32_t len);


#endif