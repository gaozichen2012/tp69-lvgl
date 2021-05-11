// #include "version.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ql_application.h"
#include "ql_rtos.h"
#include "ql_type.h"
#include "ql_power.h"

#include "general_include.h"
#include "lcd.h"
#include "charging.h"
#include "font.h"
#include "ui.h"

#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

#define BATTERY_LOW_VOL 3050
#define BATTERY_FULL_VOL 4100
#define LIMIT_CURR 1000 //1A
#define USB_UVLO 4200

extern stRunPar iRunPar;

QL_TERMINATE_JUDGE_E ql_charger_terminate_poll_ex(unsigned int terminate_curr_mA)
{
	unsigned int charger_curr = 0;
	static unsigned char cnt = 0;
	charger_curr = ql_get_charge_cur();

	if (terminate_curr_mA < 10)
		terminate_curr_mA = 10;
	else if (terminate_curr_mA > 500)
		terminate_curr_mA = 500;

	if (charger_curr <= terminate_curr_mA)
	{
		if (++cnt > 3)
		{
			cnt = 0;
			//pm813_charger_switch(0);
			return QL_TERMINATE_ENABLE;
		}
	}
	else
	{
		cnt = 0;
	}

	return QL_TERMINATE_DISABLE;
}

void charge_process_task(void *pData)
{
	unsigned char ucUsbConSta = 0, ucChargeSkip = 0;
	unsigned short usBatVol = 0;
	QL_CHARGER_STATUS_E eCharSta = 0;
	unsigned char ucMppFlg = 0;
	unsigned char ucTerminFlg = 0;
	//QL_VBUS_JUDGE_E eVBusSta;
	unsigned char ucChkCnt = 0;

	ql_precharge_time_set(QL_PRE_CHG_MINUTE24);			//set precharge 24 min
	ql_cccv_time_set(QL_CCCV_TIME_MINUTE96);			//set cccv 96 min
	ql_trickle_charge_time_set(QL_TRICKLE_CHG_MINUTE6); //set trickle charge 6 min

	ql_max_pre_cur_set(QL_PRE_CHG_CUR_150MA);	 //set precharger max charger current
	ql_terminate_cur_set(QL_TERM_CHG_CUR_100MA); //set charger terminate current
	ql_vbat_vol_set(QL_VBAT_VOL_4V20);			 //set charger full voltage
	ql_max_cc_cur_set(QL_CC_CHG_CUR_900MA);		 //set cc max charger current
	ql_charger_switch(1);						 //1-on 0-off

	ql_rtos_task_sleep_s(1);
	usBatVol = ql_get_charging_battery_vol();

	while (!ucChargeSkip)
	{
		ucUsbConSta = ql_get_vbus_state();

		if (ucUsbConSta)
		{
			iRunPar.ucChargeFlg = (ucTerminFlg == 1) ? E_BAT_CHAR_TRE : E_BAT_CHARGING;

			if (ucChkCnt++ >= 5)
			{
				ucChkCnt = 0;

				// ucBatConSta = ql_get_battery_state();
				eCharSta = ql_get_charge_status();
				usBatVol = ql_get_charging_battery_vol();

				{
					if (usBatVol < BATTERY_LOW_VOL)
					{
						_DEBUG("Battery low voltage! \r\n");

						ucTerminFlg = 0;

						if (eCharSta == QL_CHARGER_PRE)
						{
							ql_rtos_task_sleep_s(5);
							eCharSta = ql_get_charge_status();

							if (eCharSta == QL_CHARGER_PRE)
								iRunPar.ucPreChrFlg = 1;
						}
						else
						{
							if (!ucMppFlg && (QL_CHARGER_CC == eCharSta))
							{
								if (ql_set_charge_mpp(LIMIT_CURR, USB_UVLO))
								{
									ucMppFlg = 1;
								}
							}
						}

						ql_rtos_task_sleep_s(30);
						continue;
					}
					else
					{
						if (eCharSta == QL_CHARGER_PRE)
						{
							ucTerminFlg = 0;

							ql_rtos_task_sleep_s(5);
							eCharSta = ql_get_charge_status();

							if (eCharSta == QL_CHARGER_PRE)
								iRunPar.ucPreChrFlg = 1;
						}
						else if (eCharSta == QL_CHARGER_CC)
						{
							ucTerminFlg = 0;

							if (!ucMppFlg && ql_set_charge_mpp(LIMIT_CURR, USB_UVLO))
							{
								ucMppFlg = 1;
								ql_rtos_task_sleep_s(1);
							}
						}
						else if (eCharSta == QL_CHARGER_CV)
						{
							if (ucTerminFlg == 0)
							{
								_DEBUG("terminate poll\r\n");
								if (ql_charger_terminate_poll_ex(100) == QL_TERMINATE_ENABLE)
								{
									_DEBUG("Battery full...Charge already off\r\n");
									ucTerminFlg = 1;
									ql_rtos_task_sleep_s(1);
								}
							}
							else
							{
								_DEBUG("recharger poll\r\n");
								if (ql_recharger_poll(USB_UVLO - 100) == QL_RECHARGE_ENABLE)
								{
									ucTerminFlg = 0;
									_DEBUG("Battery not full...recharge running\r\n");
									ql_rtos_task_sleep_s(1);
								}
							}
						}
						else if (eCharSta == QL_CHARGER_TER)
						{
							if (ucTerminFlg == 1)
							{
								_DEBUG("recharger poll\r\n");
								if (ql_recharger_poll(USB_UVLO - 100) == QL_RECHARGE_ENABLE)
								{
									ucTerminFlg = 0;
									_DEBUG("Battery not full...recharge running\r\n");
									ql_rtos_task_sleep_s(1);
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if (iRunPar.ucChargeFlg != E_BAT_NOT_CHAR)
				iRunPar.ucChargeFlg = E_BAT_NOT_CHAR;

			ucTerminFlg = 0;
		}
		ql_rtos_task_sleep_s(1);
	}
}

ql_task_t charge_process_task_ref;

int charge_process_thread_create(void)
{
	return ql_rtos_task_create(&charge_process_task_ref, 2048, 90, "charge_process_task", charge_process_task, NULL);
}
