#ifndef DISPLAY_H
#define DISPLAY_H

#include "../libs/eadk.h"

#define DISPLAY_BAR_HEIGHT 18
#define DISPLAY_BAR_COLOR  0xFDA6

#define DISPLAY_SCREEN_HEIGHT (EADK_SCREEN_HEIGHT - DISPLAY_BAR_HEIGHT) // 222
#define DISPLAY_SCREEN_WIDTH  EADK_SCREEN_WIDTH                         // 320

#define CHAR_WIDTH_SMALL  7
#define CHAR_HEIGHT_SMALL 14 // 12 px + 2 px of padding

#define DISPLAY_TYPE_DELAY_MS 4

#define DISPLAY_SCREEN_NB_LINES (DISPLAY_SCREEN_HEIGHT / CHAR_HEIGHT_SMALL) // 15
#define DISPLAY_SCREEN_NB_COLS  (DISPLAY_SCREEN_WIDTH / CHAR_WIDTH_SMALL)   // 45

typedef enum {
  MSG_WELCOME_INSTALLER,
  MSG_WELCOME_LAUNCHER,
  MSG_ERR_SLOT,
  MSG_FIX_SLOT,
  MSG_ERR_MODEL,
  MSG_FIX_MODEL,
  MSG_READY,
  MSG_DATA_LOST,
  MSG_CONFIRM,
  MSG_ERASING_B,
  MSG_WRITING_B,
  MSG_ERASING_DFU,
  MSG_ERASING_THEMES,
  MSG_WRITING_THEMES,
  MSG_DONE,
  MSG_ASK_JUMP,
  MSG_CONFIRM_JUMP,
  MSG_JUMPING,
  MSG_COUNT
} display_msg_t;

extern const char* const display_messages[MSG_COUNT];

void display_draw_status_bar(const char* title);
void display_clear_screen(void);

void display_print(const char* text);
void display_message(display_msg_t id);
void display_refresh(void);

#endif