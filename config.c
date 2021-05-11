// #include "version.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ql_application.h"
#include "ql_rtos.h"
#include "ql_power.h"
#include "ql_fs.h"
#include "ql_rtc.h"

#include "Uart.h"

#include "at.h"
#include "config.h"
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

unsigned int uiRecValidFlg = 0xa5a5a5a5;
unsigned char ucRecChgFlg = 0;

unsigned char cfg_inv_record_load(void)
{
	QFILE *fd;
	int iRdCnt;
	unsigned int uiRecValidTmp;

	fd = ql_fopen(MCU_INV_RECD_PATH, "r");
	if (fd < 0)
	{
		_DEBUG("Record file:%s is not exit, create and init seting!\r\n", MCU_INV_RECD_PATH);

		fd = ql_fopen(MCU_INV_RECD_PATH, "w");
		if (fd < 0)
		{
			_DEBUG("Create Record file:%s faild, return -1!\r\n", MCU_INV_RECD_PATH);
			return -1;
		}

		if (0 >= ql_fwrite(&uiRecValidFlg, sizeof(uiRecValidFlg), 1, fd))
		{
			_DEBUG("Write Record valid flag:%s faild, return -1!\r\n", MCU_INV_RECD_PATH);
			ql_fclose(fd);
			return -1;
		}

		ql_fclose(fd);
		return 0;
	}
	else
	{
		iRdCnt = ql_fread(&uiRecValidTmp, sizeof(uiRecValidTmp), 1, fd);

		if (sizeof(uiRecValidTmp) != iRdCnt || uiRecValidTmp != uiRecValidFlg)
		{
			_DEBUG("Read Size or Record Valid:%s file CRC Error, reset all!\r\n", MCU_INV_RECD_PATH);
			ql_fclose(fd);

			fd = ql_fopen(MCU_INV_RECD_PATH, "w");
			if (fd < 0)
			{
				_DEBUG("Create Record file:%s faild, return -1!\r\n", MCU_INV_RECD_PATH);
				return -1;
			}

			if (0 >= ql_fwrite(&uiRecValidFlg, sizeof(uiRecValidFlg), 1, fd))
			{
				_DEBUG("Write Record valid flag:%s faild, return -1!\r\n", MCU_INV_RECD_PATH);
				ql_fclose(fd);
				return -1;
			}
		}
		else
		{
			struct stInvRecNode iInvRecNode;
			iRdCnt = ql_fread(&iInvRecNode, sizeof(struct stInvRecNode), 1, fd);
			while (iRdCnt < 0)
			{
				cfg_inv_record_add(iInvRecNode.ucId, iInvRecNode.uname, iInvRecNode.ucSta, iInvRecNode.time);
				iRdCnt = ql_fread(&iInvRecNode, sizeof(struct stInvRecNode), 1, fd);
			}
		}
	}

	ucRecChgFlg = 0;
	ql_fclose(fd);
	return 0;
}

struct stInvRecNode *cfg_inv_record_get(unsigned short usIdx)
{
	unsigned short i = 0;
	struct stInvRecNode *pInvRecNode = iInvRecList.pHead;

	while (pInvRecNode != NULL)
	{
		if (i == usIdx)
			return pInvRecNode;

		pInvRecNode = pInvRecNode->pNext;
		i++;
	}
	return pInvRecNode;
}

unsigned char cfg_inv_record_add(unsigned char *pUid, unsigned char *pName, unsigned char ucSta, unsigned char *pTime)
{
	ql_rtc_time_t tm;
	ql_rtc_get_time(&tm);

	struct stInvRecNode *pInvRecNode = (struct stInvRecNode *)malloc(sizeof(struct stInvRecNode));

	ucRecChgFlg = 1;

	if (pTime == NULL)
	{
		sprintf((char *)pInvRecNode->time, "%d-%02d-%02d %02d:%02d:%02d", 1970 + (tm.tm_year), 1 + (tm.tm_mon), tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}
	else
	{
		strncpy((char *)pInvRecNode->time, (char *)pTime, 20);
	}

	if (pUid == NULL)
	{
		unsigned char *pFindUid = zpoc_get_member_id_by_name(pName);

		if (pFindUid != NULL)
			memcpy(pInvRecNode->ucId, pFindUid, 8);
		else
			return -1;
	}
	else
	{
		memcpy(pInvRecNode->ucId, pUid, 8);
	}

	zpoc_name_copy((unsigned char *)pInvRecNode->uname, pName);
	pInvRecNode->ucSta = ucSta;

	if (iInvRecList.pHead == NULL)
	{
		pInvRecNode->pNext = NULL;
		pInvRecNode->pPrev = NULL;

		iInvRecList.pHead = pInvRecNode;
		iInvRecList.pTail = iInvRecList.pHead;
	}
	else
	{
		iInvRecList.pHead->pPrev = pInvRecNode;
		pInvRecNode->pNext = iInvRecList.pHead;
		iInvRecList.pHead = pInvRecNode;
		iInvRecList.pHead->pPrev = NULL;
	}

	if (iInvRecList.usTotal >= ZPOC_RECORD_MAX_ITEM)
	{
		if (iInvRecList.pTail != NULL)
		{
			struct stInvRecNode *pRec = iInvRecList.pTail;
			iInvRecList.pTail = iInvRecList.pTail->pPrev;
			free(pRec);
		}
	}
	else
	{
		iInvRecList.usTotal++;
	}

	return 0;
}

int cfg_net_read_mcu_seting(stBaseSeting *pBaseSeting)
{
	QFILE *fd;
	int iRdCnt;

	fd = ql_fopen(MCU_CFG_NET_PATH, "r");
	if (fd < 0)
	{
		_DEBUG("Config file:%s is not exit, create and init seting!\r\n", MCU_CFG_NET_PATH);

		write_flash_net_seting_default();

		*pBaseSeting = net_seting_flash.iSeting;

		fd = ql_fopen(MCU_CFG_NET_PATH, "w");
		if (fd < 0)
		{
			_DEBUG("Create Config file:%s faild, return -1!\r\n", MCU_CFG_NET_PATH);
			return -1;
		}

		if (0 >= ql_fwrite(&net_seting_flash, sizeof(net_seting_flash), 1, fd))
		{
			_DEBUG("Write New Config Seting:%s faild, return -1!\r\n", MCU_CFG_NET_PATH);
			ql_fclose(fd);
			return -1;
		}

		ql_fclose(fd);
		return 0;
	}
	else
	{
		iRdCnt = ql_fread(&net_seting_flash, sizeof(net_seting_flash), 1, fd);
		if (0 >= iRdCnt || net_seting_flash.usCrc != MCU_CFG_VALID_VALUE)
		{
			_DEBUG("Read Size or Config Seting:%s file CRC Error, reset all!\r\n", MCU_CFG_NET_PATH);

			write_flash_net_seting_default();

			*pBaseSeting = net_seting_flash.iSeting;

			ql_fclose(fd);

			QFILE *fd1;
			fd1 = ql_fopen(MCU_CFG_NET_PATH, "w");
			if (0 >= ql_fwrite(&net_seting_flash, sizeof(net_seting_flash), 1, fd1))
			{
				_DEBUG("Write Reset Config Seting:%s faild, return -1!\r\n", MCU_CFG_NET_PATH);
				ql_fclose(fd1);
				return -1;
			}
			ql_fclose(fd1);
		}
		else
		{
			*pBaseSeting = net_seting_flash.iSeting;
			ql_fclose(fd);
		}
	}

	return 0;
}

int cfg_net_write_mcu_seting(stBaseSeting *pBaseSeting)
{
	QFILE *fd;

	if (pBaseSeting->ucBLTime != net_seting_flash.iSeting.ucBLTime ||
		pBaseSeting->ucKBLock != net_seting_flash.iSeting.ucKBLock ||
		pBaseSeting->ucNetMode != net_seting_flash.iSeting.ucNetMode ||
		pBaseSeting->ucOnOffTone != net_seting_flash.iSeting.ucOnOffTone ||
		pBaseSeting->ucPortMode != net_seting_flash.iSeting.ucPortMode ||
		pBaseSeting->ucWorkMode != net_seting_flash.iSeting.ucWorkMode ||
		pBaseSeting->ucPttTone != net_seting_flash.iSeting.ucPttTone ||
		pBaseSeting->ucVolLvl != net_seting_flash.iSeting.ucVolLvl ||
		pBaseSeting->ucRecordSw != net_seting_flash.iSeting.ucRecordSw ||
		pBaseSeting->cTimeZone != net_seting_flash.iSeting.cTimeZone ||
		pBaseSeting->ucLanguage != net_seting_flash.iSeting.ucLanguage ||
		pBaseSeting->ucSendTime != net_seting_flash.iSeting.ucSendTime ||
		pBaseSeting->ucRxBL != net_seting_flash.iSeting.ucRxBL ||
		pBaseSeting->cTxGain != net_seting_flash.iSeting.cTxGain)
	{
		fd = ql_fopen(MCU_CFG_NET_PATH, "w");
		if (fd < 0)
		{
			_DEBUG("Open Write Config Seting:%s faild, return -1!\r\n", MCU_CFG_NET_PATH);
			return -1;
		}
		else
		{
			net_seting_flash.iSeting = *pBaseSeting;
			if (0 >= ql_fwrite(&net_seting_flash, sizeof(net_seting_flash), 1, fd))
			{
				_DEBUG("Write Save Config Seting:%s faild, return -1!\r\n", MCU_CFG_NET_PATH);
				ql_fclose(fd);
				return -1;
			}
		}

		ql_fclose(fd);
		return 0;
	}
	_DEBUG("Config Same, No need write:%s return 0!\r\n", MCU_CFG_NET_PATH);
	return 0;
}

int cfg_net_reset_mcu_seting(stBaseSeting *pBaseSeting)
{
	QFILE *fd;

	write_flash_net_seting_default();

	*pBaseSeting = net_seting_flash.iSeting;

	fd = ql_fopen(MCU_CFG_NET_PATH, "w");
	if (fd < 0)
	{
		_DEBUG("Reset Open Config file:%s faild, return -1!\r\n", MCU_CFG_NET_PATH);
		return -1;
	}

	if (0 >= ql_fwrite(&net_seting_flash, sizeof(net_seting_flash), 1, fd))
	{
		_DEBUG("Reset write Config Seting:%s faild, return -1!\r\n", MCU_CFG_NET_PATH);
		ql_fclose(fd);
		return -1;
	}

	ql_fclose(fd);
	return 0;
}

int cfg_ang_read_mcu_seting(analog_flash_t *ptoa_flash)
{
	QFILE *fd;
	int iRdCnt;

	fd = ql_fopen(MCU_CFG_ANG_PATH, "r");
	if (fd < 0)
	{
		_DEBUG("Config file:%s is not exit, create and init seting!\r\n", MCU_CFG_ANG_PATH);

		write_flash_ang_seting_default();

		*ptoa_flash = ang_seting_flash.toa_flash;

		fd = ql_fopen(MCU_CFG_ANG_PATH, "w");
		if (fd < 0)
		{
			_DEBUG("Create Config file:%s faild, return -1!\r\n", MCU_CFG_ANG_PATH);
			return -1;
		}

		if (0 >= ql_fwrite(&ang_seting_flash, sizeof(ang_seting_flash), 1, fd))
		{
			_DEBUG("Write New Config Seting:%s faild, return -1!\r\n", MCU_CFG_ANG_PATH);
			ql_fclose(fd);
			return -1;
		}

		ql_fclose(fd);
		return 0;
	}
	else
	{
		iRdCnt = ql_fread(&ang_seting_flash, sizeof(ang_seting_flash), 1, fd);
		if (0 >= iRdCnt || ang_seting_flash.usCrc != MCU_CFG_VALID_VALUE)
		{
			_DEBUG("Read Size or Config Seting:%s file CRC Error, reset all!\r\n", MCU_CFG_ANG_PATH);

			write_flash_ang_seting_default();

			*ptoa_flash = ang_seting_flash.toa_flash;

			ql_fclose(fd);

			QFILE *fd1;
			fd1 = ql_fopen(MCU_CFG_ANG_PATH, "w");
			if (0 >= ql_fwrite(&ang_seting_flash, sizeof(ang_seting_flash), 1, fd1))
			{
				_DEBUG("Write Reset Config Seting:%s faild, return -1!\r\n", MCU_CFG_ANG_PATH);
				ql_fclose(fd1);
				return -1;
			}
			ql_fclose(fd1);
		}
		else
		{
			*ptoa_flash = ang_seting_flash.toa_flash;
			ql_fclose(fd);
		}
	}

	return 0;
}

int cfg_ang_write_mcu_seting(analog_flash_t *ptoa_flash)
{
	QFILE *fd;

	if (ptoa_flash->freqscan_flag != ang_seting_flash.toa_flash.freqscan_flag ||
		ptoa_flash->freqscan_cdcss_freq != ang_seting_flash.toa_flash.freqscan_cdcss_freq ||
		ptoa_flash->sq_level != ang_seting_flash.toa_flash.sq_level ||
		ptoa_flash->step_freq != ang_seting_flash.toa_flash.step_freq ||
		ptoa_flash->tail_audio_type != ang_seting_flash.toa_flash.tail_audio_type ||
		ptoa_flash->tot_time != ang_seting_flash.toa_flash.tot_time ||
		ptoa_flash->save_power_level != ang_seting_flash.toa_flash.save_power_level ||
		ptoa_flash->vox_level != ang_seting_flash.toa_flash.vox_level)
	{
		fd = ql_fopen(MCU_CFG_ANG_PATH, "w");
		if (fd < 0)
		{
			_DEBUG("Open Write Config Seting:%s faild, return -1!\r\n", MCU_CFG_ANG_PATH);
			return -1;
		}
		else
		{
			ang_seting_flash.toa_flash = *ptoa_flash;
			if (0 >= ql_fwrite(&ang_seting_flash, sizeof(ang_seting_flash), 1, fd))
			{
				_DEBUG("Write Save Config Seting:%s faild, return -1!\r\n", MCU_CFG_ANG_PATH);
				ql_fclose(fd);
				return -1;
			}
		}

		ql_fclose(fd);
		return 0;
	}
	_DEBUG("Config Same, No need write:%s return 0!\r\n", MCU_CFG_ANG_PATH);
	return 0;
}

int cfg_ang_reset_mcu_seting(analog_flash_t *ptoa_flash)
{
	QFILE *fd;

	write_flash_ang_seting_default();

	*ptoa_flash = ang_seting_flash.toa_flash;

	fd = ql_fopen(MCU_CFG_ANG_PATH, "w");
	if (fd < 0)
	{
		_DEBUG("Reset Open Config file:%s faild, return -1!\r\n", MCU_CFG_ANG_PATH);
		return -1;
	}

	if (0 >= ql_fwrite(&ang_seting_flash, sizeof(ang_seting_flash), 1, fd))
	{
		_DEBUG("Reset write Config Seting:%s faild, return -1!\r\n", MCU_CFG_ANG_PATH);
		ql_fclose(fd);
		return -1;
	}

	ql_fclose(fd);
	return 0;
}