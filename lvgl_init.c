#include "lv_tp69/lv_tp69.h"
#include "lvgl_init.h"
#include "lcd.h"

extern unsigned short usMap[LCD_WIDTH * LCD_HEIGHT];

lv_color_t test_fb[LV_HOR_RES_MAX * LV_VER_RES_MAX];

static void ILI9341_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
#if 1
    LV_UNUSED(area);
    LV_UNUSED(color_p);

    memcpy(usMap, color_p, lv_area_get_size(area) * sizeof(lv_color_t));
    lcd_update_disp();

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
#else
    LV_UNUSED(area);
    LV_UNUSED(color_p);

    memcpy(usMap, color_p, lv_area_get_size(area) * sizeof(lv_color_t));
    lcd_update_disp();

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
#endif
}

#if 1
void lvgl_hal_init(void)
{
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf_1[LV_HOR_RES_MAX * 10];
    lv_disp_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * 10);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = ILI9341_flush;
    lv_disp_drv_register(&disp_drv);
}
#else
void lvgl_hal_init(void)
{
    /*A static or global variable to store the buffers*/
    static lv_disp_buf_t disp_buf; //包含内部图形缓冲区。

    /*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
    static lv_color_t buf_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];

    /*Initialize `disp_buf` with the buffer(s) */
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);

    /* 
        Display buffer准备就绪，就需要初始化Display driver 
        在最简单的情况下，仅lv_disp_drv_t需要设置以下两个字段：
        1.buffer 指向初始化lv_disp_buf_t变量的缓冲区指针。
        2.flush_cb 一个回调函数，用于将缓冲区的内容复制到显示的特定区域。
        lv_disp_flush_ready()冲洗准备就绪时需要调用它。
        LVGL可能以多个块呈现屏幕，因此会flush_cb多次调用。
        要查看哪一个是渲染的最后一块，请使用lv_disp_flush_is_last()。
    */

    lv_disp_drv_t disp_drv; //包含回调函数，可与显示交互并处理与图形相关的事物。
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = ILI9341_flush;
    lv_windows_disp = lv_disp_drv_register(&disp_drv);
}
#endif