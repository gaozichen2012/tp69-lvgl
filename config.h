#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "general_include.h"

int cfg_net_read_mcu_seting(stBaseSeting *pBaseSeting);
int cfg_net_write_mcu_seting(stBaseSeting *pBaseSeting);
int cfg_net_reset_mcu_seting(stBaseSeting *pBaseSeting);

int cfg_ang_read_mcu_seting(analog_flash_t *ptoa_flash);
int cfg_ang_write_mcu_seting(analog_flash_t *ptoa_flash);
int cfg_ang_reset_mcu_seting(analog_flash_t *ptoa_flash);

unsigned char cfg_inv_record_load(void);
struct stInvRecNode *cfg_inv_record_get(unsigned short usIdx);
unsigned char cfg_inv_record_add(unsigned char *pUid, unsigned char *pName, unsigned char ucSta, unsigned char *pTime);

#endif
