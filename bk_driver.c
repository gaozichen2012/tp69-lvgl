#include "bk_include.h"
#include <stdio.h>
#include "gpio.h"
#include "analog_controller.h"
#include "analog_model.h"
#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

// void Main()
// {
// 	RF_Initial();
// 	RF_SetFreq((43690000 >> 16) & 0xFFFF, 43690000 & 0xFFFF);
// 	//...
// 	//RF_Rxon();
// 	//RF_Txon();
// }

//静噪门限、接收音量、接收 AGC、MIC 灵敏度、VoX 门限、调制深度、发射功率、亚音频
void RF_Initial()
{
	//Soft Reset RF
	RF_Write(0x00, 0x8000);
	RF_Write(0x00, 0x0000);

	//Power Up RF
	RF_Write(0x37, REG_37 | 0xF);

	//Set SQ Threshold if needed
	RF_Write(0x4D, REG_4D | TH_GLITCH_SQ_IN);			   //设置Glitch静噪开启门限
	RF_Write(0x4E, REG_4E | TH_GLITCH_SQ_OUT);			   //设置Glitch静噪关闭门限
	RF_Write(0x4F, TH_NOISE_SQ_OUT << 8 | TH_NOISE_SQ_IN); //设置Noise静噪关闭开启门限
	RF_Write(0x78, TH_RSSI_SQ_IN << 8 | TH_RSSI_SQ_OUT);   //设置Rssi静噪开启关闭门限，可用于做静噪等级

	//Set Deviation if needed
	//RF_Write(0x40,REG_40 | DEVIATION);  //bit[12]=Dev_en调制使能；bit[11:0]=Deviation_lvl调制大小

	//Set Tx Power if needed
	//RF_Write(0x36,PACTL<<8 | PACTL_EN<<7 | PA_GAIN); //bit[15:8]=PACTL输出幅度；bit[7]=PACTLEn输出使能；[5:0]=PaGain输出功率

	//Set AGC Table
	RF_SetAgc(0);

	//Set CTCSS Threshold if needed
	//RF_Write(0x52,REG_52); //CTCSS亚音开启关闭门限

	//Set MIC Sensitivity
	RF_Write(0x7D, MIC_GAIN | REG_7D); //bit[4:0]=MicSens灵敏度

	//Set Volume
	RF_Write(0x48, REG_48 | VOL_GAIN << 4 | DAC_GAIN); //bit[9:4]=音量数字增益；bit[3:0]音量模拟增益

	//Set XTAL Frequency
	//RF_SetXtal(XTAL26M);

	//Others Setting
	RF_Write(0x1F, 0x5454); //set rfpll_regvco_vbit=0001 to save current
	RF_Write(0x3E, 0xAAAA); //415~650M
}

void RF_SetAgc(u8 mode)
{
	switch (mode)
	{
	case 0:
		RF_Write(0x13, 0x03BE);
		RF_Write(0x12, 0x037B);
		RF_Write(0x11, 0x027B);
		RF_Write(0x10, 0x007A);
		RF_Write(0x14, 0x0019);
		RF_Write(0x49, 0x2A38);
		RF_Write(0x7B, 0x8420);
		//RF_Write(0x7C,0x8000);

		break;

	case 1:
		RF_Write(0x13, 0x03BE);
		RF_Write(0x12, 0x037C);
		RF_Write(0x11, 0x027B);
		RF_Write(0x10, 0x007A);
		RF_Write(0x14, 0x0018);
		RF_Write(0x49, 0x2A38);
		RF_Write(0x7B, 0x318C);
		RF_Write(0x7C, 0x595E);
		RF_Write(0x20, 0x8DEF);
		for (int i = 0; i < 8; i++)
			RF_Write(0x06, (i & 7) << 13 | 0x4A << 7 | 0x36);

		/*RF_Write(0x06,3<<13 | 0x4A<<7 | 0x36);
			RF_Write(0x06,2<<13 | 0x4A<<7 | 0x36);
			RF_Write(0x06,1<<13 | 0x4A<<7 | 0x36);
			RF_Write(0x06,0<<13 | 0x4A<<7 | 0x36);
			RF_Write(0x06,7<<13 | 0x4A<<7 | 0x36);
			RF_Write(0x06,6<<13 | 0x4A<<7 | 0x36);
			RF_Write(0x06,5<<13 | 0x4A<<7 | 0x36);
			RF_Write(0x06,4<<13 | 0x4A<<7 | 0x36);*/
		break;
	}
}

void RF_SetChnSpace(u8 space) //可根据需求设置<14:12>,<11:9>,<8:6>接收和发射带宽
{
	switch (space)
	{
	case SPACE_12K5:
		RF_Write(0x43, 0x4048); //[14:12]=0b100(Bw3.75k); [8:6]=0b001(AfTxLpf2.5k); [5:4]=00(Space12.5k)
		break;

	case SPACE_25K:
		RF_Write(0x43, 0x3028); //[14:12]=0b100(Bw6k); [8:6]=0b000(AfTxLpf3k); [5:4]=10(Space25k/20k)
		break;

	case SPACE_6K25:
		RF_Write(0x43, 0x205C); //[14:12]=0b100(Bw2.5k); [8:6]=0b001(AfTxLpf2.5k); [5:4]=01(Space6.25k)
		break;

	case SPACE_20K:
		RF_Write(0x43, 0x2028); //[14:12]=0b100(Bw5k); [8:6]=0b000(AfTxLpf3k); [5:4]=10(Space25k/20k)
		break;
	}
}

void RF_SetFreq(u16 freq_hi16, u16 freq_lo16)
{
	//Freq(Hz) = (freq_hi16<<16 | freq_lo16)*10(Hz)
	RF_Write(0x38, freq_lo16);
	RF_Write(0x39, freq_hi16);
}

void RF_SetCtcss(u16 ctc1) //ctc1=freq(hz)*20.64888 for 13M/26M and *20.97152 for 12.8M/19.2M/25.6M/38.4M
{
	RF_Write(0x51, 0x9000 | CTC_GAIN); //Set CTCSS mode, [6:0] Gain
	RF_Write(0x07, ctc1);			   //100Hz for example, 0x0811=100*20.64888 for 13M/26M and *20.97152 for 12.8M/19.2M/25.6M/38.4M
}

void RF_SetCtc2(u16 ctc2) //ctc2=6500000/256/freq(hz), for rx ctcss tail only
//55Hz is default, and ctc2<100 must be meet，只能用于接收检测55Hz左右的亚音频尾音
{
	RF_Write(0x07, ctc2 | 1 << 13); //55Hz for example, 462=25391/55(25391 for 13M/26M and 25000 for 12.8M/19.2M/25.6M/38.4M)
}

void RF_SetCdcss(u16 hi12, u16 lo12)
{
	RF_Write(0x51, 0x8000 | DCS_GAIN); //Set CDCSS mode, [6:0] Gain

	RF_Write(0x07, 0x0AD7); //=134.4*20.64888 for 13M/26M and *20.97152 for 12.8M/19.2M/25.6M/38.4M

	RF_Write(0x08, lo12);			//set low 12-bit =0x813                                            0b100000010011
	RF_Write(0x08, hi12 | 1 << 15); //set high 12-bit=0x763                                            0b011101100011
}

void RF_ExitSubau() //Disable CTCSS/CDCSS
{
	RF_Write(0x51, 0x0000);
}

void RF_SetAf(u8 mode)
{
	// mode
	// MUTE  0, Nomarl Rx AF Mute
	// OPEN  1, Nomarl Rx AF out
	// ALAM  2, Generate Alarm When Rx, Should enable Tone1 first
	// BEEP  3, Should enable Tone1 first, and set REG_03[9]=1 to Enable AF when Tx
	// CTCO  6, CTC/DCS Rx out for Testing
	// FSKO  8, FSK Rx out for Testing

	RF_Write(0x47, REG_47 | (u16)(mode << 8));
}

void RF_EnterTxMute() //Disable Audio Tx
{
	RF_Write(0x50, REG_50 | 1 << 15);
}

void RF_ExitTxMute() //Enable Audio Tx
{
	RF_Write(0x50, REG_50);
}

//开启扰频
void RF_EnterScramble()
{
	u16 val = RF_Read(0x31);

	RF_Write(0x31, val | 1 << 1); //bit[1]=Scramble Enable

	RF_Write(0x71, 0x8517); //Scramble freq, 3300*10.32444 for 13M/26M and 10.48576 for 12.8M/19.2M/25.6M/38.4M
}

//关闭扰频
void RF_ExitScramble()
{
	u16 val = RF_Read(0x31);

	RF_Write(0x31, val & 0xFFFD); //Scramble Disable
}

void RF_EnterVox()
{
	u16 val = RF_Read(0x31);

	RF_Write(0x46, REG_46 | TH_VOX_IN);
	RF_Write(0x79, VOX_DET_INTVL << 11 | TH_VOX_OUT);
	RF_Write(0x7A, REG_7A | VOX_OUT_DELAY << 12);

	RF_Write(0x31, val | 1 << 2); //bit[2]=VOX Enable
}

void RF_ExitVox()
{
	u16 val = RF_Read(0x31);

	RF_Write(0x31, val & 0xFFFB); //VOX Disable
}

//开启压扩
void RF_EnterCompander()
{
	u16 val = RF_Read(0x31);

	RF_Write(0x31, val | 1 << 3); //bit[3]=CMPD Enable
}

//关闭压扩
void RF_ExitCompander()
{
	u16 val = RF_Read(0x31);

	RF_Write(0x31, val & 0xFFF7); //CMPD Disable
}

void RF_EnterBypass() //for dMR
{
	RF_Write(0x47, REG_47 | 9 << 8 | 3); //[11:8]=9 for Rx; [1]=1 for gain; [0]=1 for Tx
	RF_Write(0x7E, REG_7E & 0xFFC7);	 //[5:3]=000b for Tx DCC bypass
}

void RF_ExitBypass()
{
	RF_SetAf(MUTE);
	RF_Write(0x7E, REG_7E);
}

#if 1
void RF_GenTail(u8 tail)
{
	switch (tail)
	{
	case CTC55:					//for 55hz or other freq ctcss, Make sure ctcss has been enabled
		RF_Write(0x07, 0x046F); //=55*20.64888 for 13M/26M and *20.97152 for 12.8M/19.2M/25.6M/38.4M
		break;
	case CTC259:
		RF_Write(0x07, 0x14E4); //=259*20.64888
		break;
	case CTC45:
		RF_Write(0x07, 0x03A1); //=45*20.64888
		break;
	case CTC134: //for 134.4hz ctcss, Make sure cdcss has been enabled
		RF_Write(0x52, REG_52 | 4 << 13);
		break;

	case CTC120: //for ctcss 120 degree shift, Make sure ctcss has been enabled
		RF_Write(0x52, REG_52 | 5 << 13);
		break;

	case CTC180: //for ctcss 180 degree shift, Make sure ctcss has been enabled
		RF_Write(0x52, REG_52 | 6 << 13);
		break;

	case CTC240: //for ctcss 240 degree shift, Make sure ctcss has been enabled
		RF_Write(0x52, REG_52 | 7 << 13);
		break;
	}
}
#else
void RF_GenTail(u8 tail)
{
	switch (tail)
	{
	case CTC55:					//for 55hz or other freq ctcss, Make sure ctcss has been enabled
		RF_Write(0x07, 0x046F); //=55*20.64888 for 13M/26M and *20.97152 for 12.8M/19.2M/25.6M/38.4M
		break;

	case CTC134: //for 134.4hz ctcss, Make sure cdcss has been enabled
		RF_Write(0x52, REG_52 | 4 << 13);
		break;

	case CTC120: //for ctcss 120 degree shift, Make sure ctcss has been enabled
		RF_Write(0x52, REG_52 | 5 << 13);
		break;

	case CTC180: //for ctcss 180 degree shift, Make sure ctcss has been enabled
		RF_Write(0x52, REG_52 | 6 << 13);
		break;

	case CTC240: //for ctcss 240 degree shift, Make sure ctcss has been enabled
		RF_Write(0x52, REG_52 | 7 << 13);
		break;
	}
}
#endif
u16 KEYTONE[2] = {0x14F8, 0x24E6}; //{520,915}(Hz)*10.32444 for 13M/26M and 10.48576 for 12.8M/19.2M/25.6M/38.4M

void RF_Key() //产生按键音并本地播放，不发射，以两音节为例
{
	RF_SetAf(BEEP);
	RF_Write(0x70, TONE_GAIN << 8); //Enable Tone1
	RF_EnterTxMute();
	RF_Write(0x30, 0x0000);
	RF_Write(0x30, 0x0302);

	//OpenAudioPA here...

	DelayMs(75);

	for (u8 i = 0; i < 2; i++)
	{
		RF_Write(0x71, KEYTONE[i]);
		RF_ExitTxMute();
		DelayMs(100);
	}

	//ShutAudioPA here...

	RF_Write(0x30, 0x0000);
	RF_SetAf(OPEN);
	RF_Write(0x70, 0x0000); //DIsable Tone1
}

u16 CALLTONE[5] = {0x14F8, 0x1F75, 0x14F8, 0x1F75, 0x14F8}; //{520,780,520,780,520}(Hz)*10.32444 for 13M/26M and 10.48576 for 12.8M/19.2M/25.6M/38.4M

void RF_Call() //发射铃音，同时本地播放
{
	RF_SetAf(BEEP);
	RF_Write(0x70, TONE_GAIN << 8); //Enable Tone1
	RF_EnterTxMute();
	RF_Txon_Beep();

	//OpenAudioPA here...

	DelayMs(500);

	for (u8 i = 0; i < 8; i++)
	{

		for (u8 j = 0; j < 5; j++)
		{
			RF_Write(0x71, CALLTONE[j]);
			RF_ExitTxMute();
			DelayMs(50);
		}

		RF_EnterTxMute();
		DelayMs(75);
	}

	RF_GenTail(CTC55);
	DelayMs(300);

	//ShutAudioPA here...

	RF_Idle();
	RF_SetAf(OPEN);
	RF_ExitTxMute();
	RF_Write(0x70, 0x0000); //Disable Tone1
}

void RF_Txon_Beep() //带侧音的发射模式
{
	RF_Write(0x37, REG_37 | 0xF); //[1]xtal;[0]bg;[9]ldo_rf_vsel=0 when txon
	RF_Write(0x52, REG_52);		  //Set bit[15]=0 to Clear ctcss/cdcss Tail
	RF_Write(0x30, 0x0000);
	RF_Write(0x30, 0xC1FA | 1 << 9); //RF Txon; Set bit[9]=1(AF) bit[2]=0(MIC) for BEEP mode
}

//发射使用 RF_Txon()
void RF_Txon()
{
	RF_Write(0x37, REG_37 | 0xF); //[1]xtal;[0]bg;[9]ldo_rf_vsel=0 when txon
	RF_Write(0x52, REG_52);		  //Set bit[15]=0 to Clear ctcss/cdcss Tail
	RF_Write(0x30, 0x0000);
	RF_Write(0x30, 0xC1FE);
}

void RF_Rxon()
{
	RF_Write(0x37, REG_37 | 0xF | 1 << 9); //[1]xtal;[0]bg;[9]ldo_rf_vsel=1 when rxon
	//RF_SetAf(MUTE);
	RF_Write(0x30, 0x0000);
	RF_Write(0x30, 0xBFF1); //RF Rxon

	//when SQ and CTC/DCS is link
	//OpenAudioPA here...
	//DelayMs(75);
	//RF_SetAf(OPEN);

	//when SQ or CTC/DCS is lost or CTC/DCS Tail is found
	//ShutAudioPA here...
}

void RF_Rxon_Disc() //接收仅作音频解码基带使用，mic输入经解码后af输出
{
	RF_Write(0x37, REG_37 | 0xF); //[1]xtal;[0]bg;[9]ldo_rf_vsel=1 when vco output
	//RF_SetAf(OPEN);
	RF_Write(0x30, 0x0000);
#if RXON_WITH_VCO
	RF_Write(0x30, 0xC0FE | 1 << 9); //RF AF=1 and VCO output via PIN RFO at the same time
#else
	RF_Write(0x30, 0x0006 | 1 << 9); //Rx AF=1
#endif
}

void RF_Idle()
{
	RF_Write(0x30, 0x0000); //RF Idle(current~=3mA)
}

void RF_Sleep()
{
	RF_Write(0x30, 0x0000);
	RF_Write(0x37, REG_37); //current~=200uA
}

void RF_WakeUp()
{
	RF_Write(0x37, REG_37 | 0xF); //[1]xtal;[0]bg
}

void RF_SetAfResponse(u8 tx, u8 f3k, u8 db) //参数：发射or接收，3k频响or 300hz频响，调整-1~+4dB
{
	u16 d1 = 0;
	u16 d2 = 0;

	switch (f3k) //3khz or 300hz
	{
	case 1:			 //for 3khz
		d1 = 0xf50b; //default

		switch (db & 0xf)
		{
		case ADD1: //+1dB
			d1 = 0xe61c;
			break;

		case ADD2: //+2dB
			d1 = 0xdf22;
			break;

		case ADD3: //+3dB
			d1 = 0xd42d;
			break;

		case ADD4: //+4dB
			d1 = 0xcc35;
			break;

		case SUB1: //-1dB
			d1 = 0xfa02;
			break;
		}
		break;

	case 0:			 //for 300hz
		d1 = 0x8be3; //default
		d2 = 0x352d;

		switch (db & 0xf)
		{
		case ADD1: //+1dB
			d1 = 0x8bd7;
			d2 = 0x3507;
			break;

		case ADD2: //+2dB
			d1 = 0x8aed;
			d2 = 0x35e1;
			break;

		case ADD3: //+3dB
			d1 = 0x8a96;
			d2 = 0x3606;
			break;

		case ADD4: //+4dB
			d1 = 0x8908;
			d2 = 0x3779;
			break;

		case SUB1: //-1dB
			d1 = 0x8dde;
			d2 = 0x335a;
			break;

		case SUB2: //-2dB
			d1 = 0x8eb3;
			d2 = 0x32ba;
			break;

		case SUB3: //-3dB
			d1 = 0x8f88;
			d2 = 0x3201;
			break;
		}
		break;
	}

	switch (tx) //Tx or Rx
	{
	case 1: //for Tx
		switch (f3k)
		{
		case 1: //for 3khz
			RF_Write(0x74, d1);
			break;

		case 0: //for 300hz
			RF_Write(0x44, d1);
			RF_Write(0x45, d2);
			break;
		}

		break;

	case 0: //for Rx
		switch (f3k)
		{
		case 1: //for 3khz
			RF_Write(0x75, d1);
			break;

		case 0: //for 300hz
			RF_Write(0x54, d1);
			RF_Write(0x55, d2);
			break;
		}

		break;
	}
}

void RF_SetXtal(u8 mode)
{
	switch (mode)
	{
	case XTAL26M:
		break;

	case XTAL13M:
		RF_Write(0x40, REG_40 | DEVIATION); //DEVIATION=0x5D0 for example
		RF_Write(0x41, 0x81C1);
		RF_Write(0x3B, 0xAC40);
		RF_Write(0x3C, 0x2708);
		RF_Write(0x3D, 0x3555);
		break;

	case XTAL19M2:
		RF_Write(0x40, REG_40 | DEVIATION); //DEVIATION=0x53A for example
		RF_Write(0x41, 0x81C2);
		RF_Write(0x3B, 0x9800);
		RF_Write(0x3C, 0x3A48);
		RF_Write(0x3D, 0x2E39);
		break;

	case XTAL12M8:
		RF_Write(0x40, REG_40 | DEVIATION); //DEVIATION=0x5D0 for example
		RF_Write(0x41, 0x81C1);
		RF_Write(0x3B, 0x1000);
		RF_Write(0x3C, 0x2708);
		RF_Write(0x3D, 0x3555);
		break;

	case XTAL25M6:
		RF_Write(0x3B, 0x2000);
		RF_Write(0x3C, 0x4E88);
		break;

	case XTAL38M4:
		RF_Write(0x40, REG_40 | DEVIATION); //DEVIATION=0x43A for example
		RF_Write(0x41, 0x81C5);
		RF_Write(0x3B, 0x3000);
		RF_Write(0x3C, 0x75C8);
		RF_Write(0x3D, 0x261C);
		break;
	}
}

u8 RF_GetSq()
{
	u16 rdata;

	rdata = RF_Read(0x0C); //bit[1]=SQ
	//SQ Algorithm
	//if(rssi>TH_RSSI_SQ_IN && noise<TH_NOISE_SQ_IN && glitch<TH_GLITCH_SQ_IN)
	//    SQ = 1;
	//else
	//if(rssi<TH_RSSI_SQ_OUT || noise>TH_NOISE_SQ_OUT || glitch>TH_GLITCH_SQ_OUT)
	//    SQ = 0;

	return (rdata >> 1) & 1;
}

//返回 1 表示收到 CTC1（主 CTC），返回 2 表示收到CTC2（如 55Hz 尾音）；
u8 RF_GetCtcss()
{
	u16 rdata;

	rdata = RF_Read(0x0C); //bit[11]=55Hz,bit[10]=CTC1

	return (rdata >> 10) & 3;
}

//返回 1 表示收到 120°相位变化尾音，
//返回 2 表示收到 180°相位变化尾音，
//返回 3 表示收到 240°相位变化尾音。
u8 RF_GetTail()
{
	u16 rdata;

	rdata = RF_Read(0x0C); //bit[13:12]=Tail_Type,01=120;10=180;11=240

	return (rdata >> 12) & 3;
}

//返回 1 表示收到 CDC 正码，返回 2 表示收到 CDC 反码；
u8 RF_GetCdcss()
{
	u16 rdata;

	rdata = RF_Read(0x0C); //bit[15]=DCS_P,bit[14]=DCS_N

	return (rdata >> 14) & 3;
}

u8 RF_GetVox()
{
	u16 rdata;

	rdata = RF_Read(0x0C); //bit[2]=VOX
	//VOX Algorithm
	//if(voxamp>TH_VOX_IN)       VOX = 1;
	//else if(voxamp<TH_VOX_OUT) (After Delay) VOX = 0;

	return (rdata >> 2) & 1;
}

u16 RF_GetVoxAmp()
{
	u16 rdata;

	rdata = RF_Read(0x64); //bit[14:0]=voxamp

	return rdata & 0x7FFF;
}

//显示接收信号强度
u16 RF_GetRssi()
{
	u16 rdata;

	rdata = RF_Read(0x67); //bit[8:0]=rssi, LSB->0.5dB

	return rdata & 0x1FF;
}

u8 RF_GetNoise()
{
	u16 rdata;

	rdata = RF_Read(0x65); //bit[6:0]=noise

	return rdata & 0x7F;
}

u8 RF_GetGlitch()
{
	u16 rdata;

	rdata = RF_Read(0x63); //bit[7:0]=glitch

	return rdata & 0xFF;
}

void RF_SetGpioOut(u8 num, u8 type, u8 val) //num=0~7

//gpio0->num=7(PIN17)
//gpio1->num=6(PIN18)
//gpio2->num=5(PIN19)
//gpio3->num=4(PIN20)
//gpio4->num=0(PIN1 )
//
//type=0 -> 0 or 1  		//输出高低电平（默认）
//type=1 -> Interrupt 		//输出中断电平，高有效，等同于REG_0C[0]
//type=2 -> Squelch			//输出Squelch监测结果，等同于REG_0C[1]
//type=3 -> VoX				//输出Vox监测结果，等同于REG_0C[2]
//type=4 -> Subau Result	//输出数/模亚音频监测结果，包含开启和尾音关闭
//type=5 -> CTCSS Result	//输出CTCSS监测结果，不含尾音，等同于REG_0C[10]
//type=6 -> CDCSS Result	//输出CDCSS检测结果，不含尾音，等同于REG_0C[14]|REG_0C[15]
//type=7 -> Tail			//输出尾音监测结果，CTC2和相位，等同于REG_0C[11]|REG_0C[12]|REG_0C[13]
//type=8 -> DTMF/5Tone		//输出DTMF/5TONE接收提示，高有效，等同于REG_0B[12]
//type=9 -> CTCSS/CDCSS Wave//输出数/模亚音频接收波形，方波
//
//Set PIN20 = CTCSS/CDCSS Wave: RF_SetGpioOut(4,9,0);
//Set PIN17 = High:             RF_SetGpioOut(7,0,1);
//Set PIN19 = Squelch:          RF_SetGpioOut(5,2,0);

{
	u16 rdata;

	rdata = RF_Read(0x33); //bit[15:8]=Gpio_out_en_b;bit[7:0]=Gpio_out_val
	rdata &= ~(0x0101 << num);
	rdata |= val << num;
	RF_Write(0x33, rdata);

	switch (num & 4)
	{
	case 0: //num 0~3
		rdata = RF_Read(0x34);
		rdata &= ~(0xf << (num & 3));
		rdata |= type << (num & 3);
		RF_Write(0x34, rdata);
		break;
	case 4: //num 4~7
		rdata = RF_Read(0x35);
		rdata &= ~(0xf << (num & 3));
		rdata |= type << (num & 3);
		RF_Write(0x35, rdata);
		break;
	}
}

u8 RF_GetGpioIn(u8 num) //num=0~7
						//gpio0->num=7(PIN17)
						//gpio1->num=6(PIN18)
						//gpio2->num=5(PIN19)
						//gpio3->num=4(PIN20)
						//gpio4->num=0(PIN1 )
{
	u16 rdata;

	rdata = RF_Read(0x33); //bit[15:8]=Gpio_out_en_b
	rdata |= (0x100 << num);
	RF_Write(0x33, rdata);

	rdata = RF_Read(0x0A); //bit[7:0]=Gpio_in_val

	return (rdata >> num) & 1;
}
