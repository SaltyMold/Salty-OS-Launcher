#ifndef DFU_SLOT_H
#define DFU_SLOT_H

#include "../libs/eadk.h"
#include "../system/flash.h"
#include "../system/slots.h"

typedef struct {
    uint32_t address;
    uint32_t length;  // multiple of 4
    const uint8_t *data;
} dfu_record_t;

// Model of the userland carried by the slot: 110, 115 or 120. 0 if there is none.
uint32_t dfu_model(void);

// Does nothing, but keeps the slot in the installed app (the linker would drop it).
void dfu_keep(void);

// Next record of the userland. Start with *offset = 0. False at the end.
bool dfu_next(uint32_t *offset, dfu_record_t *record);

// Empty the slot and mark it as having no userland.
flash_status_t dfu_wipe(void);

#endif