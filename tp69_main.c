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

ql_task_t lv_tick_ref;
ql_task_t lv_task_ref;

unsigned int cnt1 = 0;
unsigned int cnt2 = 0;

static void lv_tick_process(void *pData)
{
	while (1)
	{
		ql_rtos_task_sleep_ms(5);
		if (cnt1++ > 1000)
		{
			cnt1 = 0;
			_DEBUG("tom test: tick 5s\r\n");
		}

		lv_tick_inc(5);
	}
}

static void lv_task_process(void *pData)
{
	while (1)
	{
		ql_rtos_task_sleep_ms(5);

		if (cnt2++ > 1000)
		{
			cnt2 = 0;
			_DEBUG("tom test: task 5s\r\n");
		}
		lv_task_handler();
	}
}

void tp69_mcu_main_task(void *pData)
{
	ql_rtos_queue_create(&msqid, sizeof(ZPOC_MSG_TYPE), 1000);
#if 1
	ql_gpio_init(PIN_KEY1_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY2_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY3_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY4_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
#else
	gpio_port_init();
#endif

	/*Initialize LittlevGL*/
	lv_init();

	/*Initialize the HAL for LittlevGL*/
	lvgl_hal_init();

	lcd_device_init();

	font_pkg_init();

	// lcd_disp_box(0, 16, COLOR_BLUE, LCD_WIDTH, LCD_HEIGHT);
	// lcd_update_disp();
	ql_rtos_task_sleep_s(5);

	lv_startup();
	ql_rtos_task_create(&lv_task_ref, 2048, 99, "lv_task_process", lv_task_process, NULL);
	ql_rtos_task_create(&lv_tick_ref, 2048, 100, "lv_tick_process", lv_tick_process, NULL);

	while (1)
		ql_rtos_task_sleep_s(1000);
}

application_init(tp69_mcu_main_task, "tp69_mcu", 16, 1);
