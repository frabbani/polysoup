#pragma once

#include "defs.h"

typedef struct{
  uint32   size;
  uint32   bool;
  uint32  *testbools;
}dirty_tracker_t;

void dirty_tracker_term( dirty_tracker_t *tracker );
void dirty_tracker_init( dirty_tracker_t *tracker, uint32 size );
void dirty_tracker_destructor( void *tracker );

INLINE void  dirty_tracker_next( dirty_tracker_t *tracker );
INLINE int32 dirty_tracker_isdirty( dirty_tracker_t *tracker, size_t index, int32 clean );
int32 INLINE dirty_tracker_isdirty2( const dirty_tracker_t *tracker, size_t index );
INLINE int32 dirty_tracker_isclean( const dirty_tracker_t *tracker, size_t index );
INLINE int32 dirty_tracker_clean( dirty_tracker_t *tracker, size_t index );
INLINE int32 dirty_tracker_dirty( dirty_tracker_t *tracker, size_t index );
INLINE void dirty_tracker_cleanall( dirty_tracker_t *tracker );

