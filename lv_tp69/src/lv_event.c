#include "../lv_tp69.h"
#include "lv_event.h"

#include "lv_startup.h"

void event_handler(lv_obj_t *obj, lv_event_t event)
{
    static unsigned char test_flg = 0;

    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");

        if (test_flg == 0)
        {
            test_flg = 1;
            lv_scr_load(menu_src);
        }
        else
        {
            test_flg = 0;
            lv_scr_load(desktop_src);
        }
    }
}
