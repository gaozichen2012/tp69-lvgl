#ifndef __LV_TP69_H
#define __LV_TP69_H

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
#include "../lvgl/lvgl.h"
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
#if LV_VERSION_CHECK(8, 0, 0) == 0
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

#endif /*LV_EXAMPLES_H*/
