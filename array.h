#pragma once

#include "defs.h"

#define ISA_ARRAY( type, varn ) \
    size_t  size;               \
    union{                      \
    void   *data;             \
      type   *varn;             \
    };                          \
    size_t  stride;             \
    size_t  limit;              \
    const char *_typeof;        \


typedef struct array_s{
  ISA_ARRAY( uint8, p )
}array_t;


typedef struct array_iter_s{
  array_t *_arr;
  size_t   i;
  union{
    uint8 *p;
    void  *data;
  };
}array_iter_t;

typedef struct array_const_iter_s{
  const array_t *_arr;
  size_t         i;
  union{
    const uint8 *p;
    const void  *data;
  };
}array_const_iter_t;


typedef struct {
  const array_t   *_arr;
  const size_t    *_ins;
  size_t         n;
  size_t         i;
  size_t         j;
  union{
    const uint8 *p;
    const void  *data;
  };
}array_const_indirect_iter_t;

INLINE void *array_data_safe( array_t *array, size_t index );

INLINE void *array_data( array_t *array, size_t index );

INLINE const void *array_data_ro( const array_t *array, size_t index );

void     array_reset( array_t *array, int clear );
array_t  array_make( size_t stride, size_t prelim_limit );
array_t *array_init( size_t stride, size_t prelim_limit );
void     array_pop0( array_t *array, int32 clear, const char *func, int32 line  );
void    *array_add0( array_t *array, const void *data, size_t *index, const char *func, int32 line );
void     array_expand0( array_t *array, size_t include_index, const char func[], int32 line );
size_t   array_index( array_t *array, const void *data, int (*equal)(const void *, const void *) );
void    *array_add_ifdne( array_t *array, const void *data, int (*equal)(const void *, const void *), size_t *index );
int32    sorted_array_sandwich_pair( array_t *array, const void *data, int32 (*compare)(const void *, const void *), size_t *left, size_t *right );
size_t   sorted_array_index( const array_t *array, const void *data, comparator_t compare );
int32    array_isoftype( const array_t *array, const char *type );
void     array_free( array_t **array );
void     array_sort( array_t *array, comparator_t compare );
void     array_term( array_t *array );



#define array_add( a, d, i )     array_add0   ( a, d, i, __FUNCTION__, __LINE__ )
#define array_pop( a, c )        array_pop0   ( a, c,    __FUNCTION__, __LINE__ )
#define array_expand( a, s )     array_expand0( a, s,    __FUNCTION__, __LINE__ )


INLINE array_iter_t array_iter_init( array_t *array );
INLINE int32 array_iterate( array_iter_t *iter );
INLINE array_const_iter_t array_const_iter_init( const array_t *array );
INLINE int32 array_iterate_const( array_const_iter_t *iter );
INLINE array_const_indirect_iter_t array_const_indirect_iter_init( const array_t *array, const size_t indices[], size_t n );
INLINE int32 array_iterate_indirect_const( array_const_indirect_iter_t *iter );
