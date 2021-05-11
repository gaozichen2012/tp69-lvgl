#include "ql_application.h"
#include "ql_rtos.h"
#include "ql_power.h"
#include "ql_fs.h"
#include "ql_rtc.h"

#include "netdb.h"
#include "sockets.h"

#include "Uart.h"
#include "general_include.h"
#include "at.h"

// #include "version.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
extern int iWakelockFd;
extern unsigned char ucPocReady;

int iAtNetFd;
struct sockaddr_in gsin;
ql_task_t at_poc_task_ref;

void at_send_at_cmd(unsigned char *pCmd, unsigned char ucLeng)
{
	_DEBUG("---->Mcu Send At:%s \r\n", pCmd);
	sendto(iAtNetFd, pCmd, ucLeng + 1, 0, (struct sockaddr *)&gsin, sizeof(struct sockaddr_in));
}

void at_push_cmd_msg(char *pBuf, unsigned short us_Len)
{
	ZPOC_MSG_TYPE iMsg;
	iMsg.enMsgType = ZPOC_MSG_AT_RECV;
	iMsg.pMsg = malloc(us_Len + 1);
	iMsg.usMsgLen = us_Len + 1;
	memset(iMsg.pMsg, 0, us_Len + 1);
	strncpy(iMsg.pMsg, pBuf, us_Len);

	ql_rtos_queue_release(msqid, sizeof(iMsg), (unsigned char *)&iMsg, QL_WAIT_FOREVER);
}

void at_poc_recv_process(void *context)
{
	char cAtRecvBuf[AT_BUF_SIZE];
	int recvLeng;
	int sin_len = sizeof(struct sockaddr_in);

	while (1)
	{
		memset(cAtRecvBuf, 0, sizeof(cAtRecvBuf));
		recvLeng = recvfrom(iAtNetFd, (void *)cAtRecvBuf, AT_BUF_SIZE, 0, (struct sockaddr *)&gsin, (socklen_t *)&sin_len);

		if (recvLeng < 0 || recvLeng > AT_BUF_SIZE)
		{
			_DEBUG("Invalid AT packet!!\r\n");
			continue;
		}

		ucPocReady = 1;

		_DEBUG("<-------Mcu Recv Leng:%d, Data:%s\r\n", recvLeng, cAtRecvBuf);
		recvLeng = strlen(cAtRecvBuf);
		if (recvLeng > 3)
		{
			at_push_cmd_msg(cAtRecvBuf, recvLeng);
		}
	}
}

int at_recv_thread_create(void)
{
	struct sockaddr_in sin_local;

	_DEBUG("AT Init Net Port!\r\n");

	memset(&gsin, 0, sizeof(sin_local));
	gsin.sin_family = AF_INET;
	gsin.sin_addr.s_addr = inet_addr("127.0.0.1");
	;
	gsin.sin_port = htons(QUEC_AT_NET_PORT);

	sin_local.sin_family = AF_INET;
	sin_local.sin_port = htons(ql_soc_generate_port());
	sin_local.sin_addr.s_addr = inet_addr("127.0.0.1");

	iAtNetFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (iAtNetFd != -1)
	{
		setsockopt(iAtNetFd, SOL_SOCKET, SO_REUSEADDR, 0, sizeof(0));

		if (bind(iAtNetFd, (struct sockaddr *)&sin_local, sizeof(sin_local)) == -1)
		{
			_DEBUG("AT UDP bind socket faild!!!");
			close(iAtNetFd);
		}
	}
	else
	{
		_DEBUG("AT UDP socket create faild!!!");
	}

	return ql_rtos_task_create(&at_poc_task_ref, 4096, 74, "at_poc_recv_process", at_poc_recv_process, NULL);
}
