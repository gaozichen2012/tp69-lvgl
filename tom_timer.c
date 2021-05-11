//asr的定时器功能不可用，会导致异常重启
#define TOM_TIMER
#include "tom_timer.h"
#include "ql_type.h"
#include "ql_rtos.h"
#include "general_include.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bk4819.h"
#include "analog_controller.h"
#include "analog_model.h"
#include "analog_view.h"
#include "state.h"
#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

ql_task_t timer_10ms_task_ref;

static void timer_10ms_process(void *pData);

int timer_10ms_create(void)
{
    //
    return ql_rtos_task_create(&timer_10ms_task_ref, 2048, 100, "timer_10ms_process", timer_10ms_process, NULL);
}

static void timer_100ms_process(void)
{
    static unsigned char cnt = 0;

    if (cnt++ > 50)
    {
        cnt = 0;
        if (STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_NET ||
            STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_ANALOG ||
            STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_MIX ||
            STATE_MgrGetCurStateId() == POC_STATE_DESKTOP_REPEATER )
            analog_signal_disp(toa.rssi_level, 0);
    }
}

static void timer_10ms_process(void *pData)
{
    //ZPOC_MSG_TYPE iMsg;
    while (1)
    {
        ql_rtos_task_sleep_ms(10);
        timer_100ms_process();
        bk4819_sleep_wake_tim_handle();
        if (toa.freq_scan_flg == 1)
        {
            freqmatch_freq_scan_handle();
        }
        else if (toa.fsk_flg != 0)
        {
            fsk_handle();
        }
        else
        {
            toa_process();
        }

#if 0
        iMsg.enMsgType = ZPOC_MSG_TIMER;
        iMsg.pMsg = NULL;
        iMsg.usMsgLen = 0;

        //_DEBUG("Hb process send timer to queen!!\r\n");
        ql_rtos_queue_release(msqid, sizeof(iMsg), (unsigned char *)&iMsg, QL_WAIT_FOREVER);
#endif
    }
}
