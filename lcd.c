#include "ql_rtos.h"
#include "ql_lcd.h"
#include "ql_gpio.h"
#include "ql_application.h"

#include "general_include.h"
#include "lcd.h"
#include "font.h"
#include "ui.h"
// #include "version.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

const unsigned char UI_MENU_UNIC_GROUP_SELECT[] = {0x7f, 0xa4, 0x7e, 0xc4, 0x90, 0x09, 0x62, 0xe9, 0x00, 0x31};
const unsigned char UI_MENU_UNIC_MEMBER_SELECT[] = {0x62, 0x10, 0x54, 0x58, 0x90, 0x09, 0x62, 0xe9, 0x00, 0x32};

unsigned short usMap[LCD_WIDTH * LCD_HEIGHT];

void lcd_set_bl(unsigned int uiBlSta)
{
	if (uiBlSta > 0)
	{
		ql_lcd_set_brightness(5);
		ql_lcd_write_fb((unsigned char *)usMap);
		ql_gpio_set_level(LCD_BL_PIN_NUM, PIN_LEVEL_LOW);
	}
	else
	{
		ql_gpio_set_level(LCD_BL_PIN_NUM, PIN_LEVEL_HIGH);
		ql_lcd_set_brightness(0);
	}
}

void lcd_fill_color(unsigned char ucX, unsigned char ucY, unsigned char ucW, unsigned char ucH, unsigned short usColor)
{
	unsigned int i, j;
	unsigned char ucXE, ucYE;
	unsigned long ulIdx;
	unsigned short usTmp = (usColor << 8) & 0xff00;

	usColor = (usColor >> 8) | usTmp;

	if (ucX >= LCD_WIDTH || ucY >= LCD_HEIGHT)
		return;

	ucXE = ucX + ucW;
	ucYE = ucY + ucH;

	if (ucXE >= LCD_WIDTH)
		ucXE = LCD_WIDTH;
	if (ucYE >= LCD_HEIGHT)
		ucYE = LCD_HEIGHT;

	for (j = ucY; j < ucYE; j++)
	{
		ulIdx = j * LCD_WIDTH;
		for (i = ucX; i < ucXE; i++)
			usMap[i + ulIdx] = usColor;
	}

	//lcd_update_disp();
}

//画任意角度直线
void lcd_disp_line(unsigned short xS, unsigned short yS, unsigned short xE, unsigned short yE, unsigned short w, unsigned short usColor)
{
	int y = 0, i = 0;
	int x0, y0, x1;

	if (xS < xE)
	{
		x0 = xS;
		x1 = xE;
		y0 = yS;
	}
	else
	{
		x0 = xE;
		x1 = xS;
		y0 = yE;
	}

	for (i = x0; i <= x1; i++)
	{
		y = (int)((((double)(i - x0)) / ((double)(xE - xS))) * ((double)yE - (double)yS));
		lcd_fill_color(i, (unsigned char)(y0 + y), 1, w, usColor);
	}
}

void lcd_disp_tan(unsigned short X0, unsigned short Y0, unsigned short X1, unsigned short Y1, unsigned short X2, unsigned short Y2, unsigned short LineWidth, unsigned short Color)
{
	lcd_disp_line(X0, Y0, X1 - 1, Y1, LineWidth, Color);
	lcd_disp_line(X1, Y1, X2 - 1, Y2, LineWidth, Color);
	lcd_disp_line(X2, Y2, X0, Y0, LineWidth, Color);
}

void lcd_disp_range(unsigned short x, unsigned short y, unsigned short width, unsigned short hight, unsigned short LineWidth, unsigned short Color)
{
	lcd_fill_color(x, y, width, LineWidth, Color);
	lcd_fill_color(x, y, LineWidth, hight, Color);
	lcd_fill_color(x + width - LineWidth, y, LineWidth, hight, Color);
	lcd_fill_color(x, y + hight - LineWidth, width, LineWidth, Color);
}

void lcd_disp_bit1bmp_SingleByte(unsigned char x, unsigned char y, unsigned char *pBmp, unsigned char w, unsigned char h, unsigned short usColor, unsigned short usBkColor)
{
	unsigned char i, j;
	unsigned char ucXE, ucYE;
	unsigned short usTmp;
	unsigned long ulIdx;

	usTmp = (usColor << 8) & 0xff00;
	usColor = (usColor >> 8) | usTmp;

	usTmp = (usBkColor << 8) & 0xff00;
	usBkColor = (usBkColor >> 8) | usTmp;

	ucXE = x + w;
	if (ucXE > LCD_WIDTH)
		ucXE = LCD_WIDTH;

	ucYE = y + h;
	if (ucYE > LCD_HEIGHT)
		ucYE = LCD_HEIGHT;

	for (j = y; j < ucYE; j++)
	{
		ulIdx = j * LCD_WIDTH;
		for (i = x; i < ucXE; i++)
		{
			if ((*(pBmp + (i - x) + ((j - y) >> 3) * w)) & (0x01 << ((j - y) % 8)))
				usMap[i + ulIdx] = usColor;
			else
				usMap[i + ulIdx] = usBkColor;
		}
	}
}

void lcd_draw_bmp(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char *pBuf)
{
	unsigned char i, j;
	unsigned char ucXE, ucYE;
	unsigned long ulIdx;

	ucXE = x + w;
	if (ucXE > LCD_WIDTH)
		ucXE = LCD_WIDTH;

	ucYE = y + h;
	if (ucYE > LCD_HEIGHT)
		ucYE = LCD_HEIGHT;

	for (j = y; j < ucYE; j++)
	{
		ulIdx = j * LCD_WIDTH;
		for (i = x; i < ucXE; i++)
		{
			usMap[i + ulIdx] = (unsigned short)(*(pBuf + 1)) + ((((unsigned short)(*(pBuf))) << 8));
			//usMap[i+ulIdx] = (unsigned short)(*(pBuf)) + ((((unsigned short)(*(pBuf+1)))<<8));
			pBuf += 2;
		}
	}
}

static ql_spi_lcd_write_t spi_init_st7735[] =
	{
		{QL_LCD_CMD, 0x11, 0},
		{QL_LCD_DELAY, 120, 0},
		{QL_LCD_CMD, 0xB1, 3},
		{QL_LCD_DATA, 0x01, 0},
		{QL_LCD_DATA, 0x08, 0},
		{QL_LCD_DATA, 0x05, 0},
		{QL_LCD_CMD, 0xB2, 3},
		{QL_LCD_DATA, 0x05, 0},
		{QL_LCD_DATA, 0x3C, 0},
		{QL_LCD_DATA, 0x3C, 0},
		{QL_LCD_CMD, 0xB3, 6},
		{QL_LCD_DATA, 0x05, 0},
		{QL_LCD_DATA, 0x3C, 0},
		{QL_LCD_DATA, 0x3C, 0},
		{QL_LCD_DATA, 0x05, 0},
		{QL_LCD_DATA, 0x3C, 0},
		{QL_LCD_DATA, 0x3C, 0},
		{QL_LCD_CMD, 0xB4, 1},
		{QL_LCD_DATA, 0x03, 0},
		{QL_LCD_CMD, 0xC0, 3},
		{QL_LCD_DATA, 0x28, 0},
		{QL_LCD_DATA, 0x08, 0},
		{QL_LCD_DATA, 0x04, 0},
		{QL_LCD_CMD, 0xC1, 1},
		{QL_LCD_DATA, 0xC0, 0},
		{QL_LCD_CMD, 0xC2, 2},
		{QL_LCD_DATA, 0x0D, 0},
		{QL_LCD_DATA, 0x00, 0},
		{QL_LCD_CMD, 0xC3, 2},
		{QL_LCD_DATA, 0x8D, 0},
		{QL_LCD_DATA, 0x2A, 0},
		{QL_LCD_CMD, 0xC4, 2},
		{QL_LCD_DATA, 0x8D, 0},
		{QL_LCD_DATA, 0xEE, 0},
		{QL_LCD_CMD, 0xC5, 1},
		{QL_LCD_DATA, 0x12, 0},
		{QL_LCD_CMD, 0x36, 1},
		{QL_LCD_DATA, 0xa4, 0},
		{QL_LCD_CMD, 0xE0, 16},
		{QL_LCD_DATA, 0x04, 0},
		{QL_LCD_DATA, 0x22, 0},
		{QL_LCD_DATA, 0x07, 0},
		{QL_LCD_DATA, 0x0A, 0},
		{QL_LCD_DATA, 0x2E, 0},
		{QL_LCD_DATA, 0x30, 0},
		{QL_LCD_DATA, 0x25, 0},
		{QL_LCD_DATA, 0x2A, 0},
		{QL_LCD_DATA, 0x28, 0},
		{QL_LCD_DATA, 0x26, 0},
		{QL_LCD_DATA, 0x2E, 0},
		{QL_LCD_DATA, 0x3A, 0},
		{QL_LCD_DATA, 0x00, 0},
		{QL_LCD_DATA, 0x01, 0},
		{QL_LCD_DATA, 0x03, 0},
		{QL_LCD_DATA, 0x13, 0},
		{QL_LCD_CMD, 0xE1, 16},
		{QL_LCD_DATA, 0x04, 0},
		{QL_LCD_DATA, 0x16, 0},
		{QL_LCD_DATA, 0x06, 0},
		{QL_LCD_DATA, 0x0D, 0},
		{QL_LCD_DATA, 0x2D, 0},
		{QL_LCD_DATA, 0x26, 0},
		{QL_LCD_DATA, 0x23, 0},
		{QL_LCD_DATA, 0x27, 0},
		{QL_LCD_DATA, 0x27, 0},
		{QL_LCD_DATA, 0x25, 0},
		{QL_LCD_DATA, 0x2D, 0},
		{QL_LCD_DATA, 0x3B, 0},
		{QL_LCD_DATA, 0x00, 0},
		{QL_LCD_DATA, 0x01, 0},
		{QL_LCD_DATA, 0x04, 0},
		{QL_LCD_DATA, 0x13, 0},

		{QL_LCD_CMD, 0x3A, 1},
		{QL_LCD_DATA, 0x05, 0},
		{QL_LCD_CMD, 0x35, 1},
		{QL_LCD_DATA, 0x00, 0},
		{QL_LCD_CMD, 0x29, 0},
		{QL_LCD_CMD, 0x2C, 0},
};

static ql_lcd_data_t lcd_data =
	{
		.init_para = spi_init_st7735,
		.init_para_len = sizeof(spi_init_st7735) / sizeof(ql_spi_lcd_write_t),
		.width = LCD_WIDTH,
		.height = LCD_HEIGHT,
		.spi_clk = QL_SPI_LCD_CLK_26M,
		.te_signal = QL_LCD_CAP_NOTE,
		.lcd_id = 0x7c89f0,
		.format = QL_SPI_FORMAT_RGB565,
		.sample_edge = QL_SPI_EDGE_RISING,
		.endian = QL_SPI_ENDIAN_MSB,
		.data_line = 4,
};

void lcd_device_init(void)
{
	memset(usMap, 0xff, LCD_WIDTH * LCD_HEIGHT * 2);
	ql_gpio_init(LCD_BL_PIN_NUM, PIN_DIRECTION_OUT, PIN_PULL_PD, PIN_LEVEL_LOW);
	ql_lcd_init(&lcd_data);
}

void lcd_update_disp(void)
{
	ql_lcd_write_fb((unsigned char *)usMap);
}
