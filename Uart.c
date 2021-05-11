#include "ql_rtos.h"
#include "ql_power.h"
#include "ql_fs.h"
#include "ql_uart.h"

#include "Uart.h"
#include "gpio.h"

// #include "version.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "write_code.h"
#include "analog_model.h"
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

extern ql_queue_t msqid;

void uart_gps_push_msg(void)
{
	ZPOC_MSG_TYPE iMsg;
	iMsg.enMsgType = ZPOC_MSG_GPS;
	iMsg.pMsg = NULL;
	iMsg.usMsgLen = 0;
	ql_rtos_queue_release(msqid, sizeof(iMsg), (unsigned char *)&iMsg, QL_WAIT_FOREVER);
}

int uart_gps_ddmm_to_dd(char *ddmm, char *dd)
{
	char *pcMm;
	double dMm;
	int iMm;
	int i = 0;
	//int lenMm = 0;
	int flag = 1;

	if (NULL == ddmm || NULL == dd)
	{
		return -1;
	}

	while (ddmm[i] != ',' && ddmm[i] != 0)
	{
		dd[i] = ddmm[i];
		i++;
	}
	dd[i] = 0;

	pcMm = strstr(dd, ".");

	if (pcMm == NULL)
	{
		pcMm = dd + strlen(dd) - 2;
		iMm = atoi(pcMm);
		dMm = iMm / 60.0;
	}
	else
	{
		if (pcMm - dd > 2)
		{
			pcMm = pcMm - 2;
		}
		else
		{
			pcMm = dd;
			flag = 0;
		}
		dMm = atof(pcMm);
		dMm /= 60.0;
	}

	sprintf(pcMm, "%.6lf", dMm);
	if (flag)
	{
		strcpy(pcMm, pcMm + 1);
	}
	return 1;
}

int uart_gps_check_pack_valid(char *pStr)
{
	unsigned short usFramCnt = 0;
#if 1
	while (pStr != NULL && *pStr != '\r' && *pStr != '\n')
#else
	while ((*pStr) != NULL && *pStr != '\r' && *pStr != '\n')
#endif
	{
		if (*pStr < 33 || *pStr > 126)
		{
			return -1;
		}
		else
		{
			if (*pStr == ',')
			{
				usFramCnt++;
			}
		}
		pStr++;
	}

	if (usFramCnt > 7)
		return 0;
	else
		return -1;
}

int uart_gps_info_decode(char *pStr)
{
	unsigned char ucValidFlg = 0;

	int i = 0, j = 0;
	int iFramCnt = 0;

	char *pGGA = pStr;

	if (0 != uart_gps_check_pack_valid(pStr))
	{
		return -1;
	}

	while (pGGA[i])
	{
		if (pGGA[i] == ',')
		{
			if (iFramCnt == 2)
			{
				memset(iRunPar.iGpsInfo.stLat, 0, sizeof(iRunPar.iGpsInfo.stLat));
				if (*(pGGA + j + 1) != ',')
				{
					uart_gps_ddmm_to_dd(pGGA + j + 1, iRunPar.iGpsInfo.stLat);
				}
			}
			else if (iFramCnt == 3)
			{
				iRunPar.iGpsInfo.cNS = *(pGGA + i - 1);
			}
			else if (iFramCnt == 4)
			{
				memset(iRunPar.iGpsInfo.stLon, 0, sizeof(iRunPar.iGpsInfo.stLon));
				if (*(pGGA + j + 1) != ',')
				{
					iRunPar.iGpsInfo.stLon[0] = *(pGGA + j + 1);
					uart_gps_ddmm_to_dd(pGGA + j + 2, &iRunPar.iGpsInfo.stLon[1]);
				}
			}
			else if (iFramCnt == 5)
			{
				iRunPar.iGpsInfo.cEW = *(pGGA + i - 1);
			}
			else if (iFramCnt == 6)
			{
				iRunPar.iGpsInfo.cLocSta = *(pGGA + i - 1);
			}
			else if (iFramCnt == 7)
			{
				memset(iRunPar.iGpsInfo.cStarNum, 0, 4);
				memcpy(iRunPar.iGpsInfo.cStarNum, pGGA + j + 1, (i - j - 1) > 3 ? 3 : (i - j - 1));

				ucValidFlg = 1;
			}
			j = i;
			iFramCnt++;
		}
		i++;
	}

	iRunPar.iGpsInfo.iState = GPS_DEVICE_NORMAL;

	if (ucValidFlg == 1 && (iRunPar.iGpsInfo.cLocSta == '0' || iRunPar.iGpsInfo.cLocSta == '1') && (iRunPar.iGpsInfo.cNS == 'N' || iRunPar.iGpsInfo.cNS == 'S') && (iRunPar.iGpsInfo.cEW == 'E' || iRunPar.iGpsInfo.cEW == 'W'))
	{
		_DEBUG("----------\r\nGps Info, Sta:%c Lat:%s%c Log:%s%c StarNum:%s\r\n----------\r\n",
			   iRunPar.iGpsInfo.cLocSta, iRunPar.iGpsInfo.stLat, iRunPar.iGpsInfo.cNS,
			   iRunPar.iGpsInfo.stLon, iRunPar.iGpsInfo.cEW, iRunPar.iGpsInfo.cStarNum);

		uart_gps_push_msg();
	}

	return 0;
}

char cBuf[GPS_CMD_MAX_BUF_LENGTH];

void uart_gps_callback(QL_UART_PORT_NUMBER_E port, void *para)
{
	int iLen;
	char *pValidInf;

	memset(cBuf, 0, sizeof(cBuf));
	iLen = ql_uart_read(port, (unsigned char *)cBuf, sizeof(cBuf) - 1);
	cBuf[iLen] = 0;

	if (NULL != (pValidInf = strstr(cBuf, "$GNGGA")) || (NULL != (pValidInf = strstr(cBuf, "$GPGGA"))))
	{
		//_DEBUG("gps-->%s\r\n", pValidInf);
		uart_gps_info_decode(pValidInf);
	}
}

void uart_gps_init(void)
{
	ql_uart_open(QL_BT_UART_PORT, QL_UART_BAUD_9600, QL_FC_NONE);
	ql_uart_register_cb(QL_BT_UART_PORT, uart_gps_callback);
}

////////////////////////////////////////////////////////////////////////////////////////
//
//         for write code line mode to write account
//
////////////////////////////////////////////////////////////////////////////////////////
void uart_writecode_reply(char *pCmd)
{
	ql_uart_write(QL_USB_CDC_PORT, (unsigned char *)pCmd, strlen(pCmd));
}

void uart_writecode_send_data(unsigned char *pData, unsigned char ucLen)
{
	ql_uart_write(QL_USB_CDC_PORT, (unsigned char *)pData, ucLen);
}

unsigned char check_sum(unsigned char *msg, unsigned short msglen)
{
	unsigned short i;
	unsigned char sum = 0;

	for (i = 0; i < msglen; i++)
	{
		sum += msg[i];
	}
	return sum;
}

int uart_set_poc_cfg_to_file(void)
{
	QFILE *fd = ql_fopen(POC_CFG_FILE_PATH, "wb");

	if (fd == NULL)
	{
		_DEBUG("Open File Faild!\r\n");
		return 0;
	}

	if (0 > ql_fwrite((void *)&iPocSet, sizeof(iPocSet), 1, fd))
	{
		_DEBUG("Write Cfg File Faild!!\r\n");
		ql_fclose(fd);
		return 0;
	}

	ql_fclose(fd);
	return 1;
}

int uart_set_analog_cfg_to_file(void)
{
	//Length: 78, Data: FE FE EE EF E4 30 30 30 30 32 30 30 34 34 35 39 36 30 32 30 34 34 35 39 36 30 32 46 46 30 46 46 46 30 46 30 30 30 30 30 36 32 30 32 30 32 30 32 30 32 30 32 30 41 34 43 42 39 37 30 32 41 34 43
	//Length: 78, Data: FE FE EE EF E4 30 30 32 30 32 30 30 46 30 30 30 30 30 36 32 30 32 30 32 30 32 30 32 30 32 30 41 34 43 42 39 37 30 32 41 34 43 42 39 37 30 32 46 46 30 46 46 46 30 46 30 30 30 30 30 36 32 30 32

	//43402500-0x2964504-02 96 45 04-04 45 96 02
	//30 34 34 35 39 36 30 32
	QFILE *fd = ql_fopen(ANA_CFG_FILE_PATH, "wb");

	if (fd == NULL)
	{
		_DEBUG("Open File Faild!\r\n");
		return 0;
	}

	if (0 > ql_fwrite((void *)&toa_flash2, sizeof(ana_all_pocket_t), 1, fd))
	{
		_DEBUG("Write Cfg File Faild!!\r\n");
		ql_fclose(fd);
		return 0;
	}

	ql_fclose(fd);
	return 1;
}

void uart_writecode_callback(QL_UART_PORT_NUMBER_E port, void *para)
{
	memset(wc_data.rx_buf, 0, 256);
	memset(wc_data.tx_buf, 0, 256);

	wc_data.rx_len = ql_uart_read(port, wc_data.rx_buf, sizeof(wc_data.rx_buf));

	memcpy(wc_data.tx_buf, wc_data.rx_buf, wc_data.rx_len);
	write_code_process();

	//_DEBUG("writecode read-->%s\r\n", (char *)wc_data.rx_buf);
}

void writecode_rx_handle_for_fsk(unsigned char rx_buf, unsigned short rx_len)
{
	memset(wc_data.tx_buf, 0, 256);

	memcpy(wc_data.tx_buf, rx_buf, rx_len);
	write_code_process();

	//_DEBUG("writecode read-->%s\r\n", (char *)wc_data.rx_buf);
}

void write_pc_handle_for_fsk(void)
{
	static unsigned char step = 0;

	if (step == 0)
	{
		/* code */
	}

	//step1 tx FE FE EE EF E0 26 98 00 00 FD
	//step1 rx FE FE EF EE E1 26 98 00 00 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 34 33 34 45 00 00 FD
	//step2 tx FE FE EE EF E3 26 98 00 00 FD 写频指令
	//step2 rx FE FE EF EE E6 00 FD 回复写频
	//step3 tx FE FE EE EF E4 30 30 30 30 32 30 30 34 34 35 39 36 30 32 30 34 34 35 39 36 30 32 46 46 30 46 46 46 30 46 30 30 30 30 30 36 32 30 32 30 32 30 32 30 32 30 32 30 41 34 43 42 39 37 30 32 41 34 43
	//step3 rx FE FE EF EE E6 00 FD 回复写频
	//step4 tx FE FE EE EF E5 26 98 00 00 FD 写频结束
	//step4 rx FE FE EF EE E6 00 FD 回复写频
}

void uart_writecode_port_open(void)
{
	while (0 != ql_uart_open(QL_USB_CDC_PORT, QL_UART_BAUD_115200, QL_FC_NONE))
	{
		_DEBUG("ql_uart_open Faild!!!\r\n");
	}
	ql_uart_register_cb(QL_USB_CDC_PORT, uart_writecode_callback);
}

void uart_writecode_port_close(void)
{
	ql_uart_close(QL_USB_CDC_PORT);
}
