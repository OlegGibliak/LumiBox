#include <stdio.h>

#include "lvgl.h"

#define LOG_BUF_MAX 4096

// Рекомендовані кольори (наближено до Yaru Dark)
#define UB_BG      0x111217  // фон
#define UB_TEXT    0xEAEAEA  // текст
#define UB_CURSOR  0x33D17A  // зелений курсор
#define UB_SCROLL  0x3A3C41  // смуга прокрутки

static lv_obj_t *log_ta;
static size_t log_len = 0;
#if 0
void gui_log_append(const char *txt)
{
    size_t add = strlen(txt);


    while (log_len + add > LOG_BUF_MAX) {
        lv_textarea_set_cursor_pos(log_ta, 0);
        lv_textarea_del_char_forward(log_ta);
        log_len--;
    }

    lv_textarea_set_cursor_pos(log_ta, LV_TEXTAREA_CURSOR_LAST);
    lv_textarea_add_text(log_ta, txt);
    lv_textarea_set_cursor_pos(log_ta, LV_TEXTAREA_CURSOR_LAST);

    log_len += add;
}
#else
void gui_log_append(const char *fmt, ...)
{
    if(!log_ta || !fmt) return;

    // 1) Обчислити потрібну довжину
    va_list ap;
    va_start(ap, fmt);
    int need = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (need <= 0) return;

    // +1 для '\0'
    size_t buflen = (size_t)need + 1u;
    char *buf = (char*)lv_mem_alloc(buflen);
    if(!buf) return;

    va_start(ap, fmt);
    vsnprintf(buf, buflen, fmt, ap);
    va_end(ap);

    // 2) Трім буфера textarea, щоб вліз новий текст
    // trim_head_if_needed((size_t)need);

    // 3) Додати текст у кінець
    lv_textarea_set_cursor_pos(log_ta, LV_TEXTAREA_CURSOR_LAST);
    lv_textarea_add_text(log_ta, buf);
    log_len += (size_t)need;

    // 4) Автоскрол у кінець
    lv_textarea_set_cursor_pos(log_ta, LV_TEXTAREA_CURSOR_LAST);
    // додатково гарантуємо прокрутку вниз (без анімації)
    lv_obj_scroll_to_y(log_ta, LV_COORD_MAX, LV_ANIM_OFF);

    lv_mem_free(buf);
}
    #endif

void gui_log_terminal_create(lv_obj_t *parent)
{
    log_ta = lv_textarea_create(parent);
    lv_obj_set_size(log_ta, LV_PCT(100), LV_PCT(100));
    lv_textarea_set_text(log_ta, "");
    lv_obj_add_state(log_ta, LV_STATE_DISABLED);          // read-only вид
    lv_textarea_set_cursor_click_pos(log_ta, false);
    lv_obj_set_scrollbar_mode(log_ta, LV_SCROLLBAR_MODE_AUTO);

    // --- ФОН і текст (головна частина) ---
    lv_obj_set_style_bg_color(log_ta, lv_color_hex(UB_BG), LV_PART_MAIN);
    lv_obj_set_style_bg_opa  (log_ta, LV_OPA_COVER,         LV_PART_MAIN);
    lv_obj_set_style_text_color(log_ta, lv_color_hex(UB_TEXT), LV_PART_MAIN);
    lv_obj_set_style_border_width(log_ta, 0, LV_PART_MAIN);       // без рамок
    lv_obj_set_style_radius(log_ta, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_all(log_ta, 6, LV_PART_MAIN);            // внутр. відступи

    // --- Курсор як блок (вигляд терміналу) ---
    lv_obj_set_style_bg_opa(log_ta, LV_OPA_TRANSP, LV_PART_CURSOR);
    lv_obj_set_style_border_opa(log_ta, LV_OPA_TRANSP, LV_PART_CURSOR);

    // --- Смуга прокрутки (тонка, майже непомітна) ---
    lv_obj_set_style_bg_color(log_ta, lv_color_hex(UB_SCROLL), LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa  (log_ta, LV_OPA_50,               LV_PART_SCROLLBAR);
    lv_obj_set_style_width   (log_ta, 4,                        LV_PART_SCROLLBAR);
    lv_obj_set_style_radius  (log_ta, 2,                        LV_PART_SCROLLBAR);

    // (опційно) зменшити інерцію/скрол швидкість під “термінальний” вайб
    lv_obj_set_scroll_dir(log_ta, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(log_ta, LV_SCROLLBAR_MODE_ACTIVE);
}
