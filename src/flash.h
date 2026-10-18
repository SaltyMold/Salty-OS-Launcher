#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

typedef enum {
    NW_FLASH_OK,
    NW_FLASH_FAILED  // The firmware refused
} nw_flash_status_t;

// Erase the 64 KiB sector containing `addr`.
nw_flash_status_t nw_flash_erase_sector(uint32_t addr);

// Write `len` bytes from `src` to `dst`. `dst` and `len` must be multiples of 4.
nw_flash_status_t nw_flash_write(uint32_t dst, const void *src, uint32_t len);


#endif