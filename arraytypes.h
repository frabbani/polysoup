#pragma once

#include "array.h"

#define ARRTAG  __FUNCTION__, __LINE__

#define ARRAYDEF( name, type ) \
typedef struct name##array_s{ \
  union{  \
    array_t array;    \
    struct{ \
      ISA_ARRAY( type, elems ) \
    };\
  };\
}name##array_t; \
\
\
INLINE int32 isoftype_##name##array( const array_t *array ); \
INLINE name##array_t  name##array_make( size_t prelim_limit );\
INLINE name##array_t *name##array_init( size_t prelim_limit );\
INLINE type    name##array_add0( name##array_t *array, type elem, const char func[], int32 line );\
INLINE type   *name##array_add1( name##array_t *array, const type *elem, const char func[], int32 line );\
INLINE size_t  name##array_add_ifdne( name##array_t *array, type elem, const char func[], int32 line ); \
INLINE void    name##array_term( name##array_t *array );\
INLINE void    name##array_free( name##array_t **array );\


#define ARRAYFUNC( name, type ) \
\
INLINE int32 isoftype_##name##array( const array_t *array ){ \
  return array_isoftype( array, #type );  \
}\
\
INLINE name##array_t name##array_make( size_t prelim_limit ){  \
  name##array_t array;  \
  array.array = array_make( sizeof(type), prelim_limit );  \
  array._typeof = #type; \
    return array; \
} \
\
INLINE name##array_t *name##array_init( size_t prelim_limit ){  \
  array_t *array = array_init( sizeof(type), prelim_limit );  \
  array->_typeof = #type; \
  return (void *)array; \
} \
\
INLINE type name##array_add0( name##array_t *array, type elem, const char func[], int32 line ){\
  if( array->size == array->limit ) \
    array_expand0( &array->array, array->size, func, line ); \
  array->elems[ array->size++ ] = elem; \
  return array->elems[ array->size-1 ]; \
} \
\
INLINE type *name##array_add1( name##array_t *array, const type *elem, const char func[], int32 line ){\
  if( array->size == array->limit ) \
    array_expand0( &array->array, array->size, func, line ); \
  if( elem )  \
    memcpy( &array->elems[ array->size++ ], elem, sizeof(type) ); \
  else \
    memset( &array->elems[ array->size++ ], 0,    sizeof(type) ); \
  return &array->elems[ array->size-1 ]; \
} \
\
INLINE size_t  name##array_add_ifdne( name##array_t *array, type elem, const char func[], int32 line ){ \
  for( size_t i = 0; i < array->size; i++ ){ \
    if( 0 == memcmp( &array->elems[ i ], &elem, sizeof(type) ) )  \
      return i; \
  } \
  name##array_add0( array, elem, func, line );  \
  return array->size - 1; \
}\
\
INLINE void name##array_term( name##array_t *array ){  \
  if( array )  \
    array_term( &array->array ); \
} \
\
INLINE void name##array_free( name##array_t **array ){  \
  if( array && *array ){  \
    array_term( &(*array)->array ); \
    free( *array ); \
    *array = NULL;  \
  } \
} \
\


ARRAYDEF( size, size_t );
#define sizearray_add( a, v )  sizearray_add0( a, v, ARRTAG )
#define sizearray_new( a, v )  sizearray_add1( a, v, ARRTAG )

ARRAYDEF( int32, int32 );
#define int32array_add( a, v ) int32array_add0( a, v, ARRTAG )
#define int32array_new( a, v ) int32array_add1( a, v, ARRTAG )

ARRAYDEF( uint32, uint32 );
#define uint32array_add( a, v ) uint32array_add0( a, v, ARRTAG )
#define uint32array_new( a, v ) uint32array_add1( a, v, ARRTAG )

ARRAYDEF( float, float );
#define floatarray_add( a, v ) floatarray_add0( a, v, ARRTAG )
#define floatarray_new( a, v ) floatarray_add1( a, v, ARRTAG )

