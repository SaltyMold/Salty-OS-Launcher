#include "libs/eadk.h"
#include "display.h"
#include "timing.h"
#include "slots.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "Salty OS Launcher";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

int main(void) {
    slot_t slot = current_slot();

    return 0;
}