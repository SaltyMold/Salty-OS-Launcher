#ifndef THEMES_SIZE
#error "THEMES_SIZE is not defined: build with make (see Makefile)"
#endif
#define STR2(x) #x
#define STR(x) STR2(x)

__asm__(
    ".section .rodata.themes, \"a\"\n"
    ".balign 4\n"
    ".global themes_size\n"
    "themes_size:\n"
    ".long " STR(THEMES_SIZE) "\n"
    ".global themes_data\n"
    "themes_data:\n"
    ".incbin \"data/theme_area.bin\", 0, " STR(THEMES_SIZE) "\n"
    ".text\n"
);