//写频
//PC TX Length: 8, Data: 02 53 31 39 47 52 41 4D

//读频
//PC TX 02 53 31 39 47 52 41 4D 	
//PC RX 06
//PC TX 02
//PC RX 53 4D 50 35 35 38 02 00
//PC TX 06
//PC RX 06
//PC TX 05
//PC RX FF FF FF FF FF FF
//PC TX 06
//PC RX 06
//PC TX 52 00 00 08
//PC RX 57 00 00 08 FF FF FF FF FF FF FF FF
//PC TX 52 00 08 08
//PC RX
//PC TX
//PC RX
//PC TX
//PC RX
//PC TX
//PC RX
#ifndef __WRITE_CODE_H__
#define __WRITE_CODE_H__

#define PWD_SUPER_USER "TYTYD998"

#define ConMaxbyte 32 //设定最大读写频数据
#define ConWribyte 78 //设定单次写频接收字节数=14(其它数据长度固定)+32(单次写频保存数)*2

typedef struct
{
    unsigned short rx_len;
    unsigned char rx_buf[256];
    unsigned char tx_buf[256];

    unsigned int GsIIcAddress;
    unsigned char write_code_mode; //0模拟 1公网
    unsigned char test_mode;       //0默认模式 1普通测试模式 2批量测试模式
    unsigned char ang_tx_flg;
    unsigned char wr_flag; //0default 1write 2read
} wc_data_t;

#ifdef WRITE_CODE
#define WRITE_CODE_EXTERN
#else
#define WRITE_CODE_EXTERN extern
#endif

WRITE_CODE_EXTERN wc_data_t wc_data;
WRITE_CODE_EXTERN unsigned char GbRdKilled; //机器被遥毙
WRITE_CODE_EXTERN unsigned char GbRdStuned; //机器被遥晕

WRITE_CODE_EXTERN void write_code_process(void);
WRITE_CODE_EXTERN void answer_wrcode_end(void);
WRITE_CODE_EXTERN unsigned char ascii2num(unsigned char Data);
WRITE_CODE_EXTERN unsigned char num2ascii(unsigned char Data);

#endif