#include "ql_rtos.h"
#include "ql_power.h"
#include "ql_fs.h"
#include "ql_sim.h"
#include "ql_nw.h"
#include "ql_rtc.h"
#include "ql_timer.h"
#include "ql_dev.h"
#include "ql_audio.h"
#include "ql_power.h"
#include "ql_tts.h"
#include "general_include.h"
#include "at.h"
#include "lcd.h"
#include "font.h"
#include "ui.h"
#include "state.h"
#include "Uart.h"
#include "gpio.h"
#include "state.h"
#include "state_init.h"
// #include "version.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "state_desktop.h"
#include "state_invite.h"
#include "bk_include.h"
#include "bk4819.h"
#include "analog_model.h"
#include "analog_controller.h"

extern ql_queue_t msqid;
extern unsigned char ucRotKeyCnt;
extern unsigned char ucKeepPaCnt;
extern euInviteType iInviteType;

stGroupList iGroupList;
stMemberList iMemberList;
stUserInfo iUserInfo;
stStateFlg iStateFlg;

stRecordList iRecordList;

stInvRecList iInvRecList;

stBaseInfo iBaseInfo;
stBaseSeting iBaseSeting;

stRunPar iRunPar;

unsigned char ucStaChkCnt = 0;
unsigned char ucPwrKeyCnt = 0;
unsigned char ucPwrOnFlg = 0;
unsigned char ucStartTimCnt = 0;

ql_task_t zpoc_check_state_task_ref;
ql_task_t zpoc_hb_task_ref;

//static int h_sim = 0;

#ifdef LANGUAGE_SETTING_CH
#define POWER_OFF_DIS_X_OFFSET 46
#define NO_SIM_DIS_X_OFFSET 52

const char *pDefaultIp = "47.100.169.173";
#endif

#ifdef LANGUAGE_SETTING_EN
#define POWER_OFF_DIS_X_OFFSET 30
#define NO_SIM_DIS_X_OFFSET 28

const char *pDefaultIp = "47.74.211.230";
#endif

#ifdef LANGUAGE_SETTING_TU
#define POWER_OFF_DIS_X_OFFSET 40
#define NO_SIM_DIS_X_OFFSET 16

const char *pDefaultIp = "47.74.211.230";
#endif

//unsigned char ucBatAlmCnt = 0;

#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

const unsigned short BatLevelValue[11] = {3200, 3250, 3370, 3440, 3510, 3580, 3650, 3720, 3820, 3940, 4030};

const char *pDefaultPwd = "111111";

int zpoc_enter_sleep(int wakelockfd)
{
	iStateFlg.bSleep = 1;
	//Ql_SLP_WakeLock_Unlock(wakelockfd);
	_DEBUG("\r\n*****\r\nMcu Unlock wakelock!!\r\n");
	ql_lpm_wakelock_unlock(wakelockfd);
	return 0;
}

int zpoc_exit_sleep(int wakelockfd)
{
	iStateFlg.bSleep = 0;
	//Ql_SLP_WakeLock_Lock(wakelockfd);
	_DEBUG("\r\n*****\r\nMcu Lock wakelock!!\r\n");
	ql_lpm_wakelock_lock(wakelockfd);
	return 0;
}

void zpoc_hb_process(void *pData)
{
	ZPOC_MSG_TYPE iMsg;
	while (1)
	{
		ql_rtos_task_sleep_ms(1000);

		iMsg.enMsgType = ZPOC_MSG_TIMER;
		iMsg.pMsg = NULL;
		iMsg.usMsgLen = 0;

		//_DEBUG("Hb process send timer to queen!!\r\n");
		ql_rtos_queue_release(msqid, sizeof(iMsg), (unsigned char *)&iMsg, QL_WAIT_FOREVER);
	}
}

int zpoc_hb_timer_create(void)
{
	return ql_rtos_task_create(&zpoc_hb_task_ref, 2048, 75, "zpoc_hb_process", zpoc_hb_process, NULL);
}

void zpoc_check_state_process(void *pData)
{
	int csq;
	QL_NW_SIGNAL_STRENGTH_INFO_T ql_nw_signal_strength;
	QL_NW_REG_STATUS_INFO_T ql_nw_reg_status;

	while (1)
	{
		if (iStateFlg.bSimState == 0)
		{
			iRunPar.ucCSQ = 0;
			iRunPar.iNetMode = ZPOC_NET_UNKNOW;
		}
		else if (QL_NW_SUCCESS == ql_nw_get_reg_status(&ql_nw_reg_status))
		{
			if (ql_nw_reg_status.data_reg.state != QL_NW_REG_STATE_HOME_NETWORK && ql_nw_reg_status.data_reg.state != QL_NW_REG_STATE_ROAMING)
			{
				iRunPar.iNetMode = ZPOC_NET_UNKNOW;
				iRunPar.ucCSQ = 0;
			}
			else
			{
				switch (ql_nw_reg_status.data_reg.rat)
				{
				case QL_NW_ACCESS_TECH_GSM:
				case QL_NW_ACCESS_TECH_GSM_COMPACT:
				case QL_NW_ACCESS_TECH_GSM_wEGPRS:
					iRunPar.iNetMode = ZPOC_NET_GSM;
					break;

				case QL_NW_ACCESS_TECH_UTRAN:
				case QL_NW_ACCESS_TECH_UTRAN_wHSDPA:
				case QL_NW_ACCESS_TECH_UTRAN_wHSUPA:
				case QL_NW_ACCESS_TECH_UTRAN_wHSDPA_HSUPA:
				case QL_NW_ACCESS_TECH_UTRAN_HSPAP:
					iRunPar.iNetMode = ZPOC_NET_UMTS;
					break;

				case QL_NW_ACCESS_TECH_E_UTRAN:
				case QL_NW_ACCESS_TECH_E_UTRAN_CA:
					iRunPar.iNetMode = ZPOC_NET_LTE;
					break;

				default:
					iRunPar.iNetMode = ZPOC_NET_UNKNOW;
					break;
				}

				if (QL_NW_SUCCESS == ql_nw_get_signal_strength(&ql_nw_signal_strength))
				{
					if (iRunPar.iNetMode == ZPOC_NET_LTE)
						csq = (ql_nw_signal_strength.LTE_SignalStrength.rssi + 113) / 2;
					else
						csq = (ql_nw_signal_strength.GW_SignalStrength.rssi + 113) / 2;

					iRunPar.ucCSQ = (csq > 31) ? 31 : csq;
				}
				else
				{
					iRunPar.ucCSQ = 0;
					iRunPar.iNetMode = ZPOC_NET_UNKNOW;
				}
			}
		}
		else
		{
			iRunPar.ucCSQ = 0;
			iRunPar.iNetMode = ZPOC_NET_UNKNOW;
		}

		ql_rtos_task_sleep_s(4);
	}
}

int zpoc_modem_state_thread_create(void)
{
	return ql_rtos_task_create(&zpoc_check_state_task_ref, 2048, 90, "zpoc_check_state_process", zpoc_check_state_process, NULL);
}

int zpoc_timer_process(void)
{
	//char test[50];
	static unsigned char ucBatChkCnt = 0;
	static unsigned char iNetTmp = ZPOC_NET_UNKNOW;
	static unsigned char ucCsqTmp;
	static unsigned char ucChargFlg = E_BAT_NOT_CHAR;
	static unsigned char ucChkCnt = 0;
	static unsigned char ucGpsEnFlg = 0;

	unsigned short ucBatTmp;
	unsigned char ucFlg = 0;
	stTime iTime;

	//analog
	toa_1s_process();
	//net

	if (iStateFlg.bSimState == 0 && ucStartTimCnt++ >= POC_INIT_START_TIME)
	{
		int iSimState;

		ql_sim_get_card_status(&iSimState);
		_DEBUG("ql_sim_get_card_status return %d!\r\n", iSimState);

		if ((QL_SIM_STATUS)iSimState == QL_SIM_STATUS_NOT_INSERTED)
		{
			iStateFlg.bSimState = 0;
			zpoc_play_tts((char *)cNoSimTTS);

			lcd_disp_box(0, 16, COLOR_NET_BACKGROUND, LCD_WIDTH, 144);
			ui_disp_font_15x16(NO_SIM_DIS_X_OFFSET, 52, c_no_sim_card, COLOR_NET_BACKGROUND, COLOR_FONT);
			lcd_update_disp();
		}
		else if ((QL_SIM_STATUS)iSimState == QL_SIM_STATUS_READY)
		{
			iStateFlg.bSimState = 1;
		}

		ucStartTimCnt = 0;
	}

	if (ucStaChkCnt++ >= 4)
	{
		if (iBaseInfo.cPocVer[0] == 0)
		{
			at_send_at_cmd((unsigned char *)POC_CMD_GET_POC_VERSION, strlen(POC_CMD_GET_POC_VERSION));
		}

		if (iNetTmp != iRunPar.iNetMode)
		{
			iNetTmp = iRunPar.iNetMode;
			ui_disp_icon_net_status(iNetTmp);
			ucFlg = 1;
		}

		if (ucCsqTmp != iRunPar.ucCSQ)
		{
			ucCsqTmp = iRunPar.ucCSQ;
			ui_disp_icon_signal(ucCsqTmp);
			ucFlg = 1;
		}

		if (iStateFlg.bTaking == 0 && iStateFlg.bCallIn == 0)
		{
			ucBatTmp = zpoc_get_bat_level();
			if (ucBatTmp == 0 && ((E_BAT_CHARG_TYPE)iRunPar.ucChargeFlg) == E_BAT_NOT_CHAR)
			{
				ql_rtos_task_sleep_ms(100);
				ucBatTmp = zpoc_get_bat_level();
				if (ucBatTmp == 0)
				{
					cfg_net_write_mcu_seting(&iBaseSeting);
					lcd_set_bl(1);

					lcd_disp_box(0, 0, COLOR_NET_BACKGROUND, LCD_WIDTH, LCD_HEIGHT);
					ui_disp_font_15x16((LCD_WIDTH - 68) / 2, 60, cDesktopOffTx, COLOR_NET_BACKGROUND, COLOR_NET_ICO_FONT);

					lcd_update_disp();

					printf("[YDPOC] Low battery powerdown!!!\r\n");
					ql_rtos_task_sleep_s(2);

					if (iRunPar.ucChargeFlg != 0)
					{
						ql_create_a_fake_wake_source(QL_RSTREASON_POWEROFF_CHARGE);
						ql_power_reset();
					}
					else
					{
						ql_power_down(0);
					}

					while (1)
						ql_rtos_task_sleep_s(1);
				}
			}

			if (ucBatChkCnt++ >= 12)
			{
				ucBatChkCnt = 0;
				if ((ucBatTmp <= 2) && (zpoc_get_bat_level() <= 2))
				{
					ZPOC_MSG_TYPE iMsg;
					iMsg.enMsgType = ZPOC_MSG_BATLOW;
					iMsg.pMsg = NULL;
					iMsg.usMsgLen = 0;
					ql_rtos_queue_release(msqid, sizeof(iMsg), (unsigned char *)&iMsg, QL_WAIT_FOREVER);
				}
			}

			if (ucBatTmp != iRunPar.ucBattry || ucChargFlg != iRunPar.ucChargeFlg)
			{
				iRunPar.ucBattry = ucBatTmp;
				ucChargFlg = iRunPar.ucChargeFlg;

				ui_disp_icon_bat(iRunPar.ucBattry, iRunPar.ucChargeFlg);
				ucFlg = 1;
			}

			// _DEBUG("iRunPar.iGpsInfo.cLocSta:%d, iRunPar.ucGpsSta:%d, iRunPar.iGpsInfo.iState:%d, ucGpsEnFlg:%d\r\n",
			// 	   iRunPar.iGpsInfo.cLocSta, iRunPar.ucGpsSta, iRunPar.iGpsInfo.iState, ucGpsEnFlg);

			if (iRunPar.iGpsInfo.cLocSta != iRunPar.ucGpsSta || iRunPar.iGpsInfo.iState != ucGpsEnFlg)
			{
				ucGpsEnFlg = iRunPar.iGpsInfo.iState;
				iRunPar.ucGpsSta = iRunPar.iGpsInfo.cLocSta;
				ui_disp_icon_gps(iRunPar.iGpsInfo.iState, iRunPar.ucGpsSta);
				ucFlg = 1;
			}

			zpoc_get_system_time(&iTime);

			if (iTime.ucHour != iRunPar.iTime.ucHour || iTime.ucMint != iRunPar.iTime.ucMint)
			{
				iRunPar.iTime = iTime;
				if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_NET)
				{
					ui_disp_srceen_time(&iRunPar.iTime);
					ui_disp_date(&iRunPar.iTime);
				}

				ucFlg = 1;
			}
		}

		if (ucFlg)
			lcd_update_disp();

		ucStaChkCnt = 0;
	}

	if (ucChargFlg != iRunPar.ucChargeFlg)
	{
		ucChargFlg = iRunPar.ucChargeFlg;
		ui_disp_icon_bat(iRunPar.ucBattry, iRunPar.ucChargeFlg);
		lcd_update_disp();
	}

	if (iStateFlg.bTaking == 1)
	{
		if (iRunPar.ucSendCnt <= 0)
			gpio_push_key_msg(KEY_PTT1_RELEASE);
		else
			iRunPar.ucSendCnt--;
	}

	if (iStateFlg.bPocSend == 1)
	{
		if (ucChkCnt++ > 1)
		{
			PIN_LEVEL_E lvl;
			ucChkCnt = 0;
			ql_gpio_get_level(PIN_KEYPTT1_NUM, &lvl);

			if (lvl == PIN_LEVEL_HIGH)
			{
				gpio_push_key_msg(KEY_PTT1_RELEASE);
			}
		}

		if (iStateFlg.bTaking == 0)
		{
			if (iRunPar.ucSendCnt <= 0)
				gpio_push_key_msg(KEY_PTT1_RELEASE);
			else
				iRunPar.ucSendCnt--;
		}
	}
	else
	{
		ucChkCnt = 0;
	}

	if (iRunPar.ucPreChrFlg == 1)
	{
		cfg_net_write_mcu_seting(&iBaseSeting);
		lcd_set_bl(1);

		lcd_disp_box(0, 0, COLOR_NET_BACKGROUND, LCD_WIDTH, LCD_HEIGHT);
		ui_disp_font_15x16(30, 60, cDesktopLowPwrTx, COLOR_NET_BACKGROUND, COLOR_NET_ICO_FONT);

		lcd_update_disp();

		printf("[YDPOC] Low battery powerdown!!!\r\n");
		ql_rtos_task_sleep_s(2);

		ql_create_a_fake_wake_source(QL_RSTREASON_POWEROFF_CHARGE);
		ql_power_reset();

		while (1)
			ql_rtos_task_sleep_s(1);
	}

	gpio_state_led_reflesh(&iStateFlg);

	return 0;
}

///////////////////////////////////////AT解包处理///////////////////////////////////////
void zpoc_member_list_clr(void)
{
	memset(&iMemberList, 0, sizeof(iMemberList));
}

void zpoc_process_init(void)
{
	memset(&iGroupList, 0, sizeof(iGroupList));
	memset(&iMemberList, 0, sizeof(iMemberList));
	memset(&iUserInfo, 0, sizeof(iUserInfo));

	memset(&iBaseInfo, 0, sizeof(iBaseInfo));

	memset(&iRunPar, 0, sizeof(iRunPar));

	//memset(&iBaseSeting, 0, sizeof(iBaseSeting));
	cfg_net_read_mcu_seting(&iBaseSeting);

	iBaseSeting.ucPortMode = 0;

	memset(&iStateFlg, 0, sizeof(iStateFlg));
	memset(&iInvRecList, 0, sizeof(stInvRecList));
	memset(&iRecordList, 0, sizeof(iRecordList));

	strcpy((char *)iUserInfo.ucUserName, (char *)c_smallhead_offline);
	strcpy((char *)iUserInfo.ucGroupName, (char *)c_smallhead_offline);
	strcpy((char *)iUserInfo.ucTalkName, (char *)c_idle);

	iStateFlg.bPttAllow = 1;

	iRunPar.iGpsInfo.cLocSta = 0xff;

	memset(&iPocSet, 0, sizeof(iPocSet));

	init_get_poc_cfg_from_file();

	memset(&toa_flash2, 0, sizeof(toa_flash2));
	init_get_analog_cfg_from_file();

	gpio_set_gps(iRunPar.iGpsInfo.iState);

	ql_dev_get_imei(iBaseInfo.cModIMEI, sizeof(iBaseInfo.cModIMEI) - 1);

	ql_dev_get_firmware_version(iBaseInfo.cModVer, sizeof(iBaseInfo.cModVer) - 1);

	strncpy(iBaseInfo.cMcuVer, MCU_VERSION, sizeof(iBaseInfo.cMcuVer) - 1);
}

unsigned char *zpoc_get_member_id_by_name(unsigned char *pName)
{
	unsigned short i;

	for (i = 0; i < iMemberList.usTotUser; i++)
	{
		if (strncmp((char *)iMemberList.ucUserName[i], (char *)pName, USER_NAME_MAX_SIZE) == 0)
		{
			return iMemberList.ucUserId[i];
		}
	}
	return NULL;
}

unsigned char zpoc_name_copy(unsigned char *pDst, unsigned char *pSrc)
{
	unsigned char i = 0;

	while (i < USER_NAME_MAX_SIZE)
	{
		if (pSrc[i] == '0' && pSrc[i + 1] == '0' && pSrc[i + 2] == '0' && pSrc[i + 3] == '0' && (i % 4) == 0)
		{
			pDst[i] = 0;
			return 1;
		}

		pDst[i] = pSrc[i];
		i++;
	}

	pDst[USER_NAME_MAX_SIZE - 1] = 0;
	return 0;
}

unsigned char zpoc_ascii_to_char(unsigned char nib_h, unsigned char nib_l)
{
	nib_h = (nib_h <= '9') ? (nib_h - 0x30) : ((nib_h <= 'F') ? (nib_h - 0x37) : (nib_h - 0x57));
	nib_l = (nib_l <= '9') ? (nib_l - 0x30) : ((nib_l <= 'F') ? (nib_l - 0x37) : (nib_l - 0x57));

	return ((nib_h << 4) | nib_l);
}

char zpoc_char_to_ascii(char c)
{
	return (c <= 9 ? (c + 0x30) : (c + 0x57));
}

void zpoc_reset_factory_account(void)
{
	unsigned char ucLen;
	int i, iCnt;
	char cAtBuf[200];
	char cMeid[10];

	memset(cAtBuf, 0, sizeof(cAtBuf));
	memcpy(cAtBuf, "AT+POC=01030069703d", 19); //ip=
	ucLen = 19;

	iCnt = strlen(pDefaultIp);
	for (i = 0; i < iCnt; i++)
	{
		cAtBuf[ucLen++] = zpoc_char_to_ascii(pDefaultIp[i] >> 4);
		cAtBuf[ucLen++] = zpoc_char_to_ascii(pDefaultIp[i] & 0x0f);
	}

	memcpy(&cAtBuf[ucLen], "3b69643d", 8); //;id=
	ucLen += 8;

	memset(cMeid, 0, sizeof(cMeid));
	strncpy(cMeid, &iBaseInfo.cModIMEI[strlen(iBaseInfo.cModIMEI) - 7], 7);

	iCnt = strlen(cMeid);
	for (i = 0; i < iCnt; i++)
	{
		cAtBuf[ucLen++] = zpoc_char_to_ascii(cMeid[i] >> 4);
		cAtBuf[ucLen++] = zpoc_char_to_ascii(cMeid[i] & 0x0f);
	}

	memcpy(&cAtBuf[ucLen], "3b7077643d", 10); //;pwd=
	ucLen += 10;

	iCnt = strlen(pDefaultPwd);
	for (i = 0; i < iCnt; i++)
	{
		cAtBuf[ucLen++] = zpoc_char_to_ascii(pDefaultPwd[i] >> 4);
		cAtBuf[ucLen++] = zpoc_char_to_ascii(pDefaultPwd[i] & 0x0f);
	}

	memcpy(&cAtBuf[ucLen], "3b6770733d303b696e763d313b00\r\n", 30); //;gps=0;inv=1;
	ucLen += 30;

	at_send_at_cmd((unsigned char *)cAtBuf, ucLen);
}

void zpoc_set_volum_level(unsigned char value)
{
#ifdef USING_PA_HANDSET
	ql_set_volume(value + 1);
#else
	if (PIN_LEVEL_LOW == gpio_hph_get_lvl())
		ql_set_volume(value + 3);
	else
		ql_set_volume(value + 1);
#endif
}

void zpoc_set_hp_mute(unsigned char ucEnable)
{
	if (ucEnable)
		ql_set_volume(0);
	else
		zpoc_set_volum_level(iBaseSeting.ucVolLvl);
}

int zpoc_gps_send_location(char *pLat, char *pLon, char cNS, char cEW)
{
	char cAtCmd[128];
	char cTmpDat[64];
	unsigned short i = 0, j = 0;

	if (strlen(pLat) > 31 || strlen(pLon) > 31)
		return -1;

	if (cNS == 'S')
	{
		cTmpDat[j++] = zpoc_char_to_ascii('-' >> 4);
		cTmpDat[j++] = zpoc_char_to_ascii('-' & 0x0f);
	}

	while (pLat[i])
	{
		cTmpDat[j++] = zpoc_char_to_ascii(pLat[i] >> 4);
		cTmpDat[j++] = zpoc_char_to_ascii(pLat[i] & 0x0f);
		i++;
	}

	cTmpDat[j++] = zpoc_char_to_ascii(',' >> 4);
	cTmpDat[j++] = zpoc_char_to_ascii(',' & 0x0f);

	i = 0;
	if (cEW == 'W')
	{
		cTmpDat[j++] = zpoc_char_to_ascii('-' >> 4);
		cTmpDat[j++] = zpoc_char_to_ascii('-' & 0x0f);
	}

	while (pLat[i])
	{
		cTmpDat[j++] = zpoc_char_to_ascii(pLon[i] >> 4);
		cTmpDat[j++] = zpoc_char_to_ascii(pLon[i] & 0x0f);
		i++;
	}
	cTmpDat[j++] = '0';
	cTmpDat[j++] = '0';
	cTmpDat[j] = 0;

	sprintf(cAtCmd, "AT+POC=110000%s", cTmpDat);

	at_send_at_cmd((unsigned char *)cAtCmd, strlen(cAtCmd));

	return 0;
}

int zpoc_alarm_send_location(char *pLat, char *pLon, char cNS, char cEW)
{
	char cAtCmd[128];
	char cTmpDat[64];
	unsigned char i = 0, j = 0;

	if (strlen(pLat) > 31 || strlen(pLon) > 31)
		return -1;

	if (cNS == 'S')
	{
		cTmpDat[j++] = zpoc_char_to_ascii('-' >> 4);
		cTmpDat[j++] = zpoc_char_to_ascii('-' & 0x0f);
	}

	while (pLat[i])
	{
		cTmpDat[j++] = zpoc_char_to_ascii(pLat[i] >> 4);
		cTmpDat[j++] = zpoc_char_to_ascii(pLat[i] & 0x0f);
		i++;
	}

	cTmpDat[j++] = zpoc_char_to_ascii(',' >> 4);
	cTmpDat[j++] = zpoc_char_to_ascii(',' & 0x0f);

	i = 0;
	if (cEW == 'W')
	{
		cTmpDat[j++] = zpoc_char_to_ascii('-' >> 4);
		cTmpDat[j++] = zpoc_char_to_ascii('-' & 0x0f);
	}

	while (pLat[i])
	{
		cTmpDat[j++] = zpoc_char_to_ascii(pLon[i] >> 4);
		cTmpDat[j++] = zpoc_char_to_ascii(pLon[i] & 0x0f);
		i++;
	}
	cTmpDat[j++] = '0';
	cTmpDat[j++] = '0';
	cTmpDat[j] = 0;

	sprintf(cAtCmd, "%s%s", POC_CMD_REPORT_ALARM, cTmpDat);

	at_send_at_cmd((unsigned char *)cAtCmd, strlen(cAtCmd));

	return 0;
}

int zpoc_gps_modify_process(void)
{
	static unsigned char ucSendFlg = 0;
	static double dLstLat, dLstLon, dLat, dLon;
	double dLatDst, dLonDst;

	if (iRunPar.iGpsInfo.cLocSta == '1' || iRunPar.iGpsInfo.cLocSta == '2')
	{
		dLat = atof(iRunPar.iGpsInfo.stLat);
		dLon = atof(iRunPar.iGpsInfo.stLon);

		if (dLat > 1.0 && dLon > 1.0 && dLat <= 90.0 && dLon <= 180.0)
		{
			if (iStateFlg.bInitFilish == 1 && ucSendFlg == 0)
			{
				dLstLat = dLat;
				dLstLon = dLon;
				ucSendFlg = 1;

				zpoc_gps_send_location(iRunPar.iGpsInfo.stLat, iRunPar.iGpsInfo.stLon, iRunPar.iGpsInfo.cNS, iRunPar.iGpsInfo.cEW);
			}

			dLatDst = dLat - dLstLat;
			dLonDst = dLon - dLstLon;

			if ((dLatDst > 0.000200) || (dLatDst < -0.000200) || (dLonDst > 0.000200) || (dLonDst < -0.000200))
			{
				ucSendFlg = 0;
			}
		}
	}
	return 0;
}

int zpoc_at_unpacket_process(unsigned char ucCmdLen, char *pAtCmd, POC_AT_PROCESS_CALLBACK pCallBack)
{
	PIN_LEVEL_E lvl;

	_DEBUG("Recv AT:%s\r\n", pAtCmd);

	//去除换行回车
	pAtCmd[ucCmdLen - 1] = 0;
	pAtCmd[ucCmdLen - 2] = 0;

	if (strncmp(pAtCmd, "+POC:80", 7) == 0)
	{
		//+POC:80000000000200000011 0057 4b6dd58bc47e35000000
		if (iGroupList.usAddPtr < GROUP_MAX_SIZE)
		{
			memcpy(iGroupList.ucGroupId[iGroupList.usAddPtr], &pAtCmd[17], 8);
			zpoc_name_copy((unsigned char *)iGroupList.ucGroupName[iGroupList.usAddPtr], (unsigned char *)(&pAtCmd[29]));
			iGroupList.usAddPtr++;
		}
	}
	if (strncmp(pAtCmd, "+POC:8d", 7) == 0)
	{
		if (iRecordList.ucAddPtr < RECORD_MAX_SIZE)
		{
			unsigned char ucIdx;
			unsigned char i = 0;

			for (ucIdx = 15; ucIdx < 51; ucIdx += 2)
			{
				iRecordList.ucFileName[iRecordList.ucAddPtr][i++] = ui_ascii_to_char(pAtCmd[ucIdx], pAtCmd[ucIdx + 1]);
			}

			//memcpy(iGroupList.ucGroupId[iGroupList.usAddPtr], &pAtCmd[10], 8);
			zpoc_name_copy((unsigned char *)iRecordList.ucUserName[iRecordList.ucAddPtr], (unsigned char *)(&pAtCmd[51]));
			iRecordList.ucAddPtr++;
		}
	}
	else if (strncmp(pAtCmd, "+POC:0d", 7) == 0)
	{
		//+POC:0d000000000b
		iGroupList.usTotGroup = ui_ascii_to_char((unsigned char)pAtCmd[15], (unsigned char)pAtCmd[16]);
	}
	else if (strncmp(pAtCmd, "+POC:64", 7) == 0)
	{
		//+POC:640000000b
		iRecordList.ucTotRecord = ui_ascii_to_char((unsigned char)pAtCmd[13], (unsigned char)pAtCmd[14]);
		if (pCallBack)
			pCallBack(POC_AT_CB_DIS_RECORD_MODIFY, 0, NULL);
	}
	else if (strncmp(pAtCmd, "+POC:67", 7) == 0)
	{
		//+POC:67000000
		if (ui_ascii_to_char((unsigned char)pAtCmd[7], (unsigned char)pAtCmd[8]) == 0)
		{
			if (pCallBack)
				pCallBack(POC_AT_CB_DIS_CLEAR_REC_OK, 0, NULL);
		}
		else
		{
			if (pCallBack)
				pCallBack(POC_AT_CB_DIS_CLEAR_REC_OK, 1, NULL);
		}
	}
	else if (strncmp(pAtCmd, "+POC:5300", 9) == 0)
	{
		//+POC:53000000
		zpoc_name_copy((unsigned char *)iUserInfo.ucSosName, (unsigned char *)iUserInfo.ucUserName);
		if (pCallBack)
			pCallBack(POC_AT_CB_CMD_SOS_OUT, 1, NULL);
	}
	else if (strncmp(pAtCmd, "+POC:0e", 7) == 0)
	{
		if (pCallBack)
			pCallBack(POC_AT_CB_DIS_MEMBER_MODIFY, 0, NULL);
	}
	else if (strncmp(pAtCmd, "+POC:8103", 8) == 0)
	{
		if (iMemberList.usAddPtr < MEMBER_MAX_SIZE && strncmp((char *)iUserInfo.ucUserId, &pAtCmd[17], 8) != 0)
		{
			unsigned short usIdx;

			usIdx = (((unsigned short)ui_ascii_to_char(pAtCmd[13], pAtCmd[14])) << 8) | ui_ascii_to_char(pAtCmd[15], pAtCmd[16]);

			if (usIdx < iMemberList.usAddPtr || usIdx < iMemberList.usTotUser)
			{
				iMemberList.usTotUser = 0;
			}

			iMemberList.usAddPtr = usIdx;

			strncpy((char *)(iMemberList.ucUserId[iMemberList.usTotUser]), &pAtCmd[17], 8);
			zpoc_name_copy((unsigned char *)iMemberList.ucUserName[iMemberList.usTotUser], (unsigned char *)(&pAtCmd[25]));
			//iMemberList.usAddPtr++;
			//iMemberList.usTotUser = iMemberList.usAddPtr;
			iMemberList.usTotUser++;
		}
	}
	else if (strncmp(pAtCmd, "+POC:8202", 9) == 0)
	{
		//POC_PaSet(1);
		iStateFlg.bInitFilish = 1;

		strncpy((char *)(iUserInfo.ucUserId), &pAtCmd[9], 8);
		zpoc_name_copy((unsigned char *)iUserInfo.ucUserName, (unsigned char *)(&pAtCmd[17]));

		pa_output_control(0, 1);
		iStateFlg.bKeepSpk = 1;
		ucKeepPaCnt = 0;
	}
	else if (strncmp(pAtCmd, "+POC:8200", 9) == 0)
	{
		iStateFlg.bInitFilish = 0;
		iStateFlg.bTaking = 0;
		iStateFlg.bCallIn = 0;
		if (pCallBack)
			pCallBack(POC_AT_CB_CMD_OFFLINE, 0, NULL);
		//zpoc_name_copy((unsigned char*)iUserInfo.ucUserName, (unsigned char*)(&pAtCmd[17]));
	}
	else if (strncmp(pAtCmd, "+POC:830", 8) == 0)
	{
		uart_writecode_reply(pAtCmd);

		if (pAtCmd[8] == '1')
			iStateFlg.bPttAllow = 1;
		else
			iStateFlg.bPttAllow = 0;

		if (strncmp(&pAtCmd[9], "ffff", 4) == 0)
		{
			iStateFlg.bCallIn = 0;

			if (iStateFlg.bPocSend == 0)
			{
				if (pCallBack)
					pCallBack(POC_AT_CB_CMD_BEEND_CALL, 0, NULL);
			}

			ql_rtos_task_sleep_ms(100);
		}
		else
		{
			if (iStateFlg.bTaking == 1)
				iStateFlg.bTaking = 0;

			if (iStateFlg.bPttAllow == 0)
				iStateFlg.bPocSend = 0;

			zpoc_name_copy((unsigned char *)iUserInfo.ucTalkName, (unsigned char *)(&pAtCmd[17]));
			iStateFlg.bCallIn = 1;
			pa_output_control(0, 1);

			if (iStateFlg.bPttAllow == 0 || iStateFlg.bPocSend == 0)
			{
				if (pCallBack)
					pCallBack(POC_AT_CB_CMD_BEING_CALL, 0, &iUserInfo.ucTalkName);
			}
		}
	}
	else if (strncmp(pAtCmd, "+POC:8400", 9) == 0) //消息推送包含单呼内容
	{
		if (strncmp(&pAtCmd[9], "7c54eb5310629f52", 16) == 0) //呼叫成功
		{
			if (pCallBack)
				pCallBack(POC_AT_CB_CMD_INVITE_OK, 0, NULL);
		}
		else if (strncmp(&pAtCmd[9], "0090fa51344ef6657c54eb53", 24) == 0) //退出临时呼叫
		{
			if (pCallBack)
				pCallBack(POC_AT_CB_CMD_EXIT_INVITE, 0, NULL);
		}
		else if (strncmp(&pAtCmd[9], "7c54eb533159258d", 16) == 0) //呼叫失败
		{
			if (pCallBack)
				pCallBack(POC_AT_CB_CMD_INVITE_FAILD, 0, NULL);
		}
		else if (strncmp(&pAtCmd[9], "344ef6657c54eb53", 16) == 0)
		{
			if (iStateFlg.bTaking == 1)
			{
				iStateFlg.bTaking = 0;
			}

			zpoc_name_copy(iUserInfo.ucInvName, (unsigned char *)&pAtCmd[25]);

			//cfg_inv_record_add(NULL, iUserInfo.ucInvName, ZPOC_INVITE_IN, NULL);

			if (pCallBack)
				pCallBack(POC_AT_CB_CMD_BEINVITE, 0, iUserInfo.ucInvName);
		}
		else
		{
			if (pCallBack)
				pCallBack(POC_AT_CB_BORDCAST_MESSAGE, 0, &pAtCmd[9]);
		}
	}
	else if (strncmp(pAtCmd, "+POC:8b000", 10) == 0)
	{
		if (pAtCmd[10] == '1')
		{
			if (iBaseSeting.ucPttTone == 0)
			{
				zpoc_set_hp_mute(1);
				ql_rtos_task_sleep_ms(600);
				zpoc_set_hp_mute(iStateFlg.bMute);
			}
			pa_output_control(0, 1);
		}
		else if (pAtCmd[10] == '2')
		{
			pa_output_control(0, 1);
		}
		else
		{
			iStateFlg.bPlayBack = 0;

			if (iStateFlg.bKeepSpk == 0)
				pa_output_control(0, 0);
		}
	}
	else if (strncmp(pAtCmd, "+POC:6900", 9) == 0)
	{
		iStateFlg.bPlayBack = 0;
	}
	else if (strncmp(pAtCmd, "+POC:8e00", 9) == 0)
	{
		unsigned char ucLen = strlen(pAtCmd);
		unsigned char ucIdx;

		memset(iUserInfo.ucSosName, 0, sizeof(iUserInfo.ucSosName));
		for (ucIdx = 13; ucIdx < ucLen; ucIdx += 2)
		{
			if (pAtCmd[ucIdx] == '0' && pAtCmd[ucIdx + 1] == '0')
			{
				zpoc_name_copy((unsigned char *)iUserInfo.ucSosName, (unsigned char *)(&pAtCmd[ucIdx + 2]));
				break;
			}
		}

		if (pCallBack)
			pCallBack(POC_AT_CB_CMD_SOS_IN, 0, NULL);
	}
	else if (strncmp(pAtCmd, "+POC:8f00", 9) == 0)
	{
		if (pCallBack)
			pCallBack(POC_AT_CB_CMD_SOS_STOP, 0, NULL);
	}
	else if (strncmp(pAtCmd, "+POC:8600", 8) == 0)
	{
		iStateFlg.bInitFilish = 1;
		strncpy((char *)iUserInfo.ucGroupId, &pAtCmd[9], 8);
		zpoc_name_copy((unsigned char *)iUserInfo.ucGroupName, (unsigned char *)(&pAtCmd[17]));
		if (pCallBack)
			pCallBack(POC_AT_CB_CMD_ENTER_GROUP, 0, NULL);
	}

	else if (strncmp(pAtCmd, "+POC:0b", 7) == 0)
	{
		if (pAtCmd[8] == '0')
		{
#ifdef LANGUAGE_SETTING_CH
			iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME;
#endif
#ifdef LANGUAGE_SETTING_EN
			iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME * (iBaseSeting.ucSendTime + 1);
#endif
#ifdef LANGUAGE_SETTING_TU
			iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME * (iBaseSeting.ucSendTime + 1);
#endif
			iStateFlg.bPocSend = 1;
		}

		ql_gpio_get_level(PIN_KEYPTT1_NUM, &lvl);
		if (lvl == PIN_LEVEL_HIGH)
		{
			//printf("[YDPOC] 0b release ptt\r\n");
			gpio_push_key_msg(KEY_PTT1_RELEASE);
		}
	}
	else if (strncmp(pAtCmd, "+POC:0c", 7) == 0)
	{
		ql_gpio_get_level(PIN_KEYPTT1_NUM, &lvl);
		if (lvl == PIN_LEVEL_LOW && iRunPar.ucSendCnt > 0)
		{
			//printf("[YDPOC] 0c press ptt\r\n");
			gpio_push_key_msg(KEY_PTT1_PRESS);
		}

		iStateFlg.bPocSend = 0;
	}

	else if (strncmp(pAtCmd, "+POC:6500", 9) == 0)
	{
		iStateFlg.bPlayBack = 1;
	}
	else if (strstr(pAtCmd, "+QTTS: 0"))
	{
		if (iStateFlg.bKeepSpk == 0 && iStateFlg.bCallIn == 0 && iStateFlg.bPlayBack == 0)
		{
			pa_output_control(0, 0);
		}
	}

	else if (strncmp(pAtCmd, "+POC:00000000", 10) == 0)
	{
		if (pCallBack)
			pCallBack(POC_AT_CB_CMD_POC_STARTED, 0, NULL);
	}
	else if (strncmp(pAtCmd, "+POC:Ready", 10) == 0)
	{
		if (iRunPar.ucOWCFlg == 0)
		{
			if (pCallBack)
				pCallBack(POC_AT_CB_CMD_POC_READY, 0, NULL);
			ql_rtos_task_sleep_ms(500);
			at_send_at_cmd((unsigned char *)POC_CMD_START, strlen(POC_CMD_START));
		}
	}
	else if (strncmp(pAtCmd, "+POC:4600", 9) == 0)
	{
		int i;
		strncpy(iBaseInfo.cPocVer, &pAtCmd[14], sizeof(iBaseInfo.cPocVer) - 1);

		for (i = 0; i < sizeof(iBaseInfo.cPocVer); i++)
		{
			if (iBaseInfo.cPocVer[i] == '\r' || iBaseInfo.cPocVer[i] == '\n')
				iBaseInfo.cPocVer[i] = 0;
		}
	}
#if 1
	else if (strncmp(pAtCmd, "+POC:0200", 9) == 0)
	{
		unsigned int ucIdx = 0;
		char *pAcc = strstr(pAtCmd, "69643d"); //id=
		pAcc += 6;
		while (*pAcc != 0)
		{
			if (*pAcc == '3' && *(pAcc + 1) == 'b')
				break;

			iBaseInfo.cAccount[ucIdx++] = zpoc_ascii_to_char(*pAcc, *(pAcc + 1));
			pAcc += 2;
		}
		iBaseInfo.cAccount[ucIdx] = 0;

		ucIdx = 0;
		pAcc = strstr(pAtCmd, "69703d"); //ip=
		pAcc += 6;
		while (*pAcc != 0)
		{
			if (*pAcc == '3' && *(pAcc + 1) == 'b')
				break;

			iBaseInfo.cIpAddr[ucIdx++] = zpoc_ascii_to_char(*pAcc, *(pAcc + 1));
			pAcc += 2;
		}
		iBaseInfo.cIpAddr[ucIdx] = 0;

		pAcc = strstr(pAtCmd, "696e763d"); //inv=
		pAcc += 9;
		if (*pAcc == '1')
			iRunPar.ucInvEn = 1;
		else
			iRunPar.ucInvEn = 0;

		lcd_update_disp();

		if (pCallBack)
			pCallBack(POC_AT_CB_CMD_GET_ACCOUNT, 0, 0);
	}
#endif
	else if (strncmp(pAtCmd, "+POC:41", 7) == 0)
	{
		if (pCallBack)
			pCallBack(POC_AT_CB_DIS_OTA_POC_STATE, zpoc_ascii_to_char(pAtCmd[7], pAtCmd[8]), &pAtCmd[11]);
	}
	else if (strncmp(pAtCmd, "+POC:4a", 7) == 0)
	{
		if (pCallBack)
			pCallBack(POC_AT_CB_DIS_OTA_MCU_STATE, zpoc_ascii_to_char(pAtCmd[7], pAtCmd[8]), &pAtCmd[11]);
	}
	else if (strncmp(pAtCmd, "+POC:45", 7) == 0)
	{
		if (pCallBack)
			pCallBack(POC_AT_CB_DIS_OTA_OWC_STATE, zpoc_ascii_to_char(pAtCmd[7], pAtCmd[8]), &pAtCmd[11]);
	}
	else if (strncmp(pAtCmd, "+POC:600000", 11) == 0)
	{
		if (pCallBack)
			pCallBack(POC_AT_CB_DIS_TIMEZONE_MODIFY, zpoc_ascii_to_char(pAtCmd[11], pAtCmd[12]), 0);
	}
	return 1;
}

int zpoc_set_prefer_net_mode(enNetMode iNetMode)
{
	QL_NW_CONFIG_INFO_T info;

	info.roaming_pref = 1;

	switch (iNetMode)
	{
	case ZPOC_NET_GSM:
		info.preferred_nw_mode = QL_NW_PREF_NET_TYPE_GSM;
		break;

	case ZPOC_NET_LTE:
		info.preferred_nw_mode = QL_NW_PREF_NET_TYPE_LTE;
		break;

	default:
		info.preferred_nw_mode = QL_NW_PREF_NET_TYPE_GSM_LTE_PREF_LTE;
		break;
	}

	return (int)ql_nw_set_config(&info);
}

void zpoc_set_tx_gain(signed char cGainLvl)
{
	ql_set_txcodec_gain_table(QL_VC_HANDSET, 0, 3, cGainLvl);
	ql_set_txcodec_gain_table(QL_VC_HEADSET, 0, 3, cGainLvl);
}

unsigned short zpoc_get_bat_level(void)
{
	static unsigned short usBatValLst = 0;
	unsigned short usBatVal;
	unsigned short usBatLvl = 11;

	usBatVal = ql_get_battery_vol();

	if (usBatValLst == 0)
	{
		usBatValLst = usBatVal;
	}
	else
	{
		usBatVal = (usBatVal + usBatValLst) / 2;
		usBatValLst = usBatVal;
	}

	for (usBatLvl = 0; usBatLvl < 11; usBatLvl++)
	{
		if (usBatVal < BatLevelValue[usBatLvl])
			break;
	}

	//printf("Get Bat Value:%d Level:%d\r\n", usBatVal, usBatLvl);
	return usBatLvl;
}

int zpoc_get_system_time(stTime *pTime)
{
	ql_rtc_time_t tm;
	ql_rtc_get_time(&tm);

	//_DEBUG("\r\n*****Mcu tm_year:%d tm_mon:%d tm_mday:%d tm_hour:%d tm_min:%d tm_set:%d\r\n", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	pTime->ucHour = (unsigned char)(tm.tm_hour);
	pTime->ucMint = (unsigned char)(tm.tm_min);
	pTime->ucSecond = (unsigned char)(tm.tm_sec);

	pTime->iYear = tm.tm_year;
	pTime->iMonth = tm.tm_mon;
	pTime->iMDay = tm.tm_mday;
	pTime->iWDay = tm.tm_wday;

	return 1;
}

int zpoc_tts_hton(char *pSrc, char *pDst)
{
	int i = 0;
	int iLen = strlen(pSrc);

	if (iLen > 0 && (iLen % 4 == 0))
	{
		while (pSrc[i])
		{
			pDst[i] = pSrc[i + 2];
			pDst[i + 1] = pSrc[i + 3];
			pDst[i + 2] = pSrc[i];
			pDst[i + 3] = pSrc[i + 1];

			i += 4;
		}
	}
	return 0;
}

void zpoc_play_tts(char *pTTS)
{
#ifndef LANGUAGE_SETTING_TU
	char cTTSCmd[300];
	if (iStateFlg.bCallIn == 0)
	{
		memset(cTTSCmd, 0, sizeof(cTTSCmd));

		zpoc_tts_hton(pTTS, cTTSCmd);
		pa_output_control(0, 1);
		ql_tts_play(1, cTTSCmd);
	}
#endif
}

void zpoc_play_vol_level(unsigned char ucVol)
{
#ifndef REMOVE_TTS_AND_SYNC_CHARGE
	if (iStateFlg.bCallIn == 0 && iStateFlg.bTaking == 0 && iStateFlg.bMute == 0)
	{
		zpoc_play_tts((char *)(cVolLvlTTS[ucVol]));
	}
#endif
}

void back_to_desktop(unsigned int arg1)
{
	RF_Rxon();
	toa.bk_rxon_state = 1;

	switch (iBaseSeting.ucWorkMode)
	{
	case 0:
		STATE_MgrChangeState(POC_STATE_DESKTOP_NET, arg1);
		break;
	case 1:
		STATE_MgrChangeState(POC_STATE_DESKTOP_ANALOG, arg1);
		break;
	case 2:
		STATE_MgrChangeState(POC_STATE_DESKTOP_MIX, arg1);
		break;
	case 3:
		STATE_MgrChangeState(POC_STATE_DESKTOP_REPEATER, arg1);
		break;
	default:
		STATE_MgrChangeState(POC_STATE_DESKTOP_MIX, arg1);
		break;
	}
}

void ptt1_press_handle(void)
{
	if (iStateFlg.bInitFilish)
	{
		gpio_set_mic_sw(0); //0:g 1:m
		at_send_at_cmd((unsigned char *)POC_CMD_0B0000, strlen(POC_CMD_0B0000));

		if (iStateFlg.bPttAllow)
		{
			if (iBaseSeting.ucPttTone == 1)
			{
				pa_output_control(0, 1);
			}
			else
			{
				zpoc_set_hp_mute(1);
				//pa_output_control(0, 0);//测试中继模式
			}

			iStateFlg.bTaking = 1;
			ql_rtos_task_sleep_ms(50);

			if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_NET)
			{
				iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME;
				desktop_net_modify_display(POC_GROUP_MAIN_CALL);
			}
			else if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_MIX)
			{
				iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME;
				desktop_mix_modify_display(POC_GROUP_MAIN_CALL);
			}
			else if (STATE_MgrGetCurStateId() == POC_STATE_INVITE)
			{
				iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME;
				invite_modify_display(iInviteType, POC_GROUP_MAIN_CALL);
			}
			else
			{
				/* code */
			}

			gpio_state_led_reflesh(&iStateFlg);

			if (iBaseSeting.ucPttTone == 1)
			{
				ql_rtos_task_sleep_ms(100);
				pa_output_control(0, 0);
			}
			else
			{
				ql_rtos_task_sleep_ms(500);
				zpoc_set_hp_mute(iStateFlg.bMute);
			}
		}
	}
	else
	{
		zpoc_play_tts((char *)cDesktopNotOnlineTTS);
	}

	ql_eint_enable_wakeup(PIN_KEYPTT1_NUM, PIN_BOTH_EDGE);
}

void ptt1_release_handle(void)
{
	if (iStateFlg.bInitFilish && (iStateFlg.bTaking || iStateFlg.bPocSend))
	{
		iStateFlg.bPttAllow = 1;

		if (iBaseSeting.ucPttTone == 1)
		{
			pa_output_control(0, 1);
		}
		else
		{
			zpoc_set_hp_mute(1);
			pa_output_control(0, 0);
		}

		ql_rtos_task_sleep_ms(380);
		at_send_at_cmd((unsigned char *)POC_CMD_0C0000, strlen(POC_CMD_0C0000));
		iStateFlg.bTaking = 0;

		if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_NET)
		{
			desktop_net_modify_display(POC_CALL_IDLE);
		}
		else if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_ANALOG)
		{
			desktop_analog_modify_display(POC_CALL_IDLE);
		}
		else if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_MIX)
		{
			desktop_mix_modify_display(POC_CALL_IDLE);
		}
		else if (STATE_MgrGetCurStateId() == POC_STATE_INVITE)
		{
			invite_modify_display(iInviteType, POC_CALL_IDLE);
		}
		else
		{
			/* code */
		}

		gpio_state_led_reflesh(&iStateFlg);
		if (iBaseSeting.ucPttTone == 1)
		{
			ql_rtos_task_sleep_ms(250);
			pa_output_control(0, 0);
		}
		else
		{
			ql_rtos_task_sleep_ms(600);
			zpoc_set_hp_mute(iStateFlg.bMute);
		}
	}
	ql_eint_enable_wakeup(PIN_KEYPTT1_NUM, PIN_BOTH_EDGE);
	gpio_state_led_reflesh(&iStateFlg);
}

void ptt2_press_handle(void)
{
	_DEBUG("ptt2_press_handle\r\n");
	gpio_set_mic_sw(1); //0:g 1:m
	pa_output_control(0, 1);
	tx_ctcss_and_cdcss_set();
	RF_Txon();
	bk4819_vtx_gpio1_output(1); //发射时高电平、松开时低电平

	toa.tx_flag = 1;
	if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_NET)
	{
		iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME;
		desktop_net_modify_display(ANALOG_MAIN_CALL);
	}
	else if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_ANALOG)
	{
		iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME;
		desktop_analog_modify_display(ANALOG_MAIN_CALL);
	}
	else if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_MIX)
	{
		iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME;
		desktop_mix_modify_display(ANALOG_MAIN_CALL);
	}
	else if (STATE_MgrGetCurStateId() == POC_STATE_INVITE)
	{
		iRunPar.ucSendCnt = SIGNAL_SEND_MAX_TIME;
		invite_modify_display(iInviteType, ANALOG_MAIN_CALL);
	}
	else
	{
		/* code */
	}

	ql_eint_enable_wakeup(PIN_KEYPTT2_NUM, PIN_BOTH_EDGE);
}

void ptt2_release_handle(void)
{
	release_analog_ptt_handle();

	if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_NET)
	{
		desktop_net_modify_display(POC_CALL_IDLE);
	}
	else if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_ANALOG)
	{
		desktop_analog_modify_display(POC_CALL_IDLE);
	}
	else if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_MIX)
	{
		desktop_mix_modify_display(POC_CALL_IDLE);
	}
	else if (STATE_MgrGetCurStateId() == POC_STATE_INVITE)
	{
		invite_modify_display(iInviteType, POC_CALL_IDLE);
	}
	else
	{
		/* code */
	}

	ql_eint_enable_wakeup(PIN_KEYPTT2_NUM, PIN_BOTH_EDGE);
}

void volume_up(void)
{
	if (iBaseSeting.ucVolLvl < 7)
	{
		if (iStateFlg.bMute == 1)
		{
			iStateFlg.bMute = 0;
			zpoc_set_hp_mute(iStateFlg.bMute);

			if (iStateFlg.bCallIn || iStateFlg.bPlayBack)
			{
				pa_output_control(0, 1);
			}
			ui_disp_icon_mute(iStateFlg.bMute);
			ui_disp_icon_vol(iBaseSeting.ucVolLvl);
			zpoc_play_vol_level(iBaseSeting.ucVolLvl);
			lcd_update_disp();
		}
		else
		{
			iBaseSeting.ucVolLvl++;
			ui_disp_icon_vol(iBaseSeting.ucVolLvl);

			zpoc_set_volum_level(iBaseSeting.ucVolLvl);
			zpoc_play_vol_level(iBaseSeting.ucVolLvl);
			lcd_update_disp();
		}
	}
	else
	{
		if (iStateFlg.bMute == 1)
		{
			iStateFlg.bMute = 0;
			zpoc_set_hp_mute(iStateFlg.bMute);

			if (iStateFlg.bCallIn || iStateFlg.bPlayBack)
			{
				pa_output_control(0, 1);
			}

			ui_disp_icon_mute(iStateFlg.bMute);
			ui_disp_icon_vol(iBaseSeting.ucVolLvl);
			lcd_update_disp();
		}
	}
}

void volume_down(void)
{
	if (iBaseSeting.ucVolLvl > 0)
	{
		if (iStateFlg.bMute == 1)
		{
			iStateFlg.bMute = 0;
			zpoc_set_hp_mute(iStateFlg.bMute);

			if (iStateFlg.bCallIn || iStateFlg.bPlayBack)
			{
				pa_output_control(0, 1);
			}

			ui_disp_icon_mute(iStateFlg.bMute);
			ui_disp_icon_vol(iBaseSeting.ucVolLvl);
			lcd_update_disp();
		}
		else
		{
			iBaseSeting.ucVolLvl--;
			ui_disp_icon_vol(iBaseSeting.ucVolLvl);

			zpoc_set_volum_level(iBaseSeting.ucVolLvl);
			zpoc_play_vol_level(iBaseSeting.ucVolLvl);
			lcd_update_disp();
		}
	}
	else
	{
		if (iStateFlg.bMute == 0)
		{
			pa_output_control(0, 0);
			iStateFlg.bMute = 1;
			zpoc_set_hp_mute(iStateFlg.bMute);
			ui_disp_icon_mute(iStateFlg.bMute);
			lcd_update_disp();
		}
	}
}