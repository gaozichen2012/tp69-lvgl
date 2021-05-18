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

static lv_obj_t *tv;
static lv_obj_t *t1;
static lv_obj_t *t2;
static lv_obj_t *t3;

void lv_startup(void)
{
#if 1
    _DEBUG("tom test 9-1!\r\n");
    tv = lv_tabview_create(lv_scr_act(), NULL);
    _DEBUG("tom test 9-2!\r\n");
    t1 = lv_tabview_add_tab(tv, "Controls");
    _DEBUG("tom test 9-3!\r\n");
    t2 = lv_tabview_add_tab(tv, "Visuals");
    _DEBUG("tom test 9-4!\r\n");
    t3 = lv_tabview_add_tab(tv, "Selectors");
    _DEBUG("tom test 9-5!\r\n");
#else
    _DEBUG("tom test 8!\r\n");
    desktop_src = lv_obj_create(NULL, NULL);
    menu_src = lv_obj_create(NULL, NULL);
    menu_net_src = lv_obj_create(NULL, NULL);
    //lv_disp_set_default(desktop_src);
    //lv_scr_act()
    _DEBUG("tom test 8!\r\n");
    lv_desktop(desktop_src);
    _DEBUG("tom test 9!\r\n");
    lv_menu(menu_src);
    _DEBUG("tom test 10!\r\n");
    lv_menu_net(menu_net_src);
    _DEBUG("tom test 11!\r\n");
    lv_scr_load(desktop_src);
    _DEBUG("tom test 12!\r\n");
#endif
}
