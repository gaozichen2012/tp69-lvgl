#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ql_application.h"
#include "quec_cust_feature.h"
#include "ql_rtos.h"
#include "ql_tts.h"
#include "ql_audio.h"
#include "ql_power.h"
#include "ql_platform.h"
#include "ql_nw.h"
#include "ql_atcmd.h"
#include "ql_lcd.h"

#include "gpio.h"
#include "lcd.h"
#include "font.h"

#include "lv_tp69/lv_tp69.h"
#include "lvgl_init.h"

#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

#define MCU_SPLASH_VALID_FLG_VALUE 0x5a5a5a5a
#define MCU_SPLASH_CFG_OFFSET 40960 + 128

typedef struct
{
	unsigned short usWidth;
	unsigned short usHeight;

	unsigned int uiOffset;
	unsigned int uiValidFlg;
} stSplashCfg;

int iWakelockFd;
ql_queue_t msqid;

extern const unsigned char gImage_splash[LCD_WIDTH * LCD_HEIGHT * 2];

ql_task_t tick_task_ref;

static void tick_process(void *pData)
{
	while (1)
	{
		ql_rtos_task_sleep_ms(1);
		lv_tick_inc(1);
		lv_task_handler();
	}
}

void tp69_mcu_main_task(void *pData)
{
	ql_rtos_queue_create(&msqid, sizeof(ZPOC_MSG_TYPE), 1000);

	/*Initialize LittlevGL*/
	lv_init();

	/*Initialize the HAL for LittlevGL*/
	lvgl_hal_init();

	font_pkg_init();

	gpio_port_init();

	lcd_device_init();

	ql_rtos_task_create(&tick_task_ref, 2048, 100, "tick_process", tick_process, NULL);

	while (1)
		ql_rtos_task_sleep_s(1000);
}

application_init(tp69_mcu_main_task, "tp69_mcu", 16, 1);
