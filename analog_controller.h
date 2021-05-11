#ifndef __ANALOG_CONTROLLER_H__
#define __ANALOG_CONTROLLER_H__

typedef enum
{
    ANALOG_RX_START,
    ANALOG_RX_END,
    ANALOG_FREQSCAN_FREQ_SUCCESS,
    ANALOG_FREQSCAN_SUBAUDIO_IDLE,
    ANALOG_FREQSCAN_SUBAUDIO_CTC,
    ANALOG_FREQSCAN_SUBAUDIO_CDC
} enAnalogValue;

#ifdef ANALOG_C
#define ANALOG_C_EXTERN
#else
#define ANALOG_C_EXTERN extern
#endif

ANALOG_C_EXTERN void toa_process(void);
ANALOG_C_EXTERN void toa_1s_process(void);
ANALOG_C_EXTERN u8 look_up_table(u16 value, u16 *item, u8 max_item_num);
ANALOG_C_EXTERN void rx_ctcss_and_cdcss_handle(void);
ANALOG_C_EXTERN void tx_ctcss_and_cdcss_set(void);
ANALOG_C_EXTERN void tx_ctcss_and_cdcss_reset(void);

ANALOG_C_EXTERN void tx_power_handle(void);
ANALOG_C_EXTERN void channel_spacing_handle(void);

ANALOG_C_EXTERN void ang_all_time(void);
ANALOG_C_EXTERN void alltime_remove_tail_audio(unsigned char sub_audio_mode);
ANALOG_C_EXTERN unsigned char alltime_detect_subaudio_match(unsigned char sub_audio_mode, unsigned char cdcss_direction);
ANALOG_C_EXTERN void alltime_detect_sq(void);
ANALOG_C_EXTERN void alltime_detect_tail(void);
ANALOG_C_EXTERN void sq_and_subaudio_match_handle(void);
ANALOG_C_EXTERN void freqmatch_freq_scan_handle(void);
ANALOG_C_EXTERN void fsk_handle(void);
ANALOG_C_EXTERN void release_analog_ptt_handle(void);
ANALOG_C_EXTERN void Rd1846NoCtcssTxTailPro(void);
#endif
