#include "timing.h"
#include "libs/eadk.h"

// So you can quit during a msleep
void sleep(int ms, bool *quit) {
    for (int i = 0; i < ms / 10; i++) eadk_timing_msleep(10);
}