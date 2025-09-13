#include <stdio.h>
#include <stdint.h>

#include "gui.h"
#include "gui_log.h"
#include "lvgl.h"
#include "macros.h"

#include "ili9341.h"

#define DISP_BUFFER_SIZE_PIXEL  (DISP_HOR_RES * 10)

typedef struct {
    ili9341_t *ili9341_drv;

    lv_disp_draw_buf_t draw_buf_dsc;
    lv_color_t disp_buffer[DISP_BUFFER_SIZE_PIXEL];   /*A buffer for 10 rows*/
    
    lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
} gui_disp_t;

static gui_disp_t s_gui_disp;

static void disp_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    if(area->x2 < 0 || area->y2 < 0 || area->x1 > (DISP_HOR_RES - 1) || area->y1 > (DISP_VER_RES - 1)) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    /* Truncate the area to the screen */
    int32_t act_x1 = MAX(area->x1, 0);
    int32_t act_y1 = MAX(area->y1, 0);
    int32_t act_x2 = MIN(area->x2, DISP_HOR_RES - 1);
    int32_t act_y2 = MIN(area->y2, DISP_VER_RES - 1);

    
    int32_t len = (act_x2 - act_x1 + 1);
    lv_coord_t w = (area->x2 - area->x1) + 1;

    ili9341_window_set(s_gui_disp.ili9341_drv, act_x1, act_x2, act_y1, act_y2);

    for(int32_t y = act_y1; y <= act_y2; y++) {
        ili9341_write_color_buffer(s_gui_disp.ili9341_drv, &color_p->full, len);
        color_p += w;
    }

    lv_disp_flush_ready(disp_drv);
}

static void lv_port_disp_init(gui_disp_t *gui_disp) {
    /*Initialize the display buffer*/
    lv_disp_draw_buf_init(&gui_disp->draw_buf_dsc, gui_disp->disp_buffer, NULL, DISP_BUFFER_SIZE_PIXEL);
 
    lv_disp_drv_init(&gui_disp->disp_drv);   /*Basic initialization*/

    /*Set the resolution of the display*/
    gui_disp->disp_drv.hor_res = DISP_HOR_RES;
    gui_disp->disp_drv.ver_res = DISP_VER_RES;

    /*Set a display buffer*/
    gui_disp->disp_drv.draw_buf = &gui_disp->draw_buf_dsc;

    /*Used to copy the buffer's content to the display*/
    gui_disp->disp_drv.flush_cb = disp_flush_cb;
    

    // disp_drv.full_refresh = 1;

    /*Finally register the driver*/
    lv_disp_drv_register(&gui_disp->disp_drv);
}

void gui_process_task(void *arg) {
    s_gui_disp.ili9341_drv = ili9341_init(NULL);
    lv_port_disp_init(&s_gui_disp);

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    // gui_log_terminal_create(lv_scr_act());
    // gui_log_append("first\r\n");
    // gui_log_append("second\r\n");

    // /*Create a spinner*/
    lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
    lv_obj_set_size(spinner, 64, 64);
    lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, 0);

    for (;;) {
        lv_timer_handler();
        HAL_Delay(5);
    }
}

void gui_init(void) {
    
    lv_init();
}


