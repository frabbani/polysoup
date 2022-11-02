#include "array.h"

#include "libs.h"

INLINE void *array_data_safe( array_t *array, size_t index ){
  if( !array || index >= array->size )
    return NULL;
  return (void *)&array->p[ index * array->stride ];
}

INLINE void *array_data( array_t *array, size_t index ){
  return (void *)&array->p[ index * array->stride ];
}

INLINE const void *array_data_ro( const array_t *array, size_t index ){
  if( !array || index >= array->size )
    return NULL;
  return (const void *)&array->p[ index * array->stride ];
}

void array_reset( array_t *array, int clear ){
  if( array->data ){
      array->size = 0;
    if( clear )
      memset( array->data, 0, array->limit * array->stride );
  }
}

array_t array_make( size_t stride, size_t prelim_limit ){
  array_t array;
  array.size   = 0;
  array.stride = stride;
  array.limit  = prelim_limit;
  array.data   = calloc( array.limit, array.stride );
  return array;
}

array_t *array_init( size_t stride, size_t prelim_limit ){
  array_t *array = calloc( 1, sizeof(array_t) );
  array->size   = 0;
  array->stride = stride;
  array->limit  = prelim_limit;
  array->data   = calloc( array->limit, array->stride );
  return array;
}

void array_pop0( array_t *array, int32 clear, const char *func, int32 line  ){
  if( 0 == array->size ){
    printf( "%s:%d - can't pop empty array\n", func, line );
    return;
  }
  if( clear ){
    void *copyto = (void *)&array->p[ ( array->size - 1 ) * array->stride ];
    switch( array->stride ){
    case sizeof(uint8) : *((uint8* )copyto) = 0; break;
    case sizeof(uint16): *((uint16*)copyto) = 0; break;
    case sizeof(uint32): *((uint32*)copyto) = 0; break;
    case sizeof(uint64): *((uint64*)copyto) = 0; break;
    default: memset( copyto, 0, array->stride ); break;
    }
  }
  array->size--;
}


void *array_add0( array_t *array, const void *data, size_t *index, const char *func, int32 line ){
  if( array->size == array->limit ){
    size_t oldlimit = array->limit;
    array->limit += oldlimit;
    printf( "%s:%d - old limit v. new limit: %zu v. %zu\n", func, line, oldlimit, array->limit );

    void *new = malloc( array->limit * array->stride );
    memcpy( new, array->data, array->size * array->stride );

    if( array->data )
      free( array->data );
    array->data  = new;
  }

  void *copyto = (void *)&array->p[ array->size * array->stride ];
  if( data ){
    switch( array->stride ){
    case sizeof(uint8) : *((uint8* )copyto) = *((uint8*  )data); break;
    case sizeof(uint16): *((uint16*)copyto) = *((uint16* )data); break;
    case sizeof(uint32): *((uint32*)copyto) = *((uint32* )data); break;
    case sizeof(uint64): *((uint64*)copyto) = *((uint64* )data); break;
    default:  memcpy( copyto, data, array->stride ); break;
    }
  }
  else{
    switch( array->stride ){
    case sizeof(uint8) : *((uint8* )copyto) = 0; break;
    case sizeof(uint16): *((uint16*)copyto) = 0; break;
    case sizeof(uint32): *((uint32*)copyto) = 0; break;
    case sizeof(uint64): *((uint64*)copyto) = 0; break;
    default: memset( copyto, 0, array->stride ); break;
    }
  }

  if( index )
    *index = array->size;

  array->size++;
  return copyto;
}

void array_expand0( array_t *array, size_t include_index, const char func[], int32 line ){
  if( array->limit > include_index )
    return;

  if( !array->limit ){
    printf( "%s - array not initialized\n", func );
    return;
  }

  size_t oldlimit = array->limit;
  while( array->limit <= include_index )
    array->limit += array->limit;

  printf( "%s:%d - old limit v. new limit: %zu v. %zu\n", func, line, oldlimit, array->limit );

  void *new = malloc( array->limit * array->stride );
  memcpy( new, array->data, array->size * array->stride );
  free  ( array->data );
  array->data  = new;
}

size_t array_index( array_t *array, const void *data, int (*equal)(const void *, const void *) ){
  size_t index = 0; //not found
  if( array && array->size && data )
    for( index = array->size; index >= 1; index-- )
      if( equal( array_data( array, index - 1 ), data ) )
        break;
  //return index == 0 ? -1 : index - 1;
  return index - 1;
}


void *array_add_ifdne0( array_t *array, const void *data, int (*equal)(const void *, const void *), size_t *index ){
 size_t pos = array_index( array, data, equal );
  if( -1 != pos ){
    if( index )
      *index = pos;
    return array_data( array, pos );
  }

  void *added = array_add( array, data, &pos );
  if( index )
    *index = pos;
  return added;
}

void *array_add_ifdne( array_t *array, const void *data, size_t *index ){
  comparator_t comp_func = get_compare_func( array->_typeof );
  if( !comp_func ){
    return array_add0( array, data, index, __FUNCTION__, __LINE__ );
  }
 size_t n = array->size;
 size_t pos  = 0;
 while( n >= 1 ){
   pos = n - 1;
   void *elem = array_data( array, pos );
   if( 0 == comp_func( elem, data ) ){
     if( index )
       *index = pos;
     return elem;
   }
   n--;
 }

  void *added = array_add( array, data, &pos );
  if( index )
    *index = pos;
  return added;
}

size_t _array_binsrch( array_t *array, const void *data, comparator_t compare, size_t l, size_t r ){
  if( l == r )
    return 0 == compare( data, array_data( array, r ) ) ? r : -1;
  else if( l < r ){
    size_t m      = ( l + r ) >> 1;
    int    result = compare( data, array_data( array, m ) );
    if( 0 == result )
      return m;
    else if( result > 0 )
      return _array_binsrch( array, data, compare, m+1, r );
    else
      return _array_binsrch( array, data, compare, l, m-1 );
  }
  return -1;
}


size_t _array_binsrch_ro( const array_t *array, const void *data, int (*compare)(const void *, const void *), size_t l, size_t r ){
  if( l == r )
    return 0 == compare( data, array_data_ro( array, r ) ) ? r : -1;
  else if( l < r ){
    size_t m      = ( l + r ) >> 1;
    int    result = compare( data, array_data_ro( array, m ) );
    if( 0 == result )
      return m;
    else if( result > 0 )
      return _array_binsrch_ro( array, data, compare, m+1, r );
    else
      return _array_binsrch_ro( array, data, compare, l, m-1 );
  }
  return -1;
}


int32 _array_binsndwch( array_t *array, const void *data, int32 (*compare)(const void *, const void *), size_t *l, size_t *r, int32 lcomp, int32 rcomp ){
  //binary sandwhich search - find the left/right pair the data is sandwiched between
  // * assumes the data isn't contained within the array
  // * assumes the data isn't less than the first element in the array
  // * assumes the data isn't greater than the last element in the array
  // * returns 0 if data exists, 1 if sandwiched
  // * if data exists, l & r both contain the index of the data

  /*
  if( -1 == lcomp ){
    lcomp = compare( data, array_data( array, *l ) );
    if( 0 == lcomp ){
      *r = *l;
      return 0;
    }
  }
  if( -1 == rcomp ){
    rcomp = compare( data, array_data( array, *r ) );
    if( 0 == rcomp ){
      *l = *r;
      return 0;
    }
  }
  */

  size_t dist = *r - *l;
  if( 1 == dist )
    return 1;
  else if( dist > 1 ){
    size_t m    = ( *l + *r ) >> 1;
    int32  comp = compare( data, array_data( array, m ) );
    if( 0 == comp ){
      *l = *r = m;
      return 0;
    }
    if( comp > 0 ){
      *l = m;
      lcomp = comp;
    }
    else{
      *r = m;
      rcomp = comp;
    }
    return _array_binsndwch( array, data, compare, l, r, lcomp, rcomp );
  }

  return 0;
}


int32 sorted_array_sandwich_pair( array_t *array, const void *data, int32 (*compare)(const void *, const void *), size_t *left, size_t *right ){
  // sorted array sandwich search:
  // - find the left/right pair the data is sandwiched between
  // - returns 1 if data d.n.e in array, 0 otherwise
  // - if data already exists, both left & right will contain the existing data's index
  // - if data doesn't exist left and right will contain the sandwich indices
  // - if left & right are -1, the array is empty (returns 1)
  // - if only left is -1, the data is less than the lowest value
  // - if only right is -1, the data is higher than the highest value
  size_t a, b;
  size_t *l = NULL != left  ? left  : &a;
  size_t *r = NULL != right ? right : &b;

  if( 0 == array->size ){
    *l = *r = -1;
    return 1;
  }
  else
  if( 1 == array->size ){
    int32 comp = compare( data, array_data( array, 0 ) );
    if( comp < 0 ){
      *l = -1;
      *r =  0;
      return 1;
    }
    if( comp > 0 ){
      *l =  0;
      *r = -1;
      return 1;
    }
    *l = *r = 0;
    return 0;
  }


  *l = 0;
  *r = array->size - 1;

  int32 lcomp = compare( data, array_data( array, *l ) );
  if( lcomp < 0 ){
    *l = -1;
    *r  = 0;
    return 1;
  }
  else
  if( 0 == lcomp ){
    *r = *l;
    return 0;
  }

  int32 rcomp = compare( data, array_data( array, *r ) );
  if( rcomp > 0 ){
    *l = *r;
    *r = -1;
    return 1;
  }
  else
  if( 0 == rcomp ){
    *l = *r;
    return 0;
  }
  return _array_binsndwch( array, data, compare, l, r, lcomp, rcomp );
}




size_t sorted_array_index( const array_t *array, const void *data, comparator_t compare ){
  if( array->size ){
    if( 0 == compare( data, array_data_ro( array, 0 ) ) )
      return 0;
    if( 0 == compare( data, array_data_ro( array, array->size - 1 ) ) )
      return array->size - 1;

    size_t index = _array_binsrch_ro( array, data, compare, 0, array->size - 1 );
    if( 0 == index || -1 == index )
      return index;
    while( index - 1 != 0 ){
      if( 0 != compare( data, array_data_ro( array, index - 1 ) ) )
        break;
      index--;
    }
    return index;
  }
  return -1;
}



/*
void *array_insert( array_t *array, const void *data, size_t index ){
  assert( array );
  if( index > array->size ){
    printf( "%s - cannot insert past append point\n", __FUNCTION__ );
    return NULL;
  }
  if( index == array->size )
    return array_add( array, data, NULL );

  size_t numshifts = array->size - index;
  size_t numbytes  = numshifts * array->stride;
  if( numbytes <= 16 ){
    uint8 *copyto =
  }

  array_add( array, NULL, NULL );

  size_t numbytes = array->stride * ( array->size - index );
  for( size_t i = array->size-1; i > index; i-- )
}


void sorted_array_insert( array_t *array, const void *data, int (*compare)(const void *, const void *), size_t *index ){
  assert( NULL != array );
  assert( NULL != data  );
}
*/


int32 array_isoftype( const array_t *array, const char *type ){
  int32 same = 0;
  if( array->_typeof && type ){
    const char *p0 = array->_typeof;
    const char *p1 = type;
    while( 1 ){
      char a = toupper( *p0 );
      char b = toupper( *p1 );
      if( a != b )
        break;
      if( '\0' == a ){
        same = 1;
        break;
      }
      p0++;
      p1++;
    }
  }
  return same;
}

void array_free( array_t **array ){
  if( array && *array ){
    array_term( *array );
    free( *array );
    *array = NULL;
  }
}


void array_term( array_t *array ){
  SAFE_FREE( array->data );
  memset( array, 0, sizeof(array_t) );
}

void array_sort( array_t *array, comparator_t compare ){
  if( array && array->size )
    qsort( array->data, array->size, array->stride, compare );
}


INLINE array_iter_t array_iter_init( array_t *array ){
  array_iter_t iter;
  iter._arr = array;
  iter.i    = -1;
  iter.data = array->data;
  return iter;
}

INLINE int32 array_iterate( array_iter_t *iter ){
  iter->i++;
  if( iter->i < iter->_arr->size ){
    if( iter->i )
      iter->p += iter->_arr->stride;
    return 1;
  }
  iter->p = NULL;
  return 0;
}

INLINE array_const_iter_t array_const_iter_init( const array_t *array ){
  array_const_iter_t iter;
  iter._arr = array;
  iter.i    = -1;
  iter.data = array->data;
  return iter;
}

INLINE int32 array_iterate_const( array_const_iter_t *iter ){
  iter->i++;
  if( iter->i < iter->_arr->size ){
    if( iter->i )
      iter->p += iter->_arr->stride;
    return 1;
  }
  iter->p = NULL;
  return 0;
}

INLINE array_const_indirect_iter_t array_const_indirect_iter_init( const array_t *array, const size_t indices[], size_t size ){
  array_const_indirect_iter_t iter;
  iter._arr = array;
  iter._ins = indices;
  iter.n    = size;
  iter.i    = -1;
  iter.j    = -1;
  iter.p    = NULL;
  return iter;
}

INLINE int32 array_iterate_indirect_const( array_const_indirect_iter_t *iter ){
  iter->i++;
  const array_t *array = iter->_arr;
  if( iter->i < iter->n ){
    iter->j = iter->_ins[ iter->i ];
    if( iter->j >= array->size )
      iter->p = NULL;
    else{
      iter->p =  array->data;
      iter->p += ( iter->j * array->stride );
    }
    return 1;
  }
  iter->p =  NULL;
  iter->j = -1;
  return 0;
}

