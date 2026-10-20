#include "timing.h"

// So you can quit during a msleep
void sleep(int ms, bool *quit, eadk_key_t quit_key) {
    for (int i = 0; i < ms / 10; i++) {
        eadk_keyboard_state_t state = eadk_keyboard_scan();
        if (eadk_keyboard_key_down(state, quit_key)) *quit = true;
        eadk_timing_msleep(10);
        if (*quit) break;
    }
}