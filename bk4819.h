#ifndef __BK4819_H__
#define __BK4819_H__

#ifdef BK4819
#define BK4819_EXTERN
#else
#define BK4819_EXTERN extern
#endif

BK4819_EXTERN void bk4819_test(void);
BK4819_EXTERN void bk4819_init(void);
BK4819_EXTERN void bk4819_vtx_gpio1_output(unsigned char state);
BK4819_EXTERN void bk4819_HL_gpio0_output(unsigned char state);
BK4819_EXTERN void bk4819_off(void);
BK4819_EXTERN void bk4819_sleep_wake_tim_handle(void);
BK4819_EXTERN void bk4819_set_freq(unsigned int freq);

BK4819_EXTERN void bk4819_set_cdcss(unsigned int num);
BK4819_EXTERN void bk4819_set_tx_power(unsigned char level);
BK4819_EXTERN void bk4819_set_channel_spacing(unsigned char level);

#endif