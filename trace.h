#pragma once

#include "collision.h"
#include "bpcd.h"


typedef enum{
  COLL_GEOM_TRACE_SPHERE,
  COLL_GEOM_TRACE_SPHEROID,
}coll_geom_trace_test_type_t;


typedef struct coll_geom_trace_object_s{
  union{
    sphere_t   sphere;
    spheroid_t spheroid;
  };
}coll_geom_trace_object_t;

typedef struct{
  int32  type;     //0 for edge, 1 for edge face 0, 2 for edge face 1, 3 for face, 4 for vert
  size_t feat_no;  //feature no. (edge/face/vert index)
  float3 p;
  float3 n;
}coll_geom_trace_info_t;

ARRAYDEF( collgeomtrace, coll_geom_trace_info_t );
#define collgeomtracearray_new( a, v )  collgeomtracearray_add1( a, v, ARRTAG )


typedef struct{
  float3 p, n;
}coll_hitpoint_t;


ARRAYDEF( collhit, coll_hitpoint_t );
#define collhitarray_add( a, v )  collhitarray_add0( a, v, ARRTAG )
#define collhitarray_new( a, v )  collhitarray_add1( a, v, ARRTAG )


typedef struct{
  const coll_geom_t *geom;
  const bpcd_grid_t *grid;
  struct{
    sizearray_t vertindices;
    sizearray_t edgeindices;
    sizearray_t faceindices;
  }cull, hits;

  collhitarray_t  verthitpoints;
  collhitarray_t  edgehitpoints;
  collhitarray_t  facehitpoints;

  collgeomtracearray_t traceinfos;

  dirty_tracker_t  verttracker;
  dirty_tracker_t  edgetracker;
  dirty_tracker_t  facetracker;

  int32            stuck;
  int32            penetrating;
  float            penetrating_dist;

  coll_geom_trace_test_type_t type;

}coll_geom_trace_test_t;

void coll_geom_trace_test_term( coll_geom_trace_test_t *test );
int32 coll_geom_trace_test_init(
    coll_geom_trace_test_t      *test,
    const coll_geom_t           *geom,
    const bpcd_grid_t           *grid,
    coll_geom_trace_test_type_t  type,
    int32                        term );

void coll_geom_trace_test_sphere( coll_geom_trace_test_t *test, sphere_t sphere );
void coll_geom_trace( coll_geom_trace_test_t *test, float3 dir );
