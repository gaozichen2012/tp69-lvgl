
/*
FSK(Frequency-shift keying)：频移键控，又称数字调频，就是用数字信号去调制载波的频率，
是信息传输中使用得较早的一种调制方式,最常见的是用两个频率承载二进制1和0的双频FSK系统，
除此之外还有代表多个符号的多进制频移键控，简称多频调制。

FSK的主要优点是: 实现起来较容易,抗噪声与抗衰减的性能较好。
在中低速数据传输中得到了广泛的应用。
*/
#include "bk_include.h"

u16 FSK_TXDATA[FSK_LEN];
u16 FSK_RXDATA[FSK_LEN];

void RF_EnterFsk()
{
	RF_Write(0x70, 0x00E0); //[7]=1,Enable Tone2 for FSK; [6:0]=Gain

#if FSK2400
	RF_Write(0x72, FSKBUAD << 1); //2400bps
	RF_Write(0x58, 0x00C9);
#else
	RF_Write(0x72, FSKBUAD); //1200bps
	RF_Write(0x58, 0x00C1);
#endif //FSK2400
	RF_Write(0x5C, 0x5665);
	RF_Write(0x5D, (FSK_LEN * 2 - 1) << 8); //[15:8]fsk tx length(byte)
}

void RF_ExitFsk()
{
	RF_Write(0x70, 0x0000); //Disable Tone2
	RF_Write(0x58, 0x0000); //Disable FSK
}

void RF_FskIdle()
{
	RF_Write(0x3F, 0x0000); //tx sucs irq mask=0
	RF_Write(0x59, REG_59); //fsk_tx_en=0, fsk_rx_en=0

	RF_Idle();
}



//发射 FSK 使用 RF_FskTransmit()，返回 1 失败，返回 0 成功
u8 RF_FskTransmit() //1:fail;0:sucs
{
	RF_Txon();

	RF_Write(0x3F, 0x8000);			 //tx sucs irq mask=1
	RF_Write(0x59, REG_59 | 0x8000); //[15]fifo clear
	RF_Write(0x59, REG_59);

	u8 i;

	for (i = 0; i < FSK_LEN; i++)
	{
		RF_Write(0x5F, FSK_TXDATA[i]); //push data to fifo
	}

	DelayMs(20);

	RF_Write(0x59, REG_59 | 0x2800); //[11]fsk_tx_en;[13]scrb=1

	u16 rdata = 0;
	u8 cnt = 200; //~=1s protection

	while (cnt && !(rdata & 0x1)) //polling int
	{
		DelayMs(5);
		rdata = RF_Read(0x0C);
		cnt--;
	}

	RF_Write(0x02, 0x0000); //clear int

	RF_FskIdle();

	if (!cnt) //over time
	{
		//$display("FSK Tx FAILED..");
		return 1;
	}
	else
	{
		//$display("FSK Tx SUCCEED..");
		return 0;
	}
}

//接收 FSK 使用 RF_FskReceive()，返回 1 失败，返回 0 成功
u8 RF_FskReceive() //1:fail;0:sucs
{
	RF_Rxon();

	RF_Write(0x3F, 0x3000);			 //rx sucs/fifo_af irq mask=1
	RF_Write(0x59, REG_59 | 0x4000); //[14]fifo clear
	RF_Write(0x59, REG_59 | 0x3000); //[12]fsk_rx_en;[13]scrb=1

	u16 rdata;
	u8 cnt;
	u8 i, j, k = 0;

	for (i = 0; i < (FSK_LEN >> 2); i++)
	{
		rdata = 0;
		cnt = 200;					  //~=1s protection
		while (cnt && !(rdata & 0x1)) //polling int
		{
			DelayMs(5);
			rdata = RF_Read(0x0C);
			cnt--;
		}

		RF_Write(0x02, 0x0000); //clear int

		if (!cnt) //over time
		{
			RF_FskIdle();
			return 1;
		}

		for (j = 0; j < 4; j++)
		{
			rdata = RF_Read(0x5F); //pop data from fifo
			FSK_RXDATA[k] = rdata;
			k++;
		}
	}

	rdata = 0;
	cnt = 200; //~=1s protection

	while (cnt && !(rdata & 0x1)) //polling int
	{
		DelayMs(5);
		rdata = RF_Read(0x0C);
		cnt--;
	}

	RF_Write(0x02, 0x0000); //clear int

	if (!cnt) //over time
	{
		RF_FskIdle();
		return 1;
	}

	cnt = FSK_LEN & 3;

	while (cnt)
	{
		rdata = RF_Read(0x5F); //pop data from fifo
		FSK_RXDATA[k] = rdata;
		k++;
		cnt--;
	}

	//$display("FSK Rx SUCCEED..");

	rdata = RF_Read(0x0B); //[4]crc

	RF_FskIdle();

	if (!(rdata & 0x10))
	{
		return 1; //$display("FSK CRC16 Check Fail.");
	}

	return 0;
}
