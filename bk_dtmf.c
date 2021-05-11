#include "bk_include.h"

u8 DTMF_RXDATA[DTMF_LEN];
u8 DTMF_SYMBOL[DTMF_LEN] = {0};

void RF_EnterDtmf()
{

	//Set DTMF Symbol Rx Coefficent
	//bit7:0=cos(3*PI*f*FACTOR/1024/128)*64*2; FACTOR=10.32444 for 13M/26M and FACTOR=10.48576 for 12.8M/19.2M/25.6M/38.4M
	RF_Write(0x09, 0x6F | 0x0 << 12);
	RF_Write(0x09, 0x6B | 0x1 << 12);
	RF_Write(0x09, 0x67 | 0x2 << 12);
	RF_Write(0x09, 0x62 | 0x3 << 12);
	RF_Write(0x09, 0x50 | 0x4 << 12);
	RF_Write(0x09, 0x47 | 0x5 << 12);
	RF_Write(0x09, 0x3A | 0x6 << 12);
	RF_Write(0x09, 0x2C | 0x7 << 12);
	RF_Write(0x09, 0x41 | 0x8 << 12);
	RF_Write(0x09, 0x37 | 0x9 << 12);
	RF_Write(0x09, 0x25 | 0xA << 12);
	RF_Write(0x09, 0x17 | 0xB << 12);
	RF_Write(0x09, 0xE4 | 0xC << 12);
	RF_Write(0x09, 0xCB | 0xD << 12);
	RF_Write(0x09, 0xB5 | 0xE << 12);
	RF_Write(0x09, 0x9F | 0xF << 12);

	RF_Write(0x21, 0x06D8);
	RF_Write(0x24, 0x807E | DTMF_TH << 7); //[12:7]threshold
}

void RF_ExitDtmf()
{
	RF_Write(0x24, 0x0000); //Disable DTMF
}

void RF_DtmfTransmit()
{
	RF_SetAf(BEEP);
	RF_Write(0x70, 0xE0E0); //Enable Tone1 & Tone2
	RF_EnterTxMute();
	RF_Txon_Beep();

	for (u8 i = 0; i <= DTMF_LEN; i++)
	{
		DelayMs(50);
		switch (DTMF_SYMBOL[i])
		{
		case 0:
			RF_Write(0x71, FREQ_941); //FREQ_941=941*10.32444; 10.32444 for 13M/26M and 10.48576 for 12.8M/19.2M/25.6M/38.4M
			RF_Write(0x72, FREQ_1336);
			break;

		case 1:
			RF_Write(0x71, FREQ_697);
			RF_Write(0x72, FREQ_1209);
			break;

		case 2:
			RF_Write(0x71, FREQ_697);
			RF_Write(0x72, FREQ_1336);
			break;

		case 3:
			RF_Write(0x71, FREQ_697);
			RF_Write(0x72, FREQ_1477);
			break;

		case 4:
			RF_Write(0x71, FREQ_770);
			RF_Write(0x72, FREQ_1209);
			break;

		case 5:
			RF_Write(0x71, FREQ_770);
			RF_Write(0x72, FREQ_1336);
			break;

		case 6:
			RF_Write(0x71, FREQ_770);
			RF_Write(0x72, FREQ_1477);
			break;

		case 7:
			RF_Write(0x71, FREQ_852);
			RF_Write(0x72, FREQ_1209);
			break;

		case 8:
			RF_Write(0x71, FREQ_852);
			RF_Write(0x72, FREQ_1336);
			break;

		case 9:
			RF_Write(0x71, FREQ_852);
			RF_Write(0x72, FREQ_1477);
			break;

		case 0xA:
			RF_Write(0x71, FREQ_697);
			RF_Write(0x72, FREQ_1633);
			break;

		case 0xB:
			RF_Write(0x71, FREQ_770);
			RF_Write(0x72, FREQ_1633);
			break;

		case 0xC:
			RF_Write(0x71, FREQ_852);
			RF_Write(0x72, FREQ_1633);
			break;

		case 0xD:
			RF_Write(0x71, FREQ_941);
			RF_Write(0x72, FREQ_1633);
			break;

		case 0xE: //'*'
			RF_Write(0x71, FREQ_941);
			RF_Write(0x72, FREQ_1209);
			break;

		case 0xF: //'#'
			RF_Write(0x71, FREQ_941);
			RF_Write(0x72, FREQ_1477);
			break;
		}
		RF_ExitTxMute();
		DelayMs(50);
		RF_EnterTxMute();
	}

	DelayMs(50);
	RF_Idle();
	RF_SetAf(OPEN);
	RF_ExitTxMute();
	RF_Write(0x70, 0x0000); //Disable Tone1 & Tone2
}

u8 RF_DtmfReceive() //1:fail;0:sucs
{
	RF_Rxon();
	RF_Write(0x3F, 0x0800); //dtmf interrupt mask

	u16 rdata;
	int cnt;
	u8 i;

	for (i = 0; i < DTMF_LEN; i++)
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

		DTMF_RXDATA[i] = (rdata >> 8) & 0xF; //[11:8]dtmf_code
	}

	RF_Write(0x3F, 0x0000); //dtmf interrupt mask
	RF_Idle();

	return 0;
}
