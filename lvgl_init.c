#include "lv_tp69/lv_tp69.h"
#include "lvgl_init.h"
#include "lcd.h"
#include "gpio.h"

#include "ql_lcd.h"

#include <stdio.h>
#include <string.h>
#define DEBUG_EN 1
#define DEBUG_TO_USB 0
#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

extern unsigned short usMap[LCD_WIDTH * LCD_HEIGHT];
static void ili9341_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
#if 1
    memcpy(usMap, color_p, LCD_WIDTH * LCD_HEIGHT * 2);
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

#if 1 //Input device interface

/* Initialize your buttons */
static void button_init(void)
{
    /*Your code comes here*/
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

    if (id == 0) //ok
    {
        if (get_key_ok_state() == 1)
            return true;
    }
    else if (id == 1) //back
    {
        if (get_key_back_state() == 1)
            return true;
    }
    else if (id == 2) //up
    {
        if (get_key_up_state() == 1)
            return true;
    }
    else if (id == 3) //down
    {
        if (get_key_down_state() == 1)
            return true;
    }
    else
    {
        /* code */
    }

    return false;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for (i = 0; i < 4; i++)
    {
        /*Return the pressed button's ID*/
        if (button_is_pressed(i))
        {
            return i;
        }
    }

    /*No button pressed*/
    return -1;
}

/* Will be called by the library to read the button */
static bool button_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{

    static uint8_t last_btn = 0; /*Store the last pressed button*/

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if (btn_act >= 0)
    {                                    /*Is there a button press? (E.g. -1 indicated no button was pressed)*/
        data->state = LV_INDEV_STATE_PR; /*Save the ID of the pressed button*/
        last_btn = btn_act;              /*Set the pressed state*/
    }
    else
    {
        data->state = LV_INDEV_STATE_REL; /*Set the released state*/
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

#endif

void lvgl_hal_init(void)
{
#if 0   //reflesh type 1
    /*A static or global variable to store the buffers*/
    static lv_disp_buf_t disp_buf;

    /*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t buf_1[LV_HOR_RES_MAX * 10];

    /*Initialize `disp_buf` with the buffer(s) */
    lv_disp_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * 10);
#elif 0 //reflesh type 2
    /*A static or global variable to store the buffers*/
    static lv_disp_buf_t disp_buf;

    /*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t buf_1[LV_HOR_RES_MAX * 10];
    static lv_color_t buf_2[LV_HOR_RES_MAX * 10];

    /*Initialize `disp_buf` with the buffer(s) */
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * 10);
#else   //reflesh type 3
    /*A static or global variable to store the buffers*/
    static lv_disp_buf_t disp_buf; //包含内部图形缓冲区。

    /*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t buf_1[LCD_WIDTH * LCD_HEIGHT];
    static lv_color_t buf_2[LCD_WIDTH * LCD_HEIGHT];

    /*Initialize `disp_buf` with the buffer(s) */
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, LCD_WIDTH * LCD_HEIGHT);
#endif

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
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;

    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = ili9341_flush_cb;
    lv_disp_drv_register(&disp_drv);

    //button_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv); /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_BUTTON;
    indev_drv.read_cb = button_read;
    /*Register the driver in LVGL and save the created input device object*/
    lv_indev_t *my_indev = lv_indev_drv_register(&indev_drv);

    /*Assign buttons to points on the screen*/
    static const lv_point_t btn_points[4] = {
        {0 + 16, 128 - 8},        /*Button 0 -> x:10; y:10*/
        {160 - 16, 128 - 8},      /*Button 1 -> x:40; y:100*/
        {32 + 16, 128 - 8},       /*Button 1 -> x:40; y:100*/
        {160 - 32 - 16, 128 - 8}, /*Button 1 -> x:40; y:100*/
    };
    lv_indev_set_button_points(my_indev, btn_points);
}
