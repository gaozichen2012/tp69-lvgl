#ifndef __UART_H__
#define __UART_H__

typedef enum
{
	WRITE_CODE_CMD_NO,
	WRITE_CODE_CMD_VER,
	WRITE_CODE_CMD_APN_ID,
	WRITE_CODE_CMD_APN_PWD,
	WRITE_CODE_CMD_POC_ID,
	WRITE_CODE_CMD_POC_PWD,
	WRITE_CODE_CMD_POC_IP,
	WRITE_CODE_CMD_DEALERS_PWD,
	WRITE_CODE_CMD_GPS_ENABLE,
	WRITE_CODE_CMD_OUTLINE_SOUND,
	WRITE_CODE_CMD_CALL_ENABLE,
	WRITE_CODE_PARA_RUN,
	WRITE_CODE_END,
} WRITE_CODE_CMD;

#define GPS_CMD_MAX_BUF_LENGTH 1024

void uart_gps_init(void);
void uart_writecode_port_open(void);
void uart_writecode_reply(char *pCmd);
void uart_writecode_send_data(unsigned char *pData, unsigned char ucLen);
unsigned char check_sum(unsigned char *msg, unsigned short msglen);
int uart_set_poc_cfg_to_file(void);
int uart_set_analog_cfg_to_file(void);

void writecode_rx_handle_for_fsk(unsigned char rx_buf, unsigned short rx_len);
#endif
