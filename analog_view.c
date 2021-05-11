#include "analog_view.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ql_rtos.h"
#include "ql_power.h"
#include "ql_fs.h"

#include "general_include.h"
#include "lcd.h"
#include "font.h"
#include "ui.h"
#include "charging.h"

#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

//模拟ico Y起始地址
unsigned char analog_ico_height_start_point(void)
{
    if (iBaseSeting.ucWorkMode == 1)
    {
        return 3;
    }
    else
    {
        return 59;
    }
}

#if 1
void ui_disp_icon_analog_signal(unsigned char level)
{
    lcd_disp_tan(2, analog_ico_height_start_point() - 3 + 3, 12, analog_ico_height_start_point() - 3 + 3, 7, analog_ico_height_start_point() - 3 + 9, 1, COLOR_ANA_ICO_FONT);
    lcd_disp_LineH(6, analog_ico_height_start_point() - 3 + 8, 6, 2, COLOR_ANA_ICO_FONT);

    lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 0, analog_ico_height_start_point() + 2 * 3, 5 + 2 * 0, 2, COLOR_ANA_ICO_BACKGROUND);
    lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 1, analog_ico_height_start_point() + 2 * 2, 5 + 2 * 1, 2, COLOR_ANA_ICO_BACKGROUND);
    lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 2, analog_ico_height_start_point() + 2 * 1, 5 + 2 * 2, 2, COLOR_ANA_ICO_BACKGROUND);
    lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 3, analog_ico_height_start_point() + 2 * 0, 5 + 2 * 3, 2, COLOR_ANA_ICO_BACKGROUND);

    if (level > 0)
        lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 0, analog_ico_height_start_point() + 2 * 3, 5 + 2 * 0, 2, COLOR_ANA_ICO_FONT);
    if (level > 1)
        lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 1, analog_ico_height_start_point() + 2 * 2, 5 + 2 * 1, 2, COLOR_ANA_ICO_FONT);
    if (level > 2)
        lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 2, analog_ico_height_start_point() + 2 * 1, 5 + 2 * 2, 2, COLOR_ANA_ICO_FONT);
    if (level > 3)
        lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 3, analog_ico_height_start_point() + 2 * 0, 5 + 2 * 3, 2, COLOR_ANA_ICO_FONT);
}
#else
void ui_disp_icon_analog_signal(unsigned char rssi)
{
    lcd_disp_tan(2, analog_ico_height_start_point() - 3 + 3, 12, analog_ico_height_start_point() - 3 + 3, 7, analog_ico_height_start_point() - 3 + 9, 1, COLOR_ANA_ICO_FONT);
    lcd_disp_LineH(6, analog_ico_height_start_point() - 3 + 8, 6, 2, COLOR_ANA_ICO_FONT);

    if (rssi > 80)
        lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 0, analog_ico_height_start_point() + 2 * 3, 5 + 2 * 0, 2, COLOR_ANA_ICO_FONT);
    if (rssi > 90)
        lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 1, analog_ico_height_start_point() + 2 * 2, 5 + 2 * 1, 2, COLOR_ANA_ICO_FONT);
    if (rssi > 100)
        lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 2, analog_ico_height_start_point() + 2 * 1, 5 + 2 * 2, 2, COLOR_ANA_ICO_FONT);
    if (rssi > 120)
        lcd_disp_LineH(ANALOG_ICO_RSSI_X_START + 4 + 4 * 3, analog_ico_height_start_point() + 2 * 0, 5 + 2 * 3, 2, COLOR_ANA_ICO_FONT);
}
#endif

//refresh_type 0:change 1:force
void analog_signal_disp(unsigned char level, unsigned char refresh_type)
{
    static unsigned char level_old = 0xff;

    if (refresh_type == 1)
    {
        ui_disp_icon_analog_signal(level);
    }
    else
    {
        if (level_old != level)
        {
            level_old = level;
            ui_disp_icon_analog_signal(level);
            _DEBUG("ICO REFREASH  toa.rssi_level=%d!\r\n", level);
            lcd_update_disp();
        }
    }
}

void ui_disp_icon_power(unsigned char val)
{
    if (val == 0)
    {
        ui_disp_ascii_str_11x12(ANALOG_ICO_POWER_X_START, analog_ico_height_start_point(), "H", COLOR_ANA_ICO_BACKGROUND, COLOR_ANA_ICO_FONT);
    }
    else if (val == 1)
    {
        ui_disp_ascii_str_11x12(ANALOG_ICO_POWER_X_START, analog_ico_height_start_point(), "M", COLOR_ANA_ICO_BACKGROUND, COLOR_ANA_ICO_FONT);
    }
    else
    {
        ui_disp_ascii_str_11x12(ANALOG_ICO_POWER_X_START, analog_ico_height_start_point(), "L", COLOR_ANA_ICO_BACKGROUND, COLOR_ANA_ICO_FONT);
    }
}

//0:rx 1:tx
void ui_disp_icon_ctcss_or_dcs(unsigned char txrx_mode, SUBAUDIO_TYPE sub_audio_mode)
{
    switch (sub_audio_mode)
    {
    case SUBAUDIO_NONE: //off
        ui_disp_ascii_str_11x12(ANALOG_ICO_CTC_X_START, analog_ico_height_start_point(), "   ", COLOR_ANA_ICO_BACKGROUND, COLOR_ANA_ICO_BACKGROUND);
        break;
    case SUBAUDIO_CTCSS:    //ctcss
        if (txrx_mode == 0) //rx
            ui_disp_ascii_str_11x12(ANALOG_ICO_CTC_X_START, analog_ico_height_start_point(), "CTC", COLOR_ANA_ICO_BACKGROUND, COLOR_BLUE);
        else //tx
            ui_disp_ascii_str_11x12(ANALOG_ICO_CTC_X_START, analog_ico_height_start_point(), "CTC", COLOR_ANA_ICO_BACKGROUND, COLOR_RED);
        break;
    case SUBAUDIO_CDCSS:    //cdcss
        if (txrx_mode == 0) //rx
            ui_disp_ascii_str_11x12(ANALOG_ICO_CTC_X_START, analog_ico_height_start_point(), "DCS", COLOR_ANA_ICO_BACKGROUND, COLOR_BLUE);
        else //tx
            ui_disp_ascii_str_11x12(ANALOG_ICO_CTC_X_START, analog_ico_height_start_point(), "DCS", COLOR_ANA_ICO_BACKGROUND, COLOR_RED);
        break;
    default:
        break;
    }
}

void ui_disp_icon_wide_or_narrow(unsigned char val)
{
    if (val == 0)
    {
        ui_disp_ascii_str_11x12(ANALOG_ICO_WIDE_X_START, analog_ico_height_start_point(), "W", COLOR_ANA_ICO_BACKGROUND, COLOR_ANA_ICO_FONT);
    }
    else
    {
        ui_disp_ascii_str_11x12(ANALOG_ICO_WIDE_X_START, analog_ico_height_start_point(), "N ", COLOR_ANA_ICO_BACKGROUND, COLOR_ANA_ICO_FONT);
    }
}

void ui_disp_icon_vfo_or_mr(unsigned char val)
{
    if (val == 0)
    {
        ui_disp_ascii_str_11x12(ANALOG_ICO_VFO_X_START, analog_ico_height_start_point(), "MR ", COLOR_ANA_ICO_BACKGROUND, COLOR_ANA_ICO_FONT);
    }
    else
    {
        ui_disp_ascii_str_11x12(ANALOG_ICO_VFO_X_START, analog_ico_height_start_point(), "VFO", COLOR_ANA_ICO_BACKGROUND, COLOR_ANA_ICO_FONT);
    }
}