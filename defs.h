#pragma once

#include <float.h>
#include <stddef.h>
#include <stdint.h>

#pragma GCC diagnostic ignored "-Wunused-function"


#define PI  3.141592653589793
#define E   2.71828183
#define TOL (1e-5)


#define SWAP( a, b ){ \
  __typeof__(a) t;  \
  t=a;  \
  a=b;  \
  b=t;  \
}

#define SORT2( a, b )  \
{\
    if( (b)<(a) )\
        SWAP( a, b );\
}\


#define SORT3( a, b, c )  \
{\
    if( (c)<(b) )\
        SWAP(b,c);\
    if( (c)<(a) )\
        SWAP(a,c);\
    if( (b)<(a) )\
        SWAP(a,b);\
}\

#define BIT(n) ( 1 << (n) )
#define MIN(a,b,c){ if( (b)<=(c) ) (a)=(b); else (a)=(c); }
#define MAX(a,b,c){ if( (b)>=(c) ) (a)=(b); else (a)=(c); }
#define CLAMP(a,min,max){ if( (a) < (min) ) (a)=(min); else if( (a) > (max) ) (a)=(max); }
#define LERP(u,v,a) ( (u) + (a) * ( (v) - (u) ) )

#define SQUARE(a) ((a)*(a))

#ifndef INTDEFS
#define INTDEFS
typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
#endif

typedef float float2[2];
typedef float float3[3];
typedef float float4[4];


typedef float float2x2[2][2];
typedef float float3x3[3][3];
typedef float float4x4[4][4];

/*
#ifndef char16
typedef char char16[16];
typedef char char20[20];
typedef char char40[40];
typedef char char80[80];
#endif
*/

typedef union {
  float  f;
  int32  s;
  uint32 u;
  char   c4[4];
}union32_t;


typedef int32 (*comparator_t)( const void *, const void * );
typedef void  (*destructor_t)( void * );



extern int32 DEBUG;

#define INLINE
