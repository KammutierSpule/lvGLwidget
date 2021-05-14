// Copyright (c) 2020 Mario Luzeiro
// MIT License

#include "lvgl_wrap.h"
#include <lvgl/lvgl.h>
#include <lv_hal/lv_hal_disp.h>

#include "../../lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"

static uint32_t * s_ptrPBO = NULL;

uint32_t LVGLWrap_Handler(uint32_t * apPBO)
{
    s_ptrPBO = apPBO;

    return lv_task_handler();
}

static uint32_t s_currentTime_ms = 0;

void LVGLWrap_SetMillis(uint32_t aCurrentMiliseconds)
{
    s_currentTime_ms = aCurrentMiliseconds;
}

uint32_t LVGLWrap_GetMillis()
{
    return s_currentTime_ms;
}

static bool s_left_button_down = false;
static int16_t s_last_x = 0;
static int16_t s_last_y = 0;

static bool lvglwrap_mouse_read(struct _lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    /*Store the collected data*/
    data->point.x = s_last_x;
    data->point.y = s_last_y;
    data->state = s_left_button_down ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    return false;
}

static void lvglwrap_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if((area->x1 < 0) || (area->y1 < 0) ||
       (area->x2 < 0) || (area->y2 < 0) ||
       (area->x1 > (LV_HOR_RES_MAX - 1)) || (area->y1 > (LV_VER_RES_MAX - 1)) ||
       (area->x2 > (LV_HOR_RES_MAX - 1)) || (area->y2 > (LV_VER_RES_MAX - 1))) {
        lv_disp_flush_ready(disp_drv);

        return;
    }

    uint32_t sizeX = (area->x2 - area->x1) + 1;

#if LV_COLOR_DEPTH != 24 || LV_COLOR_DEPTH != 32    /*32 is valid but support 24 for backward compatibility too*/
    for(int32_t y = area->y1; y <= area->y2; y++) {
        uint32_t * ptrPBO = &s_ptrPBO[(LV_VER_RES_MAX - 1 - y) * LV_HOR_RES_MAX + area->x1];

        uint32_t x = sizeX;
        do {
            *ptrPBO = lv_color_to32(*color_p);
            ptrPBO++;
            color_p++;
            x--;
        } while(x > 0);
    }
#else
    // !TODO:
    /*
    uint32_t w = x2 - x1 + 1;
    for(y = y1; y <= y2; y++) {
        memcpy(&tft_fb[y * MONITOR_HOR_RES + x1], color_p, w * sizeof(lv_color_t));

        color_p += w;
    }
    */
#endif

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

#define BUFFER_PIXEL_SIZE (LV_HOR_RES_MAX * 10)
static lv_disp_buf_t s_disp_buf_1;
static lv_color_t s_buf1_1[BUFFER_PIXEL_SIZE];

void LVGLWrap_Init()
{
    lv_init();

    lv_disp_buf_init(&s_disp_buf_1, s_buf1_1, NULL, BUFFER_PIXEL_SIZE);

    lv_disp_drv_t disp_drv;

    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;

    disp_drv.buffer = &s_disp_buf_1;

    disp_drv.flush_cb = lvglwrap_flush;

    lv_disp_drv_register(&disp_drv);

    s_left_button_down = false;
    s_last_x = 0;
    s_last_y = 0;

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);            /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvglwrap_mouse_read;
    lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);

    lv_demo_widgets();
}


void LVGLWrap_MouseMoveEvent(int16_t x, int16_t y)
{
    s_last_x = x;
    s_last_y = y;
}

void LVGLWrap_MousePressEvent(bool isPressed)
{
    s_left_button_down = isPressed;
}
