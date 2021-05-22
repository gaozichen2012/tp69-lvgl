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
            page_switch(1);
        }
        else
        {
            test_flg = 0;
            page_switch(0);
        }
    }
}
