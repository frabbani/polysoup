#include "timer.h"

#ifdef _WIN32
#include <windows.h>
double timer_freq = -1.0; //counter frequency (per second)
#else
#include <sys/time.h>
#endif

void usec_timer_init( usec_timer_t *t ) {
  t->avg_usecs = t->tot_usecs = t->cycles = 0;
#ifdef _WIN32
  if( -1.0 == timer_freq ){
    int64 query = 0;
    QueryPerformanceFrequency( (PLARGE_INTEGER)(void*)&query );
    timer_freq = (double)query;
  }
#endif

}

void usec_timer_start( usec_timer_t *t ) {
#ifdef _WIN32
  QueryPerformanceCounter( (PLARGE_INTEGER)(void*)&t->bpc );
#else
  gettimeofday( &t->btv, NULL );
#endif
}

void usec_timer_stop( usec_timer_t *t ) {
#ifdef WIN32
  QueryPerformanceCounter( (PLARGE_INTEGER)(void*)&t->epc );
  t->diff_usecs = (int64)((double)(t->epc - t->bpc) / timer_freq * 1.0e6);
#else
  gettimeofday( &t->etv, NULL );

  int dt_secs = t->etv.tv_sec - t->btv.tv_sec;
  int dt_usecs = t->etv.tv_usec - t->btv.tv_usec;

  t->diff_usecs = (long long)dt_secs*1000000 + (long long)dt_usecs;
#endif
  t->tot_usecs += t->diff_usecs;
  t->cycles++;
  t->avg_usecs = t->tot_usecs / t->cycles;
}

void usec_timer_step( usec_timer_t *t ) {
#ifdef _WIN32
  QueryPerformanceCounter( (PLARGE_INTEGER)(void*)&t->epc );
  t->diff_usecs = (int64)((double)(t->epc - t->bpc) / timer_freq * 1.0e6);
#else
  gettimeofday( &t->etv, NULL );
  int dt_secs = t->etv.tv_sec - t->btv.tv_sec;
  int dt_usecs = t->etv.tv_usec - t->btv.tv_usec;

  t->diff_usecs = (long long)dt_secs * 1000000 + (long long)dt_usecs;
#endif

  t->tot_usecs += t->diff_usecs;
  t->cycles++;
  t->avg_usecs = t->tot_usecs / t->cycles;
}

#define usec_timer_pop( t ) {  usec_timer_stop(t); usec_timer_start(t);  }

