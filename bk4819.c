#define BK4819
#include "bk4819.h"
#include "bk_include.h"
#include "ql_rtos.h"
#include "ql_gpio.h"
#include "gpio.h"
#include "analog_model.h"
#include "analog_controller.h"
#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

u8 RF_read_CTCSS_CDCSS_Tail_Found(void)
{
#if 1
    u16 rdata;

    RF_Write(0x02, 0x0000); //clear int

    rdata = RF_Read(0x02);

    return (rdata >> 10) & 1;
#else
    u16 rdata;

    RF_Write(0x02, 0x0000); //clear int

    rdata = RF_Read(0x02);

    return (rdata >> 10) & 1;
#endif
}

u8 RF_read_interrupt_state(void)
{
    u16 rdata;

    rdata = RF_Read(0x02);
    return (rdata >> 0) & 1;
}

u8 RF_read_sq_found_and_lost(void)
{
    u16 rdata;

    RF_Write(0x02, 0x0000); //clear int

    rdata = RF_Read(0x02);
    return (rdata >> 2) & 3;
}

u8 RF_read_reg34_gpio5_flag(void)
{
    u16 rdata;

    rdata = RF_Read(0x34);
    return (rdata >> 8) & 7;
}

u8 RF_read_reg3f_flag(void)
{
    u16 rdata;

    rdata = RF_Read(0x3F);
    return (rdata >> 2) & 3;
}

u16 RF_read_sq_1_gain(void)
{
    u16 rdata;

    rdata = RF_Read(0x78);
    return (rdata >> 8) & 0xFF;
}

u16 RF_read_sq_0_gain(void)
{
    u16 rdata;

    rdata = RF_Read(0x78);
    return (rdata >> 0) & 0xFF;
}

void bk4819_test(void)
{
#if 1 //读取CTCCDCTail
    static u8 tail_old = 0;
    static u8 ctcss_old = 0;
    u8 tail = 0xff;
    u8 ctcss = 0xff;

    ctcss = RF_GetCtcss();
    tail = RF_read_CTCSS_CDCSS_Tail_Found();

    if (ctcss_old != ctcss || tail_old != tail)
    {
        ctcss_old = ctcss;
        tail_old = tail;
        _DEBUG("tom:ctcss=0x%X\r\ntail=0x%X\r\n", ctcss, tail);
    }

#endif
#if 0 //测试ctcss cdcss

    static u8 ctcss_old = 0;
    static u8 cdcss_old = 0;
    static u8 tail_old = 0;

    u8 ctcss = 0;
    u8 cdcss = 0;
    u8 tail = 0;

    ctcss = RF_GetCtcss();
    cdcss = RF_GetCdcss();
    tail = RF_GetTail();
    if (ctcss_old != ctcss ||
        cdcss_old != cdcss ||
        tail_old != tail)
    {
        ctcss_old = ctcss;
        cdcss_old = cdcss;
        tail_old = tail;
        _DEBUG("tom:ctcss=0x%X\r\ncdcss=0x%X\r\ntail=0x%X\r\n", ctcss, cdcss, tail);
    }

#endif
#if 0 //测试rssi和sq
    u16 Rssi = 0;
    u8 Sq = 0;

    Rssi = RF_GetRssi();
    Sq = RF_GetSq();

    _DEBUG("Rssi=0x%X\r\nSq=0x%X\r\n", Rssi, Sq);
#endif
#if 0
    u8 interrupt = 0;
    u16 Rssi = 0;
    u8 Sq = 0;
    u8 sq_found_and_lost = 0;
    u8 reg34_flag = 0;
    u8 reg3f_flag = 0;
    u16 sq_1_gain = 0;
    u16 sq_0_gain = 0;
    u8 GpioIn5 = 0;

    interrupt = RF_read_interrupt_state();
    Rssi = RF_GetRssi();
    Sq = RF_GetSq();
    sq_found_and_lost = RF_read_sq_found_and_lost();
    reg34_flag = RF_read_reg34_gpio5_flag();
    reg3f_flag = RF_read_reg3f_flag();
    sq_1_gain = RF_read_sq_1_gain();
    sq_0_gain = RF_read_sq_0_gain();
    GpioIn5 = RF_GetGpioIn(5);
    _DEBUG("\r\ninterrupt=0x%X\r\nRssi=0x%X\r\nSq=0x%X\r\nsq_found_and_lost=0x%X\r\nreg34_flag=0x%X\r\nreg3f_flag=0x%X\r\nsq_1_gain=0x%X\r\nsq_0_gain=0x%X\r\nGpioIn5=0x%X\r\n",
           interrupt, Rssi, Sq, sq_found_and_lost, reg34_flag, reg3f_flag, sq_1_gain, sq_0_gain, GpioIn5);
    if (interrupt == 1)
    {
        RF_Write(0x02, 0x0000); //clear int
    }

#endif
#if 0
    u16 VoxAmp = 0;
    
    u8 Sq = 0;
    u8 Ctcss = 0;
    u8 Tail = 0;
    u8 Cdcss = 0;
    u8 Vox = 0;
    u8 Noise = 0;
    u8 Glitch = 0;
    u8 GpioIn7 = 0;
    u8 GpioIn6 = 0;
    
    u8 GpioIn4 = 0;
    u8 GpioIn0 = 0;

    VoxAmp = RF_GetVoxAmp();
    Rssi = RF_GetRssi();
    Sq = RF_GetSq();
    Ctcss = RF_GetCtcss();
    Tail = RF_GetTail();
    Cdcss = RF_GetCdcss();
    Vox = RF_GetVox();
    Noise = RF_GetNoise();
    Glitch = RF_GetGlitch();
    GpioIn7 = RF_GetGpioIn(7);
    GpioIn6 = RF_GetGpioIn(6);
    
    GpioIn4 = RF_GetGpioIn(4);
    GpioIn0 = RF_GetGpioIn(0);
    _DEBUG("VoxAmp=0x%X-Rssi=0x%X-Sq=0x%X-Ctcss=0x%X-Tail=0x%X-Cdcss=0x%X-Vox=0x%X-Noise=0x%X-Glitch=0x%X\r\n",
           VoxAmp, Rssi, Sq, Ctcss, Tail, Cdcss, Vox, Noise, Glitch);
    _DEBUG("GpioIn0=0x%X-GpioIn4=0x%X-GpioIn5=0x%X-GpioIn6=0x%X-GpioIn7=0x%X\r\n",
           GpioIn0, GpioIn4, GpioIn5, GpioIn6, GpioIn7);

#endif
}

void bk4819_init(void)
{
    //todo read analog flash...

    gpio_set_bk_power_en(PIN_LEVEL_HIGH);
    gpio_set_rp_en(PIN_LEVEL_HIGH);
    ql_rtos_task_sleep_ms(1000);

    //模拟init
    RF_Initial();
    //RF_SetFreq((43690000 >> 16) & 0xFFFF, 43690000 & 0xFFFF);
#if 1
    //set CTCSS/CDCSSTail Found InterruptEnable enable
    RF_Write(0x3F, 0x200);  //0b001000000000
    RF_Write(0x02, 0x0000); //clear int
#endif
    //set Squelch Found&Lost InterruptEnable
    //RF_Write(0x3F, 0x0C); //0b1100

    //RF_Write(0x02, 0x0000); //clear int

    RF_SetGpioOut(7, 0, 0); //gpio0 output control
    RF_SetGpioOut(6, 0, 0); //gpio1 output control
}

void bk4819_vtx_gpio1_output(unsigned char state)
{
    if (state == 0)
    {
        RF_SetGpioOut(6, 0, 0);
    }
    else
    {
        RF_SetGpioOut(6, 0, 1);
    }
}

void bk4819_HL_gpio0_output(unsigned char state)
{
    if (state == 0)
    {
        RF_SetGpioOut(7, 0, 0);
    }
    else
    {
        RF_SetGpioOut(7, 0, 1);
    }
}

void bk4819_off(void)
{
    //todo save analog flash...

    gpio_set_bk_power_en(PIN_LEVEL_LOW);
}

void bk4819_set_freq(unsigned int freq)
{
    //RF_SetFreq((43690000 >> 16) & 0xFFFF, 43690000 & 0xFFFF);
    RF_SetFreq((freq >> 16) & 0xFFFF, freq & 0xFFFF);
}

void bk4819_set_cdcss(unsigned int num)
{
    RF_SetCdcss((num >> 12) & 0xFFF, num & 0xFFF);
}

void bk4819_set_tx_power(unsigned char level)
{
    unsigned char pa_output_amplitude = 0; //0-3.2V (0x00=0V 0xFF=3.2V)
    unsigned char pactl_en = 0;

    pactl_en = 1;
    switch (level)
    {
    case 0:
        pa_output_amplitude = 0x3F;
        bk4819_HL_gpio0_output(1);
        break;
    case 1:
        pa_output_amplitude = 0x7F;
        bk4819_HL_gpio0_output(0);
        break;
    case 2:
        pa_output_amplitude = 0xFF;
        bk4819_HL_gpio0_output(0);
        break;
    default:
        pa_output_amplitude = 0x3F;
        bk4819_HL_gpio0_output(1);
        break;
    }
    //Set Tx Power if needed
    RF_Write(0x36, pa_output_amplitude << 8 | pactl_en << 7 | PA_GAIN); //bit[15:8]=PACTL输出幅度；bit[7]=PACTLEn输出使能；[5:0]=PaGain输出功率
}

void bk4819_set_channel_spacing(unsigned char level)
{
    switch (level)
    {
    case 0: //宽
        RF_SetChnSpace(SPACE_25K);
        break;
    case 1: //中
        RF_SetChnSpace(SPACE_12K5);
        break;
    case 2: //窄
        RF_SetChnSpace(SPACE_12K5);
        break;
    default:
        RF_SetChnSpace(SPACE_25K);
        break;
    }
}

#if 1
#define BASE_VOL 1
void bk4819_sleep_wake_tim_handle(void)
{
    static unsigned char save_power_level_old = 0xff;
    static unsigned short idle_count = 0;
    static unsigned short work_count = 0;
    static unsigned short idle_threshold = 0;
    static unsigned short work_threshold = 0;
    static unsigned short cnt = 0;

    if(cnt++>10)
    {
        cnt=0;
        if (save_power_level_old != ang_seting_flash.toa_flash.save_power_level)
        {
            save_power_level_old = ang_seting_flash.toa_flash.save_power_level;

            switch (ang_seting_flash.toa_flash.save_power_level)
            {
            case 0: //1:1
                work_threshold = 1 * BASE_VOL;
                idle_threshold = 1 * BASE_VOL;
                break;
            case 1: //1:2
                work_threshold = 1 * BASE_VOL;
                idle_threshold = 2 * BASE_VOL;
                break;
            case 2: //1:4
                work_threshold = 1 * BASE_VOL;
                idle_threshold = 4 * BASE_VOL;
                break;
            case 3: //1:8
                work_threshold = 1 * BASE_VOL;
                idle_threshold = 8 * BASE_VOL;
                break;
            default:
                break;
            }
        }

        if (toa.sleep_flag == 1)
        {
            if (work_count < work_threshold)//1
            {
                work_count++;
                if (work_count == work_threshold)
                {
                    //bk4819_off();
                    RF_Idle();
                    toa.bk_rxon_state = 0;
                }
            }
            else
            {
                if (idle_count < idle_threshold)//8
                {
                    idle_count++;
                    if (idle_count == idle_threshold)
                    {
                        RF_Rxon();
                        //bk4819_off();
                        toa.bk_rxon_state = 1;
                        idle_count = 0;
                        work_count = 0;
                    }
                }
            }
        }
        else
        {
            if (toa.bk_rxon_state == 0)
            {
                RF_Rxon();
                toa.bk_rxon_state = 1;
            }
        }
    }
}
#else

#define BASE_VOL 10
void bk4819_sleep_wake_tim_handle(void)
{
    static unsigned char save_power_level_old = 0xff;
    static unsigned short idle_count = 0;
    static unsigned short work_count = 0;
    static unsigned short idle_threshold = 0;
    static unsigned short work_threshold = 0;

    if (save_power_level_old != ang_seting_flash.toa_flash.save_power_level)
    {
        save_power_level_old = ang_seting_flash.toa_flash.save_power_level;

        switch (ang_seting_flash.toa_flash.save_power_level)
        {
        case 0: //1:1
            work_threshold = 1 * BASE_VOL;
            idle_threshold = 1 * BASE_VOL;
            break;
        case 1: //1:2
            work_threshold = 1 * BASE_VOL;
            idle_threshold = 2 * BASE_VOL;
            break;
        case 2: //1:4
            work_threshold = 1 * BASE_VOL;
            idle_threshold = 4 * BASE_VOL;
            break;
        case 3: //1:8
            work_threshold = 1 * BASE_VOL;
            idle_threshold = 8 * BASE_VOL;
            break;
        default:
            break;
        }
    }

    if (toa.sleep_flag == 1)
    {
        if (work_count < work_threshold)//1
        {
            work_count++;
            if (work_count == work_threshold)
            {
                //bk4819_off();
                RF_Idle();
                toa.bk_rxon_state = 0;
            }
        }
        else
        {
            if (idle_count < idle_threshold)//8
            {
                idle_count++;
                if (idle_count == idle_threshold)
                {
                    RF_Rxon();
                    //bk4819_off();
                    toa.bk_rxon_state = 1;
                    idle_count = 0;
                    work_count = 0;
                }
            }
        }
    }
    else
    {
        if (toa.bk_rxon_state == 0)
        {
            RF_Rxon();
            toa.bk_rxon_state = 1;
        }
    }
}
#endif