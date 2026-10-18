#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
    #endif
    
    // Firmware "userland header": a small structure at the start of the firmware's
    // software part. Its address is given by extapp_userlandAddress() (storage.h),
    // which already handles the model (N0110 / N0120) and the slot (A / B).
    typedef struct {
        uint32_t magic;                             // 0xDEC0EDFE
        char version[8];                            // "26.4.0" (not necessarily null-terminated)
        uint32_t storage_ram, storage_size;         // file system (Python scripts, saves...)
        uint32_t apps_flash_start, apps_flash_end;  // flash area used by external apps
        uint32_t apps_ram_start, apps_ram_end;
        uint32_t name_flash_start, name_flash_end;
        uint32_t footer;                            // 0xDEC0EDFE
    } nw_userland_t;
    
    // Header of the running firmware.
    const nw_userland_t *nw_userland(void);
    
    // Copy the version into `out` (always null-terminated).
    void nw_software_version(char out[9]);
    
    // Flash area used by external apps: [start, end).
    void nw_apps_range(uint32_t *start, uint32_t *end);
    
    #ifdef __cplusplus
}
#endif

#endif