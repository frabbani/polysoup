#pragma once

#include "defs.h"

typedef struct{
  uint32   size;
  uint32   bool;
  uint32  *testbools;
}dirty_tracker_t;

extern void dirty_tracker_term( dirty_tracker_t *tracker );
extern void dirty_tracker_init( dirty_tracker_t *tracker, uint32 size );
extern void dirty_tracker_destructor( void *tracker );

extern INLINE void  dirty_tracker_next( dirty_tracker_t *tracker );
extern INLINE int32 dirty_tracker_isdirty( dirty_tracker_t *tracker, size_t index, int32 clean );
extern INLINE int32 dirty_tracker_isclean( const dirty_tracker_t *tracker, size_t index );
extern INLINE int32 dirty_tracker_clean( dirty_tracker_t *tracker, size_t index );
extern INLINE int32 dirty_tracker_dirty( dirty_tracker_t *tracker, size_t index );
extern INLINE void dirty_tracker_cleanall( dirty_tracker_t *tracker );

