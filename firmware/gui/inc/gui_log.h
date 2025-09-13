#ifndef GUI_LOG_H
#define GUI_LOG_H

#include "lvgl.h"

void gui_log_terminal_create(lv_obj_t *parent);
void gui_log_append(const char *fmt, ...);

#endif // GUI_LOG_H
