#include "../lv_tp69.h"
#include "lv_startup.h"

#include "lv_desktop.h"
#include "lv_menu.h"
#include "lv_menu_net.h"
#include "lv_event.h"

lv_obj_t *desktop_src;
lv_obj_t *menu_src;
lv_obj_t *menu_net_src;

static lv_style_t style_box;

void lv_startup(void)
{
    desktop_src = lv_obj_create(NULL, NULL);
    menu_src = lv_obj_create(NULL, NULL);
    menu_net_src = lv_obj_create(NULL, NULL);
    //lv_disp_set_default(desktop_src);
    //lv_scr_act()
#if 1 //测试修改字体无效
    lv_style_init(&style_box);
    lv_style_set_text_font(&style_box, LV_STATE_DEFAULT, &lv_font_montserrat_12); /*Set a larger font*/

    lv_obj_add_style(desktop_src, LV_OBJ_PART_MAIN, &style_box);
    lv_obj_add_style(menu_src, LV_OBJ_PART_MAIN, &style_box);
    lv_obj_add_style(menu_net_src, LV_OBJ_PART_MAIN, &style_box);

    lv_obj_add_style(desktop_src, LV_BTN_PART_MAIN, &style_box);
    lv_obj_add_style(menu_src, LV_BTN_PART_MAIN, &style_box);
    lv_obj_add_style(menu_net_src, LV_BTN_PART_MAIN, &style_box);
#endif

    lv_desktop(desktop_src);
    lv_menu(menu_src);
    lv_menu_net(menu_net_src);

    lv_scr_load(desktop_src);
}
