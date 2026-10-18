#include "header.h"

#include <string.h>

#include "libs/storage.h"

const nw_userland_t *nw_userland(void) {
    return (const nw_userland_t *)extapp_userlandAddress();
}

void nw_software_version(char out[9]) {
    memcpy(out, nw_userland()->version, 8);
    out[8] = '\0';
}