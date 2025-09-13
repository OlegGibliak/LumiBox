#ifndef ILI9341_H
#define ILI9341_H

#include <stdint.h>

#define DISP_HOR_RES      (240)
#define DISP_VER_RES      (320)

typedef enum {
    ILI9341_ORIENT_0,
    ILI9341_ORIENT_90,
    ILI9341_ORIENT_180,
    ILI9341_ORIENT_270
} ili9341_orient_t;

typedef struct ili9341_t ili9341_t;

typedef void (*ili9341_xfer_done_cb_t)(ili9341_t *s);

ili9341_t* ili9341_init(ili9341_xfer_done_cb_t cb);

uint16_t ili9341_hor_res_get(ili9341_t *s);
uint16_t ili9341_ver_res_get(ili9341_t *s);

void ili9341_rotate(ili9341_t *s ,ili9341_orient_t degrees);
void ili9341_window_set(ili9341_t *s, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
void ili9341_write_color_buffer(ili9341_t *s, const uint16_t *buffer, uint32_t len);

void ili9341_fill_area(ili9341_t *s, uint16_t x1, uint16_t x2,
                       uint16_t y1, uint16_t y2, uint16_t color);
#endif // ILI9341_H
