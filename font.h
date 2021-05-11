#ifndef __FONT_H__
#define __FONT_H__

//字库取模方式: 方式四纵向取模，先左后右，先上后下
//0x00000000 - 0x00000800 存放ASCII 16*8 大小的字模  0x00 - 0x7f
//0x00000800 - 0x000A4800 存放UNICODE 汉字部分 16*16
//0x000A4800 - 0x000A4E00 存放ASCII 12*6 大小的字模  0x00 - 0x7f
//0x000A4E00 -            存放UNICODE 汉字部分 12*11

//#define FONT_DZK_PATH				"/usr/bin/FT.dzk"
#define FONT_DZK_PATH "C:/FT.dzk"

#define FONT_ASCII_16_BASE_ADD 0x00000000
#define FONT_CHN_16_BASE_ADD 0x00000800

#define FONT_ASCII_12_BASE_ADD 0x000A4800
#define FONT_CHN_12_BASE_ADD 0x000A4E00

typedef enum
{
	PTT_FONT_TYPE_12,
	PTT_FONT_TYPE_16
} enFontType;

void font_pkg_init(void);
int font_get_16_bmp(unsigned short usUnicode, unsigned char *pBuf);
int font_display_test(unsigned short usUnicode);

int font_get_data(unsigned short usUnicode, enFontType iFont, unsigned char *pData);
int font_get_data_ex(unsigned short usUnicode, enFontType iFont, unsigned char *pData);

#endif
