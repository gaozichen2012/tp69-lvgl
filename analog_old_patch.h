#ifndef __ANALOG_OLD_PATCH_H__
#define __ANALOG_OLD_PATCH_H__
#include "analog_model.h"

typedef unsigned char INT8U;
typedef unsigned short INT16U;
typedef unsigned int INT32U;

#ifdef ANALOG_OP
#define ANALOG_OP_EXTERN
#else
#define ANALOG_OP_EXTERN extern
#endif

ANALOG_OP_EXTERN void ang_rcode_handle(void);
ANALOG_OP_EXTERN void ang_wcode_handle(void);
ANALOG_OP_EXTERN void TestModCommandPro(void);

ANALOG_OP_EXTERN void SetCtcDscInfPro(one_sub_audio *Ptemp1, INT8U *Data); //读取信道亚音频信息
ANALOG_OP_EXTERN unsigned char CalNowChannelNumMax(void);
#endif