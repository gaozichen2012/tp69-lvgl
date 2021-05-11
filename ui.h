#ifndef __UI__H__
#define __UI__H__

#include "charging.h"
#include "language.h"
#include "Zpoc.h"
#include "font.h"
typedef struct tagUiPar
{
	unsigned short usTxBkColor;
	unsigned short usTxFkColor;
	unsigned short usStBkColor;
	unsigned short usStFkColor;
} stUiPar;

typedef enum
{
	UI_FONT_BIG_EDS,
	UI_FONT_SMALL_EDS,
} euFontEds;

typedef enum
{
	display_left,
	display_middle,
	display_right,
	display_other, //手
} DISPLAY_TYPE;

extern stUiPar iUiPar;
extern const unsigned char bmpSpk[];
extern const unsigned char bmpNetMode[][12];
extern const unsigned char bmpGpsSta[];
extern const unsigned char bmpNum[][36];
extern const unsigned char bmpMute[];
extern const unsigned char bmpBattery[];
extern const unsigned char bmpBatAdd[];

extern const char cTxGainTx[7][24];

extern const char ucWeek[][5];
extern const char cDevIniTx[]; //初始化检测
extern const char cDebugTx[];
extern const char cSetMenuTx1[];

extern const char cSetMenuTx[];

extern const char cInviteMenuTx[]; //临时呼叫
extern const char cInviteExitTx[]; //挂断
extern const char cRecPlayTx[];
extern const char cInvRecvTx[];
extern const char cRecordTx[];
extern const char cMenuMainTTS[];
extern const char cMemberNoOneTTS[];
extern const char cMemberInvOKTTS[];
extern const char cMemberInvFaildTTS[];
extern const char cDesktopNotOnlineTTS[];
extern const char cDesktopLKLockTTS[];	 //键盘已锁
extern const char cDesktopLKUnLockTTS[]; //解锁成功
extern const char cInviteOffTTS[];
extern const char cFuncNotFinishTTS[];
extern const char cInviteExitTTS[];
extern const char cInviteCallMeTTS[];

extern const char cUSBONTTS[];	//USB已打开
extern const char cUSBOFFTTS[]; //USB已关闭
extern const char cSetFaildTTS[];

extern const char cRecClrOKTTS[];	 //清除成功
extern const char cRecClrFAILDTTS[]; //清除失败
extern const char cDesktopOffTx[];
extern const char cDesktopOnTx[];
extern const char cDesktopLowPwrTx[];

extern const char cGpsLngTx[];
extern const char cGpsLatTx[];
extern const char cGpsStarTx[];

extern const char cRecUserTx[]; //讲话成员:
extern const char cRecTimeTx[]; //讲话时间:
extern const char cMenuTTS[5][24];

extern const char cDsktopCruGroupTTS[]; //当前群组
extern const char cDsktopCruUserTTS[];	//当前用户
extern const char cDsktopBatLowTTS[];	//点量低

extern const char cDsktopBatLvlTTS[11][30];

extern const char cOtaPocStateTx[][40];

extern const char cOtaOwcStateTx[][40];

extern const char cOtaPocUpdateTx[]; //POC软件升级
extern const char cOtaMcuUpdateTx[]; //MCU软件升级
extern const char cOtaOwcUpdateTx[]; //空中写码

extern const char cOtaWarningTx[]; //警告:请勿断电!
extern const char cOtaRestartTx[]; //重启机器以生效!
extern const char cOtaReturnTx[];  //无需升级,请返回!
extern const char cOtaErrorTx[];   //升级失败,请返回!
extern const char cOtaRebootTx[];

extern const char cOtaAccountTx[]; //账号:
extern const char cOtaOwcStaTx[];  //状态:

extern const char ucBusyTips[];
extern const char ucLanguageTx[2][32];

#ifdef LANGUAGE_SETTING_TU
extern const unsigned char ucMenuItem[9][24];
#endif

#ifdef LANGUAGE_SETTING_EN
extern const unsigned char ucMenuItem[9][24];
#endif

#ifdef LANGUAGE_SETTING_CH
extern const unsigned char ucMenuItem[7][24];
#endif

extern const unsigned char ucTimeZoneItem[25][20];
extern const char cSetSubMenu[37][45];
extern const char cFriendNoOneTx[]; //无在线好友
extern const char cSosStopTx[];
extern const char cMemberDlLstTx[]; //列表下载中...
extern const char cMenberNoOneTx[];
extern const char cFriendNoOneTTS[];
extern const char cFriendSelTTS[];

extern const unsigned char gImage_group[];
extern const unsigned char gImage_friend[];
extern const unsigned char gImage_gps[];
extern const unsigned char gImage_member[];
extern const unsigned char gImage_log[];
extern const unsigned char gImage_setup[];
extern const unsigned char gImage_menu[];
extern const unsigned char gImage_record[];

extern const unsigned char gImage_group_sel[];
extern const unsigned char gImage_member_sel[];
extern const unsigned char gImage_friend_sel[];
extern const unsigned char gImage_setup_sel[];
extern const unsigned char gImage_gps_sel[];
extern const unsigned char gImage_record_sel[];
extern const unsigned char gImage_shadow[];

extern const unsigned char gImage_sb_group[];
extern const unsigned char gImage_sb_member[];
extern const unsigned char gImage_user_20x20[];
extern const unsigned char gImage_group_20x20[];
extern const unsigned char gImage_freq_20x20[];
extern const unsigned char gImage_ch_20x20[];
extern const unsigned char gImage_sos[];
extern const unsigned char gImage_select[];

extern const char cVolLvlTTS[8][17];

void ui_init(void);
void ui_disp_icon_signal(unsigned char ucCsq);

void ui_disp_icon_bat(unsigned short usVal, E_BAT_CHARG_TYPE eCha);

void ui_disp_icon_net_status(unsigned char ucNetMode);
void ui_disp_icon_gps(unsigned char ucEnable, unsigned char ucState);
void ui_disp_icon_time(stTime *pTime);
void ui_disp_date(stTime *pTime);
void ui_disp_icon_mute(unsigned char ucMute);
void ui_disp_icon_vol(unsigned char ucLvl);

int ui_get_font_disp_pixel(char *pUnicodeStr, enFontType font, euFontEds eds);

void ui_disp_srceen_time(stTime *pTime);
unsigned char ui_ascii_to_char(unsigned char nib_h, unsigned char nib_l);

void ui_disp_font_11x12(DISPLAY_TYPE display_type, unsigned short x, unsigned short y, const char *pStr, unsigned short usBkColor,
						unsigned short usColor, euFontEds iEds, euCodeType iCodeType);
void ui_disp_font_15x16(unsigned short x, unsigned short y, const char *pUnicodeStr, unsigned short usBkColor, unsigned short usColor);

void ui_disp_ascii_str_11x12(unsigned short x, unsigned short y, char *pAscii, unsigned short usBkColor, unsigned short usColor);
void ui_dips_clean_a_line(unsigned short x, unsigned short y, unsigned short usColor);

#ifdef LANGUAGE_SETTING_EN
#define CALC_BG_X_SIZE(a) (strlen(a) * 6 + 4)
#endif

#ifdef LANGUAGE_SETTING_TU
#define CALC_BG_X_SIZE(a) (strlen(a) * 6 + 8)
#endif

#endif
