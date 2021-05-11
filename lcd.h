#ifndef __LCD_H__
#define __LCD_H__

#include "ql_gpio.h"

#define LCD_WIDTH 160
#define LCD_HEIGHT 128

#define COLOR_WHITE 0xFFFF //白色
#define COLOR_BLACK 0x0000 //黑色

#define COLOR_RED 0x00F8 //红色

#define COLOR_GREEN 0xE007  //绿色
#define COLOR_BLUE 0x1F00   //蓝色
#define COLOR_YELLOW 0xE0FF //黄色

#define COLOR_QUBLUE 0xdd8e //0x8edd-1000111011011101-10001000 11011000 11101000-R136 G216 B232

#if 0
#define COLOR_NET_ICO_BACKGROUND COLOR_BLUE
#define COLOR_NET_ICO_FONT COLOR_WHITE
#define COLOR_NET_BACKGROUND COLOR_QUBLUE

#define COLOR_ANA_ICO_BACKGROUND COLOR_BLUE
#define COLOR_ANA_ICO_FONT COLOR_WHITE
#define COLOR_ANA_BACKGROUND COLOR_QUBLUE

#define COLOR_LINE COLOR_WHITE
#define COLOR_FONT COLOR_BLACK
#define COLOR_BMP COLOR_BLUE
#else
#define COLOR_NET_ICO_BACKGROUND COLOR_BLACK
#define COLOR_NET_ICO_FONT COLOR_WHITE
#define COLOR_NET_BACKGROUND COLOR_BLACK

#define COLOR_ANA_ICO_BACKGROUND COLOR_BLACK
#define COLOR_ANA_ICO_FONT COLOR_WHITE
#define COLOR_ANA_BACKGROUND COLOR_BLACK

#define COLOR_LINE COLOR_WHITE
#define COLOR_FONT COLOR_WHITE
#define COLOR_BMP COLOR_WHITE
#endif

#define LCD_BL_PIN_NUM GPIO_PIN_NO_32

#define lcd_disp_LineH(x, y, l, w, c) lcd_fill_color(x, y, w, l, c)
/*
		lcd_disp_box(0, 16, COLOR_LINE, LCD_WIDTH, 1);
		lcd_disp_box(0, 17, COLOR_NET_BACKGROUND, LCD_WIDTH, 57 - 17);
		lcd_disp_box(0, 57, COLOR_LINE, LCD_WIDTH, 1);
		lcd_disp_box(0, 58, COLOR_ANA_BACKGROUND, LCD_WIDTH, 16);
		lcd_disp_box(0, 74, COLOR_NET_BACKGROUND, LCD_WIDTH, 111 - 74);
		lcd_disp_box(0, LCD_HEIGHT - 17, COLOR_LINE, LCD_WIDTH, 1);
		lcd_disp_box(0, LCD_HEIGHT - 16, COLOR_NET_ICO_BACKGROUND, LCD_WIDTH, 16);
*/
#define lcd_disp_box(x, y, c, w, h) lcd_fill_color(x, y, w, h, c)
#define lcd_disp_bmp_SingleByte(x, y, p, w, h) lcd_draw_bmp(x, y, w, h, p)

void lcd_device_init(void);
void lcd_update_disp(void);
void lcd_set_bl(unsigned int uiBlSta);

void lcd_bl_test(void);
void lcd_draw_bmp(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char *pBuf);
void lcd_fill_color(unsigned char ucX, unsigned char ucY, unsigned char ucW, unsigned char ucH, unsigned short usColor);
void lcd_disp_bit1bmp_SingleByte(unsigned char x, unsigned char y, unsigned char *pBmp, unsigned char w, unsigned char h, unsigned short usColor, unsigned short usBkColor);
void lcd_disp_tan(unsigned short X0, unsigned short Y0, unsigned short X1, unsigned short Y1, unsigned short X2, unsigned short Y2, unsigned short LineWidth, unsigned short Color);
void lcd_disp_range(unsigned short x, unsigned short y, unsigned short width, unsigned short hight, unsigned short LineWidth, unsigned short Color);
#endif
