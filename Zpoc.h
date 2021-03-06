#ifndef __ZPOC_H__
#define __ZPOC_H__

#include "general_include.h"

#define DEBUG_EN 1
#define DEBUG_TO_USB 0

#if !defined(LANGUAGE_SETTING_EN) && !defined(LANGUAGE_SETTING_CH) && !defined(LANGUAGE_SETTING_TU)
#error Field descriptor for SYSTEM_MENU_CODE_TYPE. Please Choise one type language.
#endif

#ifdef LANGUAGE_SETTING_CH
#define SYSTEM_MENU_CODE_TYPE UI_MENU_CODE_UNICODE
#define SYSTEM_MCU_PROC_NAME "mcu_ch"
#define MCU_VERSION "QU_TP69_V00.21"
#endif

#ifdef LANGUAGE_SETTING_EN
#define SYSTEM_MENU_CODE_TYPE UI_MENU_CODE_ASCII
#define SYSTEM_MCU_PROC_NAME "mcu_en"
#define MCU_VERSION "QU_TP69E_V00.06"
#endif

#ifdef LANGUAGE_SETTING_TU
#define SYSTEM_MENU_CODE_TYPE UI_MENU_CODE_UTF8
#define SYSTEM_MCU_PROC_NAME "mcu_tu"
#define MCU_VERSION "QU_TP69T_V00.06"
#endif

#define SYSTEM_MCU_PROC_EN "mcu_en"
#define SYSTEM_MCU_PROC_CH "mcu_ch"
#define SYSTEM_MCU_PROC_TU "mcu_tu"

#define MIC_TX_DEFAULT_GAIN 12

#define GROUP_MAX_SIZE 300
#define MEMBER_MAX_SIZE 500

#define RECORD_MAX_SIZE 30

#define USER_NAME_MAX_SIZE 48
#define GROUP_NAME_MAX_SIZE 48
#define FILE_NAME_MAX_SIZE 18

#define ZPOC_RECORD_MAX_ITEM 50

#ifdef LANGUAGE_SETTING_CH
#define SIGNAL_SEND_MAX_TIME 50
#endif

#ifdef LANGUAGE_SETTING_EN
#define SIGNAL_SEND_MAX_TIME 60
#endif

#ifdef LANGUAGE_SETTING_TU
#define SIGNAL_SEND_MAX_TIME 60
#endif

#define POC_INIT_START_TIME 10

#define NET_MODE_GSM (1 << 0)
#define NET_MODE_WCDMA (1 << 1)
#define NET_MODE_CDMA (1 << 2)
#define NET_MODE_EVDO (1 << 3)
#define NET_MODE_LTE (1 << 4)
#define NET_MODE_TDSCDAM (1 << 5)
#define NET_MODE_16_PRL (1 << 6)

#define NET_MODE_ALL 0x3F

typedef enum
{
	ZPOC_LANGUAGE_EN,
	ZPOC_LANGUAGE_TU
} ENUM_LANGUAGE;

typedef enum
{
	ZPOC_MSG_ANALOG = 1,
	ZPOC_MSG_AT_RECV,
	ZPOC_MSG_KEY,
	ZPOC_MSG_TIMER,
	ZPOC_MSG_UART,
	ZPOC_MSG_GPS,
	ZPOC_MSG_BATLOW,
	ZPOC_MSG_TIMER10MS,
} ENUM_MSG_TYPE;

typedef enum
{
	ZPOC_NET_UNKNOW = 0,
	ZPOC_NET_GSM,
	ZPOC_NET_UMTS,
	ZPOC_NET_LTE,
	ZPOC_NET_AUTO
} enNetMode;

typedef enum
{
	UI_MENU_CODE_ASCII,
	UI_MENU_CODE_UNICODE,
	UI_MENU_CODE_UTF8,
} euCodeType;

typedef struct
{
	ENUM_MSG_TYPE enMsgType;
	void *pMsg;
	unsigned int usMsgLen;
} ZPOC_MSG_TYPE;

typedef struct
{
	unsigned short usCurGroup;
	unsigned short usSelGroup;
	unsigned short usTotGroup;
	unsigned short usAddPtr;
	unsigned char ucGroupName[GROUP_MAX_SIZE][GROUP_NAME_MAX_SIZE];
	unsigned char ucGroupId[GROUP_MAX_SIZE][8];
} stGroupList;

typedef struct
{
	unsigned char ucTotRecord;
	unsigned char ucAddPtr;
	unsigned char ucFileName[RECORD_MAX_SIZE][FILE_NAME_MAX_SIZE];
	unsigned char ucUserName[RECORD_MAX_SIZE][USER_NAME_MAX_SIZE];
} stRecordList;

typedef struct
{
	unsigned short usSelUser;
	unsigned short usTotUser;
	unsigned short usAddPtr;
	unsigned char ucUserName[MEMBER_MAX_SIZE][USER_NAME_MAX_SIZE];
	unsigned char ucUserId[MEMBER_MAX_SIZE][8];
} stMemberList;

typedef struct
{
	unsigned char ucUserId[8];
	unsigned char ucGroupId[8];
	unsigned char ucUserName[USER_NAME_MAX_SIZE];
	unsigned char ucGroupName[GROUP_NAME_MAX_SIZE];
	unsigned char ucTalkName[USER_NAME_MAX_SIZE];
	unsigned char ucInvName[USER_NAME_MAX_SIZE];
	unsigned char ucSosName[USER_NAME_MAX_SIZE];
} stUserInfo;

typedef struct
{
	unsigned int bSimState : 1;
	unsigned int bInitFilish : 1; //?????????????????????????????????
	unsigned int bCallIn : 1;	  //???????????????
	unsigned int bTaking : 1;	  //?????????????????????
	unsigned int bPttAllow : 1;	  //????????????????????????
	unsigned int bBlSta : 1;	  //????????????
	unsigned int bMute : 1;		  //????????????
	unsigned int bKeepSpk : 1;
	unsigned int bSleep : 1;
	unsigned int bUsbIn : 1;
	unsigned int bChrFlg : 1;
	unsigned int bAlarm : 1;
	unsigned int bPlayBack : 1;
	unsigned int bPocSend : 1;
	unsigned int bSpkSw : 1;
} stStateFlg;

typedef struct
{
	char cModVer[80];
	char cPocVer[20];
	char cMcuVer[20];
	char cModIMEI[20];
	char cAccount[20];
	char cIpAddr[20];
} stBaseInfo;

typedef enum
{
	GPS_DEVICE_NOT_DETECT = '0', //??????????????????
	GPS_DEVICE_NORMAL = '1',	 //????????????
} euGpsModSta;

typedef struct
{
	char stLat[32];		//?????? ???
	char cNS;			//??????
	char stLon[32];		//?????????
	char cEW;			//??????
	char cLocSta;		//???????????? ???1??? ???????????????  ???0??? ?????????
	char cStarNum[4];	//????????????
	euGpsModSta iState; //GPS????????????
} stGpsInfo;

typedef enum
{
	ZPOC_INVITE_OUT_OK,
	ZPOC_INVITE_OUT_FAILD,
	ZPOC_INVITE_IN,
} enInviteState;

struct stInvRecNode
{
	unsigned char ucId[8];
	unsigned char uname[50];
	unsigned char time[20];
	enInviteState ucSta;

	struct stInvRecNode *pNext;
	struct stInvRecNode *pPrev;
};

typedef struct tagInvRecList
{
	unsigned short usTotal;

	struct stInvRecNode *pHead;
	struct stInvRecNode *pTail;
} stInvRecList;

typedef struct
{
	unsigned char ucHour;
	unsigned char ucMint;
	unsigned char ucSecond;

	int iYear;
	int iMonth;
	int iMDay;
	int iWDay;
} stTime;
typedef struct
{
	unsigned char ucLock;
	unsigned char ucCSQ;
	unsigned char ucBattry;
	unsigned char ucGps;
	unsigned char ucPwrKeyFlg;
	unsigned char ucGpsSta;
	unsigned char iNetMode;
	stGpsInfo iGpsInfo;
	stTime iTime;
	unsigned char ucInvEn;
	unsigned char ucOWCFlg;
	unsigned char ucChargeFlg;
	unsigned char ucPreChrFlg;
	unsigned char ucSendCnt; //??????????????????
	signed char cDefTxGain;
} stRunPar;

typedef enum
{
	POC_AT_CB_CMD_POC_READY,   //POC ??????????????????
	POC_AT_CB_CMD_POC_STARTED, //POC ????????????
	POC_AT_CB_CMD_LOGIN_OK,	   //POC ????????????
	POC_AT_CB_CMD_OFFLINE,	   //POC ????????????
	POC_AT_CB_CMD_ENTER_GROUP, //POC ????????????
	POC_AT_CB_CMD_BEING_CALL,  //POC ?????????
	POC_AT_CB_CMD_BEEND_CALL,  //POC ???????????????

	POC_AT_CB_CMD_INVITE_OK,	//POC ??????????????????
	POC_AT_CB_CMD_INVITE_FAILD, //POC ??????????????????
	POC_AT_CB_CMD_EXIT_INVITE,	//POC ????????????
	POC_AT_CB_CMD_BEINVITE,		//POC ?????????

	POC_AT_CB_CMD_SOS_IN,  //????????????SOS??????
	POC_AT_CB_CMD_SOS_OUT, //SOS??????????????????
	POC_AT_CB_CMD_SOS_STOP,

	POC_AT_CB_CMD_GET_ACCOUNT,

	POC_AT_CB_SYS_SYNC_TIME,

	POC_AT_CB_BORDCAST_MESSAGE, //?????????????????????

	POC_AT_CB_DIS_OTA_POC_STATE, //POC ??????????????????
	POC_AT_CB_DIS_OTA_MCU_STATE,
	POC_AT_CB_DIS_OTA_OWC_STATE,

	POC_AT_CB_DIS_GROUP_MODIFY,
	POC_AT_CB_DIS_MEMBER_MODIFY,

	POC_AT_CB_DIS_RECORD_MODIFY,
	POC_AT_CB_DIS_CLEAR_REC_OK,

	POC_AT_CB_DIS_TIMEZONE_MODIFY,
} euAtCBType;

typedef enum //????????????
{
	POC_CALL_IDLE,
	POC_GROUP_MAIN_CALL,
	POC_GROUP_BEING_CALL,
	ANALOG_MAIN_CALL,
	ANALOG_BEING_CALL,
} euCallType;

typedef enum //?????????????????????
{
	POC_INVITE_IDEL,
	POC_INVITE_MAIN_CALL, //??????????????????
	POC_INVITE_BEING_CALL //???????????????
} euInviteType;

typedef enum //????????????
{
	FREQSCAN_IDLE,
	FREQSCAN_FREQ_SUCCESS,
	FREQSCAN_SUBAUDIO_IDLE,
	FREQSCAN_SUBAUDIO_CTC,
	FREQSCAN_SUBAUDIO_CDC
} euFreqscanType;

typedef short (*POC_AT_PROCESS_CALLBACK)(unsigned int uiMsgType, unsigned int uiArg1, void *pArg2);

extern stGroupList iGroupList;
extern stMemberList iMemberList;
extern stUserInfo iUserInfo;
extern stStateFlg iStateFlg;
extern stBaseInfo iBaseInfo;
extern stBaseSeting iBaseSeting;
extern stRunPar iRunPar;
extern stInvRecList iInvRecList;
extern stRecordList iRecordList;

int zpoc_enter_sleep(int wakelockfd);
int zpoc_exit_sleep(int wakelockfd);

void zpoc_process_init(void);
void zpoc_member_list_clr(void);

void zpoc_push_uart_msg(char *pBuf);
void zpoc_work_proc(ZPOC_MSG_TYPE *pMsg);

int zpoc_hb_timer_create(void);
int zpoc_at_unpacket_process(unsigned char ucCmdLen, char *pAtCmd, POC_AT_PROCESS_CALLBACK pCallBack);
int zpoc_timer_process(void);
int zpoc_gps_modify_process(void);

unsigned char zpoc_name_copy(unsigned char *pDst, unsigned char *pSrc);

int zpoc_set_prefer_net_mode(enNetMode iNetMode);
enNetMode zpoc_get_net_mode(void);
unsigned short zpoc_get_bat_level(void);
unsigned char zpoc_get_signal_level(void);

unsigned char zpoc_ascii_to_char(unsigned char nib_h, unsigned char nib_l);

void zpoc_play_tts(char *pTTS);

unsigned char *zpoc_get_member_id_by_name(unsigned char *pName);
void zpoc_set_volum_level(unsigned char value);
void zpoc_wakeup_timer_create(void);
int zpoc_get_system_time(stTime *pTime);
char zpoc_char_to_ascii(char c);
void zpoc_reset_factory_account(void);
void zpoc_set_hp_mute(unsigned char ucEnable);

int zpoc_alarm_send_location(char *pLat, char *pLon, char cNS, char cEW);
int zpoc_modem_state_thread_create(void);

void zpoc_set_tx_gain(signed char cGainLvl);
void zpoc_play_vol_level(unsigned char ucVol);
void back_to_desktop(unsigned int arg1);

void ptt1_press_handle(void);
void ptt1_release_handle(void);
void ptt2_press_handle(void);
void ptt2_release_handle(void);
void volume_up(void);
void volume_down(void);

#endif
