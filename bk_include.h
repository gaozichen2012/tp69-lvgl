
#if 1
#include "ql_type.h"
#else
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
#endif

#define RF_I2CID 0x5C

#define SPACE_12K5 1
#define SPACE_25K 2
#define SPACE_6K25 3
#define SPACE_20K 4

#define MUTE 0
#define OPEN 1
#define ALAM 2
#define BEEP 3
#define CTCO 6
#define FSKO 8

#define ADD1 1
#define ADD2 2
#define ADD3 3
#define ADD4 4
#define SUB1 -1 & 0xf
#define SUB2 -2 & 0xf
#define SUB3 -3 & 0xf

#define XTAL26M 0
#define XTAL13M 1
#define XTAL19M2 2
#define XTAL12M8 3
#define XTAL25M6 4
#define XTAL38M4 5

#define CTC_ERR 0  //0=0.1Hz error mode;1=0.1% error mode
#define CTC_IN 10  //~=1   Hz for ctcss in  threshold
#define CTC_OUT 15 //~=1.5 Hz for ctcss out threshold

#define CTC134 0
#define CTC120 1
#define CTC180 2
#define CTC240 3
#define CTC55 4
#define CTC259 5
#define CTC45 6

#define CTC_GAIN 0x45
#define DCS_GAIN 0x3D

#define TONE_GAIN 0xC0 //0x00=Disable;0x80~0xFF=Enable

#define DEVIATION 0x4D0 //0~0xFFF, 0x5D0 for 13M/12.8M, 0x53A for 19.2M 0x3D0 for 38.4M

#define MIC_GAIN 16 //0~31

#define PACTL 0      //0~255
#define PACTL_EN 0   //1:Enable;0:Disable
#define PA_GAIN 0x3F //0~0x3F

#define TH_RSSI_SQ_IN 0x47    //0~0xFF, LSB->0.5dB
#define TH_RSSI_SQ_OUT 0x46   //0~0xFF, LSB->0.5dB
#define TH_NOISE_SQ_IN 0x2E   //0~0x7F
#define TH_NOISE_SQ_OUT 0x2F  //0~0x7F
#define TH_GLITCH_SQ_IN 0x04  //0~0xFF
#define TH_GLITCH_SQ_OUT 0x16 //0~0xFF

#define VOX_OUT_DELAY 5  //0~15
#define VOX_DET_INTVL 12 //0~31
#define TH_VOX_IN 0xA    //0~0x7FF
#define TH_VOX_OUT 0x5   //0~0x7FF

#define DAC_GAIN 15 //0~15
#define VOL_GAIN 59 //0~63, 0=mute, LSB->0.5dB

#define RXON_WITH_VCO 1

#define REG_32 0x0244
#define REG_37 0x1D00
#define REG_40 0x3000
#define REG_46 0xA000
#define REG_47 0x6040
#define REG_48 0xB000
#define REG_4D 0xA000
#define REG_4E 0x6F00
#define REG_50 0x3B20
#define REG_52 CTC_ERR << 12 | CTC_IN << 6 | CTC_OUT
#define REG_59 0x0028
#define REG_7A 0x089A
#define REG_7D 0xE940
#define REG_7E 0x302E

//#define FSKBUAD 0x3065 //1200*10.32444; 10.32444 for 13M/26M and 10.48576 for 12.8M/19.2M/25.6M/38.4M
#define FSKBUAD 0x1AD518F2 //43602672*10.32444;//450173170

#define FSK_LEN 16 //0~127, numbers_of_byte=(FSK_LEN*2)
#define FSK2400 0  //0=1200;1=2400

extern u16 FSK_TXDATA[FSK_LEN];
extern u16 FSK_RXDATA[FSK_LEN];

#define MDC_LEN 7 //0~127, numbers_of_byte=(MDC_LEN*2)
#define MDC2400 0 //1=1200/2400; 0=1200/1800

extern u8 MDC_SYNC[5];
extern u16 MDC_TXDATA[MDC_LEN];
extern u16 MDC_RXDATA[MDC_LEN];

#define DTMF_TH 24 //0~63
#define DTMF_LEN 4 //4 means total 5 symbols

#define FREQ_697 0x1C1C //697*10.32444; 10.32444 for 13M/26M and 10.48576 for 12.8M/19.2M/25.6M/38.4M
#define FREQ_770 0x1F0E
#define FREQ_852 0x225C
#define FREQ_941 0x25F3
#define FREQ_1209 0x30C2
#define FREQ_1336 0x35E1
#define FREQ_1477 0x3B91
#define FREQ_1633 0x41DC

extern u8 DTMF_SYMBOL[DTMF_LEN]; //0~F; E='*', F='#'
extern u8 DTMF_RXDATA[DTMF_LEN];

#define TONE_TH 24 //0~63
#define TONE_LEN 4 //4 means total 5 symbols

extern u8 TONE_SYMBOL[TONE_LEN]; //0~E
extern u8 TONE_RXDATA[TONE_LEN];

extern u16 FRQ_HI16;
extern u16 FRQ_LO16;
extern u16 CTC_FREQ;
extern u16 DCS_HI12;
extern u16 DCS_LO12;

#define NOAA_MAX 32 //may be

extern u8 NOAA_RXDATA[NOAA_MAX];

void DelayMs(u16 t);
void RF_Initial();
void RF_SetAgc(u8 mode);
void RF_WakeUp();
void RF_Sleep();
void RF_SetChnSpace(u8 space);
void RF_SetFreq(u16 freq_hi16, u16 freq_lo16);
void RF_SetCtcss(u16 ctc1);
void RF_SetCtc2(u16 ctc2);
void RF_SetCdcss(u16 hi12, u16 lo12);
void RF_ExitSubau();
void RF_SetAf(u8 mode);
void RF_EnterTxMute();
void RF_ExitTxMute();
void RF_EnterScramble();
void RF_ExitScramble();
void RF_EnterCompander();
void RF_ExitCompander();
void RF_EnterVox();
void RF_ExitVox();
void RF_EnterBypass();
void RF_ExitBypass();
void RF_GenTail(u8 tail);
void RF_Key();
void RF_Call();
void RF_Txon_Beep();
void RF_Txon();
void RF_Rxon();
void RF_Rxon_Disc();
void RF_Idle();
void RF_WakeUp();
void RF_Bypass();
void RF_SetGpioOut(u8 num, u8 type, u8 val);
void RF_Write(u8 addr, u16 data);
void RF_SetAfResponse(u8 tx, u8 f3k, u8 db);
void RF_SetXtal(u8 mode);
void RF_EnterDtmf();
void RF_ExitDtmf();
void RF_DtmfTransmit();
u8 RF_DtmfReceive();
void RF_Enter5tone();
void RF_Exit5tone();
void RF_5toneTransmit();
u8 RF_5toneReceive();
void RF_FskIdle();
void RF_EnterFsk();
void RF_ExitFsk();
u8 RF_FskTransmit();
u8 RF_FskReceive();
void RF_EnterMdc();
void RF_ExitMdc();
u8 RF_MdcTransmit();
u8 RF_MdcReceive();
void RF_EnterNoaa();
void RF_ExitNoaa();
u8 RF_NoaaReceive();
u8 RF_FreqScan();
u8 RF_CtcDcsScan();

u16 RF_GetVoxAmp();
u16 RF_GetRssi();
u16 RF_Read(u8 addr);
u8 RF_GetSq();
u8 RF_GetCtcss();
u8 RF_GetTail();
u8 RF_GetCdcss();
u8 RF_GetVox();
u8 RF_GetNoise();
u8 RF_GetGlitch();
u8 RF_GetGpioIn(u8 num);
