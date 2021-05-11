#ifndef __ANALOG_VIEW_H__
#define __ANALOG_VIEW_H__

#include "analog_model.h"

#define ANALOG_ICO_SPACE 5

#define ANALOG_ICO_RSSI_X_SIZE 26
#define ANALOG_ICO_RSSI_X_START (0 + ANALOG_ICO_SPACE)

#define ANALOG_ICO_POWER_X_SIZE 6
#define ANALOG_ICO_POWER_X_START (ANALOG_ICO_RSSI_X_START + ANALOG_ICO_RSSI_X_SIZE + ANALOG_ICO_SPACE)

#define ANALOG_ICO_CTC_X_SIZE (6 * 3)
#define ANALOG_ICO_CTC_X_START (ANALOG_ICO_POWER_X_START + ANALOG_ICO_POWER_X_SIZE + ANALOG_ICO_SPACE)

#define ANALOG_ICO_WIDE_X_SIZE 6
#define ANALOG_ICO_WIDE_X_START (ANALOG_ICO_CTC_X_START + ANALOG_ICO_CTC_X_SIZE + ANALOG_ICO_SPACE)

#define ANALOG_ICO_VFO_X_SIZE (6 * 3)
#define ANALOG_ICO_VFO_X_START (ANALOG_ICO_WIDE_X_START + ANALOG_ICO_WIDE_X_SIZE + ANALOG_ICO_SPACE)

unsigned char analog_ico_height_start_point(void);
void ui_disp_icon_analog_signal(unsigned char level);
void analog_signal_disp(unsigned char level, unsigned char refresh_type);

void ui_disp_icon_power(unsigned char val);
void ui_disp_icon_ctcss_or_dcs(unsigned char txrx_mode, SUBAUDIO_TYPE sub_audio_mode);
void ui_disp_icon_wide_or_narrow(unsigned char val);
void ui_disp_icon_vfo_or_mr(unsigned char val);

#endif