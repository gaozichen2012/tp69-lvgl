#define ANALOG_OP
#include "general_include.h"
#include "analog_old_patch.h"
#include "write_code.h"
#include "analog_model.h"
#include "Uart.h"
#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

//将ASCII码的数据转成相应数字 0x32->3 AsciiToDisData
unsigned char ascii2num(unsigned char Data)
{
    if (Data >= 0x20 && Data <= 0x2F) ///特别字符，请查ASC表
        Data = Data + 4;
    else if (Data >= 0x30 && Data <= 0x39) //1 2 3..9
        Data = Data - 0X30;
    else if (Data >= 0x3A && Data <= 0x40) //特别字符，请查ASC表
        Data = Data - 6;
    else if (Data >= 0x41 && Data <= 0x5A) //A B..Z
        Data = Data - 0X37;
    else if (Data >= 0x5B && Data <= 0x5F) //特别字符，请查ASC表
        Data = Data - 32;
    else if (Data >= 0x61 && Data <= 0x7A) //a b..z
        Data = Data - 0X57;
    else //其它默认为空
        Data = 36;

    return (Data);
}

//将数字转成相应的ASCII码
unsigned char num2ascii(unsigned char Data)
{
    if (Data >= 36 && Data <= 51) //特别字符，请查ASC表
        Data = Data - 4;
    else if (Data <= 9) //1 2 3..9
        Data = Data + 0X30;
    else if (Data >= 52 && Data <= 58) //特别字符，请查ASC表
        Data = Data + 6;
    else if (Data >= 10 && Data <= 35) //A B..Z
        Data = Data + 0X37;
    else if (Data >= 36 && Data <= 51) //特别字符，请查ASC表
        Data = Data + 32;
    else //其它默认为空
        Data = 0X20;
    return (Data);
}

//0x30 0x30 0x32 0x30->0x0020
unsigned short get_data_addr(unsigned char *start_buf)
{
    unsigned char tmp_buf[4] = {0};
    unsigned short des_addr;

    for (unsigned char i = 0; i < 4; i++)
    {
        tmp_buf[i] = ascii2num(start_buf[i]); //0x31->0x01
    }

    des_addr = tmp_buf[0] * 0x1000 +
               tmp_buf[1] * 0x100 +
               tmp_buf[2] * 0x10 +
               tmp_buf[3];

    return des_addr;
}

//0x32 0x30->0x20
unsigned char get_data(unsigned char *start_buf)
{
    unsigned char tmp_buf[4] = {0};
    unsigned char des;

    for (unsigned char i = 0; i < 2; i++)
    {
        tmp_buf[i] = ascii2num(start_buf[i]); //0x31->0x01
    }

    des = tmp_buf[0] * 0x10 + tmp_buf[1];

    return des;
}

//0x32 0x30->0x20
unsigned char get_data_len(unsigned char *start_buf)
{
    unsigned char des;

    des = get_data(start_buf);

    return des;
}

// unsigned char get_check_code(unsigned char *p, unsigned char len)
// {
//     unsigned char tmp = 0;

//     for (unsigned char i = 0; i < len; i++)
//     {
//         tmp += p[i];
//     }
//     return tmp;
// }

void normal_ang_rcode(void)
{
    unsigned short addr = 0;
    unsigned char data_len = 0;
    unsigned char buf_32[ConMaxbyte];
    unsigned char buf_64[ConMaxbyte * 2];
    unsigned char answer_cmd_buf[128] = {0xFE, 0xFE, 0xEF, 0xEE, 0xE4};

    unsigned char answer_cmd_len = 0;
    unsigned char check_code_and_end_frame[3] = {0x41, 0x41, 0xFD};

    addr = get_data_addr(wc_data.rx_buf + 5);    //0x30 0x30 0x32 0x30 -> 0x0020
    data_len = get_data_len(wc_data.rx_buf + 9); //0x32 0x30 -> 0x20

    memset(buf_32, 0, sizeof(buf_32));
    memcpy(buf_32, toa_flash2.pocket + addr, data_len);

    //比如buf_32=0x49，buf_64=0x34 0x39
    for (unsigned char i = 0; i < data_len; i++)
    {
        buf_64[2 * i + 0] = num2ascii(buf_32[i] >> 4 & 0x0F);
        buf_64[2 * i + 1] = num2ascii(buf_32[i] & 0x0F);
    }

    memcpy(answer_cmd_buf + 5, wc_data.rx_buf + 5, 6);
    memcpy(answer_cmd_buf + 11, buf_64, data_len * 2); //data_len=0x20即32个，实际发送的是64个
    memcpy(answer_cmd_buf + 11 + data_len * 2, check_code_and_end_frame, 3);

    answer_cmd_len = 11 + data_len * 2 + 3;

    uart_writecode_send_data(answer_cmd_buf, answer_cmd_len);
}

void special_ang_rcode(void)
{
    unsigned short addr = 0;
    unsigned short GapAddr = 0;
    unsigned char data_len = 0;
    unsigned char buf_32[ConMaxbyte];
    unsigned char buf_64[ConMaxbyte * 2];
    unsigned char ucBuff[128];
    unsigned char answer_cmd_buf[128] = {0xFE, 0xFE, 0xEF, 0xEE, 0xE4};

    unsigned char answer_cmd_len = 0;
    unsigned char check_code_and_end_frame[3] = {0x41, 0x41, 0xFD};

    addr = get_data_addr(wc_data.rx_buf + 5);    //0x30 0x30 0x32 0x30 -> 0x0020
    data_len = get_data_len(wc_data.rx_buf + 9); //0x32 0x30 -> 0x20

    GapAddr = addr - ConTetBegAdd3;
    memcpy(ucBuff, toa_flash2.pocket + ConTetBegAdd3, 128);

    memset(buf_32, 0, sizeof(buf_32));
    memcpy(buf_32, &ucBuff[GapAddr], data_len);
    memcpy(buf_32, toa_flash2.pocket + addr, data_len);

    //比如buf_32=0x49，buf_64=0x34 0x39
    for (unsigned char i = 0; i < data_len; i++)
    {
        buf_64[2 * i + 0] = buf_32[i];
        buf_64[2 * i + 1] = buf_32[i];
    }

    memcpy(answer_cmd_buf + 5, wc_data.rx_buf + 5, 6);
    memcpy(answer_cmd_buf + 11, buf_64, data_len * 2); //data_len=0x20即32个，实际发送的是64个
    memcpy(answer_cmd_buf + 11 + data_len * 2, check_code_and_end_frame, 3);

    answer_cmd_len = 11 + data_len * 2 + 3;

    uart_writecode_send_data(answer_cmd_buf, answer_cmd_len);
}

/*
不加校验码+尾帧看看会不会出现异常
FE FE EE EF EB 31 36 44 30 32 30 FD 进入调试模式，申请读取0x16D0的32位数据
应该回复
FE FE EF EE E4 31 36 44 30 32 30 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46 46

*/
void ang_rcode_handle(void)
{
    // unsigned short addr = 0;

    // addr = get_data_addr(wc_data.rx_buf + 5); //0x30 0x30 0x32 0x30 -> 0x0020

    // if (addr >= ConTetBegAdd3 && addr < ConAdjTetBegAdd)
    // {
    //     special_ang_rcode();
    // }
    // else
    {
        normal_ang_rcode();
    }
}
/*
    Length: 78, Data: FE FE EE EF E4-30 30 30 30-32 30-30 34 34 35 39 36 30 32 30 34 34 35 39 36 30 32-46 46 30 46 46 46 30 46 30 30 30 30 30 36 32 30 32 30 32 30 32 30 32 30 32 30 41 34 43 42 39 37 30 32 41 34 43
    Length: 78, Data: FE FE EE EF E4-30 30 32 30-32 30-30 46 30 30 30 30 30 36 32 30 32 30 32 30 32 30-32 30 32 30 41 34 43 42 39 37 30 32 41 34 43 42 39 37 30 32 46 46 30 46 46 46 30 46 30 30 30 30 30 36 32 30 32
    0000200445960204459602FF0FFF0F000006202020202020A4CB9702A4C
命令+地址+个数+ 数据+较验+FD
命令 FE FE EE EF E4 
地址 30 30 30 30 = 0000
长度 32 30 = 0x20 = 32个
数据 
30 34 34 35 39 36 30 32 接收=04459602
30 34 34 35 39 36 30 32 发射=04459602
46 46 30 46 46 46 步进+带宽+功率=FF0FFF
30 46 30 30 30 30 30 36 32 30 +=0F000006
32 30 32 30 32 30 32 30 32 30 41 34 43 42 39 37 30 32 41 34 43 = 202020202020A4CB9702A4C

00:rx
04:tx
08:rx亚音
10:tx亚音
11:rx反向亚音0x40
11:tx反向亚音0x80
12:带宽0x30
12:功率0xC0
14:步进

04 45 96 02
04 45 96 02
FF 0F = rx亚音(Bit0～Bit3)
FF 0F = rx亚音(Bit0～Bit3) (反向亚音0x40:0b01000000 0x80:0b10000000)
00 = 带宽(0x30:0b00110000)+功率(0xC0:0b11000000)
00 
06 步进
202020202020A4CB9702A4C

*/
void ang_wcode_handle(void) //信道参数一个21个字节
{
    unsigned short buf_index = 0;
    unsigned char data_len = 0;

    buf_index = get_data_addr(wc_data.tx_buf + 5) / 0x20;
    data_len = get_data_len(wc_data.tx_buf + 9); //0x32 0x30 -> 0x20

    for (unsigned char i = 0; i < data_len; i++)
    {
        toa_flash2.pocket[buf_index * 32 + i] = get_data(wc_data.tx_buf + 11 + 2 * i);
    }

    answer_wrcode_end();
}

//old

INT16U realDataPro(INT16U Temp, INT16U ShiftData) //计算当前真正亚音值
{
    Temp = Temp & ShiftData;
    return Temp;
}

unsigned long Byte_X_ChgLongPro(unsigned char *Buffer, unsigned char ByteCt)
{
    unsigned char i;
    unsigned long FreTemp = 0;
    FreTemp = Buffer[ByteCt - 1];
    for (i = ByteCt - 1; i > 0; i--)
    {
        FreTemp = FreTemp << 8;
        FreTemp = FreTemp | (unsigned long)Buffer[i - 1];
    }
    return FreTemp;
}

void SetCtcDscInfPro(one_sub_audio *Ptemp1, INT8U *Data) //读取信道亚音频信息
{
    INT16U SubChFre;

    Ptemp1->sub_audio_mode = SUBAUDIO_NONE;
    Ptemp1->ctcss_freq = 0X03E8; //默认亚音频为100HZ
    Ptemp1->cdcss_freq = 0X0013; //默认亚音频为023N
    Ptemp1->cdcss_direction = 0;
    SubChFre = (INT32U)Byte_X_ChgLongPro(&Data[0], 2); //	D8+D9(Bit0～Bit3) 	当前接收亚音频
                                                       //	D10+D11(Bit0～Bit3) 当前发射亚音频
    SubChFre = realDataPro(SubChFre, 0X0FFF);
    if (SubChFre <= 0X0A28) //当前有亚音频
    {
        if (SubChFre <= 0X1FF) //当前为	DCS
        {
            Ptemp1->cdcss_freq = SubChFre;
            Ptemp1->sub_audio_mode = SUBAUDIO_CDCSS;
        }
        else //当前为	Ctcss
        {
            Ptemp1->ctcss_freq = SubChFre;
            Ptemp1->sub_audio_mode = SUBAUDIO_CTCSS;
        }
    }
}

unsigned char CalNowChannelNumMax(void)
{
    //11802003000000000000000000000000000000000000000000000000007249F308F141 2channel
    //11802007000000000000000000000000000000000000000000000000007249F308F141 3channel
    unsigned char uci, ucbit;
    unsigned int ulValue = 0;
    unsigned int ucReadIndex = 0;
    unsigned char GiChUseBuf[32]; //读当前有用信道
    unsigned char AnaChSumNum;    /// 总有效信道数目

    AnaChSumNum = 0;

    //read(ConMenEnBegAddr, 32, GiChUseBuf); //读当前有用信道
    //对应index=140
    memcpy(GiChUseBuf, toa_flash2.pocket + ConMenEnBegAddr, 32);
    for (uci = 0; uci < 200; uci++)
    {
        ucReadIndex = uci / 8;
        ucbit = (0x01 << (uci % 8));
        ulValue = GiChUseBuf[ucReadIndex];
        if (ulValue & ucbit)
        {
            AnaChSumNum++;
        }
        else
        {
        }
    }
    return AnaChSumNum;
}