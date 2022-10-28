#include "compare.h"
#include "strn.h"

#include "libs.h"


#define MAX_COMPARE_FUNCS 256


typedef struct compare_s{
  str128 name;
  comparator_t compare_func;
}compare_t;

static char upper_tab[256];

typedef int32_t  int32;
typedef uint32_t uint32;

void init_upper(){
  for( uint32 i = 0; i < 256; i++ )
    upper_tab[i] = (char)i;

  int32 i = 0;
  for( char c = 'a'; c <= 'z'; c++, i++ ){
    upper_tab[(int32)c] = (char)( (int32)'A' + i );
  }
}

int32 compare_uppercase( const char *ptr0, const char *ptr1 ){
  const char *lhs = ptr0;
  const char *rhs = ptr1;
  for( int32 i = 0; i < 128; i++ ){
    char a = upper_tab[ (uint32)*lhs ];
    char b = upper_tab[ (uint32)*rhs ];
    if( a < b )
      return -1;
    else
    if( a > b )
      return +1;
    else{
      if( '\0' == a ){
        return 0;
      }
    }
    lhs++;
    rhs++;
  }
  return 0; //truncated
}


static size_t    num_compares = 0;
static compare_t compares[ MAX_COMPARE_FUNCS ];


int32 int32_comparator( const void *ptr0, const void *ptr1 ){
  const int32 *lhs = ptr0;
  const int32 *rhs = ptr1;
  if( *lhs < *rhs )
    return -1;
  if( *lhs > *rhs )
    return +1;
  return 0;
}

int32 uint32_comparator( const void *ptr0, const void *ptr1 ){
  const uint32 *lhs = ptr0;
  const uint32 *rhs = ptr1;
  if( *lhs < *rhs )
    return -1;
  if( *lhs > *rhs )
    return +1;
  return 0;
}

int32 float_comparator( const void *ptr0, const void *ptr1 ){
  const float *lhs = ptr0;
  const float *rhs = ptr1;
  const float  tol = 1e-9f;
  if( *lhs < ( *rhs - tol ) )
    return -1;
  if( *lhs > ( *rhs + tol ) )
    return +1;
  return 0;
}

int32 size_comparator( const void *ptr0, const void *ptr1 ){
  const size_t *lhs = ptr0;
  const size_t *rhs = ptr1;
  if( *lhs < *rhs )
    return -1;
  if( *lhs > *rhs )
    return +1;
  return 0;
}

int32 compare_comparator( const void *ptr0, const void *ptr1 ){
  const compare_t *lhs = ptr0;
  const compare_t *rhs = ptr1;
  return compare_uppercase( lhs->name.chars, rhs->name.chars );
}

int32 compares_find( const char *name ){
  int32 l = 0;
  int32 r = (int32)( num_compares - 1 );
  while( l <= r ){
    int32 m = ( l + r ) >> 1;
    int32 check = compare_uppercase( compares[ m ].name.chars, name );
    if( check < 0 ){
      l = m + 1;
    }
    else
    if( check > 0 ){
      r = m - 1;
    }
    else
      return m;
  }
  return -1;
}


void init(){
  if( num_compares > 0 )
    return;
  init_upper();

  compares[ num_compares ].name = str128_( "int32" );
  compares[ num_compares ].compare_func = int32_comparator;
  num_compares++;

  compares[ num_compares ].name = str128_("float");
  compares[ num_compares ].compare_func = float_comparator;
  num_compares++;

  compares[ num_compares ].name = str128_("uint32");
  compares[ num_compares ].compare_func = uint32_comparator;
  num_compares++;

  compares[ num_compares ].name = str128_("size_t");
  compares[ num_compares ].compare_func = size_comparator;
  num_compares++;

  qsort( compares, num_compares, sizeof(compare_t), &compare_comparator );
}

void add_compare_func( const char* name, comparator_t compare_func ){
  init();

  if( compares_find( name ) > 0 )
    return;
  compares[ num_compares ].name = str128_( name );
  compares[ num_compares ].compare_func = compare_func;
  num_compares++;
  qsort( compares, num_compares, sizeof(compare_t), &compare_comparator );
}

comparator_t get_compare_func( const char *name ){
  init();

  int32 i = compares_find( name );
  if( i >= 0 ){
    return compares[i].compare_func;
  }
  return NULL;
}
