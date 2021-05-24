#include "../lv_tp69.h"
#include "lv_menu.h"

static lv_obj_t *list1;
static lv_obj_t *list_btn1, *list_btn2, *list_btn3, *list_btn4;

static unsigned char menu_sel = 0;

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

        page_switch(2);
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

        page_switch(0);
    }
}

static void btn_up_event_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");

        if (menu_sel > 0)
        {
            menu_sel--;
        }

        switch (menu_sel)
        {
        case 0:
            lv_list_focus_btn(list1, list_btn1);
            break;
        case 1:
            lv_list_focus_btn(list1, list_btn2);
            break;
        case 2:
            lv_list_focus_btn(list1, list_btn3);
            break;
        case 3:
            lv_list_focus_btn(list1, list_btn4);
            break;
        default:
            break;
        }
    }
}

static void btn_down_event_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");

        if (menu_sel < 3)
        {
            menu_sel++;
        }

        switch (menu_sel)
        {
        case 0:
            lv_list_focus_btn(list1, list_btn1);
            break;
        case 1:
            lv_list_focus_btn(list1, list_btn2);
            break;
        case 2:
            lv_list_focus_btn(list1, list_btn3);
            break;
        case 3:
            lv_list_focus_btn(list1, list_btn4);
            break;
        default:
            break;
        }
    }
}

static lv_obj_t *bottom_bar(lv_obj_t *parent, lv_obj_t *obj_ref)
{
    lv_obj_t *cont;

    cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, 160, 16);
    lv_obj_align(cont, obj_ref, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t *btn1 = lv_btn_create(cont, NULL);
    lv_obj_set_event_cb(btn1, btn_ok_event_cb);
    lv_obj_set_size(btn1, 32, 16);
    lv_obj_align(btn1, cont, LV_ALIGN_IN_LEFT_MID, 0, 0);
    lv_btn_set_checkable(btn1, true);
    lv_btn_toggle(btn1);
    lv_obj_t *label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Enter");

    lv_obj_t *btn4 = lv_btn_create(cont, NULL);
    lv_obj_set_event_cb(btn4, btn_back_event_cb);
    lv_obj_set_size(btn4, 32, 16);
    lv_obj_align(btn4, cont, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    lv_btn_set_checkable(btn4, true);
    lv_btn_toggle(btn4);
    label = lv_label_create(btn4, NULL);
    lv_label_set_text(label, "Back");

    lv_obj_t *btn2 = lv_btn_create(cont, NULL);
    lv_obj_set_event_cb(btn2, btn_up_event_cb);
    lv_obj_set_size(btn2, 32, 16);
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_btn_set_checkable(btn2, true);
    lv_btn_toggle(btn2);
    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Up");

    lv_obj_t *btn3 = lv_btn_create(cont, NULL);
    lv_obj_set_event_cb(btn3, btn_down_event_cb);
    lv_obj_set_size(btn3, 32, 16);
    lv_obj_align(btn3, btn4, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    lv_btn_set_checkable(btn3, true);
    lv_btn_toggle(btn3);
    label = lv_label_create(btn3, NULL);
    lv_label_set_text(label, "Down");

    return cont;
}

void lv_menu(lv_obj_t *parent)
{
    /*Create a list*/
    list1 = lv_list_create(parent, NULL);
    lv_obj_set_size(list1, 160, 128 - 16);
    lv_obj_align(list1, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    /*Add buttons to the list*/
    list_btn1 = lv_list_add_btn(list1, LV_SYMBOL_VIDEO, "Anglog menu");
    lv_obj_set_event_cb(list_btn1, list_event_handler);

    list_btn2 = lv_list_add_btn(list1, LV_SYMBOL_HOME, "Poc menu");
    lv_obj_set_event_cb(list_btn2, list_event_handler);

    list_btn3 = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Mode Switch");
    lv_obj_set_event_cb(list_btn3, list_event_handler);

    list_btn4 = lv_list_add_btn(list1, LV_SYMBOL_SETTINGS, "General settings");
    lv_obj_set_event_cb(list_btn4, list_event_handler);

    bottom_bar(parent, list1);
}
