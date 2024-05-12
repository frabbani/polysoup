#pragma once

#ifndef _USEC_TIMER_H_
#define _USEC_TIMER_H_

#include "defs.h"

//usec "microseconds" timer

#ifdef _WIN32
typedef struct usec_timer_s {

  int64 bpc, epc;
  int64 avg_usecs, tot_usecs, diff_usecs, cycles;
} usec_timer_t;

#else
typedef struct usec_timer_s{
  struct timeval btv, etv;
  long long avg_usecs, tot_usecs, diff_usecs, cycles;
}usec_timer_t;

#endif

void usec_timer_init( usec_timer_t *t );
void usec_timer_start( usec_timer_t *t );
void usec_timer_stop( usec_timer_t *t );
void usec_timer_step( usec_timer_t *t );

#define USEC_TIMER_AVG_SECS( t ) ( (float)(1e6 / (double)(t).avg_usecs ) )
#define USEC_TIMER_DIF_SECS( t ) ( (float)( (double)(t).diff_usecs / 1e6 ) )

#define USEC_TIMER_AVG_MS( t ) ( (float)(1e3 / (double)(t).avg_usecs ) )
#define USEC_TIMER_DIF_MS( t ) ( (float)( (double)(t).diff_usecs / 1e3 ) )

#endif
