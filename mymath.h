#pragma once

#include "floatn.h"

//BCS - Barycentric Coordinate System

typedef struct bcs2_s{
  float2   o, u, v;
  float    det;

  // Converting to Barycentric coordinates:
  // * vector o is the origin
  // * vectors u & v are the 2D Cartesian axes
  float2x2 M,
           M_inv;

  // Used to find 3D Cartesian axes
  float2x2 A,
           A_inv;

}bcs2_t;

int32 bcs2_calc( bcs2_t *bcs, const float2 p0, const float2 p1, const float2 p2 );
void  bcs2_axes( bcs2_t *bcs, const float3 p0, const float3 p1, const float3 p2, float3 axes[3], int ortho );
int32 bcs2_find( const bcs2_t *bcs, const float2 p, float *uval, float *vval );
float bcs2_area( const bcs2_t *bcs );

typedef struct bcs3_s{
  float3   o, u, v;
  float    det;

  // converting to Barycentric coordinates:
  // * vector o is the origin
  // * vectors u & v are the 3D Cartesian axes
  // * use the 3D axes projected values
  float2x2 M,
           M_inv;
}bcs3_t;


int32 bcs3_calc( bcs3_t *bcs, const float3 p0, const float3 p1, const float3 p2 );
int32 bcs3_find( const bcs3_t *bcs, const float3 p, float *uval, float *vval );
float bcs3_area( const bcs3_t *bcs );


typedef struct ray_s{
  float3 p,
         d;
}ray_t;


typedef struct plane_s{
  float3 n;
  float  dist;

}plane_t;


INLINE void  plane_make( plane_t *plane, const float3 n, const float3 p );
INLINE void  plane_calc( plane_t *plane, const float3 p0, const float3 p1, const float3 p2 );
INLINE void  plane_calc_fast( plane_t *plane, const float3 p0, const float3 p1, const float3 p2 );
INLINE int32 plane_test( const plane_t *plane, const float3 p );
INLINE float plane_line( const plane_t *plane, const float3 p, const float3 d );
INLINE float plane_line_fast( const plane_t *plane, const float3 p, const float3 d, float ddotn );
INLINE float plane_solve( const plane_t *plane, const float3 p );
INLINE int32 plane_crosses( const plane_t *plane, const float3 ps[], size_t nps );

//Separate Axis Test
typedef struct sat_s{
  float3 v;
  float  mins[2], maxs[2], mids[2];
}sat_t;


void sat_init( sat_t *sat, const float3 axis );
void sat_calc( sat_t *sat, int second, const float3 ps[], size_t nps );
void sat_make( sat_t *sat, int second, const float3 c, float hdist );
void sat_init_from_plane( sat_t *sat, const plane_t *plane, int32 second );
INLINE int sat_olap( sat_t *sat );


typedef struct{
  float3 ps[2],
         dir;
  float  length;
}lseg_t;

#define INHERIT_LSEG  \
  union{              \
    lseg_t lseg;      \
    struct{           \
      float3 ps[2],   \
             dir;     \
      float  length;  \
    };                \
  };                  \



void lseg_calc( lseg_t *lseg, const float3 p0, const float3 p1 );
void lseg_slide( lseg_t *lseg, float dist, size_t end );
void lseg_perp( const lseg_t *lseg, const float3 p, float3 n );


typedef struct{
  float2 p;
  float  radius;
}circle_t;

INLINE void circle_init( circle_t *circle, const float2 p, float radius );
void circle_calc( circle_t *circle, const float2 *ps, uint32 nps );
INLINE int32 circle_touch( const circle_t *circle, const float2 p );


typedef struct{
  float2 ps[2];
  float2 dir;
  union{ float  length, length_sq; };
  int32  norm;
}lseg2_t;

void  lseg2_calc( lseg2_t *lseg, const float2 p0, const float2 p1, int32 normalize );
int32 lseg2_isects( const lseg2_t *lseg0, const lseg2_t *lseg1, float *mu, float *nu );
int32 lseg2_circle_hit( const lseg2_t *lseg, const circle_t *circle );
int32 lseg2_circle_isect( const lseg2_t *lseg, const circle_t *circle, float2 alpha );


// calculate bases for a given up vector, using x,y & z axes
void calc_bases_from_up( const float3 u, float3 bs[3] );
// calculate bases for a given look vector, using x,y & z axes
void  calc_bases_from_look( const float3 l, float3 bs[3] );
float triangle_area( const float3 p0, const float3 p1, const float3 p2 );


typedef struct{
  float3  p,
          axes[2];
  float2  size;
  plane_t plane;
}rect_t;

void rect_calc( rect_t *rect, const float3 p, const float3 n, const float3 raxis, float w, float h );
void rect_calc2( rect_t *rect, const float3 p, const float3 raxis, const float3 uaxis, float w, float h );
int32 rect_isect_ray( const rect_t *rect, const ray_t *ray, float *uval, float *vval );


typedef struct rect_isect_ray_test_s{
  int32  hit,
         away;
  float2 uv;
  float  dist;
  float3 p;
}rect_isect_ray_test_t;


int32 rect_isect_ray_test( const rect_t *rect, const ray_t *ray, rect_isect_ray_test_t *test );

