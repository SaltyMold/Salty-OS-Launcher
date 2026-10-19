#include "header.h"
#include "libs/eadk.h"
#include "libs/storage.h"
#include "slots.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Salty OS Launcher";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

/*
Check if is in slot A
Check if calculator model correspond to salty OS
Check if salty OS version correspond to epsilon

Erase slot B flash
Install salty OS in slot B flash
Erase salty OS in app flash (So the flash can be used for themes)
Then use the app as a launcher
*/





int main(void) {
    // slot_t slot = current_slot();
    // char version[9];
    // nw_software_version(version);
    
    eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
    
    const char *id = nw_fcc_id();
    eadk_display_draw_string(id ? id : "NULL", (eadk_point_t){0, 0}, true, eadk_color_white, eadk_color_black);
    
    while (!eadk_keyboard_key_down(eadk_keyboard_scan(), eadk_key_back)) eadk_timing_msleep(50);

    return 0;
}