#pragma once

#include "defs.h"
#include "bound.h"
#include "array.h"
#include "arraytypes.h"
#include "dirty.h"
#include "strn.h"
#include "bpcd.h"

typedef struct{
  size_t index;
  str80  name;
}coll_surf_t;

ARRAYDEF( collsurf, coll_surf_t )
#define collsurfarray_new( a, v ) collsurfarray_add1( a, v, ARRTAG )

int32 coll_surf_compare( const void *ptr0, const void *ptr1 );

typedef struct {
  size_t   index;
  size_t   key,
           edgeindices[3],
           vertindices[3],
           surfindex;
  float3   ps[3];
  sphere_t sphere;
  bcs3_t   bcs;
  plane_t  plane,
           sideplanes[3];
}coll_face_t;

ARRAYDEF( collface, coll_face_t )
#define collfacearray_new( a, v ) collfacearray_add1( a, v, ARRTAG )

typedef struct {
  size_t   index,
           faceindices[2],
           vertindices[2];
  int32    convex;
  INHERIT_LSEG
  float3   vs[2];
  sphere_t sphere;
  const collfacearray_t *_collfaces;

}coll_edge_t;

ARRAYDEF( colledge, coll_edge_t )
#define colledgearray_new( a, v ) colledgearray_add1( a, v, ARRTAG )

#define COLL_VERT_MAX_FEATS 40

typedef struct{
  size_t index,
          //    edgeindex = 2*(edge #) + (0/1 for begin/end point)
          //       edge # = edgeindex / 2;
          //  which point = edgeindex & 0b01 (0 for begin, 1 for end)
          //           -1 = end of list
         edgeindices[ COLL_VERT_MAX_FEATS ],
         faceindices[ COLL_VERT_MAX_FEATS ];
  float3 p;
  const colledgearray_t *_colledges;
  const collfacearray_t *_collfaces;
}coll_vert_t;

ARRAYDEF( collvert, coll_vert_t )
#define collvertarray_new( a, v ) collvertarray_add1( a, v, ARRTAG )

void coll_vert_init( coll_vert_t *vert,
                            size_t index, const float3 p,
                            const colledgearray_t *colledges,
                            const collfacearray_t *collfaces );


//collision feature type
#define COLL_FEAT_FACE  0
#define COLL_FEAT_EDGE  1
#define COLL_FEAT_EDGE_REVERSE -1

void  coll_vert_add_feat ( coll_vert_t *vert, int32 feat_type, size_t feat_no );
void  coll_edge_calc      ( coll_edge_t *edge, const float3 p0, const float3 p1 );
int32 coll_edge_sphere_hit( const coll_edge_t *edge, const sphere_t *sphere, int32 ignore_corners );
void  coll_edge_calc_voronoi  ( coll_edge_t *edge );
int32 coll_edge_inside_voronoi( const coll_edge_t *edge, const float3 n, int32 flip );
void  coll_vert_constrict_normal( const coll_vert_t *vert, float3 n );
void  coll_face_calc( coll_face_t *face, const float3 p0, const float3 p1, const float3 p2 );
int32 coll_face_sphere_hit( const coll_face_t *face, const coll_edge_t *edges, const sphere_t *sphere );
int32 coll_face_aabb_hit  ( const coll_face_t *face, const aabb_t *aabb );
int32 coll_face_lseg_isect( const coll_face_t *face, const lseg_t *lseg, float *rate );

INLINE void coll_edge_get_faces( const coll_edge_t *edge, const coll_face_t **face0, const coll_face_t **face1 );

typedef struct{
  size_t index;
  float3 p;
  float  dist,
         mu, nu;
  int32  facing;
}coll_face_lseg_isect_t;

int32 coll_face_lseg_isect_ex( const coll_face_t *face, const lseg_t *lseg, int32 both_sides, coll_face_lseg_isect_t *isect );
int32 coll_face_lseg_touch   ( const coll_face_t *face, const lseg_t *lseg, int32 both_sides );

typedef struct coll_geom_s{
  collsurfarray_t surfs;
  collvertarray_t verts;
  collfacearray_t faces;
  colledgearray_t edges;
  aabb_t   aabb;
  int32    _finalized;
}coll_geom_t;


void coll_geom_term( coll_geom_t *geom );
void coll_geom_addsurf( coll_geom_t *geom, const char name[] );
size_t coll_geom_addedge( coll_geom_t *geom, const float3 p0, const float3 p1, size_t face_no );
size_t coll_geom_addface(
    coll_geom_t *geom,
    const float3 p0, const float3 p1, const float3 p2,
    size_t surf_no, size_t key );
int32 coll_geom_finalize( coll_geom_t *geom );
int32 coll_geom_face_sphere_hit( const coll_geom_t *geom, size_t face_no, const sphere_t *sphere, int32 validate );
int32 coll_geom_edge_sphere_hit(  const coll_geom_t *geom, size_t edge_no, const sphere_t *sphere, int32 ignore_corners, int32 validate );
void  coll_geom_getedgefaces   ( const coll_geom_t *geom, size_t edge_no, const coll_face_t **face0, const coll_face_t **face1 );

INLINE int32        coll_geom_isloaded( const coll_geom_t *geom );
INLINE coll_edge_t *coll_geom_getedge( coll_geom_t *geom, size_t index );
INLINE coll_face_t *coll_geom_getface( coll_geom_t *geom, size_t index );
INLINE const coll_face_t *coll_geom_getface_ro( const coll_geom_t *geom, size_t index );


int32 coll_geom_load( coll_geom_t *geom, const char objfile[], int32 term );
int32 coll_geom_load_grid( const coll_geom_t *geom, bpcd_grid_t *grid, float cellsize );
