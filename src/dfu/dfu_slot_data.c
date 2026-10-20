// The userland slot itself, alone in its own object.
// Compiled WITHOUT LTO (see the Makefile) and linked last, so the linker puts
// .rodata.dfu after the strings and the code of the app.
//
// Layout: marker (16) | capacity (4) | model (4) | size (4) | data (capacity bytes).
// The data is a list of records: address (4) | length (4) | bytes, in this order.
// It is filled after the build, without recompiling, by scripts/patch_dfu.py.
#ifndef DFU_CAPACITY
#define DFU_CAPACITY 0x190000  // bytes; the CI sets it to what the userland needs
#endif
#define STR2(x) #x
#define STR(x) STR2(x)

__asm__(
    ".section .rodata.dfu, \"a\"\n"
    ".balign 4\n"
    ".global dfu_slot\n"
    "dfu_slot:\n"
    ".ascii \"SALTYDFU_SLOT2\\0\\0\"\n"  // the marker the script searches for
    ".long " STR(DFU_CAPACITY) "\n"
    ".long 0\n"                          // model: set by the script
    ".long 0\n"                          // size of the records: set by the script
    ".fill " STR(DFU_CAPACITY) ", 1, 0xFF\n"
    ".text\n"
);
