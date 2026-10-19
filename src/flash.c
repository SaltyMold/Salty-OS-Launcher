#include "flash.h"

// Do a system call with 4 arguments (r0 to r3), return r0.
#define SVC(number, a0, a1, a2, a3)                                 \
({                                                                  \
    register uint32_t r0 __asm__("r0") = (uint32_t)(uintptr_t)(a0); \
    register uint32_t r1 __asm__("r1") = (uint32_t)(uintptr_t)(a1); \
    register uint32_t r2 __asm__("r2") = (uint32_t)(uintptr_t)(a2); \
    register uint32_t r3 __asm__("r3") = (uint32_t)(uintptr_t)(a3); \
    __asm__ volatile("svc %[imm]"                                   \
    : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3)       \
    : [imm] "I"(number)                            \
    : "r12", "memory");                            \
    r0;                                                             \
})

// The firmware answers with a boolean in the low byte of r0.
static flash_flash_status_t flash_status_from(uint32_t r0) {
    return (r0 & 0xFF) ? FLASH_OK : FLASH_FAILED;
}

flash_flash_status_t flash_erase_sector(uint32_t addr) {
    uint32_t sector = FIRST_64K_SECTOR + (addr - FLASH_BASE) / SECTOR_SIZE;
    return flash_status_from(SVC(SVC_FLASH_ERASE_SECTOR, sector, 1, 0, 0));
}

flash_flash_status_t flash_write(uint32_t dst, const void *src, uint32_t len) {
    const uint8_t *data = src;
    uint32_t end = dst + len;
    
    // The firmware only reads from RAM, so each chunk goes through this buffer.
    uint8_t buffer[PAGE_SIZE];
    
    while (dst < end) {
        // Chunk: up to the end of the current page, or of the data.
        uint32_t chunk = PAGE_SIZE - (dst % PAGE_SIZE);
        if (chunk > end - dst) chunk = end - dst;
        
        memcpy(buffer, data, chunk);
        if (flash_status_from(SVC(SVC_FLASH_WRITE_MEMORY, dst, buffer, chunk, 1)) != FLASH_OK) {
            return FLASH_FAILED;
        }
        
        dst += chunk;
        data += chunk;
    }
    return FLASH_OK;
}