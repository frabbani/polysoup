#include "collision.h"

#include "array.h"
#include "strn.h"
#include "floatn.h"

#include "libs.h"

ARRAYFUNC( collsurf, coll_surf_t )
ARRAYFUNC( collface, coll_face_t )
ARRAYFUNC( colledge, coll_edge_t )
ARRAYFUNC( collvert, coll_vert_t )

void coll_vert_init( coll_vert_t *vert,
                     size_t index, const float3 p,
                     const colledgearray_t *colledges,
                     const collfacearray_t *collfaces ){
  f3copy( vert->p, p );
  vert->index =  index;
  for( size_t i = 0; i < COLL_VERT_MAX_FEATS; i++ )
    vert->edgeindices[i] = vert->faceindices[i] = -1;
  vert->_colledges = colledges;
  vert->_collfaces = collfaces;
}

void coll_edge_get_faces( const coll_edge_t *edge, coll_face_t const **face0, coll_face_t const **face1 ){
  if( face0 )
    *face0 = edge->faceindices[0] == -1 ? NULL : &edge->_collfaces->elems[ edge->faceindices[0] ];
  if( face1 )
    *face1 = edge->faceindices[1] == -1 ? NULL : &edge->_collfaces->elems[ edge->faceindices[1] ];
}

int32 coll_surf_compare( const void *ptr0, const void *ptr1 ){
  const coll_surf_t *surf0 = ptr0;
  const coll_surf_t *surf1 = ptr1;
  return strcmp( surf0->name.chars, surf1->name.chars );
}


void coll_vert_add_feat( coll_vert_t *vert, int32 feat_type, size_t feat_no ){
  size_t *indices = COLL_FEAT_FACE == feat_type ? vert->faceindices : vert->edgeindices;
  size_t  no      = feat_no;
  if( +1 == feat_type )   //forward edge
    no = feat_no * 2 + 0;
  else
  if( -1 == feat_type )   //reverse edge
    no = feat_no * 2 + 1;

  size_t i;
  for( i = 0; i < COLL_VERT_MAX_FEATS; i++ ){
    if( indices[i] == no )
      break;  //already added
    if( -1 == indices[i] ){
      indices[i] = no;
      break;
    }
  }
  if( COLL_VERT_MAX_FEATS == i )
    printf( "%s:warning - exceeded maximum no. of features supported (%u)\n", __FUNCTION__, COLL_VERT_MAX_FEATS );
}

/*
int32 coll_vert_normal_within( const coll_vert_t *vert, const float3 n ){

  int32 within = 1;
  for( size_t i = 0; i < COLL_VERT_MAX_FEATS; i++ ){
    if( -1 == vert->edgeindices[i] )
      break;
    size_t k   = vert->edgeindices[i] >> 1;
    int32 flip = vert->edgeindices[i] & 0b01;
    const coll_edge_t *e = array_data_ro( vert->_colledges, k );
    if( !e )
      continue;
    float3 u;
    f3copy( u, e->dir );
    if( flip )
      f3muls( u, -1.0f );
    if( f3dot( n, u ) > 0.0f ){
      within = 0;
      break;
    }
  }
  return within;
}
*/

void coll_edge_calc( coll_edge_t *edge, const float3 p0, const float3 p1 ){
  edge->index  = edge->faceindices[0] = edge->faceindices[1] = -1;
  edge->convex = 0;
  lseg_calc( &edge->lseg, p0, p1 );
  sphere_calc( &edge->sphere, (const float3*)edge->lseg.ps, 2 );
}


int32 coll_edge_sphere_hit( const coll_edge_t *edge, const sphere_t *sphere, int32 ignore_corners ){
  return sphere_lseg_hit( sphere, &edge->lseg, ignore_corners );
}


void coll_edge_calc_voronoi( coll_edge_t *edge ){
  if( edge->faceindices[0] != -1 &&
      edge->faceindices[1] != -1 ){
    const coll_face_t *face0 = &edge->_collfaces->elems[ edge->faceindices[0] ];
    const coll_face_t *face1 = &edge->_collfaces->elems[ edge->faceindices[1] ];
    int32  i;
    float3 u = { 0.0f, 0.0f, 0.0f };
    for( i = 0; i < 3; i++ )
      if( face1->edgeindices[i] == edge->index ){
        f3make( u, face1->ps[i], face1->ps[(i+2)%3] );  // use the point away from edge
                                                        // to determine inward direction
        break;
      }
    float dot = f3dot( u, face0->plane.n );
    if( dot < 0.0f )
      edge->convex = +1;
    else if( dot > 0.0f )
      edge->convex = -1;
    //printf( "%s - edge %d faces {%d, %d}\n",
    //    __FUNCTION__, edge->index, edge->faces[0], edge->faces[1] );
    //printf( " * faces 1 edges {%d, %d, %d}\n", face0->edges[0], face0->edges[1], face0->edges[2] );
    //printf( " * faces 2 edges {%d, %d, %d}\n", face1->edges[0], face1->edges[1], face1->edges[2] );
    //printf( " * convex type is %d\n", edge->convex );

    // the edge vector matches that of face #1
    f3cross( edge->dir, face0->plane.n, edge->vs[0] );
    f3norm ( edge->vs[0] );

    // the edge vector is reversed for face #2
    f3cross( face1->plane.n, edge->dir, edge->vs[1] );
    f3norm ( edge->vs[1] );

  }
}

int32 coll_edge_inside_voronoi( const coll_edge_t *edge, const float3 n, int32 flip ){
  if( edge->convex &&
      -1 != edge->faceindices[0] &&
      -1 != edge->faceindices[1] ){

    int32 outside = 0;
    float3 u;
    f3copy( u, edge->dir );
    if( flip )
      f3muls( u, -1.0f );
    if( f3dot( n, u ) < 0.0f )
      outside++;

    const coll_face_t *face0 = &edge->_collfaces->elems[ edge->faceindices[0] ];
    const coll_face_t *face1 = &edge->_collfaces->elems[ edge->faceindices[1] ];
    if( f3dot( n, face0->plane.n ) > 0.0f &&
        f3dot( n, edge->vs[0]    ) < 0.0f )
      outside++;
    if( f3dot( n, face1->plane.n ) > 0.0f &&
        f3dot( n, edge->vs[1]    ) < 0.0f )
      outside++;
    return 0 == outside;
  }
  return 0;
}

void coll_vert_constrict_normal( const coll_vert_t *vert, float3 n ){
  for( size_t i = 0; i < COLL_VERT_MAX_FEATS; i++ ){
    if( -1 == vert->edgeindices[i] )
      break;
    size_t k   = vert->edgeindices[i] >> 1;
    int32 flip = vert->edgeindices[i] & 0b01;
    const coll_edge_t *e = &vert->_colledges->elems[k];
    if( !e ) //|| !coll_edge_inside_voronoi( e, n ) )
      continue;

    float3 u;
    f3copy( u, e->dir );
    if( flip )
      f3muls( u, -1.0f );
    float proj = f3dot( n, u );
    if( proj > 0.0f )
      f3madd( n, -proj, u );
  }
  f3norm( n );
}


void coll_face_calc( coll_face_t *face, const float3 p0, const float3 p1, const float3 p2 ){
  int32  i;
  float3 u, v;
  f3copy( face->ps[0], p0 );
  f3copy( face->ps[1], p1 );
  f3copy( face->ps[2], p2 );
  sphere_calc( &face->sphere, (const float3*)face->ps, 3 );
  plane_calc ( &face->plane,  face->ps[0], face->ps[1], face->ps[2] );
  bcs3_calc  ( &face->bcs,    face->ps[0], face->ps[1], face->ps[2] );
  for( i = 0; i < 3; i++ ){
    f3make ( u, face->ps[i], face->ps[ (i+1)%3 ] );
    f3cross( u, face->plane.n, v );
    plane_make( &face->sideplanes[i], v, face->ps[i] );
  }
  face->index = face->edgeindices[0] = face->edgeindices[1] = face->edgeindices[2] = -1;
}

int32 coll_face_sphere_hit( const coll_face_t *face, const coll_edge_t *edges, const sphere_t *sphere ){
  int32  i;
  float3 u;
  float2 b;
  float  mu, nu;

  if( !sphere_sphere_hit( &face->sphere, sphere ) )
    return 0;
  //f3make( u, face->sphere., c );
  //if( f3dot( u, u ) > SQUARE( radius + face->radius) )
  //  return 0;

  if( fabsf( plane_solve( &face->plane, sphere->c ) ) > sphere->radius )
    return 0;

  for( i = 0; i < 3; i++ ){
    if( sphere_touch( sphere, face->ps[i] ) )
      break;
    //f3make( u, face->ps[i], sphere->c );
    //if( f3dot( u, u ) < SQUARE(radius) )
    //  break;
  }
  if( i < 3 )
    return 1;

  for( i = 0; i < 3; i++ )
    if( sphere_lseg_hit( sphere, &edges[ face->edgeindices[i] ].lseg, 1 ) )//coll_edge_sphere_hit( &edges[ face->edges[i] ], c, radius, 1 ) )
      break;
  if( i < 3 )
    return 1;

  f3make( u, face->ps[0], sphere->c );
  b[0] = f3dot( u, face->bcs.u );
  b[1] = f3dot( u, face->bcs.v );
  mu   = f2dot( face->bcs.M_inv[0], b );
  nu   = f2dot( face->bcs.M_inv[1], b );
  if( mu >= 0.0f && nu >= 0.0f && ( mu + nu ) <= 1.0f )
    return 1;

  return 0;
}


int coll_face_aabb_hit( const coll_face_t *face, const aabb_t *aabb ){
  size_t i, j;
  float3 u, n;
  sat_t  sat;

  if( !sphere_sphere_hit( &aabb->sphere, &face->sphere ) )
    return 0;

  /*
  sat_init( &sat, face->plane.n );
  aabb_make_sat ( aabb, &sat, 0 );
  sat_calc( &sat, 1, face->ps, 1 );
  if( !sat_olap( &sat ) )
    return 0;
  */
  if( !plane_crosses( &face->plane, aabb->ps, 8 ) )
    return 0;

  const float3 aabb_axes[] = {
      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f }
  };

  for( i = 0; i < 3; i++ ){
    sat_init( &sat, aabb_axes[i] );
    sat_make( &sat, 0, aabb->sphere.c, aabb->halfsize[i] );
    sat_calc( &sat, 1, face->ps, 3 );
    if( !sat_olap( &sat ) )
      break;
  }
  if( i < 3 )
    return 0;

  for( i = 0; i < 3; i++ ){
    f3make( u, face->ps[ i ] , face->ps[ (i+1) % 3 ] );
    for( j = 0; j < 3; j++ ){
      f3cross( u, aabb_axes[j], n );
      if( f3dot( n, n ) > 0.0f ){
        sat_init( &sat, n );
        sat_calc( &sat, 0, face->ps, 3 );
        aabb_make_sat ( aabb, &sat, 1 );
        //sat_calc( &sat, 1, aabb->ps, 8 );
        if( !sat_olap( &sat ) )
          break;
      }
    }
    if( j < 3 )
      break;
  }
  if( i < 3 )
    return 0;

  return 1;
}


int32 coll_face_lseg_isect( const coll_face_t *face, const lseg_t *lseg, float *rate ){
  float3 u, p;
  float2 b;
  float  dist, mu, nu;
  //float3 p;
  //float  dist;

  if( f3dot( lseg->dir, face->plane.n ) >= 0.0f )
    return 0;
  dist = plane_line( &face->plane, lseg->ps[0], lseg->dir );
  if( dist < 0.0f || dist > lseg->length )
    return 0;

  f3copy( p, lseg->ps[0] );
  f3madd( p, dist, lseg->dir );

  /*
  int32 i;
  for( i = 0; i < 3; i++ )
    if( f3dot( p, face->sideplanes[i].n ) > face->sideplanes[i].dist )
      break;
  if( i < 3 )
    return 0;
  */

  f3make( u, face->bcs.o, p );
  b[0] = f3dot( u, face->bcs.u );
  b[1] = f3dot( u, face->bcs.v );
  mu   = f2dot( face->bcs.M_inv[0], b );
  nu   = f2dot( face->bcs.M_inv[1], b );
  if( mu < 0.0f || nu < 0.0f || ( mu + nu ) > 1.0f )
    return 0;

  if( rate )
    *rate = dist;
  return 1;
}

int32 coll_face_lseg_isect_ex( const coll_face_t *face, const lseg_t *lseg, int32 both_sides, coll_face_lseg_isect_t *isect ){
  float  dist, mu, nu, ldotn;
  float3 u, p;
  float2 b;

  ldotn = f3dot( lseg->dir, face->plane.n );
  if( (  both_sides && fabsf(ldotn) < 1e-5f ) ||
      ( !both_sides && ldotn >= 0.0f ) )
    return 0;

  dist = plane_line( &face->plane, lseg->ps[0], lseg->dir );
  if( dist < 0.0f || dist > lseg->length )
    return 0;

  f3copy( p, lseg->ps[0] );
  f3madd( p, dist, lseg->dir );

  /*
  int32 i;
  for( i = 0; i < 3; i++ )
    if( f3dot( p, face->sideplanes[i].n ) > face->sideplanes[i].dist )
      break;
  if( i < 3 )
    return 0;
  */

  f3make( u, face->bcs.o, p );
  b[0] = f3dot( u, face->bcs.u );
  b[1] = f3dot( u, face->bcs.v );
  mu  = f2dot( face->bcs.M_inv[0], b );
  nu  = f2dot( face->bcs.M_inv[1], b );
  if( mu < 0.0f || nu < 0.0f || ( mu + nu ) > 1.0f )
    return 0;

  f3copy( isect->p, p );
  isect->index  = face->index;
  isect->mu     = mu;
  isect->nu     = nu;
  isect->dist   = dist;
  isect->facing = ldotn > 0.0f ? 0 : 1;
  return 1;
}


int32 coll_face_lseg_touch( const coll_face_t *face, const lseg_t *lseg, int32 both_sides ){
  float3 p;
  float  dist;
  //float3 p, u;
  //float2 b;

  //float  dist, mu, nu;
  if( both_sides ){
    if( fabsf( f3dot( lseg->dir, face->plane.n ) ) < 0.01f )
      return 0;
  }
  else
    if( f3dot( lseg->dir, face->plane.n ) >= 0.0f )
      return 0;

  dist = plane_line( &face->plane, lseg->ps[0], lseg->dir );
  if( dist < 0.0f || dist > lseg->length )
    return 0;

  f3copy( p, lseg->ps[0] );
  f3madd( p, dist, lseg->dir );

  int32 i;
  for( i = 0; i < 3; i++ )
    if( f3dot( p, face->sideplanes[i].n ) > face->sideplanes[i].dist )
      break;
  if( i < 3 )
    return 0;

  return 1;
}


void coll_geom_term( coll_geom_t *geom ){
  array_term( &geom->verts.array );
  array_term( &geom->faces.array );
  array_term( &geom->edges.array );
  array_term( &geom->surfs.array );
  aabb_empty( &geom->aabb );
  geom->_finalized = 0;
}

void coll_geom_addsurf( coll_geom_t *geom, const char name[] ){
  if( NULL == name )
    return;

  str80 s = str80_(name);

  if( !isoftype_collsurfarray( &geom->surfs.array ) ){
    geom->surfs = collsurfarray_make( 32 );
  }

  for( size_t i = 0; i < geom->surfs.size; i++ ){
    if( 0 == strcmp( geom->surfs.elems[i].name.chars, s.chars ) )
      return;
  }

  coll_surf_t *surf = collsurfarray_new( &geom->surfs, NULL );
  surf->index = geom->surfs.size - 1;
  surf->name  = s;
}

size_t coll_geom_addedge( coll_geom_t *geom, const float3 p0, const float3 p1, size_t face_no ){
  if( geom->faces.size == 0 ){
    printf( "%s:%d - error: collision geometry's collision faces not initialized\n",
            __FUNCTION__, __LINE__ );
    return -1;
  }

  const float tol = 1e-5f;

  if( !isoftype_colledgearray( &geom->edges.array ) ){
     geom->edges = colledgearray_make( 256 );
   }

  size_t      i;
  coll_edge_t edge, *edgeptr;

  coll_edge_calc( &edge, p0, p1 );
  if( 0.0f == edge.length ){
    printf( "%s:%d - warning: invalid edge %zu",
        __FUNCTION__, __LINE__, geom->edges.size );
    return -1;
  }

  edgeptr = geom->edges.elems;
  for( i = 0; i < geom->edges.size; i++ ){
    if( f3eqt( edgeptr->ps[0], edge.ps[1], tol ) &&
        f3eqt( edgeptr->ps[1], edge.ps[0], tol ) )
      break;
    edgeptr++;
  }
  if( i < geom->edges.size )
    edgeptr->faceindices[1] = face_no;
  else{
    edgeptr = colledgearray_new( &geom->edges, &edge );
    edgeptr->_collfaces = &geom->faces;
    edgeptr->index = geom->edges.size - 1;
    edgeptr->faceindices[0] = face_no;
  }

  return edgeptr->index;
}


size_t coll_geom_addface(
    coll_geom_t *geom,
    const float3 p0, const float3 p1, const float3 p2,
    size_t surf_no, size_t key ){

  if( !isoftype_collfacearray( &geom->faces.array ) ){
     geom->faces = collfacearray_make( 256 );
   }

  coll_face_t face, *ptr;

  coll_face_calc( &face, p0, p1, p2 );
  if( 0.0f == face.bcs.det ){
    printf( "%s:%d - warning: invalid collision face (skipped %zu).\n",
        __FUNCTION__, __LINE__, geom->faces.size );
    return -1;
  }
  ptr = collfacearray_new( &geom->faces, &face );
  ptr->index = geom->faces.size-1;
  ptr->key   = key;
  ptr->surfindex = surf_no;

  ptr->edgeindices[0] = coll_geom_addedge( geom, ptr->ps[0], ptr->ps[1], ptr->index );
  ptr->edgeindices[1] = coll_geom_addedge( geom, ptr->ps[1], ptr->ps[2], ptr->index );
  ptr->edgeindices[2] = coll_geom_addedge( geom, ptr->ps[2], ptr->ps[0], ptr->index );

  return ptr->index;
}


int32 coll_geom_finalize( coll_geom_t *geom ){
  if( !geom->faces.size || !geom->edges.size ){
    printf( "%s - error: collision geometry not initialized\n", __FUNCTION__ );
    return 0;
  }
  const float tol = 1e-5f;

  coll_edge_t *edges = geom->edges.elems;
  size_t       esize = geom->edges.size;

  coll_face_t *faces = geom->faces.elems;
  size_t       fsize = geom->faces.size;

  coll_surf_t *surfs = geom->surfs.elems;
  size_t       ssize = geom->surfs.size;

  size_t *remap = malloc( ssize * sizeof(size_t) );

  for( size_t i = 0; i < ssize; i++ )
    surfs[i].index = i;
  array_sort( &geom->surfs.array, &coll_surf_compare );
  for( size_t i = 0; i < ssize; i++ ){
    remap[ surfs[i].index ] = i;
    surfs[i].index = i;
  }

  for( size_t i = 0; i < esize; i++ )
    coll_edge_calc_voronoi( &edges[i] );

  if( !isoftype_collvertarray( &geom->verts.array ) )
    geom->verts = collvertarray_make( fsize * 3 );

  for( size_t i = 0; i < fsize; i++ ){
    coll_face_t *f = &faces[i];
    f->surfindex = remap[ f->surfindex ];

    for( size_t j = 0; j < 3; j++ ){
      size_t k;
      for( k = 0; k < geom->verts.size; k++ ){
        coll_vert_t *vert = &geom->verts.elems[k];
        if( f3eqt( f->ps[j], vert->p, tol ) ){
          coll_vert_add_feat( vert, 0, f->index );
          break;
        }
      }
      if( k == geom->verts.size ){
        coll_vert_t *vert = collvertarray_new( &geom->verts, NULL );
        coll_vert_init( vert, geom->verts.size - 1, f->ps[j], &geom->edges, &geom->faces );
        coll_vert_add_feat( vert, 0, f->index );
      }
    }
  }

  for( size_t i = 0; i < esize; i++ ){
    const coll_edge_t *e = &edges[i];
    for( size_t j = 0; j < 2; j++ ){
      int32 feat = 0 == j ? +1 : -1;
      for( size_t k = 0; k < geom->verts.size; k++ ){
        coll_vert_t *vert = &geom->verts.elems[k];
        if( f3eqt( e->ps[j], vert->p, tol ) ){
          coll_vert_add_feat( vert, feat, e->index );
          break;
        }
      }
    }
  }


  size_t       vsize = geom->verts.size;
  coll_vert_t *verts = geom->verts.elems;
  float3       min, max;
  f3copy( min, verts[0].p );
  f3copy( max, verts[0].p );
  for( size_t i = 1; i < vsize; i++ )
    for( size_t j = 0; j < 3; j++ ){
      if( verts[i].p[j] < min[j] )
        min[j] = verts[i].p[j];
      if( verts[i].p[j] > max[j] )
        max[j] = verts[i].p[j];
    }

  aabb_set( &geom->aabb, min, max, 0.0f, 1 );

  free( remap );
  return geom->_finalized = 1;
}

int32 coll_geom_isloaded( const coll_geom_t *geom ){
  return geom->edges.data && geom->faces.data && geom->verts.data && geom->_finalized;
}

coll_edge_t *coll_geom_getedge( coll_geom_t *geom, size_t index ){
  if( geom && geom->edges.data && index < geom->edges.size )
    return &geom->edges.elems[index];
  return NULL;
}

coll_face_t *coll_geom_getface( coll_geom_t *geom, size_t index ){
  if( geom && geom->faces.data && index < geom->faces.size )
    return &geom->faces.elems[index];
  return NULL;
}

const coll_face_t *coll_geom_getface_ro( const coll_geom_t *geom, size_t index ){
  if( geom && geom->faces.data && index < geom->faces.size )
    return &geom->faces.elems[index];
  return NULL;
}

const coll_edge_t *coll_geom_getedge_ro( const coll_geom_t *geom, size_t index ){
  if( geom && geom->edges.data && index < geom->edges.size )
    return &geom->edges.elems[index];
  return NULL;
}

int32 coll_geom_face_sphere_hit( const coll_geom_t *geom, size_t face_no, const sphere_t *sphere, int32 validate ){
  int32  i;
  float3 u;
  float2 b;
  float  mu, nu;

  if( validate ){
    if( !geom->_finalized || face_no >= geom->faces.size ){
      printf( "%s collision geometry not loaded or face no. is o.o.b\n", __FUNCTION__ );
      return 0;
    }
  }else //in an else-if block to avoid redundant face no. check
  if( face_no >= geom->faces.size )
    return 0;


  const coll_face_t *face = &geom->faces.elems[ face_no ];

  if( !sphere_sphere_hit( &face->sphere, sphere ) )
    return 0;

  if( fabsf( plane_solve( &face->plane, sphere->c ) ) > sphere->radius )
    return 0;

  for( i = 0; i < 3; i++ ){
    if( sphere_touch( sphere, face->ps[i] ) )
      break;
  }
  if( i < 3 )
    return 1;

  const coll_edge_t *edges = geom->edges.elems;
  for( i = 0; i < 3; i++ )
    if( sphere_lseg_hit( sphere, &edges[ face->edgeindices[i] ].lseg, 1 ) )
      break;
  if( i < 3 )
    return 1;

  f3make( u, face->ps[0], sphere->c );
  b[0] = f3dot( u, face->bcs.u );
  b[1] = f3dot( u, face->bcs.v );
  mu   = f2dot( face->bcs.M_inv[0], b );
  nu   = f2dot( face->bcs.M_inv[1], b );
  if( mu >= 0.0f && nu >= 0.0f && ( mu + nu ) <= 1.0f )
    return 1;

  return 0;
}



int32 coll_geom_edge_sphere_hit(  const coll_geom_t *geom, size_t edge_no, const sphere_t *sphere, int32 ignore_corners, int32 validate ){
  if( validate ){
    if( !geom->_finalized || edge_no >= geom->edges.size ){
      printf( "%s - collision geometry not loaded or edge no. is o.o.b\n", __FUNCTION__ );
      return 0;
    }
  }else //in an else-if block to avoid redundant face no. check
  if( edge_no >= geom->edges.size )
    return 0;

  const coll_edge_t *edge = &geom->edges.elems[ edge_no ];
  return sphere_lseg_hit( sphere, &edge->lseg, ignore_corners );
}


void coll_geom_getedgefaces( const coll_geom_t *geom, size_t edge_no, const coll_face_t **face0, const coll_face_t **face1 ){
  if( !coll_geom_isloaded( geom ) ){
    printf( "%s - collision geometry not loaded\n", __FUNCTION__ );
    return;
  }

  const coll_edge_t *edge =  &geom->edges.elems[ edge_no ];
  if( !edge ){
    printf( "%s - invalid edge no.\n", __FUNCTION__ );
    return;
  }

  const coll_face_t  *faces = geom->faces.elems;
  if( face0 )
    *face0 = edge->faceindices[0] == -1 ? NULL : &faces[ edge->faceindices[0] ];
  if( face1 )
    *face1 = edge->faceindices[1] == -1 ? NULL : &faces[ edge->faceindices[1] ];
}


int32 coll_geom_load( coll_geom_t *geom, const char objfile[], int32 term ){
  if( term )
    coll_geom_term( geom );
  else
    memset( geom, 0, sizeof(coll_geom_t) );

  FILE *fp = fopen( objfile, "r" );
  if( !fp ){
    printf( "%s:error - invalid OBJ file '%s'\n", __FUNCTION__, objfile );
    return 0;
  }

  size_t numfs = 0;
  size_t numvs = 0;
  char line[ 1024 ];
  while( fgets( line, sizeof(line), fp ) ){
    if( 'f' == line[0] && ' ' == line[1] )
      numfs++;
    if( 'v' == line[0] && ' ' == line[1] )
      numvs++;
    else{
      char *tok = strtok( line, " " );
      if( 0 == strcmp( tok, "usemtl" ) )
        coll_geom_addsurf( geom, strtok( NULL, " " ) );
    }
  }

  if( 0 == geom->surfs.size )
    coll_geom_addsurf( geom, "default" );

  fseek( fp, SEEK_SET, 0 );

  typedef struct{
    float3 xyz;
  }v_t;
  v_t *vs = malloc( numvs * sizeof(v_t) );

  typedef struct{
    size_t s;
    uint32 i0, i1, i2;
  }f_t;
  f_t *fs = malloc( numfs * sizeof(f_t) );


  numvs = 0;
  numfs = 0;
  size_t sindex = -1;
  while( fgets( line, sizeof(line), fp ) ){
    if( 'v' == line[0] && ' ' == line[1] ){
      sscanf( &line[2],
              "%f %f %f",
              &vs[ numvs ].xyz[0],
              &vs[ numvs ].xyz[1],
              &vs[ numvs ].xyz[2] );
      numvs++;
      continue;
    }
    if( 'f' == line[0] && ' ' == line[1] ){
      int32 found = 0;
      char tok0[ 80 ], tok1[ 80 ], tok2[ 80 ];
      char *p = NULL;

      p = strtok( &line[2], " " );
      if( p ){
        found++;
        strcpy( tok0, p );
      }
      p = strtok( NULL, " " );
      if( p ){
        found++;
        strcpy( tok1, p );
      }
      p = strtok( NULL, " " );
      if( p ){
        found++;
        strcpy( tok2, p );
      }
      if( found != 3 )
        continue;

      p = strchr( tok0, '/' );
      if( p )
        *p = '\0';
      p = strchr( tok1, '/' );
      if( p )
        *p = '\0';
      p = strchr( tok2, '/' );
      if( p )
        *p = '\0';

      fs[ numfs ].s  = -1 == sindex ? 0 : sindex;
      fs[ numfs ].i0 = (uint32)atoi( tok0 ) - 1;
      fs[ numfs ].i1 = (uint32)atoi( tok1 ) - 1;
      fs[ numfs ].i2 = (uint32)atoi( tok2 ) - 1;
      numfs++;
      continue;
    }
    char *tok = strtok( line, " " );
    if( 0 == strcmp( tok, "usemtl" ) ){
      sindex++;
      continue;
    }
  }

  for( size_t i = 0; i < numfs; i++ ){
    float3 ps[3];
    f_t *f = &fs[i];
    f3copy( ps[0], vs[ f->i0 ].xyz );
    f3copy( ps[1], vs[ f->i1 ].xyz );
    f3copy( ps[2], vs[ f->i2 ].xyz );
    coll_geom_addface( geom, ps[0], ps[1], ps[2], f->s, i );
  }

  free( vs );
  free( fs );
  return coll_geom_finalize( geom );
}

int32 coll_geom_load_bpcd_grid( const coll_geom_t *geom, bpcd_grid_t *grid, float cellsize ){
  if( !geom->_finalized ){
    printf( "%s:error - invalid geometry\n", __FUNCTION__ );
    return 0;
  }
  bpcd_grid_init( grid, geom->aabb.min, geom->aabb.max, cellsize, 0 );

  array_const_iter_t kit = array_const_iter_init( &geom->faces.array );
  while( array_iterate_const(&kit) ){
    const coll_face_t *face = kit.data;
    if( face->index != kit.i ){
      printf( "%s:warning - invalid face no. %zu (id=%zu)\n", __FUNCTION__, kit.i, face->index );
      continue;
    }
    float3 p, h;
    f3copy( p, face->ps[0] );
    f3add ( p, face->ps[1] );
    f3add ( p, face->ps[2] );
    f3muls( p, 1.0f/3.0f );

    for( size_t i = 0; i < 3; i++ ){
      float min, mid, max;
      min = face->ps[0][i];
      mid = face->ps[1][i];
      max = face->ps[2][i];
      SORT3( min, mid, max );
      h[i] = ( max - min ) / 2.0f;
    }

    bpcd_grid_sector_t sector = bpcd_grid_sector_make( grid, p, h );
    for( size_t l = sector.ls[0]; l <= sector.ls[1]; l++ ){
      for( size_t r = sector.rs[0]; r <= sector.rs[1]; r++ ){
        for( size_t c = sector.cs[0]; c <= sector.cs[1]; c++ ){
          aabb_t aabb;
          bpcd_grid_aabb_for_cell( grid, l, r, c, &aabb );
          if( coll_face_aabb_hit( face, &aabb )){
            bpcd_grid_cell_t *cell = bpcd_grid_add_cell( grid, l, r, c );
            bpcd_grid_cell_add( cell, face->index );
          }
        }
      }
    }

  }
  return 1;
}

