#include "bk_include.h"

u8 TONE_RXDATA[TONE_LEN];
u8 TONE_SYMBOL[TONE_LEN];

void RF_Enter5tone()
{

	//Set TONE Symbol Rx Coefficent
	//bit7:0=cos(3*PI*f*FACTOR/1024/128)*64*2; FACTOR=10.32444 for 13M/26M and FACTOR=10.48576 for 12.8M/19.2M/25.6M/38.4M
	RF_Write(0x09, 0x74 | 0x0 << 12);
	RF_Write(0x09, 0x6E | 0x1 << 12);
	RF_Write(0x09, 0x66 | 0x2 << 12);
	RF_Write(0x09, 0x5C | 0x3 << 12);
	RF_Write(0x09, 0x53 | 0x4 << 12);
	RF_Write(0x09, 0x49 | 0x5 << 12);
	RF_Write(0x09, 0x3E | 0x6 << 12);
	RF_Write(0x09, 0x30 | 0x7 << 12);
	RF_Write(0x09, 0x24 | 0x8 << 12);
	RF_Write(0x09, 0x18 | 0x9 << 12);
	RF_Write(0x09, 0xFB | 0xA << 12);
	RF_Write(0x09, 0xE2 | 0xB << 12);
	RF_Write(0x09, 0x0B | 0xC << 12);
	RF_Write(0x09, 0xEF | 0xD << 12);
	RF_Write(0x09, 0x77 | 0xE << 12);

	RF_Write(0x21, 0x06FD);
	RF_Write(0x24, 0x802E | TONE_TH << 7); //[12:7]threshold
}

void RF_Exit5tone()
{
	RF_Write(0x24, 0x0000); //Disable TONE
}

void RF_5toneTransmit()
{
	u16 freq_table[15] = {

		6195,  // '0' 600 *10.32444; 10.32444 for 13M/26M and 10.48576 for 12.8M/19.2M/25.6M/38.4M
		7650,  // '1' 741
		9106,  // '2' 882
		10562, // '3' 1023
		12018, // '4' 1164
		13473, // '5' 1305
		14929, // '6' 1446
		16385, // '7' 1587
		17841, // '8' 1728
		19296, // '9' 1869
		22208, // 'A' 2151
		25140, // 'B' 2435
		20752, // 'C' 2010
		23695, // 'D' 2295
		5111   // 'E' 495
	};

	RF_SetAf(BEEP);
	RF_Write(0x70, 0xE000); //Enable Tone1
	RF_EnterTxMute();
	RF_Txon_Beep();
	DelayMs(50);

	for (u8 i = 0; i < TONE_LEN; i++)
	{
		RF_Write(0x71, freq_table[TONE_SYMBOL[i] & 0xf]);
		RF_ExitTxMute();
		DelayMs(50);
	}

	RF_EnterTxMute();
	DelayMs(50);
	RF_Idle();
	RF_SetAf(OPEN);
	RF_ExitTxMute();
	RF_Write(0x70, 0x0000); //Disable Tone1
}

u8 RF_5toneReceive() //1:fail;0:sucs
{
	RF_Rxon();
	RF_Write(0x3F, 0x0800); //dtmf interrupt mask

	u16 rdata;
	int cnt;
	u8 i;

	for (i = 0; i < TONE_LEN; i++)
	{
		rdata = 0;
		cnt = 1000;					  //5s protection
		while (cnt && !(rdata & 0x1)) //polling int
		{
			DelayMs(5);
			rdata = RF_Read(0x0C);
			cnt--;
		}

		if (!cnt)
		{
			RF_Write(0x3F, 0x0000); //dtmf interrupt mask
			RF_Idle();
			return 1;
		}

		RF_Write(0x02, 0x0000); //clear int

		rdata = RF_Read(0x0B);

		TONE_RXDATA[i] = (rdata >> 8) & 0xF; //[11:8]dtmf_code
	}

	RF_Write(0x3F, 0x0000); //dtmf interrupt mask
	RF_Idle();

	return 0;
}
