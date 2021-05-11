#ifndef __GPIO_H__
#define __GPIO_H__

#include "ql_gpio.h"
#include "Zpoc.h"
//input
#define PIN_KEYPTT1_NUM GPIO_PIN_NO_75
#define PIN_KEYPTT2_NUM GPIO_PIN_NO_76
#define PIN_KEY1_NUM GPIO_PIN_NO_0
#define PIN_KEY2_NUM GPIO_PIN_NO_1
#define PIN_KEY3_NUM GPIO_PIN_NO_31
#define PIN_KEY4_NUM GPIO_PIN_NO_43
#define PIN_KEY5_NUM GPIO_PIN_NO_121
#define PIN_KEY6_NUM GPIO_PIN_NO_49
#define PIN_KEY7_NUM GPIO_PIN_NO_50
#define PIN_KEY8_NUM GPIO_PIN_NO_16

#define PIN_HPH_DET_NUM GPIO_PIN_NO_4

//output
#define PIN_LEDG_NUM GPIO_PIN_NO_10
#define PIN_LEDR_NUM GPIO_PIN_NO_11
#define PIN_SPKSW_NUM GPIO_PIN_NO_81
#define PIN_MICSW_NUM GPIO_PIN_NO_59

#define PIN_PAEN_NUM GPIO_PIN_NO_122
#define PIN_GPSS_NUM GPIO_PIN_NO_124
#define PIN_HP_PA_NUM GPIO_PIN_NO_19 //earphone pa control (like default pa control)
#define PIN_RPEN_NUM GPIO_PIN_NO_58
#define PIN_BK_3VEN_NUM GPIO_PIN_NO_77
#define PIN_MICEN_NUM GPIO_PIN_NO_17

#define PIN_USBS_NUM GPIO_PIN_NO_43

//bk4819
#define PIN_BK_SCK_NUM GPIO_PIN_NO_71
#define PIN_BK_SCN_NUM GPIO_PIN_NO_74
#define PIN_BK_SDA_NUM GPIO_PIN_NO_78

typedef enum
{
	KEY_PTT_NOKEY,

	KEY_PTT1_PRESS,
	KEY_PTT1_RELEASE,
	KEY_PTT1_LONG,

	KEY_PTT2_PRESS,
	KEY_PTT2_RELEASE,
	KEY_PTT2_LONG,

	//ok
	KEY_LEFT_PRESS,
	KEY_LEFT_RELEASE,
	KEY_LEFT_LONG,

	//main-up
	KEY_PREV_PRESS,
	KEY_PREV_RELEASE,
	KEY_PREV_LONG,

	//main-down
	KEY_NEXT_PRESS,
	KEY_NEXT_RELEASE,
	KEY_NEXT_LONG,

	//back
	KEY_RIGHT_PRESS,
	KEY_RIGHT_RELEASE,
	KEY_RIGHT_LONG,

	//p1
	KEY_P1_PRESS,
	KEY_P1_RELEASE,
	KEY_P1_LONG,

	//p2
	KEY_P2_PRESS,
	KEY_P2_RELEASE,
	KEY_P2_LONG,

	//p3
	KEY_P3_PRESS,
	KEY_P3_RELEASE,
	KEY_P3_LONG,

	//p4
	KEY_P4_PRESS,
	KEY_P4_RELEASE,
	KEY_P4_LONG,

#if 1 //音量2键 69没用到
	KEY_UP_PRESS,
	KEY_UP_RELEASE,
	KEY_UP_LONG,

	KEY_DN_PRESS,
	KEY_DN_RELEASE,
	KEY_DN_LONG,
#endif
	KEY_EDET_IN,
	KEY_EDET_OUT,

	KEY_GREEN_PRESS,
	KEY_GREEN_RELEASE,
	KEY_GREEN_LONG,

	KEY_WHITE_PRESS,
	KEY_WHITE_RELEASE,
	KEY_WHITE_LONG,

	KEY_RED_PRESS,
	KEY_RED_RELEASE,
	KEY_RED_LONG,

	KEY_ROT_NEXT,
	KEY_ROT_PREV,

	KEY_POWER_DOWN,
	KEY_POWER_UP
} enKeyValue;

typedef enum
{
	MIC_PATH_RECEVER,
	MIC_PATH_EARPHONE,
} EN_MIC_PATH;

void gpio_set_pa_en(PIN_LEVEL_E lvl);
void gpio_set_spk_sw(PIN_LEVEL_E lvl);

void pa_output_control(unsigned char mode, unsigned char toggle);

void gpio_set_rp_en(PIN_LEVEL_E lvl);

void gpio_set_bk_power_en(PIN_LEVEL_E lvl);
void gpio_set_bk_scl(PIN_LEVEL_E lvl);
void gpio_set_bk_scn(PIN_LEVEL_E lvl);
void gpio_set_bk_sda(PIN_LEVEL_E lvl);

void gpio_set_mic_en(PIN_LEVEL_E lvl);
void gpio_set_mic_sw(PIN_LEVEL_E lvl);
void gpio_set_mic(EN_MIC_PATH path);
void gpio_port_init(void);
void gpio_port_init_bk_sda(unsigned char ucSta);

void gpio_run_g_led(unsigned char ucSta);
void gpio_state_led_reflesh(stStateFlg *pStateFlg);
void gpio_set_usb(unsigned char ucEnable);

void gpio_set_gps(euGpsModSta euSta);

void gpio_set_g_led(PIN_LEVEL_E euSta);

void gpio_push_key_msg(enKeyValue keyValue);

PIN_LEVEL_E gpio_hph_get_lvl(void);

unsigned char gpio_bk_sda_int_cb(void);

#endif
