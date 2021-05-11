#include "bk_include.h"
#include "gpio.h"
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

#define SCL_HIGH gpio_set_bk_scl(PIN_LEVEL_HIGH)
#define SCL_LOW gpio_set_bk_scl(PIN_LEVEL_LOW)
#define SCN_HIGH gpio_set_bk_scn(PIN_LEVEL_HIGH)
#define SCN_LOW gpio_set_bk_scn(PIN_LEVEL_LOW)

#define SDA_IN_INIT gpio_port_init_bk_sda(1)
#define SDA_OUT_INIT gpio_port_init_bk_sda(0)
#define SDA_OUT_HIGH gpio_set_bk_sda(PIN_LEVEL_HIGH)
#define SDA_OUT_LOW gpio_set_bk_sda(PIN_LEVEL_LOW)
#define READ_SDA gpio_bk_sda_int_cb()

void DelayUs(u16 Time)
{
	u16 a;
	for (a = 0; a < Time; a++)
	{
		// NOP();
	}
}

#if 1
void DelayMs(u16 Time)
{
	ql_rtos_task_sleep_ms(Time);
}
#else
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
#endif

void SPI_Send_Byte(u8 txd)
{
	u8 tmp = txd;

	for (u8 i = 0; i < 8; i++)
	{
		SCL_LOW;

		if (tmp & 0x80)
			SDA_OUT_HIGH;
		else
			SDA_OUT_LOW;

		SCL_HIGH;
		tmp <<= 1;
	}
}

u16 SPI_Get_Word()
{
	SDA_IN_INIT;

	SCL_LOW;

	u16 rxd = 0;

	for (u8 i = 0; i < 16; i++)
	{
		rxd <<= 1;

		SCL_HIGH;

		if (READ_SDA)
			rxd |= 1;

		SCL_LOW;
	}

	return rxd;
}

void RF_Write(u8 addr, u16 data)
{

	//_DEBUG("RF_Write(0x%02X,0x%04X)\r\n", addr, data);
	SDA_OUT_INIT;
	SCL_LOW;
	SCN_LOW;

	SPI_Send_Byte(addr);

	u16 temp = data >> 8;

	SPI_Send_Byte(temp & 0xff);
	SPI_Send_Byte(data & 0xff);

	SCN_HIGH;
	SDA_IN_INIT;
}

u16 RF_Read(u8 addr)
{
	u16 data;
	//_DEBUG("RF_Read(0x%02X)\r\n", addr);
	SDA_OUT_INIT;
	SCL_LOW;
	SCN_LOW;

	SPI_Send_Byte(addr | 0x80);

	DelayUs(2);

	data = SPI_Get_Word();

	SCN_HIGH;

	return data;
}
