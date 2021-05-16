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

void lv_startup(void)
{
    _DEBUG("tom test 8!\r\n");
    desktop_src = lv_obj_create(NULL, NULL);
    menu_src = lv_obj_create(NULL, NULL);
    menu_net_src = lv_obj_create(NULL, NULL);
    //lv_disp_set_default(desktop_src);
    //lv_scr_act()
#if 0 //测试修改字体无效
    lv_style_init(&style_box);
    lv_style_set_text_font(&style_box, LV_STATE_DEFAULT, &lv_font_montserrat_12); /*Set a larger font*/

    lv_obj_add_style(desktop_src, LV_OBJ_PART_MAIN, &style_box);
    lv_obj_add_style(menu_src, LV_OBJ_PART_MAIN, &style_box);
    lv_obj_add_style(menu_net_src, LV_OBJ_PART_MAIN, &style_box);

    lv_obj_add_style(desktop_src, LV_BTN_PART_MAIN, &style_box);
    lv_obj_add_style(menu_src, LV_BTN_PART_MAIN, &style_box);
    lv_obj_add_style(menu_net_src, LV_BTN_PART_MAIN, &style_box);
#endif
    _DEBUG("tom test 8!\r\n");
    lv_desktop(desktop_src);
    _DEBUG("tom test 9!\r\n");
    lv_menu(menu_src);
    _DEBUG("tom test 10!\r\n");
    lv_menu_net(menu_net_src);
    _DEBUG("tom test 11!\r\n");
    lv_scr_load(desktop_src);
    _DEBUG("tom test 12!\r\n");
}
