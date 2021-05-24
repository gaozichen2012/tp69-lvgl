#include "../lv_tp69.h"
#include "lv_startup.h"

#include "lv_desktop.h"
#include "lv_menu.h"
#include "lv_menu_net.h"
#include "lv_event.h"

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

lv_obj_t *desktop_src;
lv_obj_t *menu_src;
lv_obj_t *menu_net_src;

static lv_style_t style_box;

void page_switch(unsigned char page)
{
    switch (page)
    {
    case 0:
        //lv_obj_clean(desktop_src);
        lv_obj_clean(menu_src);
        lv_obj_clean(menu_net_src);
        lv_desktop(desktop_src);
        lv_scr_load(desktop_src);
        break;
    case 1:
        lv_obj_clean(desktop_src);
        //lv_obj_clean(menu_src);
        lv_obj_clean(menu_net_src);
        lv_menu(menu_src);
        lv_scr_load(menu_src);
        break;
    case 2:
        lv_obj_clean(desktop_src);
        lv_obj_clean(menu_src);
        //lv_obj_clean(menu_net_src);
        lv_menu_net(menu_net_src);
        lv_scr_load(menu_net_src);
        break;
    case 3:
        /* code */
        break;

    default:
        break;
    }
}

void lv_startup(void)
{
#if 1
    desktop_src = lv_obj_create(NULL, NULL);
    menu_src = lv_obj_create(NULL, NULL);
    menu_net_src = lv_obj_create(NULL, NULL);
#if 1 //测试修改字体无效
    lv_style_init(&style_box);
    //lv_style_set_text_font(&style_box, LV_STATE_DEFAULT, &lv_font_montserrat_14); /*Set a larger font*/

    lv_obj_add_style(desktop_src, LV_OBJ_PART_MAIN, &style_box);
    lv_obj_add_style(menu_src, LV_OBJ_PART_MAIN, &style_box);
    lv_obj_add_style(menu_net_src, LV_OBJ_PART_MAIN, &style_box);

    lv_obj_add_style(desktop_src, LV_BTN_PART_MAIN, &style_box);
    lv_obj_add_style(menu_src, LV_BTN_PART_MAIN, &style_box);
    lv_obj_add_style(menu_net_src, LV_BTN_PART_MAIN, &style_box);
#endif

    //page_switch(0);
#else
    desktop_src = lv_obj_create(NULL, NULL);
    menu_src = lv_obj_create(NULL, NULL);
    menu_net_src = lv_obj_create(NULL, NULL);
    //lv_disp_set_default(desktop_src);
    //lv_scr_act()
#if 1 //测试修改字体无效
    lv_style_init(&style_box);
    //lv_style_set_text_font(&style_box, LV_STATE_DEFAULT, &lv_font_montserrat_14); /*Set a larger font*/

    lv_obj_add_style(desktop_src, LV_OBJ_PART_MAIN, &style_box);
    lv_obj_add_style(menu_src, LV_OBJ_PART_MAIN, &style_box);
    lv_obj_add_style(menu_net_src, LV_OBJ_PART_MAIN, &style_box);

    lv_obj_add_style(desktop_src, LV_BTN_PART_MAIN, &style_box);
    lv_obj_add_style(menu_src, LV_BTN_PART_MAIN, &style_box);
    lv_obj_add_style(menu_net_src, LV_BTN_PART_MAIN, &style_box);
#endif

    page_switch(0);
#endif
}
