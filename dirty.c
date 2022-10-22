#include "dirty.h"

#include "libs.h"

void dirty_tracker_term( dirty_tracker_t *tracker ){
  tracker->size = 0;
  tracker->bool = 0;
  SAFE_FREE( tracker->testbools );
}

void dirty_tracker_init( dirty_tracker_t *tracker, uint32 size ){
  dirty_tracker_term( tracker );
  tracker->size = size == 0 ? 1 : size;
  tracker->bool = 0;
  tracker->testbools = calloc( tracker->size, sizeof(uint32) );
}


void dirty_tracker_destructor( void *tracker ){
  dirty_tracker_term( tracker );
}


void INLINE dirty_tracker_next( dirty_tracker_t *tracker ){
  tracker->bool++;
}


int32 INLINE dirty_tracker_isdirty( dirty_tracker_t *tracker, size_t index, int32 clean ){
  if( !tracker->testbools || index >= tracker->size )
    return 0;

  if( tracker->testbools[index] < tracker->bool ){
    if( clean )
      tracker->testbools[index] = tracker->bool;
    return 1;
  }
  return 0;
}

int32 INLINE dirty_tracker_isdirty2( const dirty_tracker_t *tracker, size_t index ){
  if( tracker->testbools && index < tracker->size )
    return tracker->testbools[index] < tracker->bool;
  return 0;
}

int32 INLINE dirty_tracker_isclean ( const dirty_tracker_t *tracker, size_t index ){
  if( tracker->testbools && index < tracker->size )
    return tracker->testbools[index] == tracker->bool;
  return 0;
}



int32 INLINE dirty_tracker_clean( dirty_tracker_t *tracker, size_t index ){
  if( !tracker->testbools || index >= tracker->size )
    return 0;

  tracker->testbools[index] = tracker->bool;
  return 1;
}

 int32 INLINE dirty_tracker_dirty( dirty_tracker_t *tracker, size_t index ){
  if( !tracker->testbools || index >= tracker->size )
    return 0;

  tracker->testbools[index] = 0;
  return 1;
}


void INLINE dirty_tracker_cleanall( dirty_tracker_t *tracker ){
  if( tracker->size && tracker->testbools ){
    for( size_t i = 0; i < tracker->size; i++ )
      tracker->testbools[i] = tracker->bool;
  }
}

