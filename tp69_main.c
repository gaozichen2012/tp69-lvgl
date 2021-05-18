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
ql_task_t lvgl_start_task_ref;

static void tick_process(void *pData)
{
	static unsigned int tmp_cnt = 0;

	while (1)
	{
		ql_rtos_task_sleep_ms(1);
		//_DEBUG("tom test 5!\r\n");
		lv_tick_inc(10);
		lv_task_handler();
	}
}

static void lvgl_start_process(void *pData)
{
	//lv_startup();

	while (1)
	{
		ql_rtos_task_sleep_ms(1000);
		_DEBUG("tom test 21!\r\n");
	}
}

void tp69_mcu_main_task(void *pData)
{
	ql_rtos_queue_create(&msqid, sizeof(ZPOC_MSG_TYPE), 1000);

	/*Initialize LittlevGL*/
	lv_init();

	/*Initialize the HAL for LittlevGL*/
	lvgl_hal_init();

	_DEBUG("tom test 1!\r\n");
	lcd_device_init();
	_DEBUG("tom test 2!\r\n");
	font_pkg_init();
	_DEBUG("tom test 3!\r\n");
	gpio_port_init();

	_DEBUG("tom test 7!\r\n");
	// lcd_disp_box(0, 16, COLOR_BLUE, LCD_WIDTH, LCD_HEIGHT);
	// lcd_update_disp();
	ql_rtos_task_sleep_s(5);
	_DEBUG("tom test 88!\r\n");
	lv_startup();

	_DEBUG("tom test 4!\r\n");
	ql_rtos_task_create(&tick_task_ref, 2048, 100, "tick_process", tick_process, NULL);

	_DEBUG("tom test 6!\r\n");
	ql_rtos_task_create(&lvgl_start_task_ref, 2048, 100, "lvgl_start_process", lvgl_start_process, NULL);

	while (1)
		ql_rtos_task_sleep_s(1000);
}

application_init(tp69_mcu_main_task, "tp69_mcu", 16, 1);
