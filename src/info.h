#ifndef INFO_H
#define INFO_H

#include "libs/eadk.h"
#include "libs/storage.h"

#define SVC_FCC_ID 29

typedef struct {
    uint32_t magic;                             // 0xDEC0EDFE
    char version[8];                            // "26.4.0" (not necessarily null-terminated)
    uint32_t storage_ram, storage_size;         // file system (Python scripts, saves...)
    uint32_t apps_flash_start, apps_flash_end;  // flash area used by external apps
    uint32_t apps_ram_start, apps_ram_end;
    uint32_t name_flash_start, name_flash_end;
    uint32_t footer;                            // 0xDEC0EDFE
} info_userland_t;

// Header of the running firmware.
const info_userland_t *info_userland(void);

// Copy the version into `out` (always null-terminated).
void info_software_version(char out[9]);

/*--------------------------------------------------*/

typedef enum {
  INFO_MODEL_UNKNOWN,
  INFO_MODEL_N0110,
  INFO_MODEL_N0115,
  INFO_MODEL_N0120
} info_model_t;

// Returns fcc id
const char *info_fcc_id(void);

// Returns model
// n0115 has no fcc id so it returns UNKNOWN
info_model_t info_model(void);

char *info_model_name(info_model_t model);

#endif