#include <stdint.h>
#include <stdbool.h>

#include "main.h"

#include "ili9341.h"
#include "ili9341_defs.h"

#define USE_DMA          0

#define LCD_CMD_ADDR     ((volatile uint16_t * const)0x60000000)
#define LCD_DATA_ADDR    ((volatile uint16_t * const)0x60080000)

#define BACK_LIGHT_ON()  HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)
#define BACK_LIGHT_OFF() HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET)

#define DELAY_MS(ms)     HAL_Delay((ms))// osDelay((ms))

#define WRITE_CMD(s, cmd)   ( *(s)->write_cmd_addr = (cmd) )
#define WRITE_DATA(s, data) ( *(s)->write_data_addr = (data) )

#if USE_DMA
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
#endif

struct ili9341_t {
    const uint16_t hor_res;
    const uint16_t ver_res;

    volatile uint16_t * const write_cmd_addr;
    volatile uint16_t * const write_data_addr;
    DMA_HandleTypeDef *hdma;
    
    ili9341_xfer_done_cb_t cb;
};

static ili9341_t s_ili9341 = {
    .hor_res = DISP_HOR_RES,
    .ver_res = DISP_VER_RES,
    .write_cmd_addr = LCD_CMD_ADDR,
    .write_data_addr = LCD_DATA_ADDR,
#if USE_DMA
    .hdma = &hdma_memtomem_dma2_stream0,
#endif
};

static inline void write_arr(ili9341_t *s, const uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        *s->write_data_addr = data[i];
    }
}

static inline void write_arr_word(ili9341_t *s, const uint16_t *data, uint32_t len) {
    for (uint32_t i = 0; i < len; ++i) {
        *s->write_data_addr = data[i];
    }
}

static void dma_cmpl(DMA_HandleTypeDef *hdma) {
    if (s_ili9341.cb) {
        s_ili9341.cb(&s_ili9341);
    }
}

static void dma_err(DMA_HandleTypeDef *hdma) {
    if (s_ili9341.cb) {
        s_ili9341.cb(&s_ili9341);
    }
}

/* Initialize the ILI9341 display controller */
ili9341_t* ili9341_init(ili9341_xfer_done_cb_t cb) {
    ili9341_t *disp = &s_ili9341;

    uint8_t data[15];

    /* hardware reset */
    DELAY_MS(50);

    /* software reset */
    WRITE_CMD(disp, ILI9341_SWRESET);
    DELAY_MS(5);
    WRITE_CMD(disp, ILI9341_DISPOFF);

    /* startup sequence */
    WRITE_CMD(disp, ILI9341_PWCTRLB);
    data[0] = 0x00;
    data[1] = 0x83;
    data[2] = 0x30;
    write_arr(disp, data, 3);

    WRITE_CMD(disp, ILI9341_PWSEQCTRL);
    data[0] = 0x64;
    data[1] = 0x03;
    data[2] = 0x12;
    data[3] = 0x81;
    write_arr(disp, data, 4);

    WRITE_CMD(disp, ILI9341_TIMECTRLA_INT);
    data[0] = 0x85;
    data[1] = 0x01;
    data[2] = 0x79;
    write_arr(disp, data, 3);

    WRITE_CMD(disp, ILI9341_PWCTRLA);
    data[0] = 0x39;
    data[1] = 0x2c;
    data[2] = 0x00;
    data[3] = 0x34;
    data[4] = 0x02;
    write_arr(disp, data, 5);

    WRITE_CMD(disp, ILI9341_PUMPRATIO);
    WRITE_DATA(disp, 0x20);

    WRITE_CMD(disp, ILI9341_TIMECTRLB);
    data[0] = 0x00;
    data[1] = 0x00;
    write_arr(disp, data, 2);

    /* power control */
    WRITE_CMD(disp, ILI9341_PWCTRL1);
    WRITE_DATA(disp, 0x26);

    WRITE_CMD(disp, ILI9341_PWCTRL2);
    WRITE_DATA(disp, 0x11);

    /* VCOM */
    WRITE_CMD(disp, ILI9341_VMCTRL1);
    data[0] = 0x35;
    data[1] = 0x3e;
    write_arr(disp, data, 2);

    WRITE_CMD(disp, ILI9341_VMCTRL2);
    WRITE_DATA(disp, 0xbe);

    /* set orientation */
    ili9341_rotate(disp, ILI9341_ORIENT_180);

    /* 16 bit pixel */
    WRITE_CMD(disp, ILI9341_PIXSET);
    WRITE_DATA(disp, 0x55);

    /* frame rate */
    WRITE_CMD(disp, ILI9341_FRMCTR1);
    data[0] = 0x00;
    data[1] = 0x1b;
    write_arr(disp, data, 2);

    /* gamma curve set */
    WRITE_CMD(disp, ILI9341_GAMSET);
    WRITE_DATA(disp, 0x01);

    /* positive gamma correction */
    WRITE_CMD(disp, ILI9341_PGAMCTRL);
    data[0]  = 0x1f;
    data[1]  = 0x1a;
    data[2]  = 0x18;
    data[3]  = 0x0a;
    data[4]  = 0x0f;
    data[5]  = 0x06;
    data[6]  = 0x45;
    data[7]  = 0x87;
    data[8]  = 0x32;
    data[9]  = 0x0a;
    data[10] = 0x07;
    data[11] = 0x02;
    data[12] = 0x07;
    data[13] = 0x05;
    data[14] = 0x00;
    write_arr(disp, data, 15);

    /* negative gamma correction */
    WRITE_CMD(disp, ILI9341_NGAMCTRL);
    data[0]  = 0x00;
    data[1]  = 0x25;
    data[2]  = 0x27;
    data[3]  = 0x05;
    data[4]  = 0x10;
    data[5]  = 0x09;
    data[6]  = 0x3a;
    data[7]  = 0x78;
    data[8]  = 0x4d;
    data[9]  = 0x05;
    data[10] = 0x18;
    data[11] = 0x0d;
    data[12] = 0x38;
    data[13] = 0x3a;
    data[14] = 0x1f;
    write_arr(disp, data, 15);

    /* window horizontal */
    WRITE_CMD(disp, ILI9341_CASET);
    data[0] = 0;
    data[1] = 0;
    data[2] = (uint8_t)((disp->hor_res - 1) >> 8);
    data[3] = (uint8_t)((disp->hor_res - 1));
    write_arr(disp, data, 4);

    /* window vertical */
    WRITE_CMD(disp, ILI9341_PASET);
    data[0] = 0;
    data[1] = 0;
    data[2] = (uint8_t)((disp->ver_res - 1) >> 8);
    data[3] = (uint8_t)((disp->ver_res - 1));
    write_arr(disp, data, 4);

    WRITE_CMD(disp, ILI9341_RAMWR);

    /* entry mode set */
    WRITE_CMD(disp, ILI9341_ETMOD);
    WRITE_DATA(disp, 0x07);

    /* display function control */
    WRITE_CMD(disp, ILI9341_DISCTRL);
    data[0] = 0x0a;
    data[1] = 0x82;
    data[2] = 0x27;
    data[3] = 0x00;
    write_arr(disp, data, 4);

    /* exit sleep mode */
    WRITE_CMD(disp, ILI9341_SLPOUT);

    DELAY_MS(100);

    /* display on */
    WRITE_CMD(disp, ILI9341_DISPON);

    DELAY_MS(20);
    BACK_LIGHT_ON();

    if (disp->hdma) {
        if ((HAL_DMA_RegisterCallback(disp->hdma, HAL_DMA_XFER_CPLT_CB_ID, &dma_cmpl) != HAL_OK) ||
            (HAL_DMA_RegisterCallback(disp->hdma, HAL_DMA_XFER_ERROR_CB_ID, &dma_err) != HAL_OK)) {
            return NULL;
        }
    }

    disp->cb = cb;

    return disp;
}

uint16_t ili9341_hor_res_get(ili9341_t *s) {
    return s->hor_res;
}

uint16_t ili9341_ver_res_get(ili9341_t *s) {
    return s->ver_res;
}

void ili9341_rotate(ili9341_t *s, ili9341_orient_t degrees) {
    WRITE_CMD(s, ILI9341_MADCTL);

    switch(degrees) {
        case ILI9341_ORIENT_0:   WRITE_DATA(s, MADCTL_MX | MADCTL_BGR); break;
        case ILI9341_ORIENT_90:  WRITE_DATA(s, MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR); break;
        case ILI9341_ORIENT_180: WRITE_DATA(s, MADCTL_MY | MADCTL_BGR); break;
        case ILI9341_ORIENT_270: WRITE_DATA(s, MADCTL_MV | MADCTL_BGR); break;
        
        default: break;
    }
}

void ili9341_window_set(ili9341_t *s, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2) {
    uint8_t data[4];

    /* window horizontal */
    WRITE_CMD(s, ILI9341_CASET);
    data[0] = x1 >> 8;
    data[1] = x1;
    data[2] = x2 >> 8;
    data[3] = x2;
    write_arr(s, data, 4);

    /* window vertical */
    WRITE_CMD(s,  ILI9341_PASET);
    data[0] = y1 >> 8;
    data[1] = y1;
    data[2] = y2 >> 8;
    data[3] = y2;
    write_arr(s, data, 4);

    WRITE_CMD(s, ILI9341_RAMWR);
}

void ili9341_write_color_buffer(ili9341_t *s, const uint16_t *buffer, uint32_t len) {
    if (s->hdma) {
        HAL_DMA_Start_IT(s->hdma, (uint32_t)buffer, (uint32_t)s->write_data_addr, len);
    } else {
        write_arr_word(s, buffer, len);
    }
}

void ili9341_fill_area(ili9341_t *s, uint16_t x1, uint16_t x2,
                       uint16_t y1, uint16_t y2, uint16_t color) {
    ili9341_window_set(s, x1, x2, y1, y2);

    WRITE_CMD(s, ILI9341_RAMWR);

    for (uint16_t x = x1; x <= x2; ++x) {
        for(uint16_t y = y1; y <= y2; ++y) {
            WRITE_DATA(s, color);
        }
    }
}
