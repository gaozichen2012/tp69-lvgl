#define TOM_FLASH
#include "tom_flash.h"
#include "Zpoc.h"
void write_flash_net_seting_default(void)
{
    //net
    net_seting_flash.iSeting.ucBLTime = 2;
    net_seting_flash.iSeting.ucKBLock = 1;
    net_seting_flash.iSeting.ucNetMode = 1;
    net_seting_flash.iSeting.ucOnOffTone = 1;
    net_seting_flash.iSeting.ucWorkMode = 2;
    net_seting_flash.iSeting.ucPttTone = 1;
    net_seting_flash.iSeting.ucPortMode = 0;
    net_seting_flash.iSeting.ucRecordSw = 0;
    net_seting_flash.iSeting.cTimeZone = +8;
    net_seting_flash.iSeting.ucLanguage = 1;
    net_seting_flash.iSeting.ucSendTime = 0;
    net_seting_flash.iSeting.ucVolLvl = 7;
    net_seting_flash.iSeting.ucRxBL = 1;
    net_seting_flash.iSeting.cTxGain = MIC_TX_DEFAULT_GAIN;

    net_seting_flash.usCrc = MCU_CFG_VALID_VALUE;
}

void write_flash_ang_seting_default(void)
{
    ang_seting_flash.toa_flash.freqscan_flag = 0;
    ang_seting_flash.toa_flash.freqscan_cdcss_freq = 0;
    ang_seting_flash.toa_flash.sq_level = 0;
    ang_seting_flash.toa_flash.step_freq = 0;
    ang_seting_flash.toa_flash.tail_audio_type = 0;
    ang_seting_flash.toa_flash.tot_time = 0;
    ang_seting_flash.toa_flash.save_power_level = 0;
    ang_seting_flash.toa_flash.vox_level = 0;

    ang_seting_flash.usCrc = MCU_CFG_VALID_VALUE;
}