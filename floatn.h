#pragma once

#include "defs.h"

#include "math.h"
#include "string.h"


#define CONSTF3X3(f3x3) ( (const float3*)(const void*)f3x3 )
#define CONSTF4X4(f4x4) ( (const float4*)(const void*)f4x4 )

//FLOAT2
static inline void f2zero( float2 f2 ){
  f2[0] = f2[1] = 0.0f;
}

static inline void f2set( float2 f2, float x, float y ){
  f2[0] = x;
  f2[1] = y;
}

static inline int f2eqt( const float2 f2u, const float2 f2v, float tol ){
  return
      fabsf( f2u[0] - f2v[0] ) < tol &&
      fabsf( f2u[1] - f2v[1] ) < tol;
}

static inline void f2copy( float2 f2dst, const float2 f2src ){
  f2dst[0] = f2src[0];
  f2dst[1] = f2src[1];
}

static inline void f2add( float2 f2to, const float2 f2val ){
  f2to[0] += f2val[0];
  f2to[1] += f2val[1];
}

static inline void f2sub( float2 f2from, const float2 f2val ){
  f2from[0] -= f2val[0];
  f2from[1] -= f2val[1];
}


static inline void f2muls( float2 f2, float s ){
  f2[0] *= s;
  f2[1] *= s;
}


static inline void f2mul( float2 f2, const float2 f2val ){
  f2[0] *= f2val[0];
  f2[1] *= f2val[1];
}

static inline void f2madd( float2 f2to, float s, const float2 f2val ){
  f2to[0] += s * f2val[0];
  f2to[1] += s * f2val[1];
}

static inline void f2make( float2 f2, const float2 f2from, const float2 f2to ){
  f2[0] = f2to[0] - f2from[0];
  f2[1] = f2to[1] - f2from[1];
}

static inline float f2mag( const float2 f2 ){
  return( sqrtf( f2[0]*f2[0] + f2[1]*f2[1] ) );
}

static inline float f2norm( float2 f2 ){
  float m = f2mag(f2);
  f2muls( f2, 1.0f/m );
  return m;
}

static inline void f2lerp( float2 f2, const float2 f2u, const float2 f2v, float mu ){
  float nu = 1.0 - mu;
  f2[0] = nu * f2u[0] + mu * f2v[0];
  f2[1] = nu * f2u[1] + mu * f2v[1];
}

static inline float f2dot( const float2 f2u, const float2 f2v ){
  return( f2u[0]*f2v[0] + f2u[1]*f2v[1] );
}

static inline void f2perp( float2 f2, const float2 f2u ){
  f2[0] = -f2u[1];
  f2[1] =  f2u[0];
}




//FLOAT3
static inline int f3eqt( const float3 f3u, const float3 f3v, float tol ){
  return
      fabsf( f3u[0] - f3v[0] ) < tol &&
      fabsf( f3u[1] - f3v[1] ) < tol &&
      fabsf( f3u[2] - f3v[2] ) < tol;
}


static inline void f3zero( float3 f3 ){
  f3[0] = f3[1] = f3[2] = (float)0;
}


static inline void f3set( float3 f3, float x, float y, float z ){
  f3[0] = x;
  f3[1] = y;
  f3[2] = z;
}

static inline void f3copy( float3 f3dst, const float3 f3src ){
  f3dst[0] = f3src[0];
  f3dst[1] = f3src[1];
  f3dst[2] = f3src[2];
}

static inline void f3add( float3 f3to, const float3 f3val ){
  f3to[0] += f3val[0];
  f3to[1] += f3val[1];
  f3to[2] += f3val[2];
}

static inline void f3sub( float3 f3from, const float3 f3val ){
  f3from[0] -= f3val[0];
  f3from[1] -= f3val[1];
  f3from[2] -= f3val[2];
}

static inline void f3mul( float3 f3to, const float3 f3val ){
  f3to[0] *= f3val[0];
  f3to[1] *= f3val[1];
  f3to[2] *= f3val[2];
}

static inline void f3div( float3 f3to, const float3 f3val ){
  f3to[0] /= f3val[0];
  f3to[1] /= f3val[1];
  f3to[2] /= f3val[2];
}

static inline void f3muls( float3 f3, float s ){
  f3[0] *= s;
  f3[1] *= s;
  f3[2] *= s;
}

static inline void f3madd( float3 f3, float s, const float3 f3u ){
  f3[0] += s * f3u[0];
  f3[1] += s * f3u[1];
  f3[2] += s * f3u[2];
}

static inline void f3cadd( float3 f3, const float3 f3val1, const float3 f3val2 ){
  f3[0] =  f3val1[0] + f3val2[0];
  f3[1] =  f3val1[1] + f3val2[1];
  f3[2] =  f3val1[2] + f3val2[2];
}

static inline void f3make( float3 f3, const float3 f3from, const float3 f3to ){
  f3[0] = f3to[0] - f3from[0];
  f3[1] = f3to[1] - f3from[1];
  f3[2] = f3to[2] - f3from[2];
}

static inline float f3mag( const float3 f3 ){
  return( sqrtf( f3[0]*f3[0] + f3[1]*f3[1] + f3[2]*f3[2] ) );
}

static inline float f3norm( float3 f3 ){
  float m = f3mag(f3);
  f3muls( f3, 1.0f/m );
  return m;
}


static inline void f3lerp( float3 f3, const float3 f3u, const float3 f3v, float mu ){
  float nu = 1.0 - mu;
  f3[0] = nu * f3u[0] + mu * f3v[0];
  f3[1] = nu * f3u[1] + mu * f3v[1];
  f3[2] = nu * f3u[2] + mu * f3v[2];
}


static inline float f3dot( const float3 f3u, const float3 f3v ){
  return( f3u[0]*f3v[0] + f3u[1]*f3v[1] + f3u[2]*f3v[2] );
}



static inline void f3cross( const float3 f3u, const float3 f3v, float3 f3n ){
  f3n[0] = (f3u[1]*f3v[2] - f3u[2]*f3v[1]);
  f3n[1] =-(f3u[0]*f3v[2] - f3u[2]*f3v[0]);
  f3n[2] = (f3u[0]*f3v[1] - f3u[1]*f3v[0]);
}

static inline void f3transf( const float3x3 f3x3, float3 f3 ){
  float3 f3t;
  f3copy( f3t, f3 );
  f3[0] = f3dot( f3x3[0], f3t );
  f3[1] = f3dot( f3x3[1], f3t );
  f3[2] = f3dot( f3x3[2], f3t );
}


static inline void f3x3copy( float3x3 f3x3dst, const float3x3 f3x3src ){
  memcpy( f3x3dst, f3x3src, sizeof(float3x3) );
}

static inline void f3x3ident( float3x3 f3x3 ){
  f3x3[0][0] = f3x3[1][1] = f3x3[2][2] = (float)1;
  f3x3[0][1] = f3x3[0][2] = (float)0;
  f3x3[1][0] = f3x3[1][2] = (float)0;
  f3x3[2][0] = f3x3[2][1] = (float)0;
}

static inline void f3x3skewsymcross( float3x3 f3x3, const float3 u ){
  f3x3[0][0] = f3x3[1][1] = f3x3[2][2] = (float)0;
  f3x3[0][1] = -u[2];  f3x3[1][0] =  u[2];
  f3x3[0][2] =  u[1];  f3x3[2][0] = -u[1];
  f3x3[1][2] = -u[0];  f3x3[2][1] =  u[0];
}


static inline void f3x3transp( float3x3 f3x3 ){
  SWAP( f3x3[0][1], f3x3[1][0] );
  SWAP( f3x3[0][2], f3x3[2][0] );
  SWAP( f3x3[1][2], f3x3[2][1] );
}


static inline void f3x3transf( const float3x3 f3x3a, const float3x3 f3x3b, float3x3 f3x3 ){
  /*
  | a b c |   | l m n |      |   al + bo + cr   am + bp + cs   an + bq + ct   |
  | d e f | * | o p q |  =   |   dl + eo + fr   dm + ep + fs   an + bq + ct   |
  | g h i |   | r s t |      |   gl + ho + ir   gm + hp + is   an + bq + ct   |
  */

  float3x3 f3x3t;

  f3x3copy( f3x3t, f3x3b );
  f3x3transp( f3x3t );

  f3x3[0][0] = f3dot( f3x3a[0], f3x3t[0] );
  f3x3[0][1] = f3dot( f3x3a[0], f3x3t[1] );
  f3x3[0][2] = f3dot( f3x3a[0], f3x3t[2] );

  f3x3[1][0] = f3dot( f3x3a[1], f3x3t[0] );
  f3x3[1][1] = f3dot( f3x3a[1], f3x3t[1] );
  f3x3[1][2] = f3dot( f3x3a[1], f3x3t[2] );

  f3x3[2][0] = f3dot( f3x3a[2], f3x3t[0] );
  f3x3[2][1] = f3dot( f3x3a[2], f3x3t[1] );
  f3x3[2][2] = f3dot( f3x3a[2], f3x3t[2] );
}


static inline float f3x3det( float3x3 f3x3 )
{
  //
  return
      f3x3[0][0] * ( f3x3[1][1] * f3x3[2][2] - f3x3[1][2] * f3x3[2][1] ) -
      f3x3[0][1] * ( f3x3[1][0] * f3x3[2][2] - f3x3[1][2] * f3x3[2][0] ) +
      f3x3[0][2] * ( f3x3[1][0] * f3x3[2][1] - f3x3[1][1] * f3x3[2][0] );
}


static inline void f4x4copy( float4x4 f4x4dst, const float4x4 f4x4src ){
  memcpy( f4x4dst, f4x4src, sizeof(float4x4) );
}

static inline void f4x4ident( float4x4 f4x4 ){
  f4x4[0][0] = f4x4[1][1] = f4x4[2][2] = f4x4[3][3] = 1.0f;
  f4x4[0][1] = f4x4[0][2] = f4x4[0][3] = 0.0f;
  f4x4[1][0] = f4x4[1][2] = f4x4[1][3] = 0.0f;
  f4x4[2][0] = f4x4[2][1] = f4x4[2][3] = 0.0f;
  f4x4[3][0] = f4x4[3][1] = f4x4[3][2] = 0.0f;
}

static inline void f4x4transp( float4x4 f4x4 ){
  SWAP( f4x4[0][1], f4x4[1][0] );
  SWAP( f4x4[0][2], f4x4[2][0] );
  SWAP( f4x4[0][3], f4x4[3][0] );

  SWAP( f4x4[1][2], f4x4[2][1] );
  SWAP( f4x4[1][3], f4x4[3][1] );

  SWAP( f4x4[2][3], f4x4[3][2] );
}

static inline void f4x4transf( const float4x4 f4x4a, const float4x4 f4x4b, float4x4 f4x4 ){
  for( int32 i = 0; i < 4; i++ )
    for( int32 j = 0; j < 4; j++ ){
      f4x4[i][j] =
          f4x4a[i][0] * f4x4b[0][j] +
          f4x4a[i][1] * f4x4b[1][j] +
          f4x4a[i][2] * f4x4b[2][j] +
          f4x4a[i][3] * f4x4b[3][j];
    }

}


static inline void f4x4transf3( const float4x4 f4x4c, const float4x4 f4x4b, const float4x4 f4x4a, float4x4 f4x4 ){
  int      i, j;
  float4x4 f4x4ba;

  for( i = 0; i < 4; i++ )
    for( j = 0; j < 4; j++ ){
      f4x4ba[i][j] =
          f4x4b[i][0] * f4x4a[0][j] +
          f4x4b[i][1] * f4x4a[1][j] +
          f4x4b[i][2] * f4x4a[2][j] +
          f4x4b[i][3] * f4x4a[3][j];
    }

  for( i = 0; i < 4; i++ )
    for( j = 0; j < 4; j++ ){
      f4x4[i][j] =
          f4x4c[i][0] * f4x4ba[0][j] +
          f4x4c[i][1] * f4x4ba[1][j] +
          f4x4c[i][2] * f4x4ba[2][j] +
          f4x4c[i][3] * f4x4ba[3][j];
    }
}



static inline void f4x4world(
    float4x4 f4x4,
    const float3 f3p, const float3 f3i, const float3 f3j, const float3 f3k )
{
  f4x4[0][0] = f3i[0];
  f4x4[1][0] = f3i[1];
  f4x4[2][0] = f3i[2];
  f4x4[3][0] = 0.0f;

  f4x4[0][1] = f3j[0];
  f4x4[1][1] = f3j[1];
  f4x4[2][1] = f3j[2];
  f4x4[3][1] = 0.0f;

  f4x4[0][2] = f3k[0];
  f4x4[1][2] = f3k[1];
  f4x4[2][2] = f3k[2];
  f4x4[3][2] = 0.0f;

  f4x4[0][3] = f3p[0];
  f4x4[1][3] = f3p[1];
  f4x4[2][3] = f3p[2];
  f4x4[3][3] = 1.0f;
}

static inline void f4x4view(
    float4x4 f4x4,
    const float3 f3p, const float3 f3i, const float3 f3j, const float3 f3k )
{
  float3 f3t;
  f3t[0] = -f3dot( f3i, f3p );
  f3t[1] = -f3dot( f3j, f3p );
  f3t[2] = -f3dot( f3k, f3p );

  f4x4[0][0] = f3i[0];
  f4x4[0][1] = f3i[1];
  f4x4[0][2] = f3i[2];
  f4x4[0][3] = f3t[0];

  f4x4[1][0] = f3j[0];
  f4x4[1][1] = f3j[1];
  f4x4[1][2] = f3j[2];
  f4x4[1][3] = f3t[1];

  f4x4[2][0] = f3k[0];
  f4x4[2][1] = f3k[1];
  f4x4[2][2] = f3k[2];
  f4x4[2][3] = f3t[2];

  f4x4[3][0] = (float)0;
  f4x4[3][1] = (float)0;
  f4x4[3][2] = (float)0;
  f4x4[3][3] = (float)1;
}


static void f4x4persp( float4x4 f4x4, float aspect, float rads, float min, float max ){
  // given point <x,y,z,w>, and 4 by 4 matrix mat
  //    | ...,   0,   0,   0 |   | x |   | x' |
  //    |   0,   0, ...,   0 | * | y | = | y' |
  //    |   0,   a,   0,   b |   | z |   | z' |
  //    |   0,   1,   0,   0 |   | 1 |   | w' |
  //
  // x' = x / tan( fov/2 )
  // y' = z / tan( fov/2 * aspect )
  // z' = a * y + b
  // w' = y
  //
  // NOTES:
  // z-buffer := z'/w, or z'/y. for min/max distances, we want:
  //    i) ( a * min + b ) / min = -1
  //   ii) ( a * max + b ) / max = +1
  // solve for a & b (opengl z-buffer ranges from -1 to +1)
  //
  // screen coordinates are signed normalized, so the
  // perspective divide maps x & y values to the -1/1 range, meaning:
  //  -1 <= x'/w <= 1 &
  //  -1 <= y'/w <= 1
  // working in one dimension:
  // let x := sin( theta ), z:= cos( theta ), because look is adj to view angle, right is opp
  // x * D / z = 1, when theta is half of the fov,
  // solving for D, we get D = z/x, or cos( fov/2 ) / sin( fov/2 ), or cotan( fov/2 )


  // solution:
  //
  // 1. a * min + b = min * n,
  // 2. a * max + b = max * f,
  // subtract 2. from 1 and solve for a
  // substitute value of a in 2. & solve for b

  const float z_n = -1.0f;
  const float z_f = +1.0f;
  float a = ( z_f * max - z_n * min ) / ( max - min );
  float b = z_f * max - a * max;

  for( int32 i = 0; i < 4; i++ )
    for( int32 j = 0; j < 4; j++ )
      f4x4[i][j] = 0.0f;

  f4x4[0][0] = 1.0f / tanf( rads * 0.5f );
  f4x4[1][2] = aspect * f4x4[0][0]; //1.0f / tanf( (float)h/(float)w * fov * 0.5f * PI/180.0f ); //
  f4x4[2][1] = a;
  f4x4[2][3] = b;
  f4x4[3][1] = 1.0f;
}


static void f4x4ortho( float4x4 f4x4, int w, int h, float min, float max ){
  // given point <x,y,z,w>, and 4 by 4 matrix mat
  //    | ...,   0,   0,   0 |   | x |   | x' |
  //    |   0,   0, ...,   0 | * | y | = | y' |
  //    |   0,   a,   0,   b |   | z |   | z' |
  //    |   0,   1,   0,   0 |   | 1 |   | w' |
  //
  // x' = x / w
  // y' = z / h
  // z' = a * y + b
  // w' = 1


  memset( f4x4, 0, sizeof(float4x4) );

  float a = 1.0f / max;
  float b = -min / max;

  f4x4[0][0] = 2.0f / (float)w;
  f4x4[1][2] = 2.0f / (float)h;
  f4x4[2][1] = a;
  f4x4[2][3] = b;
  f4x4[3][3] = 1.0f;
}


//FLOAT 4
static inline void f4set( float4 f4, float x, float y, float z, float w ){
  f4[0] = x;
  f4[1] = y;
  f4[2] = z;
  f4[3] = w;
}

static inline void f4copy( float4 f4dst, const float4 f4src ){
  f4dst[0] = f4src[0];
  f4dst[1] = f4src[1];
  f4dst[2] = f4src[2];
  f4dst[3] = f4src[3];
}

static inline void f4add( float4 f4to, const float4 f4val ){
  f4to[0] += f4val[0];
  f4to[1] += f4val[1];
  f4to[2] += f4val[2];
  f4to[3] += f4val[3];
}

static inline void f4muls( float4 f4, float s ){
  f4[0] *= s;
  f4[1] *= s;
  f4[2] *= s;
  f4[3] *= s;
}

static inline float f4dot( const float4 f4u, const float4 f4v ){
  return( f4u[0]*f4v[0] + f4u[1]*f4v[1] + f4u[2]*f4v[2] + f4u[3]*f4v[3] );
}


static inline void f4transf( const float4x4 f4x4, float4 f4 ){
  float4 f4t;
  f4copy( f4t, f4 );
  f4[0] = f4dot( f4x4[0], f4t );
  f4[1] = f4dot( f4x4[1], f4t );
  f4[2] = f4dot( f4x4[2], f4t );
  f4[3] = f4dot( f4x4[3], f4t );
}


static inline void f3wtransf( const float4x4 f4x4, float3 f3, float w ){
  float4 f4;
  f4set( f4, f3[0], f3[1], f3[2], w );
  f3[0] = f4dot( f4x4[0], f4 );
  f3[1] = f4dot( f4x4[1], f4 );
  f3[2] = f4dot( f4x4[2], f4 );
}

static void f3x3ypr( float3x3 f3x3, float y, float p, float r, int32 in_degs ){
  if( in_degs ){
    y = y * PI / 180.0f;
    p = p * PI / 180.0f;
    r = r * PI / 180.0f;
  }

  float  cp = cosf( p );
  float  sp = sinf( p );

  float  cy = cosf( y );
  float  sy = sinf( y );

  float  cr = cosf( r );
  float  sr = sinf( r );

  float3x3 f3x3r, f3x3p, f3x3y, f3x3pr;

  f3set( f3x3r[0],   cr, 0.0f,  -sr );
  f3set( f3x3r[1], 0.0f, 1.0f, 0.0f );
  f3set( f3x3r[2],   sr, 0.0f,   cr );

  f3set( f3x3p[0], 1.0f, 0.0f, 0.0f );
  f3set( f3x3p[1], 0.0f,   cp,  -sp );
  f3set( f3x3p[2], 0.0f,   sp,   cp );

  f3set( f3x3y[0],   cy,  -sy, 0.0f );
  f3set( f3x3y[1],   sy,   cy, 0.0f );
  f3set( f3x3y[2], 0.0f, 0.0f, 1.0f );

  f3x3transf( CONSTF3X3(f3x3p), CONSTF3X3(f3x3r),  f3x3pr );
  f3x3transf( CONSTF3X3(f3x3y), CONSTF3X3(f3x3pr), f3x3   );
}



static float f4x4det( const float4x4 f4x4 ){
  float det = 0.0f;
  for( int32 i = 0; i < 4; i++ ){
    float3x3 f3x3;
    float s = i & 0x01 ? -f4x4[0][i] : +f4x4[0][i];

    for( int32 r = 1; r < 4; r++ ){
      int32 r0 = r - 1;
      int32 c0 = 0;
      for( int32 c = 0; c < 4; c++ )
        if( c != i ){
          f3x3[r0][c0] = f4x4[r][c];
          c0++;
        }
    }
    det += s * f3x3det( f3x3 );
  }

  return det;
}

static void f4x4ypr( float4x4 f4x4, const float3 v3p, float y, float p, float r, int32 in_degs ){
  if( in_degs ){
    y = y * PI / 180.0f;
    p = p * PI / 180.0f;
    r = r * PI / 180.0f;
  }


  float  cp = cosf( p );
  float  sp = sinf( p );

  float  cy = cosf( y );
  float  sy = sinf( y );

  float  cr = cosf( r );
  float  sr = sinf( r );

  float4x4 f4x4r, f4x4p, f4x4y, f4x4pr;

  f4set( f4x4r[0],   cr, 0.0f,  -sr, 0.0f );
  f4set( f4x4r[1], 0.0f, 1.0f, 0.0f, 0.0f );
  f4set( f4x4r[2],   sr, 0.0f,   cr, 0.0f );
  f4set( f4x4r[3], 0.0f, 0.0f, 0.0f, 1.0f );

  f4set( f4x4p[0], 1.0f, 0.0f, 0.0f, 0.0f );
  f4set( f4x4p[1], 0.0f,   cp,  -sp, 0.0f );
  f4set( f4x4p[2], 0.0f,   sp,   cp, 0.0f );
  f4set( f4x4p[3], 0.0f, 0.0f, 0.0f, 1.0f );

  f4set( f4x4y[0],   cy,  -sy, 0.0f, 0.0f );
  f4set( f4x4y[1],   sy,   cy, 0.0f, 0.0f );
  f4set( f4x4y[2], 0.0f, 0.0f, 1.0f, 0.0f );
  f4set( f4x4y[3], 0.0f, 0.0f, 0.0f, 1.0f );

  f4x4transf( CONSTF4X4(f4x4p), CONSTF4X4(f4x4r),  f4x4pr );
  f4x4transf( CONSTF4X4(f4x4y), CONSTF4X4(f4x4pr), f4x4   );

  f4x4[0][3] = v3p[0];
  f4x4[1][3] = v3p[1];
  f4x4[2][3] = v3p[2];
}

static void f3rot( float3 f3, const float3 f3axis, float rads ){
  float3 f3proj, f3perp, f3s, f3t;
  float  co, si;

  si = sinf( rads );
  co = cosf( rads );

  f3copy( f3proj, f3axis );
  f3muls( f3proj, f3dot(f3,f3axis) );

  f3copy( f3perp, f3 );
  f3sub ( f3perp, f3proj );

  f3copy( f3s, f3perp );
  f3muls( f3s, co );

  f3cross( f3axis, f3perp, f3t ); //since v^r is |v||r|sin angle, v^r is the same as v^r_perp
  f3muls ( f3t, si );             //| r_perp^v | = |r_perp |, since |v| is 1 and sin angle is 90

  f3copy( f3, f3proj );
  f3add( f3, f3s );
  f3add( f3, f3t );
}

