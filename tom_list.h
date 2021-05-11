#ifndef __TOM_LIST_H
#define __TOM_LIST_H

#define Y_MAX 128
#define X_MAX 160

#define ITEM_DISP_NUM_MAX 4             //显示的最大项目数
#define ITEM_START_POINT_X 20 + 8 + 3   //项目起始x点
#define ITEM_START_POINT_Y 34           //项目起始y点
#define ITEM_TRUE_ICO_START_POINT_X 140 //对号的起始点
#define ITEM_SPACE_Y 20                 //每个项目格的宽度

#define GROUP_ITEM_DISP_NUM_MAX 3                //显示的最大项目数
#define GROUP_ITEM_START_POINT_X 20 + 20 + 8 + 3 //项目起始x点
#define GROUP_ITEM_SPACE_X 120                   //每个项目格的宽度
#define GROUP_ITEM_SPACE_Y 26                    //每个项目格的宽度

typedef enum
{
    REFLASH_KEY_UP,
    REFLASH_KEY_DOWN,
    REFLASH_ALL, //手
} DISPLAY_REFLASH_TYPE;

void lcd_api_normal_item_disp(int current_item, char *item_name[], unsigned char items_num_max, unsigned short actual_item, DISPLAY_REFLASH_TYPE refresh_type, euCodeType iCodeType);
void normal_item_disp_with_channel_list(int current_item, DISPLAY_REFLASH_TYPE refresh_type);

#endif