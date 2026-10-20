#include "libs/eadk.h"

#include "dfu/dfu_slot.h"
#include "system/flash.h"
#include "system/slots.h"
#include "themes/themes_data.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Salty OS";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

#define THEME_AREA 0x90270000u
#define THEME_AREA_SIZE 0x180000u

int main(void) {
    dfu_keep(); // Remove optimization from compiler

    eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);

    if (!dfu_model()) jump(SLOT_B_TARGET);

    eadk_display_draw_string("Erasing slot B...", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
    flash_erase(SLOT_B_START, SLOT_B_START + SLOT_SIZE);

    uint32_t offset = 0;
    dfu_record_t record;
    eadk_display_draw_string("Writing salty os", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
    while (dfu_next(&offset, &record)) flash_write(record.address, record.data, record.length);
    
    eadk_display_draw_string("Erasing app dfu", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
    dfu_wipe();

    eadk_display_draw_string("Erasing theme area...", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
    flash_erase(THEME_AREA, THEME_AREA + THEME_AREA_SIZE);

    eadk_display_draw_string("Writing theme aera", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
    flash_write(THEME_AREA, themes_data, themes_size);

    return 0;
}