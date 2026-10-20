#include "dfu_slot.h"

// The slot itself (marker, capacity, model, size, data) is in dfu_slot_data.c,
// compiled without LTO and linked last. See the layout described there.

typedef struct {
    char magic[16];
    uint32_t capacity, model, size;
    uint8_t data[];
} dfu_slot_t;

extern const dfu_slot_t dfu_slot;

uint32_t dfu_model(void) { return dfu_slot.size ? dfu_slot.model : 0; }

void dfu_keep(void) {
    __asm__ volatile("" : : "r"(&dfu_slot) : "memory");
}

// Next record of the userland. Start with *offset = 0. False at the end, or if malformed.
bool dfu_next(uint32_t *offset, dfu_record_t *record) {
    uint32_t o = *offset;
    if (dfu_slot.size > dfu_slot.capacity || o + 8 > dfu_slot.size) return false;

    const uint8_t *p = dfu_slot.data + o;
    uint32_t address, length;
    memcpy(&address, p, 4);
    memcpy(&length, p + 4, 4);
    if (length % 4 || length > dfu_slot.size - o - 8) return false;

    record->address = address;
    record->length = length;
    record->data = p + 8;
    *offset = o + 8 + length;
    return true;
}

// Check this before writing anything: every record must land inside slot B.
// (The data comes from a release asset: never trust where it says to write.)
bool dfu_valid(void) {
    uint32_t offset = 0;
    dfu_record_t r;
    bool any = false;
    while (dfu_next(&offset, &r)) {
        if (r.address < SLOT_B_START || r.address % 4) return false;
        uint32_t inside = r.address - SLOT_B_START;
        if (inside > SLOT_SIZE || r.length > SLOT_SIZE - inside) return false;
        any = true;
    }
    return any && offset == dfu_slot.size;
}

static bool is_erased(uint32_t a, uint32_t b) {
    for (const volatile uint32_t *p = (const volatile uint32_t *)(uintptr_t)a; (uint32_t)(uintptr_t)p < b; p++)
        if (*p != 0xFFFFFFFFu) return false;
    return true;
}

// Program zeros over [a, b): allowed on any content (bits only go 1 -> 0).
static flash_status_t zero_range(uint32_t a, uint32_t b) {
    static const uint8_t zeros[PAGE_SIZE];
    while (a < b) {
        uint32_t n = PAGE_SIZE - (a % PAGE_SIZE);
        if (n > b - a) n = b - a;
        if (flash_write(a, zeros, n) != FLASH_OK) return FLASH_FAILED;
        a += n;
    }
    return FLASH_OK;
}

// Empty the slot. Erasing works on whole 64 KiB sectors, and the slot shares its
// first and last sector with app code (or the next app). So: erase the sectors
// entirely inside the slot, and only zero the partial ends; the app is untouched.
// Sets `size` to 0 first (programming zeros is always allowed), so dfu_model() is 0 afterwards.
// Call it only after the userland has been written.
flash_status_t dfu_wipe(void) {
    static const uint32_t zero;
    if (flash_write((uint32_t)(uintptr_t)&dfu_slot.size, &zero, 4) != FLASH_OK) return FLASH_FAILED;

    uint32_t a = (uint32_t)(uintptr_t)dfu_slot.data;
    uint32_t b = a + dfu_slot.capacity;
    uint32_t s0 = (a + SECTOR_SIZE - 1) & ~(SECTOR_SIZE - 1);  // first boundary >= a
    uint32_t s1 = b & ~(SECTOR_SIZE - 1);                      // last boundary <= b

    if (s0 >= s1) return zero_range(a, b);  // no whole sector inside

    if (zero_range(a, s0) != FLASH_OK) return FLASH_FAILED;
    for (uint32_t s = s0; s < s1; s += SECTOR_SIZE)
        if (!is_erased(s, s + SECTOR_SIZE) && flash_erase_sector(s) != FLASH_OK) return FLASH_FAILED;
    return zero_range(s1, b);
}