#pragma once

#include "mymath.h"

//Bounding Volumes - Spheres, Spheroids, Bounding Boxes (Axially Aligned/Oriented)

typedef struct sphere_s{
  float3 c;
  float  radius;
}sphere_t;

extern INLINE void sphere_empty( sphere_t *sphere );
extern void  sphere_set ( sphere_t *sphere, const float3 c, float radius );
extern void  sphere_calc( sphere_t *sphere, const float3 ps[], size_t nps );
extern int32 sphere_touch ( const sphere_t *sphere, const float3 p );
extern int32 sphere_touch2( const sphere_t *sphere, const float3 p, float3 n );
extern int32 sphere_sphere_hit( const sphere_t *sphere0, const sphere_t *sphere1 );
extern int32 sphere_lseg_hit( const sphere_t *sphere, const lseg_t *lseg, int32 ignore_corners );
extern int32 sphere_lseg_hit2( const sphere_t *sphere, const lseg_t *lseg, float3 norm, float3 pos );
extern int32 sphere_lseg_isect( const sphere_t *sphere, const lseg_t *lseg, float2 alpha );


#define ISA_BB        \
  float3   min,       \
           max;       \
  float3   size,      \
           halfsize;  \
  float3   ps[8];     \
  sphere_t sphere;    \

typedef struct{
  ISA_BB
}aabb_t;

extern void aabb_empty( aabb_t *aabb );
extern void aabb_set ( aabb_t *aabb, const float3 min, const float3 max, float pad, int calc_ps );
extern void aabb_fit ( aabb_t *aabb, const float3 *ps, size_t nps, float pad, int calc_ps );
extern void aabb_make_sat( const aabb_t *aabb, sat_t *sat, int32 second );
extern int32 aabb_lseg_hit( const aabb_t *aabb, const lseg_t *lseg );
extern void  aabb_lseg_clip( const aabb_t *aabb, lseg_t *lseg, int assume_touching );
extern INLINE int32 aabb_touch( const aabb_t *aabb, const float3 p );

typedef struct{
  ISA_BB
  float3 axes[3];
}obb_t;

extern void  obb_make_sat( const obb_t *obb, sat_t *sat, int32 second );
extern int32 obbs_touch  ( const obb_t *obb0, const obb_t *obb1 );
extern int32 obb_tri_hit ( const obb_t *obb, const plane_t *plane, const float3 ps[] );


typedef struct{
  INHERIT_LSEG
  float  radius;
}capsule_t;

extern void  capsule_calc( capsule_t *capsule, const float3 p0, const float3 p1, float radius );
extern int32 capsule_sphere_hit( const capsule_t *capsule, const sphere_t *sphere );
extern int32 capsule_touch( const capsule_t *capsule, const float3 p );

//axially-aligned spheroid
typedef struct spheroid_s{
  float3 c;
  float2 radii;         // horizontal, vertical
  float2 radii_invsq;   // 1 / ( radii[i]^2 )
  int32  oblate;
}spheroid_t;

extern void  spheroid_set   ( spheroid_t *spheroid, const float3 c, float a, float b );
extern int32 spheroid_touch ( const spheroid_t *spheroid, const float3 p );
extern int32 spheroid_touch2( const spheroid_t *spheroid, const float3 p, float3 n );
extern void  spheroid_calcpoint ( const spheroid_t *spheroid, const float3 v, float3 p );
extern void  spheroid_calcnormal( const spheroid_t *spheroid, const float3 v, float3 n, int32 centered );
extern void  spheroid_make_bounding_sphere( const spheroid_t *spheroid, sphere_t *sphere );
extern int32 spheroid_lseg_hit  ( const spheroid_t *spheroid, const lseg_t *lseg, float3 norm, float3 pos );
extern int32 spheroid_lseg_isect( const spheroid_t *spheroid, const lseg_t *lseg, float2 alpha );

typedef struct{
  struct{
    spheroid_t spheroid;
    float3     ps[3];
  }params;
  float3   ps[3];
  plane_t  plane;
  bcs3_t   bcs;
  sphere_t sphere;
  float2   st;
  int32    inside;
  float3   hitp;
}spheroid_insidetest_t;

extern int32 spheroid_testinside_tri( const spheroid_t *spheroid, const float3 face_ps[3], spheroid_insidetest_t *test );




