#include "info.h"

const info_userland_t *info_userland(void) {
    return (const info_userland_t *)extapp_userlandAddress();
}

void info_software_version(char out[9]) {
    memcpy(out, info_userland()->version, 8);
    out[8] = '\0';
}

/*--------------------------------------------------*/

const char *info_fcc_id(void) {
    register uint32_t r0 __asm__("r0");
    __asm__ volatile("svc %[imm]" : "=r"(r0) : [imm] "I"(SVC_FCC_ID) : "r1", "r2", "r3", "r12", "memory");
    
    bool ok = (r0 >= 0x08000000u && r0 < 0x08200000u) || (r0 >= 0x90000000u && r0 < 0x92000000u) ||
    (r0 >= 0x20000000u && r0 < 0x30000000u);
    return ok ? (const char *)r0 : NULL;
}

// n0115 has no fcc id so it returns UNKNOWN
info_model_t info_model(void) {
  const char *id = info_fcc_id();
  if (!id) return INFO_MODEL_UNKNOWN;

  char s[32];
  size_t i = 0;
  while (i < sizeof s - 1 && id[i]) {
    s[i] = id[i];
    i++;
  }
  s[i] = 0;

  if (strstr(s, "N0110")) return INFO_MODEL_N0110;
  if (strstr(s, "N0115")) return INFO_MODEL_N0115;
  if (strstr(s, "N0120")) return INFO_MODEL_N0120;
  return INFO_MODEL_UNKNOWN;
}

char *info_model_name(info_model_t model) {
  switch (model) {
    case INFO_MODEL_N0110: return "N0110";
    case INFO_MODEL_N0115: return "N0115";
    case INFO_MODEL_N0120: return "N0120";
    default: return "UNKNOWN";
  }
}