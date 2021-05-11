#ifndef __ANALOG_TEST_H__
#define __ANALOG_TEST_H__

#include "analog_old_patch.h"

#ifdef ANALOG_T
#define ANALOG_T_EXTERN
#else
#define ANALOG_T_EXTERN extern
#endif

ANALOG_T_EXTERN void TestModCommandPro(void);

ANALOG_T_EXTERN void EntTestItemPro(void);

#endif