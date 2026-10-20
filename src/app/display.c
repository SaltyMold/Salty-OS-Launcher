#include "display.h"

#define DISPLAY_PREFIX        ">>> "
#define DISPLAY_PREFIX_LENGTH 4

const char* const display_messages[MSG_COUNT] = {
    [MSG_WELCOME_INSTALLER] = ">>> Welcome to Salty OS Installer!",
    [MSG_WELCOME_LAUNCHER] = ">>> Welcome to Salty OS Launcher!",
    [MSG_ERR_SLOT]       = ">>> Error : It seems that your calculator is not currently on slot A.",
    [MSG_FIX_SLOT]       = ">>> Fix : Update your calculator twice to make sure slot A is used.",
    [MSG_ERR_MODEL]      = ">>> Error : It seems that your calculator model differs from the version of the app you installed.",
    [MSG_FIX_MODEL]      = ">>> Fix : Reinstall the right version of Salty OS for your calculator model.",
    [MSG_READY]          = ">>> Ready to install Salty OS on your calculator, with some themes included.",
    [MSG_DATA_LOST]      = ">>> All data will be lost, continue?",
    [MSG_CONFIRM]        = ">>> Press OK to continue or BACK to cancel.",
    [MSG_ERASING_B]      = ">>> Erasing slot B...",
    [MSG_WRITING_B]      = ">>> Writing Salty OS to slot B...",
    [MSG_ERASING_DFU]    = ">>> Erasing app dfu...",
    [MSG_ERASING_THEMES] = ">>> Erasing theme area...",
    [MSG_WRITING_THEMES] = ">>> Writing themes to theme area...",
    [MSG_DONE]           = ">>> Installation complete!",
    [MSG_ASK_JUMP]       = ">>> Do you want to jump to Salty OS now?",
    [MSG_CONFIRM_JUMP]   = ">>> Press OK to jump or BACK to exit.",
    [MSG_JUMPING]        = ">>> Jumping to Salty OS...",
};

static char s_buf[DISPLAY_SCREEN_NB_LINES][DISPLAY_SCREEN_NB_COLS + 1];
static bool s_dirty[DISPLAY_SCREEN_NB_LINES];
static int  s_line = 0;
static int  s_col  = 0;

static void blank_line(int i) {
    memset(s_buf[i], ' ', DISPLAY_SCREEN_NB_COLS);
    s_buf[i][DISPLAY_SCREEN_NB_COLS] = '\0';
}

static void new_line(void) {
    s_col = 0;
    if (s_line < DISPLAY_SCREEN_NB_LINES - 1) {
        s_line++;
        return;
    }
    for (int i = 0; i < DISPLAY_SCREEN_NB_LINES - 1; i++) {
        memcpy(s_buf[i], s_buf[i + 1], DISPLAY_SCREEN_NB_COLS + 1);
        s_dirty[i] = true;
    }
    blank_line(DISPLAY_SCREEN_NB_LINES - 1);
    s_dirty[DISPLAY_SCREEN_NB_LINES - 1] = true;
}

void display_draw_status_bar(const char* title) {
    eadk_display_push_rect_uniform((eadk_rect_t){0, 0, DISPLAY_SCREEN_WIDTH, DISPLAY_BAR_HEIGHT}, DISPLAY_BAR_COLOR);
    int x = (DISPLAY_SCREEN_WIDTH - (int)strlen(title) * CHAR_WIDTH_SMALL) / 2;
    eadk_display_draw_string(title, (eadk_point_t){x, 3}, false, eadk_color_white, DISPLAY_BAR_COLOR);
}

void display_clear_screen(void) {
    eadk_display_push_rect_uniform((eadk_rect_t){0, DISPLAY_BAR_HEIGHT, DISPLAY_SCREEN_WIDTH, DISPLAY_SCREEN_HEIGHT}, eadk_color_white);
    for (int i = 0; i < DISPLAY_SCREEN_NB_LINES; i++) {
        blank_line(i);
        s_dirty[i] = false;
    }
    s_line = 0;
    s_col = 0;
}

void display_refresh(void) {
    for (int i = 0; i < DISPLAY_SCREEN_NB_LINES; i++) {
        if (!s_dirty[i]) continue;
        eadk_display_draw_string(s_buf[i], (eadk_point_t){0, DISPLAY_BAR_HEIGHT + i * CHAR_HEIGHT_SMALL}, false, eadk_color_black, eadk_color_white);
        s_dirty[i] = false;
    }
}

static void draw_char(int line, int col, char c) {
    char str[2] = {c, '\0'};
    eadk_display_draw_string(str, (eadk_point_t){col * CHAR_WIDTH_SMALL, DISPLAY_BAR_HEIGHT + line * CHAR_HEIGHT_SMALL}, false, eadk_color_black, eadk_color_white);
}

static void put_char(char c) {
    s_buf[s_line][s_col] = c;
    draw_char(s_line, s_col, c);
    s_col++;
    if (DISPLAY_TYPE_DELAY_MS > 0 && c != ' ') eadk_timing_msleep(DISPLAY_TYPE_DELAY_MS);
}

static void wrap(int indent) {
    new_line();
    display_refresh();
    s_col = indent;
}


void display_print(const char* text) {
    const int indent = (strncmp(text, DISPLAY_PREFIX, DISPLAY_PREFIX_LENGTH) == 0) ? DISPLAY_PREFIX_LENGTH : 0;
    int start = 0;

    if (s_col > 0) new_line();
    display_refresh();

    while (*text) {
        if (*text == '\n') {
            wrap(indent);
            start = indent;
            text++;
            continue;
        }
        if (*text == ' ') {
            if (s_col != start && s_col < DISPLAY_SCREEN_NB_COLS) put_char(' ');
            text++;
            continue;
        }

        int len = 0;
        while (text[len] && text[len] != ' ' && text[len] != '\n') len++;

        if (s_col + len > DISPLAY_SCREEN_NB_COLS && s_col > start) {
            wrap(indent);
            start = indent;
        }
        for (int i = 0; i < len; i++) {
            if (s_col == DISPLAY_SCREEN_NB_COLS) { 
                wrap(indent);
                start = indent;
            }
            put_char(text[i]);
        }
        text += len;
    }
}

void display_message(display_msg_t id) {
    if (id < MSG_COUNT) display_print(display_messages[id]);
}