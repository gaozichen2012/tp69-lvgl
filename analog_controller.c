#define ANALOG_C
#include "ql_rtos.h"
#include "analog_controller.h"
#include "analog_view.h"
#include "state.h"
#include "general_include.h"
#include "bk_include.h"
#include "analog_model.h"
#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "bk4819.h"
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

extern ql_queue_t msqid;

void analogc_push_msg(enAnalogValue state)
{
    ZPOC_MSG_TYPE iMsg;
    iMsg.enMsgType = ZPOC_MSG_ANALOG;
    iMsg.pMsg = NULL;
    iMsg.usMsgLen = state;

    ql_rtos_queue_release(msqid, sizeof(iMsg), (unsigned char *)&iMsg, QL_WAIT_FOREVER);
}

static unsigned char get_rx_state(void)
{
    static unsigned char sq_buf[9] = {140, 130, 120, 110,
                                      100, 90, 80, 70, 0};
    static unsigned char sq_edge = 0;

    static unsigned short rssi_vol = 0;

    rssi_vol = RF_GetRssi();

    //rssi_level赋值，用于disp
    if (rssi_vol > 120)
        toa.rssi_level = 4;
    else if (rssi_vol > 100)
        toa.rssi_level = 3;
    else if (rssi_vol > 90)
        toa.rssi_level = 2;
    else if (rssi_vol > 80)
        toa.rssi_level = 1;
    else
        toa.rssi_level = 0;

    //计算sq
    if (rssi_vol <= 5)
    {
        //无检测
    }
    else if (rssi_vol <= sq_buf[ang_seting_flash.toa_flash.sq_level % 9]) //0x50
    {
        sq_edge = 0;
    }
    else
    {
        sq_edge = 1;
    }

    _DEBUG("rssi_vol=%d sq_level=%d sq_buf=%d sq_edge=%d\r\n", rssi_vol, ang_seting_flash.toa_flash.sq_level, sq_buf[ang_seting_flash.toa_flash.sq_level % 9], sq_edge);

    return sq_edge;
}

void toa_process(void)
{
    rx_ctcss_and_cdcss_handle();
    tx_power_handle();
    channel_spacing_handle();
    //bk4819_test();
    ang_all_time();
}

void toa_1s_process(void)
{
    static unsigned char cnt = 0;
    _DEBUG("toa 1s timer! tx_flag=%d-rx_flag=%d-sq_state=%d-toa.bk_rxon_state=%d\r\n", toa.tx_flag, toa.rx_flag, toa.sq_state, toa.bk_rxon_state);
    if (toa.tx_flag == 0 && toa.rx_flag == RX_IDLE)
    {
        cnt++;
        if (cnt == 10)
        {
            cnt = 11;
            toa.sleep_flag = 1;
            _DEBUG("enter apwer save mode!\r\n");
        }
    }
    else
    {
        cnt = 0;
        if (toa.sleep_flag != 0)
        {
            toa.sleep_flag = 0;
            _DEBUG("exit apwer save mode!\r\n");
        }
    }
}

u8 look_up_table(u16 value, u16 *item, u8 max_item_num)
{
    u8 i = 0;

    for (i = 0; i < max_item_num; i++)
    {
        if (value == item[i])
        {
            return i;
        }
    }
    return 0;
}

//dcs转换后写入bk4819
//未研究其数据转换逻辑，直接套用bk4818代码
unsigned int CalDcs23BitData(unsigned short DcsCode, unsigned char InvertEn)
{
    unsigned short DcsTmpCode, DcsTmpCode1;
    unsigned int DcsTmpCode2;
    //9 bits + 100 ==> 12 bits
    DcsTmpCode1 = DcsCode & 0x01FF;     //179&0x1FF=179=0xB3
    DcsTmpCode1 = DcsTmpCode1 | 0x0800; //179|0x800=0x8B3

    //Normal or Invert DCS
    DcsCode = DcsCode & 0x6000; //=0
    if (InvertEn == 0)          //注意此格式为当为反向时为1
        DcsTmpCode1 = (~DcsTmpCode1) & 0x0fff;
    DcsTmpCode = DcsTmpCode1; //0x8B3
    for (unsigned char i = 12; i > 0; i--)
    {
        if (DcsTmpCode & 0x0001)
        {
            DcsTmpCode = DcsTmpCode >> 1;
            DcsTmpCode = DcsTmpCode | 0x8000;
            DcsTmpCode ^= 0x863A;
        }
        else
            DcsTmpCode = DcsTmpCode >> 1;
    }
    DcsTmpCode2 = (long)DcsTmpCode;
    DcsTmpCode2 = DcsTmpCode2 << 12;
    DcsTmpCode2 = DcsTmpCode2 | (long)(DcsTmpCode1);

    return DcsTmpCode2;
}

void rx_ctcss_and_cdcss_handle(void)
{
    static one_sub_audio sub_old;

    unsigned int dcs_des = 0;
    if (sub_old.sub_audio_mode != ana_all.cur_channel.rx_sub_audio.sub_audio_mode ||
        sub_old.cdcss_direction != ana_all.cur_channel.rx_sub_audio.cdcss_direction ||
        sub_old.ctcss_freq != ana_all.cur_channel.rx_sub_audio.ctcss_freq ||
        sub_old.cdcss_freq != ana_all.cur_channel.rx_sub_audio.cdcss_freq)
    {
        sub_old = ana_all.cur_channel.rx_sub_audio;

        switch (ana_all.cur_channel.rx_sub_audio.sub_audio_mode)
        {
        case SUBAUDIO_NONE: //off
            RF_ExitSubau();
            break;
        case SUBAUDIO_CTCSS: //ctcss
            RF_SetCtcss((unsigned short)(sub_old.ctcss_freq / 10 * 20.64888));
            _DEBUG("ctc1=0x%04X!\r\n", (unsigned short)(sub_old.ctcss_freq / 10 * 20.64888));
            break;
        case SUBAUDIO_CDCSS: //cdcss
            if (ana_all.mr_channel_index == 1 && ang_seting_flash.toa_flash.freqscan_flag == 1)
            {
                dcs_des = sub_old.cdcss_freq;
                _DEBUG("freqscan_flag=1!\r\n");
            }
            else
            {
                if (sub_old.cdcss_direction == 1)
                    dcs_des = CalDcs23BitData(sub_old.cdcss_freq, 0);
                else
                    dcs_des = CalDcs23BitData(sub_old.cdcss_freq, 1);
            }

            _DEBUG("dcs=0x%X!\r\n", dcs_des);
            bk4819_set_cdcss(dcs_des);
            break;
        default:
            RF_ExitSubau();
            break;
        }
    }
}

void tx_ctcss_and_cdcss_set(void)
{
    unsigned int dcs_des = 0;
    switch (ana_all.cur_channel.tx_sub_audio.sub_audio_mode)
    {
    case SUBAUDIO_NONE: //off
        RF_ExitSubau();
        break;
    case SUBAUDIO_CTCSS: //ctcss
        RF_SetCtcss((unsigned short)(ana_all.cur_channel.tx_sub_audio.ctcss_freq / 10 * 20.64888));
        _DEBUG("ctc1=0x%04X!\r\n", (unsigned short)(ana_all.cur_channel.tx_sub_audio.ctcss_freq / 10 * 20.64888));
        break;
    case SUBAUDIO_CDCSS: //cdcss
        if (ana_all.mr_channel_index == 1 && ang_seting_flash.toa_flash.freqscan_flag == 1)
        {
            dcs_des = ana_all.cur_channel.tx_sub_audio.cdcss_freq;
            _DEBUG("freqscan_flag=1!\r\n");
        }
        else
        {
            if (ana_all.cur_channel.tx_sub_audio.cdcss_direction == 1)
                dcs_des = CalDcs23BitData(ana_all.cur_channel.tx_sub_audio.cdcss_freq, 0);
            else
                dcs_des = CalDcs23BitData(ana_all.cur_channel.tx_sub_audio.cdcss_freq, 1);
        }

        _DEBUG("dcs=0x%X!\r\n", dcs_des);
        bk4819_set_cdcss(dcs_des);
        break;
    default:
        RF_ExitSubau();
        break;
    }
}

void tx_ctcss_and_cdcss_reset(void)
{
    unsigned int dcs_des = 0;
    switch (ana_all.cur_channel.rx_sub_audio.sub_audio_mode)
    {
    case SUBAUDIO_NONE: //off
        RF_ExitSubau();
        break;
    case SUBAUDIO_CTCSS: //ctcss
        RF_SetCtcss((unsigned short)(ana_all.cur_channel.rx_sub_audio.ctcss_freq / 10 * 20.64888));
        _DEBUG("ctc1=0x%04X!\r\n", (unsigned short)(ana_all.cur_channel.rx_sub_audio.ctcss_freq / 10 * 20.64888));
        break;
    case SUBAUDIO_CDCSS: //cdcss
        if (ana_all.mr_channel_index == 1 && ang_seting_flash.toa_flash.freqscan_flag == 1)
        {
            dcs_des = ana_all.cur_channel.rx_sub_audio.cdcss_freq;
            _DEBUG("freqscan_flag=1!\r\n");
        }
        else
        {
            if (ana_all.cur_channel.rx_sub_audio.cdcss_direction == 1)
                dcs_des = CalDcs23BitData(ana_all.cur_channel.rx_sub_audio.cdcss_freq, 0);
            else
                dcs_des = CalDcs23BitData(ana_all.cur_channel.rx_sub_audio.cdcss_freq, 1);
        }
        _DEBUG("dcs=0x%X!\r\n", dcs_des);
        bk4819_set_cdcss(dcs_des);
        break;
    default:
        RF_ExitSubau();
        break;
    }
}

//开机或每次修改功率时设置一次发射功率
void tx_power_handle(void)
{
    static unsigned char tx_power_old = 0xff;

    if (tx_power_old != ana_all.cur_channel.tx_power)
    {
        tx_power_old = ana_all.cur_channel.tx_power;
        bk4819_set_tx_power(ana_all.cur_channel.tx_power);
    }
}

//开机或每次修改带宽时设置一次带宽
void channel_spacing_handle(void)
{
    static unsigned char channel_spacing_old = 0xff;

    if (channel_spacing_old != ana_all.cur_channel.channel_spacing)
    {
        channel_spacing_old = ana_all.cur_channel.channel_spacing;
        bk4819_set_channel_spacing(ana_all.cur_channel.channel_spacing);
    }
}

void ang_all_time(void)
{
    if (toa.tx_flag == 1 && toa.rx_flag == RX_IDLE)
    {
        //tx state
    }
    else if (toa.tx_flag == 0 && toa.rx_flag == RX_REV)
    {
        //rx state
    }
    else if (toa.tx_flag == 0 && toa.rx_flag != RX_REV)
    {
        //idle state
    }
    else
    {
        /* code */
    }

    //alltime_detect_tail();
    alltime_remove_tail_audio(ana_all.cur_channel.rx_sub_audio.sub_audio_mode);
    if (toa.tx_flag == 0 && toa.bk_rxon_state == 1)
    {
        //在空闲时的间歇性检测sq和亚音
        toa.rx_subaudio_match = alltime_detect_subaudio_match(ana_all.cur_channel.rx_sub_audio.sub_audio_mode, ana_all.cur_channel.rx_sub_audio.cdcss_direction);
        alltime_detect_sq();
        sq_and_subaudio_match_handle();
    }
}

void tx_start_moment(void)
{
    //按下PTT到tx_flag置1
    //...
    toa.tx_flag = 1;
}
void tx_end_moment(void)
{
    //松开ptt到tx_flag置0
    toa.tx_flag = 0;
}

void rx_start_moment(void)
{
    //检测到sq或亚音，rx_flag置0或置1的时间段
}

void rx_end_moment(void)
{
    //检测到sq或亚音丢失，rx_flag置0的时间段
    //...
}

//处于接收状态时，检测CTC尾音，如果是CTC2直接关喇叭
void alltime_remove_tail_audio(unsigned char sub_audio_mode)
{
    static u8 tail_old = 0;
    u8 tail = 0xff;

    if (toa.rx_flag == RX_REV)
    {
        tail = RF_GetCtcss();
        if (tail_old != tail)
        {
            tail_old = tail;

            switch (sub_audio_mode)
            {
            case SUBAUDIO_NONE:
            case SUBAUDIO_CTCSS:
                if (tail == 2) //2 表示收到CTC2（如 55Hz 尾音）
                {
                    pa_output_control(1, 0);
                    _DEBUG("CLOSE PA ctc2 matched=2!\r\n");
                }
                else if (tail == 3) //2 表示收到CTC2（如 55Hz 尾音）
                {
                    pa_output_control(1, 0);
                    _DEBUG("CLOSE PA ctc2 matched=3!\r\n");
                }
                else
                {
                    /* code */
                }
                break;
            case SUBAUDIO_CDCSS:
#if 1
                //数字亚音尾部为134.4Hz
                //实测接收开始tail=0;接收结束瞬间tail=1;
                _DEBUG("remove_tail tail=%d\r\n", tail);
                if (tail == 0)
                {
                    //RX开始
                }
                else if (tail == 1)
                {
                    //RX结束瞬间
                    pa_output_control(1, 0);
                }
                else
                {
                    /* code */
                }
#else
                // 此处有问题，开启数字亚音，用SHM发射会出现喇叭无声，用79测试正常
                //开了数字亚音，发的尾音时134.4Hz,所以收到CTC1也应该关闭喇叭
                pa_output_control(1, 0);
                _DEBUG("CLOSE PA ctc matched=4!\r\n");
#endif
                break;
            default:
                break;
            }
        }
    }
}

//在空闲时的间歇性检测sq和亚音
//输入：sub_audio_mode当前接收亚音模式 cdcss_direction当前数字亚音方向
//输出：亚音匹配结果 1：匹配可以打开喇叭 2：不匹配
unsigned char alltime_detect_subaudio_match(unsigned char sub_audio_mode, unsigned char cdcss_direction)
{
    static unsigned char cdcss_test = 0xff;
    static unsigned char ctcss_test = 0xff;
    unsigned char tmp_match = 0;

    if (toa.bk_rxon_state == 1)
    {
        switch (sub_audio_mode)
        {
        case SUBAUDIO_NONE:
            //只检测sq
            tmp_match = 1;
            break;
        case SUBAUDIO_CTCSS:
            //检测sq+模拟亚音
            toa.ctc_state = RF_GetCtcss();
            if (ctcss_test != toa.ctc_state)
            {
                ctcss_test = toa.ctc_state;
                _DEBUG("ctcss=0x%X\r\n", ctcss_test);
            }

            if (toa.ctc_state == 1) //1 表示收到 CTC1（主 CTC）
            {
                tmp_match = 1;
                //_DEBUG("ctc1 matched!\r\n");
            }
            else if (toa.ctc_state == 2) //2 表示收到CTC2（如 55Hz 尾音）
            {
                tmp_match = 0;
            }
            else if (toa.ctc_state == 3) //2 表示收到CTC2（如 55Hz 尾音）
            {
                tmp_match = 0;
            }
            else
            {
                /* code */
            }
            break;
        case SUBAUDIO_CDCSS: //检测sq+数字亚音
            toa.dcs_state = RF_GetCdcss();
            if (cdcss_test != toa.dcs_state)
            {
                cdcss_test = toa.dcs_state;
                _DEBUG("cdcss=0x%X\r\n", cdcss_test);
            }

            if (toa.dcs_state == 1) //1表示收到CDC正码
            {
                if (cdcss_direction == 0)
                {
                    tmp_match = 1;
                    //_DEBUG("dcs matched! direction=0\r\n");
                }
                else
                {
                    tmp_match = 1;
                    //_DEBUG("dcs matched! direction=1\r\n");
                }
            }
            else if (toa.dcs_state == 2) //2表示收到CDC反码
            {
                if (cdcss_direction == 0)
                {
                    //_DEBUG("dcs not matched 0!\r\n");
                }
                else
                {
                    //_DEBUG("dcs not matched 1!\r\n");
                }
            }
            else
            {
                /* code */
            }
            break;
        default:
            break;
        }
    }
    return tmp_match;
}

void alltime_detect_sq(void)
{
    static unsigned char sq_state_old = 0;

    if (toa.bk_rxon_state == 1)
    {
        toa.sq_state = get_rx_state();
        if (sq_state_old != toa.sq_state)
        {
            sq_state_old = toa.sq_state;

            //sq出现时打开RF_Rxon，消失时RF_Idle
            if (sq_state_old == 1)
            {
                RF_Rxon();
                toa.bk_rxon_state = 1;
                toa.rx_flag = RX_ONLYSQ;
            }
        }
    }
}

void alltime_detect_tail(void)
{
    static unsigned char tail_state_old = 0;

    toa.tail_state = RF_GetTail();
    if (tail_state_old != toa.tail_state)
    {
        tail_state_old = toa.tail_state;
        _DEBUG("tail_state=0x%X\r\n", tail_state_old);
    }
}

void freqmatch_freq_scan_handle(void)
{
    unsigned int freq_get = 0xffff;
    static unsigned int freq_get_old = 0xffff;

    static unsigned char freq_scan_result = 0;
    unsigned char subaudio_type = 0;

    //step1 当进入扫描模式且未找到频率则RF_FreqScan()
    if (toa.freq_scan_flg == 1 && freq_scan_result == 0)
    {
        toa.sleep_flag = 0;
        RF_FreqScan();
        freq_get = (FRQ_HI16 << 16) + FRQ_LO16;

        if (freq_get > 40000000)
        {
            if (freq_get_old != freq_get)
            {
                freq_get_old = freq_get;
            }
            else
            {
                //扫描2次结果一样，则判断生效
                freq_scan_result = 1;

                //set current freq
                bk4819_set_freq(freq_get);
                ana_all.freqscan_channel.rx_freq = freq_get;
                ana_all.freqscan_channel.tx_freq = freq_get;

                _DEBUG("==== ANALOG_FREQSCAN_FREQ_SUCCESS! freq_get=%d\r\n", freq_get);
                analogc_push_msg(ANALOG_FREQSCAN_FREQ_SUCCESS);
            }
        }
    }

    if (freq_scan_result == 1)
    {
        subaudio_type = RF_CtcDcsScan();
        if (subaudio_type == 1) //CTC
        {
            toa.freq_scan_flg = 0;
            freq_scan_result = 0;

            ana_all.freqscan_channel.rx_sub_audio.sub_audio_mode = SUBAUDIO_CTCSS;
            ana_all.freqscan_channel.tx_sub_audio.sub_audio_mode = SUBAUDIO_CTCSS;

            ana_all.freqscan_channel.rx_sub_audio.ctcss_freq = CTC_FREQ * 10 / 20.64888; //CTC_FREQ=2064 ctc=999.5699=1000=100.0HZ
            ana_all.freqscan_channel.tx_sub_audio.ctcss_freq = CTC_FREQ * 10 / 20.64888; //CTC_FREQ=2064 ctc=999.5699=1000=100.0HZ

            _DEBUG("==== ANALOG_FREQSCAN_SUBAUDIO_CTC! CTC=%d\r\n", CTC_FREQ);
            analogc_push_msg(ANALOG_FREQSCAN_SUBAUDIO_CTC);
        }
        else if (subaudio_type == 2) //CDC
        {
            toa.freq_scan_flg = 0;
            freq_scan_result = 0;

            ana_all.freqscan_channel.rx_sub_audio.sub_audio_mode = SUBAUDIO_CDCSS;
            ana_all.freqscan_channel.tx_sub_audio.sub_audio_mode = SUBAUDIO_CDCSS;
            //读取的DCS_HI12+DCS_LO12为寄存器的值，需要换算比较复杂，参考S198，对于此类值在写频软件上直接显示“加密Encrypted”
            ana_all.freqscan_channel.rx_sub_audio.cdcss_freq = (DCS_HI12 << 12) + DCS_LO12; //DCS_HI12=0x459 DCS_LO12=0xDE8  cdc= 459DE8  179 D263N
            ana_all.freqscan_channel.tx_sub_audio.cdcss_freq = (DCS_HI12 << 12) + DCS_LO12; //DCS_HI12=0x459 DCS_LO12=0xDE8  cdc= 459DE8  179 D263N
            ang_seting_flash.toa_flash.freqscan_cdcss_freq = (DCS_HI12 << 12) + DCS_LO12;   //DCS_HI12=0x459 DCS_LO12=0xDE8  cdc= 459DE8  179 D263N
            _DEBUG("==== ANALOG_FREQSCAN_SUBAUDIO_CDC! DCS_HI12=0x%X DCS_LO12=0x%X\r\n", DCS_HI12, DCS_LO12);
            analogc_push_msg(ANALOG_FREQSCAN_SUBAUDIO_CDC);
        }
        else
        {
            //如果连续获取几次未获取到则是无亚音
            toa.freq_scan_flg = 0;
            freq_scan_result = 0;

            ana_all.freqscan_channel.rx_sub_audio.sub_audio_mode = SUBAUDIO_NONE;
            ana_all.freqscan_channel.tx_sub_audio.sub_audio_mode = SUBAUDIO_NONE;
            ana_all.freqscan_channel.rx_sub_audio.ctcss_freq = 0;
            ana_all.freqscan_channel.tx_sub_audio.ctcss_freq = 0;
            ana_all.freqscan_channel.rx_sub_audio.cdcss_freq = 0;
            ana_all.freqscan_channel.tx_sub_audio.cdcss_freq = 0;
            _DEBUG("==== ANALOG_FREQSCAN_SUBAUDIO_IDLE! DCS_HI12=0x%X DCS_LO12=0x%X\r\n", DCS_HI12, DCS_LO12);
            analogc_push_msg(ANALOG_FREQSCAN_SUBAUDIO_IDLE);
        }
    }
}

//"0x3231,0x3433"->"0x31,0x32,0x33,0x34"
void fsk_data_conversion(unsigned char *dst_buf, unsigned short *src_buf, unsigned char src_len)
{
    for (unsigned char i = 0; i < src_len; i++)
    {
        dst_buf[2 * i] = src_buf[i] & 0x00FF;
        dst_buf[2 * i + 1] = (src_buf[i] & 0xFF00) >> 8;
    }
}

void fsk_handle(void)
{
    static unsigned char fsk_flg_old = 0xff;
    unsigned char result = 0;
    unsigned char fsk_rx_buf[FSK_LEN * 2];
    toa.sleep_flag = 0;

    if (fsk_flg_old != toa.fsk_flg)
    {
        fsk_flg_old = toa.fsk_flg;

        switch (fsk_flg_old)
        {
        case 0:
            RF_ExitFsk();
            break;
        case 1:
            RF_EnterFsk();
            toa.fsk_flg = 2;
            break;
        case 2:
            //result = RF_FskTransmit();
            break;
        case 3:
            //result = RF_FskReceive();
            break;
        default:
            break;
        }
    }

    if (toa.fsk_flg == 2) //rx fsk
    {
        result = RF_FskReceive();
        _DEBUG("RF_FskReceive result=%d\r\n", result);
        _DEBUG("BUF1=0x%X\r\nBUF2=0x%X\r\nBUF3=0x%X\r\nBUF4=0x%X\r\n", FSK_RXDATA[0], FSK_RXDATA[1], FSK_RXDATA[2], FSK_RXDATA[3]);
        _DEBUG("BUF5=0x%X\r\nBUF6=0x%X\r\nBUF7=0x%X\r\nBUF8=0x%X\r\n", FSK_RXDATA[4], FSK_RXDATA[5], FSK_RXDATA[6], FSK_RXDATA[7]);
        _DEBUG("BUF9=0x%X\r\nBUF10=0x%X\r\nBUF11=0x%X\r\nBUF12=0x%X\r\n", FSK_RXDATA[8], FSK_RXDATA[9], FSK_RXDATA[10], FSK_RXDATA[11]);
        _DEBUG("BUF13=0x%X\r\nBUF14=0x%X\r\nBUF15=0x%X\r\nBUF16=0x%X\r\n", FSK_RXDATA[12], FSK_RXDATA[13], FSK_RXDATA[14], FSK_RXDATA[15]);

        if (result) //recevie data
        {
            memset(fsk_rx_buf, 0, sizeof(fsk_rx_buf));
            fsk_data_conversion(fsk_rx_buf, FSK_RXDATA, FSK_LEN);
            writecode_rx_handle_for_fsk(fsk_rx_buf, FSK_LEN * 2);
            _DEBUG("fsk_data1=0x%X\r\nfsk_data2=0x%X\r\nfsk_data3=0x%X\r\nfsk_data4=0x%X\r\n", fsk_rx_buf[0], fsk_rx_buf[1], fsk_rx_buf[2], fsk_rx_buf[3]);
            _DEBUG("fsk_data5=0x%X\r\nfsk_data6=0x%X\r\nfsk_data7=0x%X\r\nfsk_data8=0x%X\r\n", fsk_rx_buf[4], fsk_rx_buf[5], fsk_rx_buf[6], fsk_rx_buf[7]);
        }
    }

    if (toa.fsk_flg == 3) //tx fsk
    {
        memcpy(FSK_TXDATA, "1234567890", 10);
        result = RF_FskTransmit();
        _DEBUG("RF_FskTransmit result=%d\r\n", result);
        _DEBUG("BUF1=0x%X\r\nBUF2=0x%X\r\nBUF3=0x%X\r\nBUF4=0x%X\r\n", FSK_TXDATA[0], FSK_TXDATA[1], FSK_TXDATA[2], FSK_TXDATA[3]);
        _DEBUG("BUF5=0x%X\r\nBUF6=0x%X\r\nBUF7=0x%X\r\nBUF8=0x%X\r\n", FSK_TXDATA[4], FSK_TXDATA[5], FSK_TXDATA[6], FSK_TXDATA[7]);
        _DEBUG("BUF9=0x%X\r\nBUF10=0x%X\r\nBUF11=0x%X\r\nBUF12=0x%X\r\n", FSK_TXDATA[8], FSK_TXDATA[9], FSK_TXDATA[10], FSK_TXDATA[11]);
        _DEBUG("BUF13=0x%X\r\nBUF14=0x%X\r\nBUF15=0x%X\r\nBUF16=0x%X\r\n", FSK_TXDATA[12], FSK_TXDATA[13], FSK_TXDATA[14], FSK_TXDATA[15]);
    }
}

void sq_and_subaudio_match_handle(void)
{
    if (toa.sq_state == 1)
    {
        if (toa.rx_subaudio_match == 1)
        {
            if (toa.rx_flag != RX_REV)
            {
                //检测到sq和亚音匹配
                //when SQ and CTC/DCS is link
                toa.rx_flag = RX_REV;
                pa_output_control(1, 1);
                _DEBUG("==== ANALOG RX START!\r\n");
                analogc_push_msg(ANALOG_RX_START);
            }
        }
        else
        {
            //检测到sq，亚音不匹配或未检测状态
        }
    }
    else
    {
        if (toa.rx_flag != RX_IDLE)
        {
            //when SQ or CTC/DCS is lost or CTC/DCS Tail is found
            toa.rx_flag = RX_IDLE;
            _DEBUG("==== ANALOG RX END!\r\n");
            pa_output_control(1, 0);
            RF_Rxon();
            toa.bk_rxon_state = 1;
            analogc_push_msg(ANALOG_RX_END);
        }
    }
}

/*
亚音关闭时，发射尾音=接收尾音时，无噪音，否则都有噪音
*/
void release_analog_ptt_handle(void)
{
    Rd1846NoCtcssTxTailPro();

    //延时300ms
    ql_rtos_task_sleep_ms(300);

    //恢复接收状态
    _DEBUG("ptt2_release_handle\r\n");
    RF_Rxon();
    toa.bk_rxon_state = 1;
    bk4819_vtx_gpio1_output(0); //发射时高电平、松开时低电平
    tx_ctcss_and_cdcss_reset();
    gpio_set_mic_sw(0); //0:g 1:m
    toa.tx_flag = 0;
}

#if 0 //修改delayms逻辑后尝试重新优化亚音及尾音逻辑
void Rd1846NoCtcssTxTailPro(void)
{
    _DEBUG("send tx tail! tail_audio_type=%d\r\n", ang_seting_flash.toa_flash.tail_audio_type);

    if (ana_all.cur_channel.tx_sub_audio.sub_audio_mode == 2)
    {
        //设置了数字亚音，松开ptt发134.4Hz固定亚音
        RF_GenTail(CTC134);
    }
    else
    {
        //未设置亚音或设置了模拟亚音时，松开ptt发尾音==亚音（消尾音）
        switch (ang_seting_flash.toa_flash.tail_audio_type)
        {
        case 0: //55.0Hz
            RF_GenTail(CTC55);
            break;
        case 1: //259.3Hz
            RF_GenTail(CTC259);
            break;
        case 2: //45.5Hz
            RF_GenTail(CTC45);
            break;
        default:
            break;
        }
    }
}
#else
void Rd1846NoCtcssTxTailPro(void)
{
    //发尾音的函数好像没用，尝试按马工思路直接用亚音发，测试ok
    //RF_GenTail(CTC55);

    _DEBUG("send tx tail! tail_audio_type=%d\r\n", ang_seting_flash.toa_flash.tail_audio_type);

    if (ana_all.cur_channel.tx_sub_audio.sub_audio_mode == 2)
    {
        //设置了数字亚音，松开ptt发134.4Hz固定亚音
        RF_SetCtcss((unsigned short)(1344 / 10 * 20.64888));
    }
    else
    {
        //未设置亚音或设置了模拟亚音时，松开ptt发尾音==亚音（消尾音）
        switch (ang_seting_flash.toa_flash.tail_audio_type)
        {
        case 0: //55.0Hz
            RF_SetCtcss((unsigned short)(550 / 10 * 20.64888));
            break;
        case 1: //259.3Hz
            RF_SetCtcss((unsigned short)(2593 / 10 * 20.64888));
            break;
        case 2: //45.5Hz
            RF_SetCtcss((unsigned short)(45.5 / 10 * 20.64888));
            break;
        default:
            break;
        }
    }
}
#endif

void RD1846RxModPro(void)
{
}

//在没有配置CTCSS时候，PA打开后，接收尾音== 亚音
void Rd1846NoCtcssRxTailPro(void)
{
}