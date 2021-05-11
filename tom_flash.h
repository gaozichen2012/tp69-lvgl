#ifndef __TOM_FLASH_H__
#define __TOM_FLASH_H__

#define POC_SETING_VALID_FLG 0xa0a0a0a0
#define ANA_SETING_VALID_FLG 0xb0b0b0b0
#define MCU_CFG_VALID_VALUE 0x7A7A

#define POC_CFG_FILE_PATH "U:/cfg.dat"    //公网账号密码等参数
#define ANA_CFG_FILE_PATH "U:/analog.dat" //模拟信道信息相关参数

#define MCU_CFG_NET_PATH "U:/mcucfg.dat"    //公网设置等参数
#define MCU_CFG_ANG_PATH "U:/analogcfg.dat" //模拟设置等参数

#define MCU_INV_RECD_PATH "U:/invrec.dat"

#define POC_SETING_IP_MAX_LEN 16
#define POC_SETING_ACC_MAX_LEN 20
#define POC_SETING_PASS_MAX_LEN 20
#define POC_SETING_DOMAIN_MAX_LEN 50

#define POC_SETING_APN_INFOR_LEN 50
#define POC_SETING_AGENT_PWD_LEN 20

typedef struct
{
    char ip[POC_SETING_IP_MAX_LEN];
    char account[POC_SETING_ACC_MAX_LEN];
    char passwd[POC_SETING_PASS_MAX_LEN];

    char apnname[POC_SETING_APN_INFOR_LEN];
    char apnacc[POC_SETING_APN_INFOR_LEN];
    char apnpwd[POC_SETING_APN_INFOR_LEN];

    char agentpwd[POC_SETING_AGENT_PWD_LEN];

    char inviteFlg;
    char offlineFlg;
    char gps_sw;

    unsigned int uiApnValidFlg;
    unsigned int uiServerCfg;
    unsigned int ucValidFlg;
} stPocSetPar;

typedef struct
{
    unsigned char ucWorkMode;
    unsigned char ucPttTone;
    unsigned char ucOnOffTone;
    unsigned char ucBLTime;
    unsigned char ucKBLock;
    unsigned char ucNetMode;
    unsigned char ucPortMode;
    unsigned char ucVolLvl;
    unsigned char ucRecordSw;
    signed char cTimeZone;
    unsigned char ucLanguage;
    unsigned char ucSendTime;
    unsigned char ucRxBL;

    signed char cTxGain;
    unsigned char ucRes2;
    unsigned char ucRes3;
} stBaseSeting;

typedef struct
{
    unsigned char freqscan_flag;      //0正常 1加密 表示信道1为扫描所得，即加密
    unsigned int freqscan_cdcss_freq; //扫频获取的cdc是寄存器值，不同于默认CDC值

    unsigned char sq_level;         //GiSqlLev
    unsigned char step_freq;        //步进频率等级0-9
    unsigned char tail_audio_type;  //尾音消除方式 0:OFF/1:120/2:180 GiTailEndKind
    unsigned char tot_time;         //发射限时 0:不限 1:50S 2:100s 3:180S
    unsigned char save_power_level; //0:11 1:12 2:14 3:18
    unsigned char vox_level;        //VOX检测等级 0关闭/Lv1-9
} analog_flash_t;

typedef struct
{
    unsigned short addr;
    unsigned char data_len;
    unsigned char data[32];
} ana_one_pocket_t;

typedef struct
{
    unsigned char pocket[300 * 32];
} ana_all_pocket_t;

typedef struct
{
    stBaseSeting iSeting;
    unsigned short usCrc;
} stNSetingSave;

typedef struct
{
    analog_flash_t toa_flash;
    unsigned short usCrc;
} stASetingSave;

#ifdef TOM_FLASH
#define TOM_FLASH_EXTERN
#else
#define TOM_FLASH_EXTERN extern
#endif

TOM_FLASH_EXTERN stPocSetPar iPocSet;
TOM_FLASH_EXTERN ana_all_pocket_t toa_flash2;
TOM_FLASH_EXTERN stNSetingSave net_seting_flash;
TOM_FLASH_EXTERN stASetingSave ang_seting_flash;

TOM_FLASH_EXTERN void write_flash_net_seting_default(void);
TOM_FLASH_EXTERN void write_flash_ang_seting_default(void);

#endif