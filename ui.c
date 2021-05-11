// #include "version.h"
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
#include "analog_view.h"

#define STATE_ICON_MARGIN_RIGHT 3

#define TIME_DIS_POS_OFF 22

#ifdef LANGUAGE_SETTING_CH
#define WEEK_DIS_POS_OFF 73
#endif

#ifdef LANGUAGE_SETTING_EN
#define WEEK_DIS_POS_OFF 70
#endif

#ifdef LANGUAGE_SETTING_TU
#define WEEK_DIS_POS_OFF 70
#endif

#define TIME_DIS_POS_Y 97 /*-5*/

#define UI_POC_NET_MODE_UNKNOW 0
#define UI_POC_NET_MODE_4G 1
#define UI_POC_NET_MODE_3G 2
#define UI_POC_NET_MODE_2G 3

#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

stUiPar iUiPar;

void ui_init(void)
{
}

unsigned char ui_ascii_to_char(unsigned char nib_h, unsigned char nib_l)
{
	nib_h = (nib_h <= '9') ? (nib_h - 0x30) : ((nib_h <= 'F') ? (nib_h - 0x37) : (nib_h - 0x57));
	nib_l = (nib_l <= '9') ? (nib_l - 0x30) : ((nib_l <= 'F') ? (nib_l - 0x37) : (nib_l - 0x57));

	return ((nib_h << 4) | nib_l);
}

void ui_disp_icon_signal(unsigned char ucCsq)
{
	if (ucCsq < 32)
	{
		lcd_disp_tan(2, 3, 12, 3, 7, 9, 1, COLOR_NET_ICO_FONT);
		lcd_disp_LineH(6, 8, 6, 2, COLOR_NET_ICO_FONT);

		lcd_disp_LineH(9, 9, 5, 2, COLOR_NET_ICO_BACKGROUND);
		lcd_disp_LineH(13, 7, 7, 2, COLOR_NET_ICO_BACKGROUND);
		lcd_disp_LineH(17, 5, 9, 2, COLOR_NET_ICO_BACKGROUND);
		lcd_disp_LineH(21, 3, 11, 2, COLOR_NET_ICO_BACKGROUND);

		if (ucCsq > 3)
			lcd_disp_LineH(9, 9, 5, 2, COLOR_NET_ICO_FONT);
		if (ucCsq > 12)
			lcd_disp_LineH(13, 7, 7, 2, COLOR_NET_ICO_FONT);
		if (ucCsq > 16)
			lcd_disp_LineH(17, 5, 9, 2, COLOR_NET_ICO_FONT);
		if (ucCsq > 24)
			lcd_disp_LineH(21, 3, 11, 2, COLOR_NET_ICO_FONT);
	}
}

void ui_disp_icon_bat(unsigned short usVal, E_BAT_CHARG_TYPE eCha)
{
	unsigned short usColor = COLOR_NET_ICO_FONT;
	unsigned short usCharCol;

	if (usVal <= 2)
		usColor = COLOR_RED;

	if (eCha == E_BAT_CHARGING)
		usCharCol = COLOR_RED;
	else if (eCha == E_BAT_CHAR_TRE)
		usCharCol = COLOR_GREEN;
	else
		usCharCol = usColor;

	lcd_disp_range((LCD_WIDTH - 20) - STATE_ICON_MARGIN_RIGHT, 2, 18, 12, 1, usCharCol);
	lcd_disp_box((LCD_WIDTH - 2) - STATE_ICON_MARGIN_RIGHT, 2, usCharCol, 1, 12);
	lcd_disp_box((LCD_WIDTH - 1) - STATE_ICON_MARGIN_RIGHT, 6, usCharCol, 1, 4);

	lcd_disp_box((LCD_WIDTH - 18) - STATE_ICON_MARGIN_RIGHT, 4, COLOR_NET_ICO_BACKGROUND, 15, 8);
	if (usVal >= 3)
		lcd_disp_box((LCD_WIDTH - 18) - STATE_ICON_MARGIN_RIGHT, 4, usColor, 2, 8);
	if (usVal >= 5)
		lcd_disp_box((LCD_WIDTH - 15) - STATE_ICON_MARGIN_RIGHT, 4, usColor, 2, 8);
	if (usVal >= 7)
		lcd_disp_box((LCD_WIDTH - 12) - STATE_ICON_MARGIN_RIGHT, 4, usColor, 2, 8);
	if (usVal >= 9)
		lcd_disp_box((LCD_WIDTH - 9) - STATE_ICON_MARGIN_RIGHT, 4, usColor, 2, 8);
	if (usVal >= 10)
		lcd_disp_box((LCD_WIDTH - 6) - STATE_ICON_MARGIN_RIGHT, 4, usColor, 2, 8);
}

void ui_disp_icon_net_status(unsigned char ucNetMode)
{
	switch (ucNetMode)
	{
	case 0:
		lcd_disp_bit1bmp_SingleByte(26, 4, (unsigned char *)(bmpNetMode[0]), 12, 8, COLOR_NET_ICO_FONT, COLOR_NET_ICO_BACKGROUND);
		break;

	case 1:
		ui_disp_ascii_str_11x12(26, 3, "2G", COLOR_NET_ICO_BACKGROUND, COLOR_NET_ICO_FONT);
		break;

	case 2:
		ui_disp_ascii_str_11x12(26, 3, "3G", COLOR_NET_ICO_BACKGROUND, COLOR_NET_ICO_FONT);
		break;

	case 3:
		ui_disp_ascii_str_11x12(26, 3, "4G", COLOR_NET_ICO_BACKGROUND, COLOR_NET_ICO_FONT);
		break;
	}
}

void ui_disp_icon_mute(unsigned char ucMute)
{
	char cVol[2];

	cVol[0] = 'x';
	cVol[1] = 0;

	if (ucMute)
	{
		lcd_disp_bit1bmp_SingleByte((LCD_WIDTH - 40), 2, (unsigned char *)bmpMute, 12, 12, COLOR_RED, COLOR_NET_BACKGROUND);
		lcd_disp_box((LCD_WIDTH - 35), 2, COLOR_NET_BACKGROUND, 8, 12);
		ui_disp_ascii_str_11x12((LCD_WIDTH - 33), 1, cVol, COLOR_NET_BACKGROUND, COLOR_RED);
	}
	else
		lcd_disp_box((LCD_WIDTH - 40), 2, COLOR_NET_ICO_BACKGROUND, 12, 12);
}

void ui_disp_icon_vol(unsigned char ucLvl)
{
	char cVol[2];

	cVol[0] = ucLvl + '1';
	cVol[1] = 0;

	lcd_disp_bit1bmp_SingleByte((LCD_WIDTH - 40), 2, (unsigned char *)bmpMute, 12, 12, COLOR_NET_ICO_FONT, COLOR_NET_ICO_BACKGROUND);
	lcd_disp_box((LCD_WIDTH - 34), 2, COLOR_NET_ICO_BACKGROUND, 8, 12);
	ui_disp_ascii_str_11x12((LCD_WIDTH - 32), 2, cVol, COLOR_NET_BACKGROUND, COLOR_NET_ICO_FONT);
}

void ui_disp_icon_gps(unsigned char ucEnable, unsigned char ucState)
{
	if ((ucEnable == 0x00) || (ucEnable == 0x30))
	{
		lcd_disp_bit1bmp_SingleByte(44, 3, (unsigned char *)bmpGpsSta, 12, 12, COLOR_NET_ICO_BACKGROUND, COLOR_NET_ICO_BACKGROUND);
	}
	else if (ucState == '1' || ucState == '2')
	{
		lcd_disp_bit1bmp_SingleByte(44, 3, (unsigned char *)bmpGpsSta, 12, 12, COLOR_NET_ICO_FONT, COLOR_NET_ICO_BACKGROUND);
	}
	else if (ucEnable == 0x31 || ucEnable == 0x01)
	{
		if (iRunPar.iGpsInfo.cLocSta != 0xff)
			lcd_disp_bit1bmp_SingleByte(44, 3, (unsigned char *)bmpGpsSta, 12, 12, COLOR_RED, COLOR_NET_ICO_BACKGROUND);
		else
			lcd_disp_bit1bmp_SingleByte(44, 3, (unsigned char *)bmpGpsSta, 12, 12, COLOR_NET_ICO_BACKGROUND, COLOR_NET_ICO_BACKGROUND);
	}
}

void ui_disp_icon_time(stTime *pTime)
{
	char cDate[11];
	sprintf(cDate, "%02d:%02d", pTime->ucHour, pTime->ucMint);
	ui_disp_ascii_str_11x12(52, 2, cDate, COLOR_NET_ICO_BACKGROUND, COLOR_NET_ICO_FONT);
}

void ui_disp_srceen_time(stTime *pTime)
{
	lcd_disp_bit1bmp_SingleByte(30 + 16, 34, (unsigned char *)bmpNum[pTime->ucHour / 10], 12, 24, COLOR_NET_ICO_FONT, COLOR_NET_BACKGROUND);
	lcd_disp_bit1bmp_SingleByte(44 + 16, 34, (unsigned char *)bmpNum[pTime->ucHour % 10], 12, 24, COLOR_NET_ICO_FONT, COLOR_NET_BACKGROUND);
	lcd_disp_bit1bmp_SingleByte(56 + 16, 34, (unsigned char *)bmpNum[10], 12, 24, COLOR_NET_ICO_FONT, COLOR_NET_BACKGROUND);
	lcd_disp_bit1bmp_SingleByte(70 + 16, 34, (unsigned char *)bmpNum[pTime->ucMint / 10], 12, 24, COLOR_NET_ICO_FONT, COLOR_NET_BACKGROUND);
	lcd_disp_bit1bmp_SingleByte(84 + 16, 34, (unsigned char *)bmpNum[pTime->ucMint % 10], 12, 24, COLOR_NET_ICO_FONT, COLOR_NET_BACKGROUND);
}

int ui_get_font_disp_pixel(char *pUnicodeStr, enFontType font, euFontEds eds)
{
	unsigned short usUnicode;
	int iPixel = 0;

	if (strlen((char *)pUnicodeStr) % 4 != 0)
	{
		_DEBUG("ui16 invalid length!!\r\n");
		return 0;
	}

	while (*pUnicodeStr)
	{
		if (eds == UI_FONT_BIG_EDS)
			usUnicode = ((unsigned short)(ui_ascii_to_char(*pUnicodeStr, *(pUnicodeStr + 1))) << 8) + ui_ascii_to_char(*(pUnicodeStr + 2), *(pUnicodeStr + 3));
		else
			usUnicode = ((unsigned short)(ui_ascii_to_char(*(pUnicodeStr + 2), *(pUnicodeStr + 3))) << 8) + ui_ascii_to_char(*(pUnicodeStr), *(pUnicodeStr + 1));

		if (usUnicode <= 0x007e)
		{
			if (font == PTT_FONT_TYPE_16)
				iPixel += 8;
			else
				iPixel += 6;
		}
		else
		{
			if (font == PTT_FONT_TYPE_16)
				iPixel += 16;
			else
				iPixel += 12;
		}

		pUnicodeStr += 4;
	}

	return iPixel;
}

void ui_disp_font_15x16(unsigned short x, unsigned short y, const char *pUnicodeStr, unsigned short usBkColor, unsigned short usColor)
{
	unsigned short usUnicode, i = 0;
	unsigned char ucMapDat[32];

	if (strlen((char *)pUnicodeStr) % 2 != 0)
	{
		_DEBUG("ui16 invalid length!!\r\n");
		return;
	}

	while (*pUnicodeStr)
	{
		usUnicode = ((unsigned short)(ui_ascii_to_char(*pUnicodeStr, *(pUnicodeStr + 1))) << 8) + ui_ascii_to_char(*(pUnicodeStr + 2), *(pUnicodeStr + 3));

		if (0 == font_get_data(usUnicode, PTT_FONT_TYPE_16, ucMapDat))
		{
			if (usUnicode <= 0x007e)
			{
				lcd_disp_bit1bmp_SingleByte(x + i * 8, y, ucMapDat, 8, 8, usColor, usBkColor);
				lcd_disp_bit1bmp_SingleByte(x + i * 8, y + 8, &ucMapDat[8], 8, 8, usColor, usBkColor);
			}
			else
			{
				lcd_disp_bit1bmp_SingleByte(x + i * 8, y, ucMapDat, 16, 8, usColor, usBkColor);
				lcd_disp_bit1bmp_SingleByte(x + i * 8, y + 8, &ucMapDat[16], 16, 8, usColor, usBkColor);
				i++;
			}
		}

		i++;
		pUnicodeStr += 4;
	}
}

void ui_disp_font_11x12(DISPLAY_TYPE display_type, unsigned short x, unsigned short y, const char *pStr, unsigned short usBkColor,
						unsigned short usColor, euFontEds iEds, euCodeType iCodeType)
{
	unsigned short single_data_size = 3;
	unsigned short data_len = 0;

	unsigned short usUnicode, i = 0;
	unsigned char ucMapDat[32];

	if (iCodeType == UI_MENU_CODE_ASCII)
	{
		while (*pStr)
		{
			usUnicode = *pStr;

			if (0 == font_get_data(usUnicode, PTT_FONT_TYPE_12, ucMapDat))
			{
				if (usUnicode <= 0x007e)
				{
					lcd_disp_bit1bmp_SingleByte(x + i * 6, y, ucMapDat, 6, 8, usColor, usBkColor);
					lcd_disp_bit1bmp_SingleByte(x + i * 6, y + 8, &ucMapDat[6], 6, 4, usColor, usBkColor);
				}
			}

			i++;
			pStr++;
		}
	}
	else
	{
		if (strlen((char *)pStr) % 2 != 0)
		{
			_DEBUG("ui12 invalid length!!\r\n");
			return;
		}

		data_len = strlen((char *)pStr);

		if (display_type == display_middle) //居中
		{
			x = (LCD_WIDTH - x - single_data_size * data_len) / 2;
		}
		else //居右和自定义
		{
			x = x;
		}

		while (*pStr)
		{
			if (iEds == UI_FONT_BIG_EDS)
				usUnicode = ((unsigned short)(ui_ascii_to_char(*pStr, *(pStr + 1))) << 8) + ui_ascii_to_char(*(pStr + 2), *(pStr + 3));
			else
				usUnicode = ((unsigned short)(ui_ascii_to_char(*(pStr + 2), *(pStr + 3))) << 8) + ui_ascii_to_char(*(pStr), *(pStr + 1));

			if (0 == font_get_data(usUnicode, PTT_FONT_TYPE_12, ucMapDat))
			{
				if (usUnicode <= 0x007e)
				{
					lcd_disp_bit1bmp_SingleByte(x + i * 6, y, ucMapDat, 6, 8, usColor, usBkColor);
					lcd_disp_bit1bmp_SingleByte(x + i * 6, y + 8, &ucMapDat[6], 6, 4, usColor, usBkColor);
				}
				else
				{
					lcd_disp_bit1bmp_SingleByte(x + i * 6, y, ucMapDat, 12, 8, usColor, usBkColor);
					lcd_disp_bit1bmp_SingleByte(x + i * 6, y + 8, &ucMapDat[12], 12, 4, usColor, usBkColor);
					i++;
				}
			}

			i++;
			pStr += 4;
		}
	}
}

void ui_disp_ascii_str_11x12(unsigned short x, unsigned short y, char *pAscii, unsigned short usBkColor, unsigned short usColor)
{
	unsigned short usUnicode, i = 0;
	unsigned char ucMapDat[32];

	while (*pAscii)
	{
		usUnicode = *pAscii;

		if (0 == font_get_data(usUnicode, PTT_FONT_TYPE_12, ucMapDat))
		{
			if (usUnicode <= 0x007e)
			{
				lcd_disp_bit1bmp_SingleByte(x + i * 6, y, ucMapDat, 6, 8, usColor, usBkColor);
				lcd_disp_bit1bmp_SingleByte(x + i * 6, y + 8, &ucMapDat[6], 6, 4, usColor, usBkColor);
			}
		}

		i++;
		pAscii++;
	}
}

void ui_dips_clean_a_line(unsigned short x, unsigned short y, unsigned short usColor)
{
	ui_disp_ascii_str_11x12(x, y, "                          ", usColor, usColor);
}

void ui_disp_date(stTime *pTime)
{
	char cDate[11];

	sprintf(cDate, "%d-", pTime->iYear);
	ui_disp_ascii_str_11x12(0 + 16 + TIME_DIS_POS_OFF, TIME_DIS_POS_Y, (char *)cDate, COLOR_NET_BACKGROUND, COLOR_FONT);
	sprintf(cDate, "%02d", pTime->iMonth);
	ui_disp_ascii_str_11x12(32 + 16 + TIME_DIS_POS_OFF, TIME_DIS_POS_Y, (char *)cDate, COLOR_NET_BACKGROUND, COLOR_FONT);
	ui_disp_ascii_str_11x12(44 + 16 + TIME_DIS_POS_OFF, TIME_DIS_POS_Y, "-", COLOR_NET_BACKGROUND, COLOR_FONT);
	sprintf(cDate, "%02d", pTime->iMDay);
	ui_disp_ascii_str_11x12(52 + 16 + TIME_DIS_POS_OFF, TIME_DIS_POS_Y, (char *)cDate, COLOR_NET_BACKGROUND, COLOR_FONT);

	ui_disp_ascii_str_11x12(64 + 16 + TIME_DIS_POS_OFF, TIME_DIS_POS_Y, "(", COLOR_NET_BACKGROUND, COLOR_FONT);
	ui_disp_font_11x12(display_left, 70 + 16 + TIME_DIS_POS_OFF, TIME_DIS_POS_Y, ucWeek[pTime->iWDay], COLOR_NET_BACKGROUND, COLOR_FONT, UI_FONT_BIG_EDS, UI_MENU_CODE_UNICODE);
	ui_disp_ascii_str_11x12(82 + 16 + TIME_DIS_POS_OFF, TIME_DIS_POS_Y, ")", COLOR_NET_BACKGROUND, COLOR_FONT);
}
