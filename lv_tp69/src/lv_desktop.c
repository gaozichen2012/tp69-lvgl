#include "../lv_tp69.h"
#include "lv_desktop.h"

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

LV_IMG_DECLARE(freq_20x20);

static lv_obj_t *cont;

lv_obj_t *net_notification_bar(lv_obj_t *parent, lv_obj_t *obj_ref)
{
    lv_obj_t *cont;

    cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, 160, 16);
    lv_obj_align(cont, obj_ref, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_style_local_value_str(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "net_notification");
    return cont;
}

lv_obj_t *net_display_bar(lv_obj_t *parent, lv_obj_t *obj_ref)
{
    lv_obj_t *cont;

    cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, 160, 40);
    lv_obj_align(cont, obj_ref, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    // lv_obj_set_style_local_value_str(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "net_display");

    lv_obj_t *img1 = lv_img_create(cont, NULL);
    lv_img_set_src(img1, LV_SYMBOL_PLUS "Tom 1");
    lv_obj_align(img1, cont, LV_ALIGN_IN_TOP_LEFT, 20, 5);

    lv_obj_t *img2 = lv_img_create(cont, NULL);
    lv_img_set_src(img2, LV_SYMBOL_HOME "Group 1");
    lv_obj_align(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    return cont;
}
lv_obj_t *analog_notification_bar(lv_obj_t *parent, lv_obj_t *obj_ref)
{
    lv_obj_t *cont;

    cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, 160, 16);
    lv_obj_align(cont, obj_ref, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_local_value_str(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "analog_notification");
    return cont;
}

lv_obj_t *analog_display_bar(lv_obj_t *parent, lv_obj_t *obj_ref)
{
    lv_obj_t *cont;

    cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, 160, 40);
    lv_obj_align(cont, obj_ref, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t *img1 = lv_img_create(cont, NULL);
    lv_img_set_src(img1, &freq_20x20);
    lv_obj_align(img1, cont, LV_ALIGN_IN_LEFT_MID, 15, 0);

    lv_obj_t *label1 = lv_label_create(cont, NULL);
    lv_obj_align(label1, img1, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_label_set_text(label1, "432.12340");

    lv_obj_t *label2 = lv_label_create(cont, NULL);
    lv_obj_align(label2, label1, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
    lv_label_set_text(label2, "CH001");

    return cont;
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
    lv_label_set_text(label, "Menu");

    return cont;
}

void lv_desktop(lv_obj_t *parent)
{
    lv_obj_t *cont_1, *cont_2, *cont_3, *cont_4, *cont_5;

    //创建一个容器对象
    cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, 160, 128);
    // lv_obj_set_auto_realign(cont, true);                   /*Auto realign when the size changes*/
    // lv_obj_align_origo(cont, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
    // lv_cont_set_fit(cont, LV_FIT_TIGHT);
    // lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_MID);

    lv_obj_set_style_local_value_str(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Basics");
    cont_1 = net_notification_bar(cont, cont);
    cont_2 = net_display_bar(cont, cont_1);
    cont_3 = analog_notification_bar(cont, cont_2);
    cont_4 = analog_display_bar(cont, cont_3);

    cont_5 = bottom_bar(cont, cont_4);
}
