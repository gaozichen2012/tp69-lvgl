// #include "version.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ql_rtos.h"
#include "ql_gpio.h"
#include "ql_power.h"
#include "ql_audio.h"
#include "general_include.h"
#include "gpio.h"

#include "charging.h"

#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

extern ql_queue_t msqid;

unsigned int uiKeyCnt;
unsigned int uiKeyCntFlg = 0;
enKeyValue iKeyDown;

ql_sem_t semKeyEvent;

ql_task_t gpio_cnt_task_ref;

const unsigned char ucLedRunSta[][8] =
	{
		{0, 1, 0, 1, 0, 1, 0, 1},
		{0, 0, 0, 0, 0, 0, 0, 1},
};

void gpio_enable_wakeup(void);

void gpio_push_key_msg(enKeyValue keyValue)
{
	ZPOC_MSG_TYPE iMsg;
	iMsg.enMsgType = ZPOC_MSG_KEY;
	iMsg.pMsg = NULL;
	iMsg.usMsgLen = keyValue;

	ql_rtos_queue_release(msqid, sizeof(iMsg), (unsigned char *)&iMsg, QL_WAIT_FOREVER);
}

unsigned char gpio_bk_sda_int_cb(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_BK_SDA_NUM, &lvl);

	if (lvl == PIN_LEVEL_LOW)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void gpio_keyptt1_int_cb(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_KEYPTT1_NUM, &lvl);
	if (lvl == PIN_LEVEL_LOW)
	{
		iKeyDown = (KEY_PTT1_PRESS);
	}
	else
	{
		iKeyDown = (KEY_PTT1_RELEASE);
	}

	gpio_push_key_msg(iKeyDown);

	//gpio_enable_wakeup();
}

void gpio_keyptt2_int_cb(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_KEYPTT2_NUM, &lvl);
	if (lvl == PIN_LEVEL_LOW)
	{
		iKeyDown = (KEY_PTT2_PRESS);
	}
	else
	{
		iKeyDown = (KEY_PTT2_RELEASE);
	}

	gpio_push_key_msg(iKeyDown);

	//gpio_enable_wakeup();
}

void set_key_gpio_input(GPIO_PIN_NUMBER_E pin, enKeyValue key)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(pin, &lvl);
	//_DEBUG("keygpio%d=%d!\r\n", pin, key);
	if (lvl == PIN_LEVEL_LOW)
	{
		uiKeyCnt = 0;
		uiKeyCntFlg = 1;
		iKeyDown = key;
		ql_rtos_semaphore_release(semKeyEvent);
	}
	else
	{
		uiKeyCnt = 0;
		uiKeyCntFlg = 0;
	}
	gpio_enable_wakeup();
}

void gpio_key1_int_cb(void)
{
	set_key_gpio_input(PIN_KEY1_NUM, KEY_LEFT_RELEASE);
}

void gpio_key2_int_cb(void)
{
	set_key_gpio_input(PIN_KEY2_NUM, KEY_PREV_RELEASE);
}

void gpio_key3_int_cb(void)
{
	set_key_gpio_input(PIN_KEY3_NUM, KEY_NEXT_RELEASE);
}

void gpio_key4_int_cb(void)
{
	set_key_gpio_input(PIN_KEY4_NUM, KEY_RIGHT_RELEASE);
}

void gpio_key5_int_cb(void)
{
	set_key_gpio_input(PIN_KEY5_NUM, KEY_P1_RELEASE);
}

void gpio_key6_int_cb(void)
{
	set_key_gpio_input(PIN_KEY6_NUM, KEY_P2_RELEASE);
}

void gpio_key7_int_cb(void)
{
	set_key_gpio_input(PIN_KEY7_NUM, KEY_P3_RELEASE);
}

void gpio_key8_int_cb(void)
{
	set_key_gpio_input(PIN_KEY8_NUM, KEY_P4_RELEASE);
}

void gpio_hph_det_int_cb(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_HPH_DET_NUM, &lvl);
	if (lvl == PIN_LEVEL_LOW)
	{
		iKeyDown = (KEY_EDET_IN);
	}
	else
	{
		iKeyDown = (KEY_EDET_OUT);
	}
	ql_rtos_semaphore_release(semKeyEvent);
	gpio_enable_wakeup();
}

PIN_LEVEL_E gpio_hph_get_lvl(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_HPH_DET_NUM, &lvl);
	return lvl;
}

unsigned char get_key_ok_state(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_KEY1_NUM, &lvl);

	if (lvl == PIN_LEVEL_LOW)
		return 1;
	else
		return 0;
}

unsigned char get_key_up_state(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_KEY2_NUM, &lvl);

	if (lvl == PIN_LEVEL_LOW)
		return 1;
	else
		return 0;
}

unsigned char get_key_down_state(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_KEY3_NUM, &lvl);

	if (lvl == PIN_LEVEL_LOW)
		return 1;
	else
		return 0;
}

unsigned char get_key_back_state(void)
{
	PIN_LEVEL_E lvl;
	ql_gpio_get_level(PIN_KEY4_NUM, &lvl);

	if (lvl == PIN_LEVEL_LOW)
		return 1;
	else
		return 0;
}

void gpio_pwrkey_int_cb(void)
{
	ql_pwrkey_intc_enable(0); //disable pwrkey intc

	if (1 == ql_get_pwrkey_status())
	{
		//ql_rtos_flag_release(pwrkey_flag_ref, QL_PWRKEY_LOW_FLAG, QL_FLAG_OR);
		uiKeyCnt = 0;
		uiKeyCntFlg = 1;
		iKeyDown = KEY_RIGHT_RELEASE;
		ql_rtos_semaphore_release(semKeyEvent);
	}
	else
	{
		//ql_rtos_flag_release(pwrkey_flag_ref, QL_PWRKEY_HIGH_FLAG, QL_FLAG_OR);
		uiKeyCnt = 0;
		uiKeyCntFlg = 0;
	}

	ql_pwrkey_intc_enable(1); //enable pwrkey intc
}

void gpio_cnt_task_proc(void *pCon)
{
	iKeyDown = KEY_PTT_NOKEY;

	while (1)
	{
		ql_rtos_semaphore_wait(semKeyEvent, QL_WAIT_FOREVER);
		while (1)
		{
			if (uiKeyCntFlg == 1)
			{
				uiKeyCnt++;
				ql_rtos_task_sleep_ms(50);

				if (uiKeyCnt > 30)
				{
					switch ((int)iKeyDown)
					{
					case KEY_UP_RELEASE:
						gpio_push_key_msg(KEY_UP_LONG);
						break;

					case KEY_DN_RELEASE:
						gpio_push_key_msg(KEY_DN_LONG);
						break;

					case KEY_LEFT_RELEASE: //ok
						gpio_push_key_msg(KEY_LEFT_LONG);
						break;

					case KEY_PREV_RELEASE: //up
						gpio_push_key_msg(KEY_PREV_LONG);
						break;

					case KEY_NEXT_RELEASE: //down
						gpio_push_key_msg(KEY_NEXT_LONG);
						break;

					case KEY_RIGHT_RELEASE: //back
						gpio_push_key_msg(KEY_RIGHT_LONG);
						break;
					default:
						break;
					}

					uiKeyCnt = 0;
					uiKeyCntFlg = 0;
					iKeyDown = KEY_PTT_NOKEY;
					break;
				}
			}
			else
			{
				if (iKeyDown != KEY_PTT_NOKEY)
				{
					gpio_push_key_msg(iKeyDown);
					iKeyDown = KEY_PTT_NOKEY;
				}
				break;
			}
		}
	}
}

void gpio_enable_wakeup(void)
{
	ql_eint_enable_wakeup(PIN_KEYPTT1_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEYPTT2_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY1_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY2_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY3_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY4_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY5_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY6_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY7_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY8_NUM, PIN_BOTH_EDGE);

	ql_eint_enable_wakeup(PIN_HPH_DET_NUM, PIN_BOTH_EDGE);
}

void gpio_port_init(void)
{
	ql_rtos_semaphore_create(&semKeyEvent, 0);

	//Set output pin
	ql_gpio_init(PIN_LEDG_NUM, PIN_DIRECTION_OUT, PIN_PULL_PD, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_LEDR_NUM, PIN_DIRECTION_OUT, PIN_PULL_PD, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_SPKSW_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
	ql_gpio_init(PIN_MICSW_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
	ql_gpio_init(PIN_USBS_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_PAEN_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_GPSS_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
	ql_gpio_init(PIN_HP_PA_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
	ql_gpio_init(PIN_RPEN_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
	ql_gpio_init(PIN_BK_3VEN_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
	ql_gpio_init(PIN_MICEN_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);

	ql_gpio_init(PIN_BK_SCK_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
	ql_gpio_init(PIN_BK_SCN_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
	ql_gpio_init(PIN_BK_SDA_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);

	//Set input pin
	ql_gpio_init(PIN_KEYPTT1_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEYPTT2_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY1_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY2_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY3_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY4_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY5_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY6_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY7_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);
	ql_gpio_init(PIN_KEY8_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);

	ql_gpio_init(PIN_HPH_DET_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_HIGH);

	//Register intrrupt callback
	ql_eint_register(PIN_KEYPTT1_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_keyptt1_int_cb);
	ql_eint_register(PIN_KEYPTT2_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_keyptt2_int_cb);
	ql_eint_register(PIN_KEY1_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_key1_int_cb);
	ql_eint_register(PIN_KEY2_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_key2_int_cb);
	ql_eint_register(PIN_KEY3_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_key3_int_cb);
	ql_eint_register(PIN_KEY4_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_key4_int_cb);
	ql_eint_register(PIN_KEY5_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_key5_int_cb);
	ql_eint_register(PIN_KEY6_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_key6_int_cb);
	ql_eint_register(PIN_KEY7_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_key7_int_cb);
	ql_eint_register(PIN_KEY8_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_key8_int_cb);

	ql_eint_register(PIN_HPH_DET_NUM, PIN_BOTH_EDGE, PIN_PULL_PU, NULL, gpio_hph_det_int_cb);

	//Enable intrrupt
	ql_eint_enable_wakeup(PIN_KEYPTT1_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEYPTT2_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY1_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY2_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY3_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY4_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY5_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY6_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY7_NUM, PIN_BOTH_EDGE);
	ql_eint_enable_wakeup(PIN_KEY8_NUM, PIN_BOTH_EDGE);

	ql_eint_enable_wakeup(PIN_HPH_DET_NUM, PIN_BOTH_EDGE);

	ql_pwrkey_register_irq(gpio_pwrkey_int_cb);
	ql_pwrkey_intc_enable(1); //enable pwrkey intc

	ql_rtos_task_create(&gpio_cnt_task_ref, 1024, 74, "gpio_cnt_task_proc", gpio_cnt_task_proc, NULL);
}

void gpio_port_init_bk_sda(unsigned char ucSta)
{
	if (ucSta == 0) //output
	{
		ql_gpio_init(PIN_BK_SDA_NUM, PIN_DIRECTION_OUT, PIN_PULL_PU, PIN_LEVEL_LOW);
		ql_gpio_set_pull(PIN_BK_SDA_NUM, PIN_PULL_PU);
	}
	else
	{
		//Set input pin
		ql_gpio_init(PIN_BK_SDA_NUM, PIN_DIRECTION_IN, PIN_PULL_PU, PIN_LEVEL_LOW);
		ql_gpio_set_pull(PIN_BK_SDA_NUM, PIN_PULL_PU);
	}
}

void gpio_set_hppa_en(PIN_LEVEL_E lvl)
{
	if (iStateFlg.bMute == 0)
	{
		ql_gpio_set_level(PIN_HP_PA_NUM, lvl);
	}
	else
	{
		ql_gpio_set_level(PIN_HP_PA_NUM, PIN_LEVEL_LOW);
	}
}

void gpio_set_pa_en(PIN_LEVEL_E lvl)
{
	if (iStateFlg.bMute == 0)
	{
		ql_gpio_set_level(PIN_PAEN_NUM, lvl);
	}
	else
	{
		ql_gpio_set_level(PIN_PAEN_NUM, PIN_LEVEL_LOW);
	}
}

void gpio_set_spk_sw(PIN_LEVEL_E lvl)
{
	ql_gpio_set_level(PIN_SPKSW_NUM, lvl);
}

void pa_output_control(unsigned char mode, unsigned char toggle)
{
	if (mode == 0) //net
	{
		gpio_set_spk_sw(PIN_LEVEL_LOW);
	}
	else
	{
		gpio_set_spk_sw(PIN_LEVEL_HIGH);
	}

	if (toggle == 0)
	{
		if (gpio_hph_get_lvl() == PIN_LEVEL_HIGH)
		{
			gpio_set_pa_en(PIN_LEVEL_LOW);
		}
		else
		{
			gpio_set_hppa_en(PIN_LEVEL_LOW);
		}
	}
	else
	{
		if (gpio_hph_get_lvl() == PIN_LEVEL_HIGH)
		{
			gpio_set_pa_en(PIN_LEVEL_HIGH);
		}
		else
		{
			gpio_set_hppa_en(PIN_LEVEL_HIGH);
		}
	}
}
//1:rp off 0:rp on
void gpio_set_rp_en(PIN_LEVEL_E lvl)
{
	ql_gpio_set_level(PIN_RPEN_NUM, lvl);
}

void gpio_set_bk_power_en(PIN_LEVEL_E lvl)
{
	ql_gpio_set_level(PIN_BK_3VEN_NUM, lvl);
}

void gpio_set_mic_en(PIN_LEVEL_E lvl)
{
	ql_gpio_set_level(PIN_MICEN_NUM, lvl);
}

void gpio_set_mic_sw(PIN_LEVEL_E lvl)
{
	ql_gpio_set_level(PIN_MICSW_NUM, lvl);
}

void gpio_set_mic(EN_MIC_PATH path)
{
	if (path == MIC_PATH_RECEVER)
	{
		ql_set_mic(0);
		gpio_set_mic_en(PIN_LEVEL_HIGH);
	}
	else
	{
		ql_set_mic(1);
		gpio_set_mic_en(PIN_LEVEL_LOW);
	}
	zpoc_set_volum_level(iBaseSeting.ucVolLvl);
}

void gpio_set_bk_scl(PIN_LEVEL_E lvl)
{
	ql_gpio_set_level(PIN_BK_SCK_NUM, lvl);
}

void gpio_set_bk_scn(PIN_LEVEL_E lvl)
{
	ql_gpio_set_level(PIN_BK_SCN_NUM, lvl);
}

void gpio_set_bk_sda(PIN_LEVEL_E lvl)
{
	ql_gpio_set_level(PIN_BK_SDA_NUM, lvl);
}

void gpio_run_g_led(unsigned char ucSta)
{
	unsigned char ucValue;
	static unsigned char ucRunCnt = 0;

	ucRunCnt = (ucRunCnt + 1) % sizeof(ucLedRunSta[1]);
	ucValue = ucLedRunSta[ucSta][ucRunCnt];

	ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)ucValue);
}

void gpio_state_led_reflesh(stStateFlg *pStateFlg)
{
	if (pStateFlg->bInitFilish)
	{
		if (pStateFlg->bTaking || pStateFlg->bPocSend)
		{
			ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)1);

			if (iRunPar.ucChargeFlg == E_BAT_CHAR_TRE)
				ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)1);
			else
				ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)0);
		}
		else if (pStateFlg->bCallIn)
		{
			if (iRunPar.ucChargeFlg == E_BAT_CHARGING)
				ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)1);
			else
				ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)0);

			ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)1);
		}
		else
		{
			if (iRunPar.ucChargeFlg == E_BAT_CHARGING)
			{
				ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)1);
				gpio_run_g_led(pStateFlg->bInitFilish);
			}
			else if (iRunPar.ucChargeFlg == E_BAT_CHAR_TRE)
			{
				ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)0);
				ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)1);
			}
			else
			{
				ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)0);
				gpio_run_g_led(pStateFlg->bInitFilish);
			}
		}
	}
	else
	{
		if (iRunPar.ucChargeFlg == E_BAT_CHARGING)
		{
			ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)1);

			ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)1);
			ql_rtos_task_sleep_ms(50);
			ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)0);
		}
		else if (iRunPar.ucChargeFlg == E_BAT_CHAR_TRE)
		{
			ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)0);
			ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)1);
		}
		else
		{
			ql_gpio_set_level(PIN_LEDR_NUM, (PIN_LEVEL_E)0);

			ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)1);
			ql_rtos_task_sleep_ms(50);
			ql_gpio_set_level(PIN_LEDG_NUM, (PIN_LEVEL_E)0);
		}
	}
}

void gpio_set_gps(euGpsModSta euSta)
{
	(euSta == GPS_DEVICE_NORMAL) ? ql_gpio_set_level(PIN_GPSS_NUM, PIN_LEVEL_HIGH) : ql_gpio_set_level(PIN_GPSS_NUM, PIN_LEVEL_LOW);
}
