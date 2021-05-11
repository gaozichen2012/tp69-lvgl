#ifndef __CHARGING_H__
#define __CHARGING_H__

typedef enum
{
	E_BAT_NOT_CHAR,
	E_BAT_CHARGING,
	E_BAT_CHAR_TRE
} E_BAT_CHARG_TYPE;

int charge_process_thread_create(void);


#endif
