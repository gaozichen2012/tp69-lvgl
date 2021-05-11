#ifndef __ANALOG_MODEL_H__
#define __ANALOG_MODEL_H__
//like stSetingSave
//stAnalogFlash
//normal_item_disp_with_channel_list
//stAnalogState
#define FREQ_BUF_MAX_SIZE 8

#define Channel0ParaAddr (0)       //通道0参数地址------通道199参数地址
#define ConMenEnBegAddr (0X1180)   //读当前有用信道地址---bit0-bit7 ch000-ch008
#define ConSavTestEndAddr (0X3FFF) //Flash/EEPROM结束地址

/*测试模式*/
// #define ConTetBegAdd1 (EEPROM2FLASH_ADDR_MAP + 0X16B0)      //读当前测试模式首地址
// #define ConTetBegAdd2 (EEPROM2FLASH_ADDR_MAP + 0X1730)      //读当前测试模式首地址
#define ConTetBegAdd3 (0X17D0)   //读当前测试模式首地址
#define ConAdjTetBegAdd (0X1900) //设定当前测试模式手动调节首地址
// #define Con1846BegAdd (EEPROM2FLASH_ADDR_MAP + 0X1920)      //设定1846声音调测参数地址
// #define Con138BegAdd (EEPROM2FLASH_ADDR_MAP + 0X1930)       //设定138测参数地址
// #define ConRadioOpenFirAdd (EEPROM2FLASH_ADDR_MAP + 0X1940) //设定首行参数地址
// #define ConRadioOpenSecAdd (EEPROM2FLASH_ADDR_MAP + 0X1950) //设定次行参数地址
// #define ChInfNameAddr (EEPROM2FLASH_ADDR_MAP + 0X1960)      //存储信道名称首地址
// #define ConFreCodeBegAddr (EEPROM2FLASH_ADDR_MAP + 0X2160)  //写频密码
// #define ConGpsBegAdd (EEPROM2FLASH_ADDR_MAP + 0X2180)       //GPS联系人地址
// #define ConGpsSetBegAdd (EEPROM2FLASH_ADDR_MAP + 0X2280)    //GPS设置地址
// #define ConStopSaveAdd (EEPROM2FLASH_ADDR_MAP + 0X16B0)     //设定停止存储上线
// #define OpenRadioNameAddr (EEPROM2FLASH_ADDR_MAP + 0X1940)  //设定信道名称补充上线
// #define ConMaxReadAdrr (EEPROM2FLASH_ADDR_MAP + 0X2290)     //写频最大上限
// #define ConSavTestAddLow (EEPROM2FLASH_ADDR_MAP + 0X16B0)   //设定测试模式存储区域下限

typedef enum
{
    SUBAUDIO_NONE,
    SUBAUDIO_CTCSS,
    SUBAUDIO_CDCSS,
} SUBAUDIO_TYPE;

typedef struct
{
    unsigned char sub_audio_mode;  //0关闭 1ctcss 2cdcss
    unsigned char cdcss_direction; //数字亚音方向 0positive 1negative
    unsigned int ctcss_freq;
    unsigned int cdcss_freq; //为兼容一键对频的寄存器值，由short改为int
} one_sub_audio;

typedef struct
{
    unsigned int rx_freq;
    unsigned int tx_freq;
    unsigned char step_freq;
    unsigned char channel_spacing; //带宽 0宽带 1中带 2窄带
    unsigned char tx_power;        //0高 1中 2低

    one_sub_audio tx_sub_audio; //发射亚音
    one_sub_audio rx_sub_audio;
} one_channel;

typedef struct
{

    unsigned char channel_index_buf[3 + 1];
    unsigned char tx_freq_buf[FREQ_BUF_MAX_SIZE + 1 + 1];
    unsigned char rx_freq_buf[FREQ_BUF_MAX_SIZE + 1 + 1];
    unsigned char disp_freq[FREQ_BUF_MAX_SIZE + 1 + 1];

} analog_info_for_disp;

typedef struct
{
    //菜单设置只对VFO生效
    unsigned char cur_mode; //0MR 1VFO

    one_channel vfo_channel;

    unsigned char mr_channel_index; //当前信道
    one_channel mr_channel;

    one_channel freqscan_channel; //扫描获取的信道

    one_channel cur_channel; //mr和vfo根据当前模式赋值给cur_channel再转化为disp_channel
    analog_info_for_disp disp_channel;
} analog_all_info;

typedef enum
{
    RX_IDLE,  //idle
    RX_REV,   //收到sq，亚音匹配
    RX_ONLYSQ //收到sq，亚音未匹配
} RXFLAG_TYPE;

typedef struct
{
    unsigned char tx_flag;
    RXFLAG_TYPE rx_flag;
    unsigned char sleep_flag; //0:接收模式 1:进入省电模式（空闲状态10s后进入省电模式）

    unsigned char rx_subaudio_match;

    unsigned char tail_state; //1收到120°相位变化尾音，2表示收到180°相位变化尾音，3表示收到 240°相位变化尾音。
    unsigned char sq_state;
    unsigned short rssi_level;
    unsigned char ctc_state;     //1表示收到CTC1（主CTC），2表示收到CTC2（如55Hz尾音）；
    unsigned char dcs_state;     //1表示收到CDC正码，2表示收到CDC反码；
    unsigned char bk_rxon_state; //0idle 1接收时或空闲检测

    unsigned char freq_scan_flg; //0:default 1:freq match
    unsigned char fsk_flg;       //0:close fsk 1:open fsk 2:rx fsk 3:tx fsk
} analog_t;

#ifdef ANALOG_M
#define ANALOG_M_EXTERN
#else
#define ANALOG_M_EXTERN extern
#endif

ANALOG_M_EXTERN unsigned char channel_max_num;
ANALOG_M_EXTERN one_channel channel_info[200];
ANALOG_M_EXTERN char rx_channel_buf[200][FREQ_BUF_MAX_SIZE + 1 + 1];

ANALOG_M_EXTERN analog_t toa; //talk over analog
ANALOG_M_EXTERN analog_all_info ana_all;

ANALOG_M_EXTERN void am_init(void);
ANALOG_M_EXTERN void channel_index_2_disp_str(unsigned char res, unsigned char *dest);
ANALOG_M_EXTERN void freq_value_2_disp_str(unsigned int res, char *dest);
ANALOG_M_EXTERN void write_default_channel(void);
ANALOG_M_EXTERN void read_analog_channel_info(void);

#endif
