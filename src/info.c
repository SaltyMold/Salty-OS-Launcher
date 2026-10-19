#include "info.h"

const nw_userland_t *nw_userland(void) {
    return (const nw_userland_t *)extapp_userlandAddress();
}

void nw_software_version(char out[9]) {
    memcpy(out, nw_userland()->version, 8);
    out[8] = '\0';
}

/*--------------------------------------------------*/

const char *nw_fcc_id(void) {
    register uint32_t r0 __asm__("r0");
    __asm__ volatile("svc %[imm]" : "=r"(r0) : [imm] "I"(SVC_FCC_ID) : "r1", "r2", "r3", "r12", "memory");
    
    bool ok = (r0 >= 0x08000000u && r0 < 0x08200000u) || (r0 >= 0x90000000u && r0 < 0x92000000u) ||
    (r0 >= 0x20000000u && r0 < 0x30000000u);
    return ok ? (const char *)r0 : NULL;
}

// n0115 has no fcc id so it returns UNKNOWN
nw_model_t nw_model(void) {
  const char *id = nw_fcc_id();
  if (!id) return NW_MODEL_UNKNOWN;

  char s[32];
  size_t i = 0;
  while (i < sizeof s - 1 && id[i]) {
    s[i] = id[i];
    i++;
  }
  s[i] = 0;

  if (strstr(s, "N0110")) return NW_MODEL_N0110;
  if (strstr(s, "N0115")) return NW_MODEL_N0115;
  if (strstr(s, "N0120")) return NW_MODEL_N0120;
  return NW_MODEL_UNKNOWN;
}