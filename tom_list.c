#include "general_include.h"
#include "tom_list.h"
#include "lcd.h"
#include "ui.h"
#include "analog_model.h"
#include <string.h>
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

//项目号背景
const unsigned char gImage_backgrund[60] = {
    /* 0X22,0X01,0X14,0X00,0X14,0X00, */
    0X00,
    0XE0,
    0XF0,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF8,
    0XF0,
    0XE0,
    0X00,
    0X00,
    0X7F,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0XFF,
    0X7F,
    0X00,
    0X00,
    0X00,
    0X00,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X01,
    0X00,
    0X00,
    0X00,
};
//项目号0-9
const unsigned char bits[][12] = {
    {//U+0030(0)
     0X00,
     0XF0,
     0X08,
     0X08,
     0XF0,
     0X00,
     0X00,
     0X01,
     0X02,
     0X02,
     0X01,
     0X00},
    {//U+0031(1)
     0X00,
     0X00,
     0X10,
     0XF0,
     0X00,
     0X00,
     0X00,
     0X00,
     0X00,
     0X03,
     0X00,
     0X00},
    {//U+0032(2)
     0X00,
     0X08,
     0X08,
     0X88,
     0X70,
     0X00,
     0X00,
     0X02,
     0X03,
     0X02,
     0X02,
     0X00},
    {//U+0033(3)
     0X00,
     0X00,
     0X48,
     0X48,
     0XB0,
     0X00,
     0X00,
     0X02,
     0X02,
     0X02,
     0X01,
     0X00},
    {//U+0034(4)
     0X80,
     0XC0,
     0XA0,
     0XF0,
     0X80,
     0X00,
     0X00,
     0X00,
     0X00,
     0X03,
     0X00,
     0X00},
    {//U+0035(5)
     0X00,
     0X00,
     0X70,
     0X50,
     0X90,
     0X00,
     0X00,
     0X02,
     0X02,
     0X02,
     0X01,
     0X00},
    {//U+0036(6)
     0X00,
     0XC0,
     0X60,
     0X50,
     0X80,
     0X00,
     0X00,
     0X01,
     0X02,
     0X02,
     0X01,
     0X00},
    {//U+0037(7)
     0X00,
     0X10,
     0X10,
     0XD0,
     0X30,
     0X00,
     0X00,
     0X02,
     0X03,
     0X00,
     0X00,
     0X00},
    {//U+0038(8)
     0X00,
     0XB0,
     0X48,
     0X48,
     0XB0,
     0X00,
     0X00,
     0X01,
     0X02,
     0X02,
     0X01,
     0X00},
    {//U+0039(9)
     0X00,
     0X70,
     0X88,
     0X88,
     0XF0,
     0X00,
     0X00,
     0X00,
     0X02,
     0X01,
     0X00,
     0X00},
};

const unsigned char gImage_triangle_8x16[16] = {
    /* 0X22,0X01,0X08,0X00,0X10,0X00, */
    0X00,
    0XFE,
    0XFC,
    0XF8,
    0XF0,
    0XE0,
    0XC0,
    0X80,
    0X00,
    0X3F,
    0X1F,
    0X0F,
    0X07,
    0X03,
    0X01,
    0X00,
};

//项目对号√
const unsigned char gImage_true[32] = {
    /* 0X22,0X01,0X10,0X00,0X10,0X00, */
    0X00,
    0X00,
    0X00,
    0X80,
    0X00,
    0X00,
    0X00,
    0X00,
    0X00,
    0X80,
    0XC0,
    0X60,
    0X20,
    0X00,
    0X00,
    0X00,
    0X00,
    0X00,
    0X00,
    0X01,
    0X03,
    0X06,
    0X0C,
    0X06,
    0X03,
    0X01,
    0X00,
    0X00,
    0X00,
    0X00,
    0X00,
    0X00,
};

//项目序号显示 dis_num显示的总项目数：一般为4，可能是1-3
static void item_num_display(unsigned short num, unsigned char dis_num)
{
    unsigned char num_1 = 0, num_2 = 0, num_3 = 0; //个十百位
    unsigned char display_num = 0;                 //计算数字位置

    num_1 = (num / 1) % 10;
    num_2 = (num / 10) % 10;
    num_3 = (num / 100) % 10;

    display_num = (num - 1) % ITEM_DISP_NUM_MAX;

    //lcd_api_display_range(0 + 2, ITEM_START_POINT_Y + ITEM_SPACE_Y * i + 1, 20 - 2 - 2, 20 - 2 - 2, COLOR_FONT);

    lcd_disp_bit1bmp_SingleByte(0, ITEM_START_POINT_Y + ITEM_SPACE_Y * display_num, (unsigned char *)gImage_backgrund, 20, 20, COLOR_BLUE, COLOR_NET_BACKGROUND);

    if (num <= 9)
    {
        lcd_disp_bit1bmp_SingleByte(6, 4 + ITEM_START_POINT_Y + ITEM_SPACE_Y * display_num, (unsigned char *)bits[num_1 % 10], 6, 12, COLOR_WHITE, COLOR_BLUE);
    }
    else if (num <= 99)
    {
        lcd_disp_bit1bmp_SingleByte(3, 4 + ITEM_START_POINT_Y + ITEM_SPACE_Y * display_num, (unsigned char *)bits[num_2 % 10], 6, 12, COLOR_WHITE, COLOR_BLUE);
        lcd_disp_bit1bmp_SingleByte(3 + 6, 4 + ITEM_START_POINT_Y + ITEM_SPACE_Y * display_num, (unsigned char *)bits[num_1 % 10], 6, 12, COLOR_WHITE, COLOR_BLUE);
    }
    else if (num <= 999)
    {
        lcd_disp_bit1bmp_SingleByte(0, 4 + ITEM_START_POINT_Y + ITEM_SPACE_Y * display_num, (unsigned char *)bits[num_3 % 10], 6, 12, COLOR_WHITE, COLOR_BLUE);
        lcd_disp_bit1bmp_SingleByte(0 + 6, 4 + ITEM_START_POINT_Y + ITEM_SPACE_Y * display_num, (unsigned char *)bits[num_2 % 10], 6, 12, COLOR_WHITE, COLOR_BLUE);
        lcd_disp_bit1bmp_SingleByte(0 + 12, 4 + ITEM_START_POINT_Y + ITEM_SPACE_Y * display_num, (unsigned char *)bits[num_1 % 10], 6, 12, COLOR_WHITE, COLOR_BLUE);
    }
    else
    {
    }
}

/* 
通用选项显示
current_item:当前光标所在的选项
item_name:项目名（二维数组）
items_num_max:最大项目数
actual_item:实际选中的选项（勾选，无需勾选输入0xFFFF）
*/
void lcd_api_normal_item_disp(int current_item, char *item_name[], unsigned char items_num_max, unsigned short actual_item, DISPLAY_REFLASH_TYPE refresh_type, euCodeType iCodeType)
{
    unsigned char display_first_item = 0;     //当前显示项目的第一个项目
    unsigned char last_page_item_num_sum = 0; //最后一页的总项目数
    unsigned char last_page_first_item = 0;   //最后一页的第一个项目

    display_first_item = (current_item / ITEM_DISP_NUM_MAX) * ITEM_DISP_NUM_MAX;    //显示的第一个选项
    last_page_first_item = (items_num_max / ITEM_DISP_NUM_MAX) * ITEM_DISP_NUM_MAX; //最后一页的第一个项目
    last_page_item_num_sum = items_num_max - last_page_first_item;                  //最后一页的总项目数

    //比如总项目7,0-3 第一页 4-6 第二页
    if (current_item >= last_page_first_item) //最后一页的项目数可能<ITEM_DISP_NUM_MAX，所以要特别处理
    {
        if ((refresh_type == REFLASH_KEY_DOWN && current_item == display_first_item) ||
            (refresh_type == REFLASH_KEY_UP && current_item == display_first_item + last_page_item_num_sum - 1) ||
            (refresh_type == REFLASH_ALL))
        {
            lcd_disp_box(0, ITEM_START_POINT_Y, COLOR_NET_BACKGROUND, X_MAX, ITEM_SPACE_Y * ITEM_DISP_NUM_MAX - 1);
            for (unsigned char i = display_first_item; i < display_first_item + last_page_item_num_sum; i++)
            {
                item_num_display(i + 1, last_page_item_num_sum);
                if (i == current_item) //如果选中则显示翻转色
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                }
                else //未被选中则正常显示
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_NET_BACKGROUND, COLOR_NET_BACKGROUND);
                }


                ui_disp_font_11x12(display_left, ITEM_START_POINT_X, 3 + ITEM_START_POINT_Y + (i - display_first_item) * ITEM_SPACE_Y, (const char *)item_name[i], COLOR_NET_BACKGROUND, COLOR_FONT, UI_FONT_BIG_EDS, iCodeType);

                //是否显示对号√
                if (actual_item != 0xFF) //0xFF则不显示对号
                {
                    if (actual_item == i)
                    {
                        lcd_disp_bit1bmp_SingleByte(ITEM_TRUE_ICO_START_POINT_X, ITEM_START_POINT_Y + (i - display_first_item) * ITEM_SPACE_Y, (unsigned char *)gImage_true, 16, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                    }
                }
            }
        }
        else
        {
            for (unsigned char i = display_first_item; i < display_first_item + last_page_item_num_sum; i++)
            {
                if (i == current_item) //如果选中则显示翻转色
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                }
                else //未被选中则正常显示
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_NET_BACKGROUND, COLOR_NET_BACKGROUND);
                }
            }
        }
    }
    else
    {
        if ((refresh_type == REFLASH_KEY_DOWN && current_item == display_first_item) ||
            (refresh_type == REFLASH_KEY_UP && current_item == display_first_item + ITEM_DISP_NUM_MAX - 1) ||
            (refresh_type == REFLASH_ALL))
        {
            lcd_disp_box(0, ITEM_START_POINT_Y, COLOR_NET_BACKGROUND, X_MAX, ITEM_SPACE_Y * ITEM_DISP_NUM_MAX - 1);

            for (unsigned char i = display_first_item; i < display_first_item + ITEM_DISP_NUM_MAX; i++)
            {
                item_num_display(i + 1, ITEM_DISP_NUM_MAX);
                if (i == current_item) //如果选中则显示翻转色
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                }
                else //未被选中则正常显示
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_NET_BACKGROUND, COLOR_NET_BACKGROUND);
                }

                ui_disp_font_11x12(display_left, ITEM_START_POINT_X, 3 + ITEM_START_POINT_Y + (i - display_first_item) * 20, (const char *)item_name[i], COLOR_NET_BACKGROUND, COLOR_FONT, UI_FONT_BIG_EDS, iCodeType);

                //是否显示对号√
                if (actual_item != 0xFFFF) //0xFF则不显示对号
                {
                    if (actual_item == i)
                    {
                        lcd_disp_bit1bmp_SingleByte(ITEM_TRUE_ICO_START_POINT_X, ITEM_START_POINT_Y + (i - display_first_item) * ITEM_SPACE_Y, (unsigned char *)gImage_true, 16, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                    }
                }
            }
        }
        else
        {
            for (unsigned char i = display_first_item; i < display_first_item + ITEM_DISP_NUM_MAX; i++)
            {
                if (i == current_item) //如果选中则显示翻转色
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                }
                else //未被选中则正常显示
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_NET_BACKGROUND, COLOR_NET_BACKGROUND);
                }
            }
        }
    }
    lcd_update_disp();
}

/* 
通用选项显示
current_item:当前光标所在的选项
item_name:项目名（二维数组）
items_num_max:最大项目数
actual_item:实际选中的选项（勾选，无需勾选输入0xFFFF）
*/
void normal_item_disp_with_channel_list(int current_item, DISPLAY_REFLASH_TYPE refresh_type)
{
#if 1
    unsigned char display_first_item = 0;     //当前显示项目的第一个项目
    unsigned char last_page_item_num_sum = 0; //最后一页的总项目数
    unsigned char last_page_first_item = 0;   //最后一页的第一个项目

    unsigned char items_num_max = 0;
    unsigned short actual_item = 0;
    euCodeType iCodeType;

    items_num_max = channel_max_num;
    actual_item = 0xFFFF;
    iCodeType = UI_MENU_CODE_ASCII;

    display_first_item = (current_item / ITEM_DISP_NUM_MAX) * ITEM_DISP_NUM_MAX;    //显示的第一个选项
    last_page_first_item = (items_num_max / ITEM_DISP_NUM_MAX) * ITEM_DISP_NUM_MAX; //最后一页的第一个项目
    last_page_item_num_sum = items_num_max - last_page_first_item;                  //最后一页的总项目数

    //比如总项目7,0-3 第一页 4-6 第二页
    if (current_item >= last_page_first_item) //最后一页的项目数可能<ITEM_DISP_NUM_MAX，所以要特别处理
    {
        if ((refresh_type == REFLASH_KEY_DOWN && current_item == display_first_item) ||
            (refresh_type == REFLASH_KEY_UP && current_item == display_first_item + last_page_item_num_sum - 1) ||
            (refresh_type == REFLASH_ALL))
        {
            lcd_disp_box(0, ITEM_START_POINT_Y, COLOR_NET_BACKGROUND, X_MAX, ITEM_SPACE_Y * ITEM_DISP_NUM_MAX - 1);
            for (unsigned char i = display_first_item; i < display_first_item + last_page_item_num_sum; i++)
            {
                item_num_display(i + 1, last_page_item_num_sum);
                if (i == current_item) //如果选中则显示翻转色
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                }
                else //未被选中则正常显示
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_NET_BACKGROUND, COLOR_NET_BACKGROUND);
                }

                ui_disp_font_11x12(display_left, ITEM_START_POINT_X, 3 + ITEM_START_POINT_Y + (i - display_first_item) * ITEM_SPACE_Y, (const char *)rx_channel_buf[i], COLOR_NET_BACKGROUND, COLOR_FONT, UI_FONT_BIG_EDS, iCodeType);

                //是否显示对号√
                if (actual_item != 0xFF) //0xFF则不显示对号
                {
                    if (actual_item == i)
                    {
                        lcd_disp_bit1bmp_SingleByte(ITEM_TRUE_ICO_START_POINT_X, ITEM_START_POINT_Y + (i - display_first_item) * ITEM_SPACE_Y, (unsigned char *)gImage_true, 16, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                    }
                }
            }
        }
        else
        {
            for (unsigned char i = display_first_item; i < display_first_item + last_page_item_num_sum; i++)
            {
                if (i == current_item) //如果选中则显示翻转色
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                }
                else //未被选中则正常显示
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_NET_BACKGROUND, COLOR_NET_BACKGROUND);
                }
            }
        }
    }
    else
    {
        if ((refresh_type == REFLASH_KEY_DOWN && current_item == display_first_item) ||
            (refresh_type == REFLASH_KEY_UP && current_item == display_first_item + ITEM_DISP_NUM_MAX - 1) ||
            (refresh_type == REFLASH_ALL))
        {
            lcd_disp_box(0, ITEM_START_POINT_Y, COLOR_NET_BACKGROUND, X_MAX, ITEM_SPACE_Y * ITEM_DISP_NUM_MAX - 1);

            for (unsigned char i = display_first_item; i < display_first_item + ITEM_DISP_NUM_MAX; i++)
            {
                item_num_display(i + 1, ITEM_DISP_NUM_MAX);
                if (i == current_item) //如果选中则显示翻转色
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                }
                else //未被选中则正常显示
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_NET_BACKGROUND, COLOR_NET_BACKGROUND);
                }

                ui_disp_font_11x12(display_left, ITEM_START_POINT_X, 3 + ITEM_START_POINT_Y + (i - display_first_item) * 20, (const char *)rx_channel_buf[i], COLOR_NET_BACKGROUND, COLOR_FONT, UI_FONT_BIG_EDS, iCodeType);

                //是否显示对号√
                if (actual_item != 0xFFFF) //0xFF则不显示对号
                {
                    if (actual_item == i)
                    {
                        lcd_disp_bit1bmp_SingleByte(ITEM_TRUE_ICO_START_POINT_X, ITEM_START_POINT_Y + (i - display_first_item) * ITEM_SPACE_Y, (unsigned char *)gImage_true, 16, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                    }
                }
            }
        }
        else
        {
            for (unsigned char i = display_first_item; i < display_first_item + ITEM_DISP_NUM_MAX; i++)
            {
                if (i == current_item) //如果选中则显示翻转色
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_RED, COLOR_NET_BACKGROUND);
                }
                else //未被选中则正常显示
                {
                    lcd_disp_bit1bmp_SingleByte(20, ITEM_START_POINT_Y + ITEM_SPACE_Y * (i - display_first_item) + 2, (unsigned char *)gImage_triangle_8x16, 8, 16, COLOR_NET_BACKGROUND, COLOR_NET_BACKGROUND);
                }
            }
        }
    }
    lcd_update_disp();
#else
    char *angmenu_item[5] = {
        c_tx,
        c_rx,
        c_tx,
        c_rx,
        c_tx,
    };
    lcd_api_normal_item_disp(current_item, angmenu_item, 5, 0xFFFF, refresh_type, UI_MENU_CODE_ASCII);
#endif
}