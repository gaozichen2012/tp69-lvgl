#ifndef __LV_STARTUP_H
#define __LV_STARTUP_H

extern lv_obj_t *desktop_src;
extern lv_obj_t *menu_src;
extern lv_obj_t *menu_net_src;
void event_handler(lv_obj_t *obj, lv_event_t event);

void lv_startup(void);

#endif
