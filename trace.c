#include "trace.h"

#include "array.h"
#include "strn.h"
#include "floatn.h"

#include "libs.h"


/*
void coll_geom_trace_get_edge_info( const array_t          *colledges,
                                    size_t                  edge_no,
                                    coll_geom_trace_info_t *info ){
  float3 n = { 0.0f, 0.0f, 0.0f };
  if( e->convex > 0 ){
    if( f3dot( edata[i].n, f0->plane.n ) > 0.0f &&
        f3dot( edata[i].n,  e->vs[0]   ) < 0.0f ){
      f3copy( n, f0->plane.n );
      info.type    = 1;
      info.feat_no = f0->index;
      f3copy( info.n, n );
      f3copy( info.p, edata[i].p );
    }
    else
    if( f3dot( edata[i].n, f1->plane.n ) > 0.0f &&
        f3dot( edata[i].n,  e->vs[1]   ) < 0.0f ){
      f3copy( n, f1->plane.n );
      info.type    = 2;
      info.feat_no = f1->index;
      f3copy( info.n, n );
      f3copy( info.p, edata[i].p );
    }
    else{
      f3copy( n, edata[i].n );
      info.type    = 0;
      info.feat_no = eindices[i];
      f3copy( info.n, n );
      f3copy( info.p, edata[i].p );
    }
  }
  else{
    f3copy( n, f0->plane.n );
    info.type    = 1;
    info.feat_no = f0->index;
    f3copy( info.n, n );
    f3copy( info.p, edata[i].p );
  }
}
*/

ARRAYFUNC( collgeomtrace, coll_geom_trace_info_t );
ARRAYFUNC( collhit, coll_hitpoint_t );

void coll_geom_trace_test_tracker_term( coll_geom_trace_test_tracker_t *tracker ){
  dirty_tracker_term( &tracker->verts );
  dirty_tracker_term( &tracker->edges );
  dirty_tracker_term( &tracker->faces );
}

void coll_geom_trace_test_tracker_init( coll_geom_trace_test_tracker_t *tracker,
                                        size_t vertcount,
                                        size_t edgecount,
                                        size_t facecount ){
  dirty_tracker_init( &tracker->verts, vertcount );
  dirty_tracker_init( &tracker->edges, edgecount );
  dirty_tracker_init( &tracker->faces, facecount );
}

void coll_geom_trace_test_term( coll_geom_trace_test_t *test ){
  array_term( &test->cull.vertindices.array );
  array_term( &test->cull.edgeindices.array );
  array_term( &test->cull.faceindices.array );
  array_term( &test->hits.vertindices.array );
  array_term( &test->hits.edgeindices.array );
  array_term( &test->hits.faceindices.array );
  array_term( &test->verthitpoints.array  );
  array_term( &test->edgehitpoints.array  );
  array_term( &test->facehitpoints.array  );
  array_term( &test->traceinfos.array );
  coll_geom_trace_test_tracker_term( &test->tracker );
  memset( test, 0, sizeof(coll_geom_trace_test_t) );
}


int32 coll_geom_trace_test_init(
    coll_geom_trace_test_t      *test,
    const coll_geom_t           *geom,
    const bpcd_grid_t           *grid,
    coll_geom_trace_test_type_t  type,
    int32                        term ){
  if( !coll_geom_isloaded( geom ) ){
    printf( "%s - error: collision geometry not loaded\n", __FUNCTION__ );
    return 0;
  }
  if( term )
    coll_geom_trace_test_term( test );
  else
    memset( test, 0, sizeof(coll_geom_trace_test_t) );

  test->geom = geom;
  test->grid = grid;
  test->type = type;

  test->cull.vertindices = sizearray_make( 128 );
  test->cull.edgeindices = sizearray_make( 128 );
  test->cull.faceindices = sizearray_make( 128 );

  test->hits.vertindices = sizearray_make( 128 );
  test->hits.edgeindices = sizearray_make( 128 );
  test->hits.faceindices = sizearray_make( 128 );

  test->verthitpoints  = collhitarray_make( 128 );
  test->edgehitpoints  = collhitarray_make( 128 );
  test->facehitpoints  = collhitarray_make( 128 );

  test->traceinfos = collgeomtracearray_make( 128 );
  coll_geom_trace_test_tracker_init( &test->tracker,
                                     geom->verts.size, geom->edges.size, geom->faces.size );

  return 1;
}

static INLINE void coll_geom_trace_test_tracker_reset( coll_geom_trace_test_tracker_t *tracker, int32 type ){
  switch( type )
  {
    case 2: dirty_tracker_next( &tracker->faces ); break;
    case 1: dirty_tracker_next( &tracker->edges ); break;
    case 0: dirty_tracker_next( &tracker->verts ); break;
    default:
      dirty_tracker_next( &tracker->faces );
      dirty_tracker_next( &tracker->edges );
      dirty_tracker_next( &tracker->verts );
      break;
  }
}


static INLINE void coll_geom_trace_test_tracker_flag( coll_geom_trace_test_tracker_t *tracker, size_t no, int32 type ){
  switch( type )
  {
    case 2: dirty_tracker_clean( &tracker->faces, no ); break;
    case 1: dirty_tracker_clean( &tracker->edges, no ); break;
    case 0:
    default:dirty_tracker_clean( &tracker->verts, no ); break;
  }
}

static INLINE int32 coll_geom_trace_test_tracker_flagged( const coll_geom_trace_test_tracker_t *tracker, size_t no, int32 type ){
  switch( type )
  {
    case 2:  return dirty_tracker_isclean( &tracker->faces, no ); break;
    case 1:  return dirty_tracker_isclean( &tracker->edges, no ); break;
    case 0:
    default: return dirty_tracker_isclean( &tracker->verts, no ); break;
  }
}
static INLINE void coll_geom_trace_test_tracker_unflag( coll_geom_trace_test_tracker_t *tracker, size_t no, int32 type ){
  switch( type )
  {
    case 2: dirty_tracker_dirty( &tracker->faces, no ); break;
    case 1: dirty_tracker_dirty( &tracker->edges, no ); break;
    case 0:
    default:dirty_tracker_dirty( &tracker->verts, no ); break;
  }
}

static INLINE int32 coll_geom_trace_test_tracker_unflagged( const coll_geom_trace_test_tracker_t *tracker, size_t no, int32 type ){
  switch( type )
  {
    case 2:  return dirty_tracker_isdirty2( &tracker->faces, no ); break;
    case 1:  return dirty_tracker_isdirty2( &tracker->edges, no ); break;
    case 0:
    default: return dirty_tracker_isdirty2( &tracker->verts, no ); break;
  }
}

//0 for vertex hit, 1 for edge hit, 2 for face hit
static INLINE void _coll_geom_trace_push_hit( coll_geom_trace_test_t *test, coll_hitpoint_t hit, size_t no, int32 type ){
  collhitarray_t  *hitpoints = &test->verthitpoints;
  sizearray_t     *indices   = &test->hits.vertindices;
  if( 1 == type ){
    hitpoints = &test->edgehitpoints;
    indices   = &test->hits.edgeindices;
  }
  if( 2 == type ){
    hitpoints = &test->facehitpoints;
    indices   = &test->hits.faceindices;
  }
  collhitarray_add( hitpoints, hit );
  sizearray_add( indices, no );
  coll_geom_trace_test_tracker_flag( &test->tracker, no, type );
}

#define VERT_FLAGGED( i ) coll_geom_trace_test_tracker_flagged( &test->tracker, i, 0 )
#define FLAG_VERT( i ) coll_geom_trace_test_tracker_flag   ( &test->tracker, i, 0 )
#define VERT_UNFLAGGED( I ) coll_geom_trace_test_tracker_unflagged( &test->tracker, i, 0 )
#define UNFLAG_VERT( i ) coll_geom_trace_test_tracker_unflag( &test->tracker, i, 0 )

#define EDGE_FLAGGED( i ) coll_geom_trace_test_tracker_flagged( &test->tracker, i, 1 )
#define FLAG_EDGE( i ) coll_geom_trace_test_tracker_flag   ( &test->tracker, i, 1 )
#define EDGE_UNFLAGGED( I ) coll_geom_trace_test_tracker_unflagged( &test->tracker, i, 1 )
#define UNFLAG_EDGE( i ) coll_geom_trace_test_tracker_unflag( &test->tracker, i, 1 )

#define FACE_FLAGGED( i ) coll_geom_trace_test_tracker_flagged( &test->tracker, i, 2 )
#define FLAG_FACE( i ) coll_geom_trace_test_tracker_flag   ( &test->tracker, i, 2 );
#define FACE_UNFLAGGED( I ) coll_geom_trace_test_tracker_unflagged( &test->tracker, i, 2 )
#define UNFLAG_FACE( i ) coll_geom_trace_test_tracker_unflag( &test->tracker, i, 2 )

void _coll_geom_trace_gather_vert_hits( coll_geom_trace_test_t *test, const coll_geom_trace_object_t *object ){
  array_const_indirect_iter_t it = array_const_indirect_iter_init( &test->geom->verts.array,
                                                                   test->cull.vertindices.elems,
                                                                   test->cull.vertindices.size );
  while( array_iterate_indirect_const( &it ) ){
    float3          n;
    coll_hitpoint_t hit;
    int32           touching;
    size_t          i = it.j;
    const coll_vert_t *v = it.data;
    if( !v )
      continue;

    if( FACE_FLAGGED(i) )
      continue;
    //FLAG_FACE(i);  //NOTE: should we flag here, or after hit?

    f3copy( hit.p, v->p );

    if( COLL_GEOM_TRACE_SPHERE == test->type )
      touching = sphere_touch2  ( &object->sphere,   v->p, n );
    else
    if( COLL_GEOM_TRACE_SPHEROID == test->type )
      touching = spheroid_touch2( &object->spheroid, v->p, n );

    if( !touching )
      continue;
    f3muls( n, -1.0f );

    f3copy( hit.n, n );
    _coll_geom_trace_push_hit( test, hit, i, 0 );

    //edges
    for( size_t i = 0; i < COLL_VERT_MAX_FEATS; i++ ){
      if( -1 == v->edgeindices[i] )
        break;
      size_t j = v->edgeindices[i] >> 1;
      if( EDGE_FLAGGED(j) )
        continue; //already resolved
      const coll_edge_t *e = &test->geom->edges.elems[j];
      if( !e )
        continue; //should never happen...

      f3copy( hit.n, n );
      float proj = f3dot( hit.n, e->dir );
      if( fabsf(proj) > 0.0f ){
        f3madd( hit.n, -proj, e->dir ); //zero component along edge dir
        f3norm( hit.n );
      }
      _coll_geom_trace_push_hit( test, hit, j, 1 ); //this will flag edge
    }

    //faces
    for( size_t i = 0; i < COLL_VERT_MAX_FEATS; i++ ){
      if( -1 == v->faceindices[i] )
        break;
      size_t j = v->faceindices[i];
      if( FACE_FLAGGED(j) )
        continue;
      const coll_face_t *f = &test->geom->faces.elems[j];
      if( !f )
        continue; //should never happen...

      f3copy( hit.n, f->plane.n );
      _coll_geom_trace_push_hit( test, hit, j, 2 );
    }
  }

}

void _coll_geom_trace_gather_edge_hits( coll_geom_trace_test_t *test, const coll_geom_trace_object_t *object ){
  array_const_indirect_iter_t it = array_const_indirect_iter_init( &test->geom->edges.array,
                                                                   test->cull.edgeindices.elems,
                                                                   test->cull.edgeindices.size );
  while( array_iterate_indirect_const( &it ) ){
    float3          p, n;
    coll_hitpoint_t hit;
    int32           hitting;
    const coll_edge_t *e = it.data;
    size_t          i = it.j;
    if( !e || -1 == e->faceindices[0] || -1 == e->faceindices[1] )
      continue;

    //edges with verts making contact with object have been
    //flagged already added to the hit list (indices & hits)
    if( EDGE_FLAGGED(i) )
      continue;
    //FLAG_EDGE(i); //NOTE: should we flag here, or after hit?

    //now, only edges left to test are the ones with end points
    //not touching the object. add these edges to the hit list (indices & hits)
    //(indices & data)
    if( COLL_GEOM_TRACE_SPHERE   == test->type ){
      hitting = sphere_lseg_hit2 ( &object->sphere,   &e->lseg, n, p );  //this test ignores corners
    }
    else
    if( COLL_GEOM_TRACE_SPHEROID == test->type ){
      hitting = spheroid_lseg_hit( &object->spheroid, &e->lseg, n, p );  //possibly add ignore corners check?
    }

    if( !hitting )
      continue;

    f3copy( hit.p, p );
    f3copy( hit.n, n );
    f3muls( hit.n, -1.0f );
    _coll_geom_trace_push_hit( test, hit, i, 1 );

    f3copy( hit.n, test->geom->faces.elems[ e->faceindices[0] ].plane.n );
    _coll_geom_trace_push_hit( test, hit, e->faceindices[0], 2 ); //this will flag face for us

    f3copy( hit.n, test->geom->faces.elems[ e->faceindices[1] ].plane.n );
    _coll_geom_trace_push_hit( test, hit, e->faceindices[1], 2 ); //this will flag face for us
  }

}

void _coll_geom_trace_gather_face_hits( coll_geom_trace_test_t *test, const coll_geom_trace_object_t *object ){
  array_const_indirect_iter_t it = array_const_indirect_iter_init( &test->geom->faces.array,
                                                                   test->cull.faceindices.elems,
                                                                   test->cull.faceindices.size );
  while( array_iterate_indirect_const( &it ) ){
    const coll_face_t *f = it.data;
    if( !f )
      continue;

    //faces with edges/verts making contact with object have been
    //flagged and already added to the hit list (indices & data)
    if( FACE_FLAGGED( it.j) )
      continue;
    FLAG_FACE(it.j);  //should we flag here or on hit

    //now, only faces left to test are the ones within which the entire
    //object resides (i.e, touches face but not it's edges). add these
    //faces to the hit list (indices & hits)
    int32  inside = 0;
    float3 p;
    if( COLL_GEOM_TRACE_SPHERE   == test->type ){
      inside = fabsf( plane_solve( &f->plane, object->sphere.c ) ) <= object->sphere.radius;
      if( inside ){
        float a, b;
        inside = bcs3_find( &f->bcs, object->sphere.c, &a, &b );
        if( inside ){
          f3copy( p, f->bcs.o );
          f3madd( p, a, f->bcs.u );
          f3madd( p, b, f->bcs.v );
        }
      }
    }
    else
    if( COLL_GEOM_TRACE_SPHEROID == test->type ){
      spheroid_insidetest_t  insidetest;
      inside = spheroid_testinside_tri( &object->spheroid, f->ps, &insidetest );
      f3copy( p, insidetest.hitp );
    }

    if( inside ){
      coll_hitpoint_t hit;
      f3copy( hit.p, p );
      f3copy( hit.n, f->plane.n );
      _coll_geom_trace_push_hit( test, hit, it.j, 2 );
    }
  }
}


void _coll_geom_trace_test_cull( coll_geom_trace_test_t *test, sphere_t sphere, const coll_face_t *face ){
  size_t index = face->index;
  if( FACE_FLAGGED(index) )
    return;
  FLAG_FACE(index);

  if( !sphere_sphere_hit( &sphere, &face->sphere ) )
    return;
  sizearray_add( &test->cull.faceindices, index );

  for( size_t i = 0; i < 3; i++ ){
    index = face->vertindices[i];
    if( VERT_FLAGGED(index) )
      continue;
    FLAG_VERT(index);
    sizearray_add( &test->cull.vertindices, index );
  }

  for( size_t i = 0; i < 3; i++ ){
    index = face->edgeindices[i];
    if( EDGE_FLAGGED(index) )
      continue;
    FLAG_EDGE(index);
    sizearray_add( &test->cull.edgeindices, index );
  }
}

void coll_geom_trace_test_sphere( coll_geom_trace_test_t *test, sphere_t sphere ){
  test->cull.vertindices.size = 0;
  test->cull.edgeindices.size = 0;
  test->cull.faceindices.size = 0;

  test->verthitpoints.size = 0;
  test->edgehitpoints.size = 0;
  test->facehitpoints.size = 0;
  coll_geom_trace_test_tracker_reset( &test->tracker, -1 );

  test->type = COLL_GEOM_TRACE_SPHERE;
  coll_geom_trace_object_t obj;
  obj.sphere = sphere;

  if( test->grid ){
    float3 h;
    f3set( h, sphere.radius, sphere.radius, sphere.radius );
    bpcd_grid_sector_t sector = bpcd_grid_sector_make( test->grid, sphere.c, h );
    for( size_t l = sector.ls[0]; l <= sector.ls[1]; l++ )
      for( size_t r = sector.rs[0]; r <= sector.rs[1]; r++ )
        for( size_t c = sector.cs[0]; c <= sector.cs[1]; c++ ){
          size_t no = test->grid->levs[l].rows[r].cols[c].cell;
          if( -1 ==  no )
            continue;
          const bpcd_grid_cell_t *cell = array_data_ro( &test->grid->cells, no );
          array_const_indirect_iter_t kiit = array_const_indirect_iter_init( &test->geom->faces.array,
                                                                             cell->indices.elems,
                                                                             cell->indices.size );
          while( array_iterate_indirect_const( &kiit ) ){
            const coll_face_t *face = kiit.data;
            _coll_geom_trace_test_cull( test, sphere, face );
          }
        }
  }
  else{
    array_const_iter_t kit = array_const_iter_init( &test->geom->faces.array );
    while( array_iterate_const( &kit ) ){
      const coll_face_t *face = kit.data;
      _coll_geom_trace_test_cull( test, sphere, face );
    }
  }
  /*
  struct{
    size_t elems[256];
  } *dbg;
  dbg = (void *)test->cull.faceindices.elems;
  dbg = (void *)test->cull.edgeindices.elems;
  dbg = (void *)test->cull.edgeindices.elems;
  */

  test->hits.vertindices.size = 0;
  test->hits.edgeindices.size = 0;
  test->hits.faceindices.size = 0;
  test->verthitpoints.size    = 0;
  test->edgehitpoints.size    = 0;
  test->facehitpoints.size    = 0;
  coll_geom_trace_test_tracker_reset( &test->tracker, -1 );

  _coll_geom_trace_gather_vert_hits( test, &obj );
  _coll_geom_trace_gather_edge_hits( test, &obj );
  _coll_geom_trace_gather_face_hits( test, &obj );
}


void coll_geom_trace( coll_geom_trace_test_t *test, float3 dir ){

  const coll_geom_t *geom = test->geom;

  size_t                 numvs    = test->hits.vertindices.size;
  const size_t          *vindices = test->hits.vertindices.elems;
  const coll_hitpoint_t *vhits    = test->verthitpoints.elems;

  size_t                 numes    = test->hits.edgeindices.size;
  const size_t          *eindices = test->hits.edgeindices.elems;
  const coll_hitpoint_t *ehits    = test->edgehitpoints.elems;

  size_t                 numfs    = test->hits.faceindices.size;
  const size_t          *findices = test->hits.faceindices.elems;
  const coll_hitpoint_t *fhits    = test->facehitpoints.elems;

  test->penetrating = 0;
  test->penetrating_dist = -99999.f;
  test->stuck = 0;

  test->traceinfos.size = 0;

  coll_geom_trace_info_t info;
  memset( &info, 0, sizeof(coll_geom_trace_info_t) );

  //+++ RESOLVE +++//

  //test vertices first, dirty edges and faces if necessary
  for( size_t i = 0; i < numvs; i++ ){
    const coll_vert_t *v  = &geom->verts.elems[ vindices[i] ];
    size_t  nes    = 0,
            nfs    = 0;

    for( size_t j = 0; j < COLL_VERT_MAX_FEATS; j++ ){
      if( -1 != v->edgeindices[j] )
        nes++;
      if( -1 != v->faceindices[j] )
        nfs++;
    }

    //dirty all edges (assume we wont need to test)
    for( size_t j = 0; j < nes; j++ )
      UNFLAG_EDGE( v->edgeindices[j] >> 1 );

    //dirty all faces (assume we wont need to test)
    for( size_t j = 0; j < nfs; j++ )
      UNFLAG_FACE( v->faceindices[j] );

    int32 within = 1;
    for( size_t j = 0; j < nes; j++ ){
      size_t k   = v->edgeindices[j] >> 1;
      int32 flip = v->edgeindices[j] &  0b01;
      const coll_edge_t *e = &geom->edges.elems[k];
      if( coll_edge_inside_voronoi( e, vhits[i].n, flip ) ){
        within = 0;
        //clean this edge and its faces to allow testing (assumption wrong). the test
        //will populate traceinfo.
        FLAG_EDGE( k );
        FLAG_FACE( e->faceindices[0] );
        FLAG_FACE( e->faceindices[1] );
      }
    }
    if( !within )
      continue;

    info.type    = 4;
    info.feat_no = v->index;
    f3copy( info.p, vhits[i].p );
    f3copy( info.n, vhits[i].n );
    collgeomtracearray_new( &test->traceinfos, &info );

    float ddotn = f3dot( dir, info.n );
    if( ddotn < 0.0f ){
      if( ddotn > test->penetrating_dist )
        test->penetrating_dist = ddotn;
      f3madd( dir, -ddotn * 1.001f, vhits[i].n );
      test->penetrating++;
    }
  }

  //test clean edges next, dirty faces if necessary (assuming wont test)
  for( size_t i = 0; i < numes; i++ ){
    const coll_edge_t *e  = &geom->edges.elems[ eindices[i] ];
    if( FACE_UNFLAGGED( eindices[i] ) )
      continue;

    const coll_face_t *f0 = &geom->faces.elems[ e->faceindices[0] ];
    const coll_face_t *f1 = &geom->faces.elems[ e->faceindices[1] ];
    if( e->convex < 0 || NULL == f1 )
      continue;

    UNFLAG_FACE( e->faceindices[0] );
    UNFLAG_FACE( e->faceindices[1] );

    float3 n = { 0.0f, 0.0f, 0.0f };
    if( e->convex > 0 ){
      if( f3dot( ehits[i].n, f0->plane.n ) > 0.0f &&
          f3dot( ehits[i].n,  e->vs[0]   ) < 0.0f ){
        f3copy( n, f0->plane.n );
        info.type    = 1;
        info.feat_no = f0->index;
        f3copy( info.n, n );
        f3copy( info.p, ehits[i].p );
      }
      else
      if( f3dot( ehits[i].n, f1->plane.n ) > 0.0f &&
          f3dot( ehits[i].n,  e->vs[1]   ) < 0.0f ){
        f3copy( n, f1->plane.n );
        info.type    = 2;
        info.feat_no = f1->index;
        f3copy( info.n, n );
        f3copy( info.p, ehits[i].p );
      }
      else{
        f3copy( n, ehits[i].n );
        info.type    = 0;
        info.feat_no = eindices[i];
        f3copy( info.n, n );
        f3copy( info.p, ehits[i].p );
      }
    }
    else{
      f3copy( n, f0->plane.n );
      info.type    = 1;
      info.feat_no = f0->index;
      f3copy( info.n, n );
      f3copy( info.p, ehits[i].p );
    }
    collgeomtracearray_new( &test->traceinfos, &info );

    float ddotn = f3dot( dir, n );
    if( ddotn < 0.0f ){
      if( ddotn > test->penetrating_dist )
        test->penetrating_dist = ddotn;
      f3madd( dir, -ddotn * 1.001f, n );
      test->penetrating++;
    }
  }

  //test clean faces last
  for( size_t i = 0; i < numfs; i++ ){
    const coll_face_t *f = &geom->faces.elems[ findices[i] ];
    if( FACE_UNFLAGGED( findices[i] ) )
      continue;

    float ddotn = f3dot( dir, f->plane.n );
    if( ddotn < 0.0f ){
      if( ddotn > test->penetrating_dist )
        test->penetrating_dist = ddotn;
      f3madd( dir, -ddotn * 1.001f, f->plane.n );
      test->penetrating++;
    }
    info.type    = 3;
    info.feat_no = findices[i];
    f3copy( info.n, fhits[i].n );
    f3copy( info.p, fhits[i].p );
    collgeomtracearray_new( &test->traceinfos, &info );
  }

  coll_geom_trace_info_t *infos    = test->traceinfos.elems;
  size_t                  numinfos = test->traceinfos.size;
  for( size_t i = 0; i < numinfos; i++ ){
    if( f3dot( dir, infos[i].n ) < 0.0f ){
      test->stuck = 1;
      break;
    }
  }

  if( test->stuck ){
    for( size_t i = 0; i < numinfos-1; i++ ){
      int32 found = 0;
      for( size_t j = i+1; j < numinfos; j++ ){
        float3 d;
        f3cross( infos[i].n, infos[j].n, d );
        if( fabsf(f3dot( d, d )) == 0.0f )
          continue;

        int32 bad = 0;
        for( size_t k = 0; k < numinfos; k++ )
          if( f3dot( infos[k].n, d ) < 0.0f ){
            bad++;
            break;
          }
        if( bad )
          continue;

        found++;
        f3norm( d );
        f3muls( d, f3dot( dir, d ) );
        f3copy( dir, d );
        test->stuck = 0;
        break;
      }
      if( found )
        break;
    }
  }

}
