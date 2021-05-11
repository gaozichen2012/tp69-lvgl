#ifndef __AT_H__
#define __AT_H__

#include "general_include.h"

#define QUEC_AT_NET_PORT 10086

#define AT_BUF_SIZE 512

#define POC_SET_SECOND_PLL_GAIN "at+qiic=0,0x1b,0x19,2,0xafaf\r\n"

#define POC_CMD_START "AT+POC=00000000\r\n"

#define POC_CMD_ENTER_RSET "AT+POC=45000000\r\n"

#define POC_CMD_0B0000 "AT+POC=0B000000\r\n"
#define POC_CMD_0C0000 "AT+POC=0C000000\r\n"

#define POC_CMD_RECORD_CLEAR "AT+POC=67000000\r\n"

#define POC_CMD_RECORD_ON "AT+POC=63000001\r\n"
#define POC_CMD_RECORD_OFF "AT+POC=63000000\r\n"

#define POC_CMD_REPORT_ALARM "AT+POC=530000"
#define POC_CMD_STOP_ALARM "AT+POC=540000\r\n"
#define POC_CMD_IGNORE_ALARM "AT+POC=680000\r\n"
#define POC_CMD_REQ_STOP_PLAY_RECORD "AT+POC=69000000\r\n"

#define POC_CMD_SET_TIME_ZONE "AT+POC=600000"
#define POC_CMD_UPDATE_POC "AT+POC=41000000\r\n"

#ifdef LANGUAGE_SETTING_CH
#define POC_CMD_UPDATE_MCU "AT+POC=4A0000515545435f4d43555f5450333938420000\r\n" //"QUEC_MCU_TP398B"
#define POC_CMD_SET_TTS_VOL -20000                                               //"AT+QTTSETUP=1,2,-10000\r\n"
#define AT_SET_TTS_LANGUAGE "AT+QTTSETUP=1,3,0\r\n"
#endif

#ifdef LANGUAGE_SETTING_EN
#define POC_CMD_UPDATE_MCU "AT+POC=4A0000515545435f4d43555f5450333938450000\r\n" //"QUEC_MCU_TP398E"
#define POC_CMD_SET_TTS_VOL -20000                                               //"AT+QTTSETUP=1,2,-10000\r\n"
#define AT_SET_TTS_LANGUAGE "AT+QTTSETUP=1,3,1\r\n"
#endif

#ifdef LANGUAGE_SETTING_TU
#define POC_CMD_UPDATE_MCU "AT+POC=4A0000515545435f4d43555f5450333938540000\r\n" //"QUEC_MCU_TP398T"
#define POC_CMD_SET_TTS_VOL -20000                                               //"AT+QTTSETUP=1,2,-32768\r\n"
#define AT_SET_TTS_LANGUAGE "AT+QTTSETUP=1,3,1\r\n"
#endif

#define POC_CMD_ENTER_GROUP "AT+POC=090000"
#define POC_CMD_REQ_IVITE "AT+POC=0A0000"
#define POC_CMD_REQ_MEMBER_LIST "AT+POC=0E0000"
#define POC_CMD_REQ_FRIEND_LIST "AT+POC=0E000000000000"
#define POC_CMD_REQ_PLAY_RECORD "AT+POC=650000"

#define POC_CMD_REQ_PLAY_LAST "AT+POC=65000064"
#define POC_CMD_SAVE_REC2FLASH "AT+POC=66000000\r\n"

#define POC_CMD_GET_POC_VERSION "AT+POC=46000000\r\n"
#define POC_CMD_GET_REC_LIST "AT+POC=64000000\r\n"

#define POC_CMD_GET_ACCOUNT "AT+POC=02000000\r\n"

#define POC_CMD_SET_DL_GAIN "AT+QAUDCFG=\"alc5616/dlgain\",75\r\n"
#define POC_CMD_SET_UL_GAIN "AT+QAUDCFG=\"alc5616/ulgain\",55\r\n"

int at_recv_thread_create(void);
void at_send_at_cmd(unsigned char *pCmd, unsigned char ucLeng);
void at_push_cmd_msg(char *pBuf, unsigned short us_Len);
#endif
