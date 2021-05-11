#include "bk_include.h"
#include "general_include.h"
#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif
u16 CTC_FREQ;
u16 DCS_HI12;
u16 DCS_LO12;
u16 FRQ_HI16;
u16 FRQ_LO16;

//马工说此返回值无参考价值，根据频率去判断扫描结果是否ok（13969504异常 44357208正常）
u8 RF_FreqScan() //1:fail;0:sucs
{
	RF_Write(0x32, REG_32 | 1); //[0]fscan_en=1

	DelayMs(200);

	u16 rdata = -1;
	u8 cnt = 200; //~=2s protection

	while (cnt && (rdata & 0x8000)) //[15]fscan_busy=0 means finished
	{
		DelayMs(10);
		rdata = RF_Read(0x0D);
		cnt--;
	}

	FRQ_HI16 = rdata & 0x7FF; //[10:0]

	FRQ_LO16 = RF_Read(0x0E);

	RF_Write(0x32, REG_32); //[0]fscan_en=0

	if (!cnt)
		return 1;

	return 0;
}

u8 RF_CtcDcsScan() //0:fail;1:ctc;2:dcs
{

	RF_Write(0x51, 0x0300); //Set Subau filter BW Mode

	RF_Rxon();

	DelayMs(200);

	u16 rdata = -1;
	u8 cnt = 100; //1s protection

	while (cnt) //[15]
	{
		DelayMs(10);
		cnt--;

		rdata = RF_Read(0x69);

		if (!(rdata & 0x8000)) //DCS found
		{
			DCS_HI12 = rdata & 0xFFF;
			DCS_LO12 = RF_Read(0x6A) & 0xFFF;

			RF_Idle();
			return 2;
		}

		rdata = RF_Read(0x68);

		if (!(rdata & 0x8000)) //CTC found
		{
			CTC_FREQ = rdata & 0x1FFF;

			RF_Idle();
			return 1;
		}
	}

	RF_Idle();
	return 0;
}
