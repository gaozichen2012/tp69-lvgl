#include "bk_include.h"

#if 0
void DelayUs(u16 Time)
{
	u16 a;
	for (a = 0; a < Time; a++)
	{
		NOP();
	}
}

void DelayMs(u16 Time)
{
	u16 a, b;
	for (a = 0; a < Time; a++)
	{
		for (b = 0; b < 5; b++)
		{
			DelayUs(98);
		}
	}
}

void I2C_Start(void)
{
	SDA_OUT;

	SDA = 1;
	SCL = 1;
	DelayUs(10);

	SDA = 0; //START:when CLK is high,DATA change form high to low
	DelayUs(10);

	SCL = 0;
	DelayUs(10);
}

void I2C_Stop(void)
{
	SDA_OUT;

	SCL = 0;
	SDA = 0; //STOP:when CLK is high DATA change form low to high
	DelayUs(10);

	SCL = 1;
	DelayUs(10);

	SDA = 1;
	DelayUs(10);
}

void I2C_Wait_Ack(void)
{
	SDA_IN;

	SDA = 1;
	DelayUs(5);

	SCL = 1;
	DelayUs(5);

	SCL = 0;
}

void I2C_Ack(void)
{
	SCL = 0;

	SDA_OUT;

	SDA = 0;
	DelayUs(5);

	SCL = 1;
	DelayUs(5);

	SCL = 0;
}

void I2C_NAck(void)
{
	SCL = 0;

	SDA_OUT;

	SDA = 1;
	DelayUs(5);

	SCL = 1;
	DelayUs(5);

	SCL = 0;
}

void I2C_Send_Byte(u8 txd)
{
	u8 t;

	SDA_OUT;

	SCL = 0;

	for (t = 0; t < 8; t++)
	{
		if ((txd & 0x80) >> 7)
			SDA = 1;
		else
			SDA = 0;

		txd <<= 1;
		DelayUs(5);

		SCL = 1;
		DelayUs(5);

		SCL = 0;
		DelayUs(5);
	}
}

u8 I2C_Get_Byte(void)
{
	u8 i;
	u8 rxd = 0;

	SDA_IN;

	for (i = 0; i < 8; i++)
	{
		SCL = 0;
		DelayUs(5);

		SCL = 1;
		rxd <<= 1;

		if (SDA)
			rxd++;
		DelayUs(5);
	}

	return rxd;
}

u16 RF_Read(u8 addr)
{
	u16 data = 0;
	u8 temp = RF_I2CID;

	I2C_Start();

	I2C_Send_Byte(temp);
	I2C_Wait_Ack();

	I2C_Send_Byte(addr);
	I2C_Wait_Ack();

	I2C_Start();

	temp |= 0x1;

	I2C_Send_Byte(temp);
	I2C_Wait_Ack();

	data = I2C_Get_Byte();
	I2C_Ack();

	data <<= 8;
	data |= I2C_Get_Byte();
	I2C_NAck();

	I2C_Stop();

	return data;
}

void RF_Write(u8 addr, u16 data)
{
	u8 temp = RF_I2CID;

	I2C_Start();
	I2C_Send_Byte(temp);
	I2C_Wait_Ack();

	I2C_Send_Byte(addr);
	I2C_Wait_Ack();

	temp = data >> 8;

	I2C_Send_Byte(temp);
	I2C_Wait_Ack();

	temp = data;

	I2C_Send_Byte(temp);
	I2C_Wait_Ack();

	I2C_Stop();
}
#endif
