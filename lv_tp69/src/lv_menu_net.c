#include "../lv_tp69.h"
#include "lv_menu_net.h"

static void list_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked: %s\n", lv_list_get_btn_text(obj));
    }
}

static void btn_ok_event_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");

        page_switch(1);
    }
}

static void btn_back_event_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");

        page_switch(1);
    }
}

static lv_obj_t *bottom_bar(lv_obj_t *parent, lv_obj_t *obj_ref)
{
    lv_obj_t *cont;

    cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, 160, 16);
    lv_obj_align(cont, obj_ref, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t *btn = lv_btn_create(cont, NULL);
    lv_obj_set_event_cb(btn, btn_ok_event_cb);
    lv_obj_set_size(btn, 32, 16);
    lv_obj_align(btn, cont, LV_ALIGN_IN_LEFT_MID, 0, 0);
    lv_btn_set_checkable(btn, true);
    lv_btn_toggle(btn);
    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Enter");

    btn = lv_btn_create(cont, NULL);
    lv_obj_set_event_cb(btn, btn_back_event_cb);
    lv_obj_set_size(btn, 32, 16);
    lv_obj_align(btn, cont, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    lv_btn_set_checkable(btn, true);
    lv_btn_toggle(btn);
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Back");

    return cont;
}

void lv_menu_net(lv_obj_t *parent)
{
    /*Create a list*/
    lv_obj_t *list1 = lv_list_create(parent, NULL);
    lv_obj_set_size(list1, 160, 128 - 16);
    lv_obj_align(list1, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    /*Add buttons to the list*/
    lv_obj_t *list_btn;

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_VIDEO, "group select");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_HOME, "member select");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "friend select");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_GPS, "GPS");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_BELL, "record");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_SETTINGS, "POC settings");
    lv_obj_set_event_cb(list_btn, list_event_handler);

    bottom_bar(parent, list1);
}
