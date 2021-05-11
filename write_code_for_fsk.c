#define WRITE_CODE
#include "write_code.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Uart.h"
#include "config.h"
#include "ql_rtos.h"
#include "ql_power.h"
#include "ql_fs.h"
#include "ql_uart.h"
#include "analog_old_patch.h"
#include "analog_test.h"
#include "analog_model.h"
#include "general_include.h"
#include "state_init.h"
#if DEBUG_EN
#if DEBUG_TO_USB
#define _DEBUG(fmtString, ...) CPUartLogPrintf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...) printf(fmtString, ##__VA_ARGS__)
#endif
#else
#define _DEBUG(fmtString, ...)
#endif

extern void ql_norflash_get_addr(unsigned char *patition_name, unsigned int *start_addr, unsigned int *size);
extern unsigned int ql_norflash_do_read(unsigned char *patition_name, unsigned int addr, unsigned int buf_addr, unsigned int size);
extern unsigned int ql_norflash_do_write(unsigned char *patition_name, unsigned int addr, unsigned int buf_addr, unsigned int size);
extern int ql_norflash_do_erase(unsigned char *patition_name, unsigned int addr, unsigned int size);

static void write_code_analog_handle(void);
static void write_code_poc_handle(void);

void write_code_init(void)
{
    wc_data.write_code_mode = 0;
    wc_data.test_mode = 0;
    wc_data.ang_tx_flg = 0;
}

void answer_handshake(void)
{
    //FE FE EF EE E1 26 98 00 00 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 34 33 34 45 00 00 FD
    unsigned char answer_cmd_handshake[36] = {0xFE, 0xFE, 0xEF, 0xEE, 0xE1, 0x26, 0x98, 0x00, 0x00,
                                              0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
                                              0x34, 0x33,
                                              0x34, 0x45,
                                              0x00, 0x00,
                                              0xFD}; //进入公网模式

    //被遥晕(K 0X4B) 遥毙(S 0X53) 正常(N 0X4E) 2byte
    if (1 == GbRdKilled) //遥毙时
    {
        answer_cmd_handshake[31] = 0X35;
        answer_cmd_handshake[32] = 0X33;
    }
    else if (1 == GbRdStuned) //遥晕
    {
        answer_cmd_handshake[31] = 0X34;
        answer_cmd_handshake[32] = 0X42;
    }
    else
    {
        answer_cmd_handshake[31] = 0X34;
        answer_cmd_handshake[32] = 0X45;
    }

    uart_writecode_send_data(answer_cmd_handshake, 36);
}

void answer_wrcode_end(void)
{
    unsigned char answer_cmd_wrcode_end[7] = {0xFE, 0xFE, 0xEF, 0xEE, 0xE6, 0x00, 0xFD}; //读频/写频结束

    //FE FE EF EE E6 00 FD //读频/写频结束
    uart_writecode_send_data(answer_cmd_wrcode_end, 7);
    _DEBUG("write code: analog-send FE FE EF EE E6 00 FD!\r\n");
}

void write_code_process(void)
{
    unsigned char rx_cmd_enter_netmode[13] = {0xFE, 0xFE, 0xEE, 0xEF, 0xFA, 0x26, 0x98, 0x3F, 0x00, 0x07, 0x30, 0x39, 0xFD}; //进入公网模式

    if (memcmp(wc_data.rx_buf, rx_cmd_enter_netmode, 13) == 0) //设置
    {
        wc_data.write_code_mode = 1;
        uart_writecode_send_data((unsigned char *)"AT+ENTER:4G-MODE\r\n", strlen("AT+ENTER:4G-MODE\r\n"));
        _DEBUG("write code: analog-send AT+ENTER:4G-MODE!\r\n");
    }
    else
    {
        if (wc_data.write_code_mode == 0) //模拟
        {
            write_code_analog_handle();
        }
        else //公网
        {
            _DEBUG("write code: net!\r\n");
            write_code_poc_handle();
        }
    }
}

static void write_code_analog_handle(void)
{
    if (wc_data.rx_buf[5] == 0x26 ||
        wc_data.rx_buf[6] == 0x98)
    {
        _DEBUG("write code: analog-26 98!\r\n");
        if (wc_data.rx_len == 10)
        {
            //FE FE EE EF E0 26 98 00 00 FD (握手信号)
            //FE FE EE EF E2 26 98 00 00 FD (读频指令)
            //FE FE EE EF E3 26 98 00 00 FD (写频指令)
            //FE FE EE EF E5 26 98 00 00 FD (写频结束指令)
            switch (wc_data.rx_buf[4])
            {
            case 0XE0: //handshake

                //RX:FE FE EE EF E0 26 98 00 00 FD
                //TX:FE FE EF EE E1 26 98 00 00 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 34 33 34 45 00 00 FD
                //TX:FE FE EF EE E1 26 98 00 00 +信息(20字节)+读写频密码(4字节)+cheSum(2字节)+FD
                answer_handshake();
                wc_data.wr_flag = 0;
                break;
            case 0XE2: //read
                answer_wrcode_end();
                wc_data.wr_flag = 2;
                break;
            case 0XE3: //write
                answer_wrcode_end();
                wc_data.wr_flag = 1;
                break;
            case 0XE5: //end write and read
                answer_wrcode_end();
                if (wc_data.wr_flag == 1) //write
                {
                    uart_set_analog_cfg_to_file();
                    init_get_analog_cfg_from_file();
                    wc_data.wr_flag = 0;
                }
                else
                {

                }

#if 0
                ql_rtos_task_sleep_s(2);
                ql_power_reset();
                while (1)
                    ql_rtos_task_sleep_s(1);
#endif
                break;

            default:
                break;
            }
        }
        else if (wc_data.rx_len == 11)
        {
            //FE FE EE EF E6 26 98 00 00 01 FD (改变速率指令)
            if (wc_data.rx_buf[4] == 0xE6)
            {
                /* code */
            }
        }
        else if (wc_data.rx_len == 13)
        {
            //测试模式
            TestModCommandPro();
        }
        else
        {
            _DEBUG("write code: analog-error1!\r\n");
        }
    }
    else if (wc_data.rx_len == 12)
    {
        if (wc_data.rx_buf[4] == 0xEB)
        {
            //FE FE EE EF EB 30 30 30 30 32 30 FD (读数据指令)
            ang_rcode_handle();
        }
    }
    else if (wc_data.rx_len == 14)
    {
        //FE FE EE EF E7 30 30 30 30 30 30 30 30 FD (密码验证指令)
        _DEBUG("write code: analog-FE FE EE EF E7 30 30 30 30 30 30 30 30 FD!\r\n");
    }
    else if (wc_data.rx_len == ConWribyte)
    {
        ang_wcode_handle();
    }
    else
    {
        /* code */
    }
}

static void write_code_poc_handle(void)
{
#if 1 //原

    if (memcmp(wc_data.rx_buf, "AT+SET=", 7) == 0) //设置
    {
        unsigned char dealers_pwd_len = *(wc_data.rx_buf + 7);

        if (memcmp(wc_data.rx_buf + 8, PWD_SUPER_USER, strlen(PWD_SUPER_USER)) == 0 || memcmp(wc_data.rx_buf + 8, iPocSet.agentpwd, strlen(iPocSet.agentpwd)) == 0)
        {
            //校验
            if (*(wc_data.rx_buf + wc_data.rx_len - 3) != check_sum((unsigned char *)(wc_data.rx_buf + 10 + dealers_pwd_len), *(wc_data.rx_buf + 9 + dealers_pwd_len)) && *(wc_data.rx_buf + 9 + dealers_pwd_len) > 0)
            {
                unsigned char error_code, dat = 1;
                memcpy(wc_data.rx_buf + 3, "ERR", 3);
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                error_code = 0x00;
                uart_writecode_send_data(&dat, 1);
                uart_writecode_send_data(&error_code, 1);
                uart_writecode_send_data(&error_code, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
                return;
            }
            //write_code_status = WRITE_CODE_WRITE_CFG;
            if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_APN_ID)
            {
                memset(iPocSet.apnacc, 0, sizeof(iPocSet.apnacc));
                memcpy(iPocSet.apnacc, wc_data.rx_buf + 10 + dealers_pwd_len, (*(wc_data.rx_buf + 9 + dealers_pwd_len)) % (sizeof(iPocSet.apnacc)));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");

                _DEBUG("apn account:%s\r\n", iPocSet.apnacc);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_APN_PWD)
            {
                memset(iPocSet.apnpwd, 0, sizeof(iPocSet.apnpwd));
                memcpy(iPocSet.apnpwd, wc_data.rx_buf + 10 + dealers_pwd_len, (*(wc_data.rx_buf + 9 + dealers_pwd_len)) % (sizeof(iPocSet.apnpwd)));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");

                _DEBUG("apn pwd:%s\r\n", iPocSet.apnpwd);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_POC_ID)
            {
                memset(iPocSet.account, 0, sizeof(iPocSet.account));
                memcpy(iPocSet.account, wc_data.rx_buf + 10 + dealers_pwd_len, (*(wc_data.rx_buf + 9 + dealers_pwd_len)) % (sizeof(iPocSet.account)));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_POC_PWD)
            {
                memset(iPocSet.passwd, 0, sizeof(iPocSet.passwd));
                memcpy(iPocSet.passwd, wc_data.rx_buf + 10 + dealers_pwd_len, (*(wc_data.rx_buf + 9 + dealers_pwd_len)) % (sizeof(iPocSet.passwd)));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_POC_IP)
            {
                memset(iPocSet.ip, 0, sizeof(iPocSet.ip));
                memcpy(iPocSet.ip, wc_data.rx_buf + 10 + dealers_pwd_len, (*(wc_data.rx_buf + 9 + dealers_pwd_len)) % (sizeof(iPocSet.ip)));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_DEALERS_PWD)
            {
                memset(iPocSet.agentpwd, 0, sizeof(iPocSet.agentpwd));
                memcpy(iPocSet.agentpwd, wc_data.rx_buf + 10 + dealers_pwd_len, (*(wc_data.rx_buf + 9 + dealers_pwd_len)) % (sizeof(iPocSet.agentpwd)));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_GPS_ENABLE)
            {
                iPocSet.gps_sw = 0;
                memcpy(&iPocSet.gps_sw, wc_data.rx_buf + 10 + dealers_pwd_len, sizeof(iPocSet.gps_sw));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_OUTLINE_SOUND)
            {
                iPocSet.offlineFlg = 0;
                memcpy(&iPocSet.offlineFlg, wc_data.rx_buf + 10 + dealers_pwd_len, sizeof(iPocSet.offlineFlg));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_CALL_ENABLE)
            {
                iPocSet.inviteFlg = 0;
                memcpy(&iPocSet.inviteFlg, wc_data.rx_buf + 10 + dealers_pwd_len, sizeof(iPocSet.inviteFlg));
                uart_writecode_reply((char *)wc_data.rx_buf);
                ;
                uart_writecode_reply("\r\n");
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == 0x0b)
            {
                iPocSet.ucValidFlg = POC_SETING_VALID_FLG;
                iPocSet.uiApnValidFlg = 0;

                uart_set_poc_cfg_to_file();

                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, wc_data.rx_len);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);

                ql_rtos_task_sleep_s(2);

                ql_power_reset();
                while (1)
                    ql_rtos_task_sleep_s(1);
            }

            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == 0x0C)
            {
                unsigned int a, b;
                unsigned int addr, size;

                ql_norflash_get_addr((unsigned char *)"splash_fs", &addr, &size);

                a = (*(wc_data.rx_buf + 8 + dealers_pwd_len + 2) * 256 + *(wc_data.rx_buf + 8 + dealers_pwd_len + 3)) % 400;
                b = (*(wc_data.rx_buf + 8 + dealers_pwd_len + 4) * 256 + *(wc_data.rx_buf + 8 + dealers_pwd_len + 5)) % 400;

                //Flash_Updata(START_LOGO_ADDRESS+128*a, wc_data.rx_buf+8+dealers_pwd_len+6, 128);
                if (a == 0)
                {
                    ql_norflash_do_erase((unsigned char *)"splash_fs", addr, b * 128);
                    ql_rtos_task_sleep_ms(200);
                }
                ql_norflash_do_write((unsigned char *)"splash_fs", addr + 128 * a, (unsigned int)(wc_data.rx_buf + 8 + dealers_pwd_len + 6), 128);

                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, wc_data.rx_len);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
                /*
				if(a+1 == b || a >= 400)	
				{
					write_code_status = WRITE_CODE_WRITE_UI;
				}*/
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == 0x0D)
            {
                memset(iPocSet.apnname, 0, 32);
                memcpy(iPocSet.apnname, wc_data.rx_buf + 10 + dealers_pwd_len, (*(wc_data.rx_buf + 9 + dealers_pwd_len)) % (sizeof(iPocSet.apnname)));
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, wc_data.rx_len);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);

                _DEBUG("apn name:%s\r\n", iPocSet.apnname);
            }
            else
            {
                unsigned char error_code, dat = 1;
                memcpy(wc_data.rx_buf + 3, "ERR", 3);
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                error_code = 0x03;
                uart_writecode_send_data(&dat, 1);
                uart_writecode_send_data(&error_code, 1);
                uart_writecode_send_data(&error_code, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
        }
        else
        {
            unsigned char error_code, dat = 1;
            memcpy(wc_data.rx_buf + 3, "ERR", 3);
            uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
            error_code = 0x01;
            uart_writecode_send_data(&dat, 1);
            uart_writecode_send_data(&error_code, 1);
            uart_writecode_send_data(&error_code, 1);
            uart_writecode_send_data((unsigned char *)"\r\n", 2);
        }
    }
    else if (memcmp(wc_data.rx_buf, "AT+RED=", 7) == 0) //读取
    {
        unsigned char dealers_pwd_len = *(wc_data.rx_buf + 7);
        if (memcmp(wc_data.rx_buf + 8, PWD_SUPER_USER, strlen(PWD_SUPER_USER)) == 0 || memcmp(wc_data.rx_buf + 8, iPocSet.agentpwd, strlen(iPocSet.agentpwd)) == 0) //经销商密码
        {
            //write_code_status = WRITE_CODE_READ_CFG;
            if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_VER) //版本号
            {
                unsigned char check, lenth;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                lenth = strlen(MCU_VERSION);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)MCU_VERSION, lenth);
                check = check_sum((unsigned char *)MCU_VERSION, lenth);
                uart_writecode_send_data(&check, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_APN_ID)
            {
                unsigned char check, lenth;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                lenth = strlen(iPocSet.apnacc);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)iPocSet.apnacc, lenth);
                check = check_sum((unsigned char *)iPocSet.apnacc, lenth);
                uart_writecode_send_data(&check, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_APN_PWD)
            {
                unsigned char check, lenth;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                lenth = strlen(iPocSet.apnpwd);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)iPocSet.apnpwd, lenth);
                check = check_sum((unsigned char *)iPocSet.apnpwd, lenth);
                uart_writecode_send_data(&check, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_POC_ID)
            {
                unsigned char check, lenth;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                lenth = strlen(iPocSet.account);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)iPocSet.account, lenth);
                check = check_sum((unsigned char *)iPocSet.account, lenth);
                uart_writecode_send_data(&check, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_POC_PWD)
            {
                unsigned char check, lenth;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                lenth = strlen((char *)iPocSet.passwd);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)iPocSet.passwd, lenth);
                check = check_sum((unsigned char *)iPocSet.passwd, lenth);
                uart_writecode_send_data(&check, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_POC_IP)
            {
                unsigned char check, lenth;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                lenth = strlen(iPocSet.ip);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)iPocSet.ip, lenth);
                check = check_sum((unsigned char *)iPocSet.ip, lenth);
                uart_writecode_send_data(&check, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_DEALERS_PWD)
            {
                unsigned char check, lenth;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                lenth = strlen(iPocSet.agentpwd);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)iPocSet.agentpwd, lenth);
                check = check_sum((unsigned char *)iPocSet.agentpwd, lenth);
                uart_writecode_send_data(&check, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_GPS_ENABLE)
            {
                unsigned char lenth = 1;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                uart_writecode_send_data(&lenth, 1);

                if (iPocSet.gps_sw == 0)
                    iPocSet.gps_sw = '0';

                uart_writecode_send_data((unsigned char *)&iPocSet.gps_sw, 1);
                uart_writecode_send_data((unsigned char *)&iPocSet.gps_sw, 1);

                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_OUTLINE_SOUND)
            {
                unsigned char lenth = 1;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)&iPocSet.offlineFlg, 1);
                uart_writecode_send_data((unsigned char *)&iPocSet.offlineFlg, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == WRITE_CODE_CMD_CALL_ENABLE)
            {
                unsigned char lenth = 1;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)&iPocSet.inviteFlg, 1);
                uart_writecode_send_data((unsigned char *)&iPocSet.inviteFlg, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }

            else if (*(wc_data.rx_buf + 8 + dealers_pwd_len) == 0x0D)
            {
                unsigned char check, lenth;
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                lenth = strlen((char *)iPocSet.apnname);
                uart_writecode_send_data(&lenth, 1);
                uart_writecode_send_data((unsigned char *)iPocSet.apnname, lenth);
                check = check_sum((unsigned char *)iPocSet.apnname, lenth);
                uart_writecode_send_data(&check, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
            else
            {
                unsigned char error_code, dat = 1;
                memcpy(wc_data.rx_buf + 3, "ERR", 3);
                uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
                error_code = 0x03;
                uart_writecode_send_data(&dat, 1);
                uart_writecode_send_data(&error_code, 1);
                uart_writecode_send_data(&error_code, 1);
                uart_writecode_send_data((unsigned char *)"\r\n", 2);
            }
        }
        else
        {
            unsigned char error_code, dat = 1;
            memcpy(wc_data.rx_buf + 3, "ERR", 3);
            uart_writecode_send_data((unsigned char *)wc_data.rx_buf, 9 + dealers_pwd_len);
            error_code = 0x01;
            uart_writecode_send_data(&dat, 1);
            uart_writecode_send_data(&error_code, 1);
            uart_writecode_send_data(&error_code, 1);
            uart_writecode_send_data((unsigned char *)"\r\n", 2);
        }
    }
#endif
}