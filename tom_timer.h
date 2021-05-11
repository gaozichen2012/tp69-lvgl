#ifndef __TOM_TIMER_H__
#define __TOM_TIMER_H__

#ifdef TOM_TIMER
#define TOM_TIMER_EXTERN
#else
#define TOM_TIMER_EXTERN extern
#endif

TOM_TIMER_EXTERN int timer_10ms_create(void);

#endif
