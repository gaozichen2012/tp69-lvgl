#ifndef __LVGL_INIT_H
#define __LVGL_INIT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#if defined(LV_EX_CONF_PATH)
#define __LV_TO_STR_AUX(x) #x
#define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
#include __LV_TO_STR(LV_EX_CONF_PATH)
#undef __LV_TO_STR_AUX
#undef __LV_TO_STR
#elif defined(LV_EX_CONF_INCLUDE_SIMPLE)
#include "lv_ex_conf.h"
#else
#include "../lv_ex_conf.h"
#endif

#include "src/lv_startup.h"
#include "src/lv_event.h"
#include "src/lv_desktop.h"
#include "src/lv_menu.h"
#include "src/lv_menu_net.h"
/*********************
 *      DEFINES
 *********************/
/*Test  lvgl version*/
#if LV_VERSION_CHECK(7, 5, 0) == 0
#error "lv_examples: Wrong lvgl version"
#endif

    /**********************
 *      TYPEDEFS
 **********************/

    /**********************
 * GLOBAL PROTOTYPES
 **********************/

    /**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif