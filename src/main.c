#include "libs/eadk.h"

#include "dfu/dfu_slot.h"
#include "system/flash.h"
#include "system/slots.h"
#include "themes/themes_data.h"
#include "system/info.h"
#include "app/timing.h"
#include "app/display.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Salty OS";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

int main(void){
    dfu_keep(); // Remove optimization from compiler

    char version[9];
    slot_t slot;
    info_model_t calculator_model;
    info_model_t firmware_model;

    
    info_software_version(version);
    slot = slot_current();
    calculator_model = info_model();
    if (calculator_model == INFO_MODEL_UNKNOWN) calculator_model = INFO_MODEL_N0115; // No fcc id for n0115, so it returns unknown
    firmware_model = dfu_model();

    bool quit = false;

    /*--------------------------------------------*/

    if (dfu_model() == 0){ // If the userland is already installed
        display_clear_screen();
        display_draw_status_bar("Salty OS Launcher");
        display_message(MSG_WELCOME_LAUNCHER);

        display_message(MSG_ASK_JUMP);
        sleep(800, &quit, eadk_key_back);
        display_message(MSG_CONFIRM_JUMP);

        while (!quit){
            eadk_keyboard_state_t state = eadk_keyboard_scan();
            if (eadk_keyboard_key_down(state, eadk_key_ok)) break;
            if (eadk_keyboard_key_down(state, eadk_key_back)) return 2;
            eadk_timing_msleep(10);
        }

        jump(SLOT_B_TARGET);
    }

    // Display
    display_clear_screen();
    display_draw_status_bar("Salty OS Installer");
    display_message(MSG_WELCOME_INSTALLER);

    sleep(1000, &quit, eadk_key_back);

    // Slot B or Unknown detected
    if (slot != SLOT_A){
        display_message(MSG_ERR_SLOT);
        sleep(800, &quit, eadk_key_back);
        display_message(MSG_FIX_SLOT);
        sleep(20000, &quit, eadk_key_back);
        return 1;
    }

    // Model mismatch detected
    if (calculator_model != firmware_model){
        display_message(MSG_ERR_MODEL);
        sleep(800, &quit, eadk_key_back);
        display_message(MSG_FIX_MODEL);
        sleep(20000, &quit, eadk_key_back);
        return 1;
    }

    display_message(MSG_READY);

    sleep(800, &quit, eadk_key_back);
    display_message(MSG_DATA_LOST);

    sleep(800, &quit, eadk_key_back);
    display_message(MSG_CONFIRM);

    while (!quit){
        eadk_keyboard_state_t state = eadk_keyboard_scan();
        if (eadk_keyboard_key_down(state, eadk_key_ok)) break;
        if (eadk_keyboard_key_down(state, eadk_key_back)) return 1;
        eadk_timing_msleep(10);
    }

    sleep(500, &quit, eadk_key_back);

    display_message(MSG_ERASING_B);
    flash_erase(SLOT_B_START, SLOT_B_START + SLOT_SIZE);

    display_message(MSG_WRITING_B);
    uint32_t offset = 0;
    dfu_record_t record;
    while (dfu_next(&offset, &record)) flash_write(record.address, record.data, record.length);

    display_message(MSG_ERASING_DFU);
    dfu_wipe();

    display_message(MSG_ERASING_THEMES);
    flash_erase(THEME_AREA, THEME_AREA + THEME_AREA_SIZE);

    display_message(MSG_WRITING_THEMES);
    flash_write(THEME_AREA, themes_data, themes_size);

    display_message(MSG_DONE);
    
    sleep(2000, &quit, eadk_key_back);
    display_draw_status_bar("Salty OS Launcher");
    display_print("\n");
    display_message(MSG_WELCOME_LAUNCHER);

    sleep(1000, &quit, eadk_key_back);
    display_message(MSG_ASK_JUMP);

    sleep(800, &quit, eadk_key_back);
    display_message(MSG_CONFIRM_JUMP);


    while (!quit){
        eadk_keyboard_state_t state = eadk_keyboard_scan();
        if (eadk_keyboard_key_down(state, eadk_key_ok)) break;
        if (eadk_keyboard_key_down(state, eadk_key_back)) return 2;
        eadk_timing_msleep(10);
    }

    sleep(500, &quit, eadk_key_back);

    jump(SLOT_B_TARGET);
    
    return 0;
}