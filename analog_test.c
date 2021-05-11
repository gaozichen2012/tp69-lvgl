//去掉测试模式，使用固定值写入BK4819，与厂家马工和TYT苏工确认过
#define ANALOG_T
#include "analog_test.h"
#include "write_code.h"
#include "general_include.h"
#include "Uart.h"
#include "state_init.h"
#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

INT8U ConStrTable1Num[11] = {
    9,   //接收灵敏度 	上限
    12,  //1级静噪 	上限
    15,  //9级静噪 	上限
    17,  //场强指示 上限
    80,  //功率调节 上限
    83,  //发射频偏 上限
    86,  //数字频偏 上限
    101, //模拟频偏 上限
    104, //DTMF频偏 上限
    107, //1750频偏 上限
    108  //窄带静噪 上限
};       //频段1分段列表

INT8U ConStrTable2Num[11] = {
    9,   //接收灵敏度 	上限
    13,  //1级静噪 	上限
    17,  //9级静噪 	上限
    19,  //场强指示 上限
    112, //功率调节 上限
    116, //发射频偏 上限
    120, //数字频偏 上限
    140, //模拟频偏 上限
    144, //DTMF频偏 上限
    148, //1750频偏 上限
    149  //窄带静噪 上限
};       //频段2分段列表

INT8U ConStrTable3Num[11] = {
    0,   ///13,	//接收灵敏度 上限------删掉(20181129 ws)
    7,   //1级静噪 	上限
    14,  //9级静噪 	上限
    16,  //场强指示 上限
    55,  //功率调节 上限(低+ 中 + 高)
    62,  //发射频偏 上限
    69,  //数字频偏 上限
    104, //模拟频偏 上限
    111, //DTMF频偏 上限
    118, //1750频偏 上限(注意此段超出8位特殊处理)
    119  //窄带静噪 上限(注意此段超出8位特殊处理)
};       //频段2分段列表

void ClrAllAdjDataPro(void) //批写数据时将手动调节清0 注意别清到电压较准
{
    unsigned short buf_index = 0;
    unsigned char data_len = 0;
    //unsigned char Buffer[32], i;
    // for (i = 0; i <= 26; i++)
    //     Buffer[i] = 64;
    //write(ConAdjTetBegAdd, 27, Buffer);

    buf_index = ConMenEnBegAddr / 0x20; //140
    data_len = 27;

    for (unsigned char i = 0; i < data_len; i++)
    {
        toa_flash2.pocket[buf_index * 32 + i] = 64;
    }
}

void TestModCommandPro(void)
{
    // FE FE EE EF F0 26 98 00 00 00 00 00 FD (进入测试模式) 13字节
    // FE FE EE EF F1 26 98 00 00 00 00 00 FD (退出测试模式)
    // FE FE EE EF F2 26 98 00 00 00 00 00 FD (批写测试模式)
    // FE FE EE EF F3 26 98 00 00 09 30 39 FD (退出当前调试选项)
    // FE FE EE EF F7 26 98 00 00 00 00 00 FD (退出测试模式)
    // FE FE EE EF FA 26 98 30 00 07 30 39 FD (进入相应调试选项及调试数值)---150M
    // FE FE EE EF FA 26 98 31 00 07 30 39 FD (进入相应调试选项及调试数值)---230M
    // FE FE EE EF FA 26 98 32 00 07 30 39 FD (进入相应调试选项及调试数值)---450M
    // FE FE EE EF F3 26 98 00 00 09 30 39 FD (退出当前调试选项)
    // FE FE EE EF F4 26 98 00 00 09 30 39 FD (当前状态发射命令)
    // FE FE EE EF F5 26 98 00 00 09 30 39 FD (取消当前状态发射命令)
    // FE FE EE EF F6 26 98 00 00 09 30 39 FD (当前按键键值命令 按定义来)
    switch (wc_data.rx_buf[4])
    {
    case 0XF0: //进入测试模式
        wc_data.test_mode = 1;
        break;
    case 0XF1: //退出测试模式
        wc_data.test_mode = 0;
        break;
    case 0XF2: //批写测试模式
        wc_data.test_mode = 2;
        break;
    case 0XF3: //退出当前调试选项
        uart_set_analog_cfg_to_file();
        init_get_analog_cfg_from_file();
        break;
    case 0XF4: //发射开始
        wc_data.test_mode = 2;
        wc_data.ang_tx_flg = 1;
        break;
    case 0XF5: //发射结束
        wc_data.test_mode = 0;
        wc_data.ang_tx_flg = 0;
        break;
    case 0XF6: //当前按键键值命令
        /* code */
        break;
    case 0XF7: //退出测试模式
        wc_data.test_mode = 0;

        //批写数据时将手动调节清0 注意别清到电压较准
        //ClrAllAdjDataPro();

        uart_set_analog_cfg_to_file();
        init_get_analog_cfg_from_file();

        break;
    case 0XFA: //进入相应调试选项及调试数值
        EntTestItemPro();
        break;
    default:
        _DEBUG("write code: analog-error0!\r\n");
        break;
    }

    if (wc_data.rx_buf[4] == 0XF0 ||
        wc_data.rx_buf[4] == 0XF1 ||
        wc_data.rx_buf[4] == 0XF2 ||
        wc_data.rx_buf[4] == 0XF3 ||
        wc_data.rx_buf[4] == 0XF4 ||
        wc_data.rx_buf[4] == 0XF5 ||
        wc_data.rx_buf[4] == 0XF6 ||
        wc_data.rx_buf[4] == 0XF7 ||
        wc_data.rx_buf[4] == 0XF8 ||
        wc_data.rx_buf[4] == 0XFA)
    {
        answer_wrcode_end();
    }
}

void EntTestItemPro(void)
{
#if 0
    INT16U TestItem;

    Get1ByteDataPro(&wc_data.rx_buf[0X0A]); //取得调试值

    GiTestAdDat = wc_data.rx_buf[0X0A];
    TestItem = (INT16U)wc_data.rx_buf[0X08] * 256 + (INT16U)wc_data.rx_buf[0X09]; //调试菜单项

    switch (wc_data.rx_buf[7])
    {
    case 0x30: //当前调试频段0 145M
        if (GiCurTestBand != 0)
        {
            GiTestItem = 700; //指向无效地址
            GiCurTestBand = 0;
        }
        break;
    case 0x31: //当前调试频段1 230M
        if (GiCurTestBand != 1)
        {
            GiTestItem = 700; //指向无效地址
            GiCurTestBand = 1;
        }
        break;
    case 0x32: //当前调试频段2 450M
        if (GiCurTestBand != 2)
        {
            GiTestItem = 700; //指向无效地址
            GiCurTestBand = 2;
        }
        break;
    default:
        break;
    }

    Left_P.GiFreBand = GiCurTestBand;
    if (TestItem >= 0X0200) //生产线测模式
    {
        EntTestItem2Pro(TestItem); //生产线测模式
    }
    else //研发测模式
    {
        switch (GiCurTestBand)
        {
        case 0:
            EntTestItem1Pro(TestItem, ConStrTable1Num);
            break;
        case 1:
            EntTestItem1Pro(TestItem, ConStrTable2Num);
            break;
        case 2:
            EntTestItem1Pro(TestItem, ConStrTable3Num); ///450M 参数设置模式
            break;
        }
    }
#endif
}
