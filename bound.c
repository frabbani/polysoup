#include "bound.h"
#include "floatn.h"

#include "libs.h"

INLINE void sphere_empty( sphere_t *sphere ){
  f3zero( sphere->c );
  sphere->radius = -1.0f;
}

void sphere_calc( sphere_t *sphere, const float3 ps[], size_t nps ){
  size_t i;
  float3 v;

  f3copy( sphere->c, ps[0] );
  for( i = 1; i < nps; i++ )
    f3add( sphere->c, ps[i] );
  f3muls( sphere->c, 1.0f/(float)nps );

  f3make( v, ps[0], sphere->c );
  sphere->radius = f3dot( v, v );
  for( i = 1; i < nps; i++ ){
    f3make( v, ps[i], sphere->c );
    float s = f3dot( v, v );
    if( s > sphere->radius )
      sphere->radius = s;
  }
  sphere->radius = sqrtf( sphere->radius );
}

void sphere_set( sphere_t *sphere, const float3 c, float radius ){
  f3copy( sphere->c, c );
  sphere->radius = fabsf(radius);
}

int32 sphere_touch( const sphere_t *sphere, const float3 p ){
  float3 v;
  int32  hit = 0;
  f3make( v, sphere->c, p );
  if( f3dot( v, v ) <= SQUARE(sphere->radius) )
    hit = 1;
  return hit;
}

int32 sphere_touch2( const sphere_t *sphere, const float3 p, float3 n ){
  int32  hit = 0;
  float3 v;

  f3zero( n );
  f3make( v, sphere->c, p );
  if( f3dot( v, v ) <= SQUARE(sphere->radius) ){
    f3copy( n, v );
    f3norm( n );
    hit++;
  }
  return hit;
}

int32 sphere_sphere_hit( const sphere_t *sphere0, const sphere_t *sphere1 ){
  float3 v;
  float  dist = sphere0->radius + sphere1->radius;
  int32  hit  = 0;
  f3make( v, sphere0->c, sphere1->c );
  if( f3dot( v, v ) <= SQUARE(dist) )
    hit = 1;
  return hit;
}

int32 sphere_lseg_hit( const sphere_t *sphere, const lseg_t *lseg, int32 ignore_corners ){
  float3 v;
  float  dist;
  int32  i;

  if( !ignore_corners ){
    for( i = 0; i < 2; i++ )
      if( sphere_touch( sphere, lseg->ps[i] ) )
        break;
    if( i < 2 )
      return 1;
  }

  f3make( v, lseg->ps[0], sphere->c );
  dist = f3dot( v, lseg->dir );
  if( dist >= 0.0f && dist <= lseg->length ){
    f3madd( v, -dist, lseg->dir );
    if( f3dot( v, v ) < SQUARE( sphere->radius ) )
      return 1;
  }

  return 0;
}

int32 sphere_lseg_hit2( const sphere_t *sphere, const lseg_t *lseg, float3 norm, float3 pos ){
  float3 v;
  float  dist;

  f3make( v, lseg->ps[0], sphere->c );
  dist = f3dot( v, lseg->dir );
  if( dist >= 0.0f && dist <= lseg->length ){
    f3madd( v, -dist, lseg->dir );
    if( f3dot( v, v ) < SQUARE( sphere->radius ) ){
      f3copy( norm, v );
      f3muls( norm, -1.0f );
      f3norm( norm );
      f3copy( pos, sphere->c );
      f3madd( pos, sphere->radius, norm );
      return 1;
    }
  }

  return 0;
}

int32 sphere_lseg_isect( const sphere_t *sphere, const lseg_t *lseg, float2 alpha ){
  /*
    (p.x + alpha*v.x)^2 + (p.y + alpha*v.y)^2 + (p.z + alpha*v.z)^2 = r^2

    p.x^2 + 2*p.x*alpha*v.x + alpha^2*v.x^2 +
    p.y^2 + 2*p.y*alpha*v.y + alpha^2*v.y^2 +
    p.z^2 + 2*p.z*alpha*v.z + alpha^2*v.z^2 = r^2

    alpha^2 * ( v.x^2 + v.y^2 + v.z^2 )            +
      alpha * (2*p.x*v.x + 2*p.y*v.y + 2*p.z*v.z ) +
              p.x^2 + p.y^2 + p.z^2                = r^2

    A = v.x^2 + v.y^2 + v.z^2
    B = 2 * ( p.x*v.x + p.y*v.y + p.z*v.z )
    C = p.x^2 + p.y^2 + p.z^2 - r^2

    A*alpha^2 + B*alpha + C = 0
    alpha one = (-B + sqrt( B^2 - 4AC) )/2A
    alpha two = (-B - sqrt( B^2 - 4AC) )/2A
 */

  int32 touch0, touch1;

  touch0 = sphere_touch( sphere, lseg->ps[0] );
  if( lseg->length == 0.0f ){
    if( touch0 ){
      alpha[0] = alpha[1] = 0.0f;
      return 1;
    }
    else
      return 0;
  }
  touch1 = sphere_touch( sphere, lseg->ps[1] );

  if( touch0 && touch1 ){
    alpha[0] = 0.0f;
    alpha[1] = 1.0f;
    return 1;
  }

  float3 p, v;
  f3make( p,   sphere->c, lseg->ps[0] );
  f3make( v, lseg->ps[0], lseg->ps[1] );

  float A = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];  // > 0 because line length > 0
  float B = 2.0f*( p[0]*v[0] + p[1]*v[1] + p[2]*v[2] );
  float C = p[0]*p[0] + p[1]*p[1] + p[2]*p[2] - sphere->radius*sphere->radius;
  float inv_2A =  0.5f / A;

  if( touch0 ){
    alpha[0] = 0.0f;

    // discriminant is >= 0 because there are 2 real roots, i.e
    // 2 intersection exists (as the 1st point is inside the circle
    // & the 2nd outside). 1st intersection occurs in front of the
    // 1st point and behind the 2nd point. 2nd intersection
    // occurs behind the 1st point (and can be discarded).
    //
    // since discrimnant is non-negative we don't need to check for
    // -value in sqrtf() call;
    float D  = sqrtf( B*B - 4.0f*A*C );
    float a0 = ( -B - D ) * inv_2A;
    float a1 = ( -B + D ) * inv_2A;
    alpha[1] = a0 >= 0.0f ? a0 : a1;
    return 1;
  }
  else
  if( touch1 ){
    alpha[1] = 1.0f;

    // discriminant is >= 0 because there are 2 real roots, i.e
    // 2 intersection exists (as the 1st point is outside the circle
    // & the 2nd inside). 1st intersection occurs in front of the
    // 1st point and behind the 2nd point. 2nd intersection
    // occurs in front of the 2nd point (and can be discarded).
    //
    // since discrimnant is non-negative we don't need to check for
    // -value in sqrtf() call;
    float D      = sqrtf( B*B - 4.0f*A*C );
    float a0 = ( -B - D ) * inv_2A;
    float a1 = ( -B + D ) * inv_2A;
    alpha[0] = a0 <= a1 ? a0 : a1;
    return 1;
  }

  //if discriminant < 0, no real roots, i.e. no intersection.
  float Dsq = B*B - 4.0f*A*C;
  if( Dsq < 0.0f )
    return 0;
  float D  = sqrtf( Dsq );
  float a0 = ( -B - D ) * inv_2A;
  float a1 = ( -B + D ) * inv_2A;
  //sort back to front
  if( a1 < a0 )
    SWAP( a0, a1 );


  //if line intersects, but the intersections are behind 1st point
  //or infront of 2nd point, line segment intersection d.n.e;
  if( a0 > 1.0f || a1 < 0.0f )
    return 0;

  //since we've already tested for the two points being inside of
  //the circle, we don't have to worry about intersection being
  //in front of 2nd point or behind 1st point. Note that if discriminant
  //is 0, 1 intersection exists and alpha[0] equals alpha[1];
  alpha[0] = a0;
  alpha[1] = a1;
  return 1;
}


void aabb_empty( aabb_t *aabb ){
  const float inf = 1.0f / 0.0f;
  f3zero( aabb->min );
  f3zero( aabb->max );
  f3zero( aabb->size     );
  f3zero( aabb->halfsize );
  for( int32 i = 0; i < 8; i++ )
    f3set( aabb->ps[i], inf, inf, inf );
  sphere_empty( &aabb->sphere );
}

void aabb_set( aabb_t *aabb, const float3 min, const float3 max, float pad, int calc_ps ){
  f3zero( aabb->sphere.c );
  for( int32 i = 0; i < 3; i++ ){
    aabb->min[i] = min[i];
    aabb->max[i] = max[i];
    if( aabb->max[i] < aabb->min[i] )
      SWAP( aabb->min[i], aabb->max[i] );

    aabb->min[i] -= fabsf(pad);
    aabb->max[i] += fabsf(pad);

    aabb->sphere.c[i] = 0.5f * ( aabb->min[i] +  aabb->max[i] );
    aabb->size[i]     = aabb->max[i] - aabb->min[i];
    aabb->halfsize[i] = 0.5f * aabb->size[i];
  }
  aabb->sphere.radius = f3mag( aabb->halfsize );

  if( calc_ps ){
    for( int32 i = 0; i < 8; i++ ){
      f3copy( aabb->ps[i], aabb->sphere.c );
      aabb->ps[i][0] += ( i & 0b100 ? +aabb->halfsize[0] : -aabb->halfsize[0] );
      aabb->ps[i][1] += ( i & 0b010 ? +aabb->halfsize[1] : -aabb->halfsize[1] );
      aabb->ps[i][2] += ( i & 0b001 ? +aabb->halfsize[2] : -aabb->halfsize[2] );
    }
  }
  //_aabb_print( aabb );
}

void aabb_fit( aabb_t *aabb, const float3 *ps, size_t nps, float pad, int calc_ps ){
  size_t i, j;

  f3zero( aabb->sphere.c );
  for( i = 0; i < 3; i++ )
    aabb->min[i] = aabb->max[i] = ps[0][i];

  for( i = 1; i < nps; i++ )
    for( j = 0; j < 3; j++ ){
      if( ps[i][j] < aabb->min[j] )
        aabb->min[j] = ps[i][j];
      if( ps[i][j] > aabb->max[j] )
        aabb->max[j] = ps[i][j];
    }


  for( i = 0; i < 3; i++ ){
    aabb->min[i] -= fabsf(pad);
    aabb->max[i] += fabsf(pad);

    aabb->sphere.c[i] = 0.5f * ( aabb->min[i] +  aabb->max[i] );
    aabb->size[i]     = aabb->max[i] - aabb->min[i];
    aabb->halfsize[i] = 0.5f * aabb->size[i];
  }
  aabb->sphere.radius = f3mag( aabb->halfsize );

  if( calc_ps ){
    for( i = 0; i < 8; i++ ){
      f3copy( aabb->ps[i], aabb->sphere.c );
      aabb->ps[i][0] += ( i & 0b100 ? +aabb->halfsize[0] : -aabb->halfsize[0] );
      aabb->ps[i][1] += ( i & 0b010 ? +aabb->halfsize[1] : -aabb->halfsize[1] );
      aabb->ps[i][2] += ( i & 0b001 ? +aabb->halfsize[2] : -aabb->halfsize[2] );
    }
  }
  //_aabb_print( aabb );

}

void aabb_make_sat( const aabb_t *aabb, sat_t *sat, int32 second ){
  float hdist = 0.0f;
  for( int32 i = 0; i < 3; i++ )
    hdist += ( aabb->halfsize[i] * fabsf( sat->v[i] ) );
  sat_make( sat, second, aabb->sphere.c, hdist );
}

INLINE int32 aabb_touch( const aabb_t *aabb, const float3 p ){
  int32 i;
  for( i = 0; i < 3; i++ )
    if( p[i] < aabb->min[i] || p[i] > aabb->max[i] )
      break;
  return i < 3 ? 0 : 1;
}

/*
int32 aabb_calcexit( const aabb_t *aabb, const float3 p, const float3 d, float3 e,  int32 exits[3]  ){
  const float inf = 1.0f/0.0f;
  float3 v;
  float  min = inf;

  f3set( v, inf, inf, inf );
  for( int32 i = 0; i < 3; i++ ){
    if( d[i] > 0.0f )
      v[i] = ( aabb->max[i] - p[i] ) / d[i];
    if( d[i] < 0.0f )
      v[i] = ( aabb->min[i] - p[i] ) / d[i];
    else
      continue;
    min = v[i] < min ? v[i] : min;
  }
  f3copy( e, p );
  exits[0] = exits[1] = exits[2] = 0;
  if( inf == min )
    return 0;
  for( int32 i = 0; i < 3; i++ )
    if( min == v[i] )
      exits[i] = d[i] > 0.0f ? +1 : -1;

  f3madd( e, min, d );
  return 1;
}
*/

int aabb_lseg_hit( const aabb_t *aabb, const lseg_t *lseg ){
  size_t i;
  sat_t  sat;

  const float3 axes[3] = {
      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f },
  };

  for( i = 0; i < 3; i++ ){
    sat_init( &sat, axes[i] );
    sat_make( &sat, 0, aabb->sphere.c, aabb->halfsize[i] );
    sat_calc( &sat, 1, lseg->ps, 2 );
    if( !sat_olap( &sat ) )
      break;
  }
  if( i < 3 )
    return 0;

  for( i = 0; i < 3; i++ ){
    float3 n;
    f3cross( lseg->dir, axes[i], n );
    if( f3dot( n, n ) > 0.0f ){
      sat_init( &sat, n );
      sat_calc( &sat, 0, lseg->ps, 2 );
      aabb_make_sat( aabb, &sat, 1 );
      //sat_calc( &sat, 1, aabb->ps, 8 );
      if( !sat_olap( &sat ) )
        break;
    }
  }
  if( i < 3 )
    return 0;

  return 1;
}

void aabb_lseg_clip( const aabb_t *aabb, lseg_t *lseg, int assume_touching ){
  // const  float inf = fabsf( 1.0f/0.0f );
  size_t i;
  float  dist;

  if( !assume_touching && !aabb_lseg_hit( aabb, lseg )  )
    return;

  dist = 0.0f;
  for( i = 0; i < 3; i++ ){
    if( 0.0f == lseg->dir[i] )
      continue;
    if( lseg->dir[i] > 0.0f && lseg->ps[0][i] < aabb->min[i] ){
      float testdist = ( aabb->min[i] - lseg->ps[0][i] ) / lseg->dir[i];
      if( testdist > dist && testdist < lseg->length )
        dist = testdist;
    }
    if( lseg->dir[i] < 0.0f && lseg->ps[0][i] > aabb->max[i] ){
      float testdist = ( aabb->max[i] - lseg->ps[0][i] ) / lseg->dir[i];
      if( testdist > dist && testdist < lseg->length )
        dist = testdist;
    }
  }
  if( dist > 0.0f )
    lseg_slide( lseg, dist, 0 );

  dist = 0.0f;
  for( i = 0; i < 3; i++ ){
    if( 0.0f == lseg->dir[i] )
      continue;
    if( lseg->dir[i] > 0.0f && lseg->ps[1][i] > aabb->max[i] ){
      float testdist = ( aabb->max[i] - lseg->ps[1][i] ) / lseg->dir[i];
      if( testdist < dist && testdist > -lseg->length )
        dist = testdist;
    }
    if( lseg->dir[i] < 0.0f && lseg->ps[1][i] < aabb->min[i] ){
      float testdist = ( aabb->min[i] - lseg->ps[1][i] ) / lseg->dir[i];
      if( testdist < dist && testdist > -lseg->length )
        dist = testdist;
    }
  }
  if( dist < 0.0f )
    lseg_slide( lseg, dist, 1 );

}


void obb_make_sat( const obb_t *obb, sat_t *sat, int32 second ){
  float hdist = 0.0f;
  for( int32 i = 0; i < 3; i++ )
    hdist += ( obb->halfsize[i] * fabsf( f3dot( obb->axes[i], sat->v ) ) );
  sat_make( sat, second, obb->sphere.c, hdist );
}

int32 obbs_touch( const obb_t *obb0, const obb_t *obb1 ){
  for( int32 i = 0; i < 3; i++ ){
    sat_t sat;
    sat_init( &sat, obb0->axes[i] );
    obb_make_sat( obb0, &sat, 0 );
    obb_make_sat( obb1, &sat, 1 );
    if( !sat_olap( &sat ) )
      return 0;
  }

  for( int32 i = 0; i < 3; i++ ){
    sat_t sat;
    sat_init( &sat, obb1->axes[i] );
    obb_make_sat( obb0, &sat, 0 );
    obb_make_sat( obb1, &sat, 1 );
    if( !sat_olap( &sat ) )
      return 0;
  }

  for( int32 i = 0; i < 3; i++ )
    for( int32 j = 0; j < 3; j++ ){
      sat_t  sat;
      float3 v;
      f3cross( obb0->axes[i], obb1->axes[j], v );
      if( f3dot( v, v ) > 0.0f ){
        sat_init( &sat, v );
        obb_make_sat( obb0, &sat, 0 );
        obb_make_sat( obb1, &sat, 1 );
        if( !sat_olap( &sat ) )
          return 0;
      }
    }

  return 1;
}

int32 obb_tri_hit( const obb_t *obb, const plane_t *plane, const float3 ps[] ){
  sat_t sat;

  sat_init_from_plane( &sat, plane, 0 );
  obb_make_sat       ( obb,  &sat,  1 );
  if( !sat_olap( &sat ) )
    return 0;

  for( int32 i = 0; i < 3; i++ ){
    sat_init    ( &sat, obb->axes[i] );
    obb_make_sat( obb, &sat, 0 );
    sat_calc    ( &sat, 1, ps, 3 );
    if( !sat_olap( &sat ) )
      return 0;
  }

  //not part of the minkowski sum
  /*
  for( int32 i = 0; i < 3; i++ ){
    float3 u, v;
    f3make ( u, ps[i], ps[ (i+1)%3 ] );
    f3cross( u, plane->n, v );
    if( f3dot( v, v ) > 0.0f ){ //check not degenerate
      sat_init    ( &sat, v );
      obb_make_sat( obb, &sat, 0 );
      sat_calc    ( &sat, 1, ps, 3 );
      if( !sat_olap( &sat ) )
        return 0;
    }
  }
  */

  for( int32 i = 0; i < 3; i++ )
    for( int32 j = 0; j < 3; j++ ){
      float3 u, v;
      f3make ( u, ps[i], ps[ (i+1)%3 ] );
      f3cross( u, obb->axes[j], v );
      if( f3dot( v, v ) > 0.0f ){ //check not degenerate
        sat_init    ( &sat, v );
        obb_make_sat( obb, &sat, 0 );
        sat_calc    ( &sat, 1, ps, 3 );
        if( !sat_olap( &sat ) )
          return 0;
      }
    }

  return 1;
}


void capsule_calc( capsule_t *capsule, const float3 p0, const float3 p1, float radius ){
  lseg_calc( &capsule->lseg, p0, p1 );
  capsule->radius = fabsf( radius );
}

int32 capsule_sphere_hit( const capsule_t *capsule, const sphere_t *sphere ){
  float3 v;
  float  proj,
         dist;

  dist = capsule->radius + sphere->radius;
  f3make( v, capsule->ps[0], sphere->c );
  proj = f3dot( capsule->dir, v );

  if( proj <= 0.0f ){
  }  //f3make( v, capsule->ps[0], c );
  else if( proj >= capsule->length )
    f3make( v, capsule->ps[1], sphere->c );
  else
    f3madd( v, -proj, capsule->dir );

  if( f3dot( v, v ) > SQUARE( dist ) )
    return 0;

  return 1;
}

int32 capsule_touch( const capsule_t *capsule, const float3 p ){
  float3 v;
  float  proj;

  f3make( v, capsule->ps[0], p );
  proj = f3dot( capsule->dir, v );

  if( proj <= 0.0f ){
  }  //f3make( v, capsule->ps[0], c );
  else if( proj >= capsule->length )
    f3make( v, capsule->ps[1], p );
  else
    f3madd( v, -proj, capsule->dir );

  if( f3dot( v, v ) > SQUARE( capsule->radius ) )
    return 0;

  return 1;
}


void spheroid_set( spheroid_t *spheroid, const float3 c, float a, float b ){
  f3copy( spheroid->c, c );
  spheroid->radii[0] = fabsf( a );
  spheroid->radii[1] = fabsf( b );
  spheroid->radii_invsq[0] = 1.0f/SQUARE( spheroid->radii[0] );
  spheroid->radii_invsq[1] = 1.0f/SQUARE( spheroid->radii[1] );
  spheroid->oblate = spheroid->radii[1] < spheroid->radii[0];
}

int32 spheroid_touch( const spheroid_t *spheroid, const float3 p ){
  float3 v;
  f3make( v, spheroid->c, p );
  v[0] /= spheroid->radii[0];
  v[1] /= spheroid->radii[0];
  v[2] /= spheroid->radii[1];

  return f3dot( v, v ) <= 1.0f ? 1 : 0;
}

int32 spheroid_touch2( const spheroid_t *spheroid, const float3 p, float3 n ){
  int32  hit = 0;
  float3 v, d;
  f3zero( n );
  f3make( v, spheroid->c, p );
  f3copy( d, v );
  v[0] /= spheroid->radii[0];
  v[1] /= spheroid->radii[0];
  v[2] /= spheroid->radii[1];
  if( f3dot( v, v ) <= 1.0f ){  //radius is 1
    n[0] = d[0] * spheroid->radii_invsq[0];
    n[1] = d[1] * spheroid->radii_invsq[0];
    n[2] = d[2] * spheroid->radii_invsq[1];
    f3norm( n );
    hit++;
  }
  return hit;
}

void spheroid_calcpoint( const spheroid_t *spheroid, const float3 v, float3 p ){
  /*
        (alpha*x/a)^2 + (alpha*y/b)^2 = 1
    alpha^2*(x/a)^2 + alpha^2*(y/b)^2 = 1
                              alpha^2 = 1.0f/( (x/a)^2 + (y/b)^2 )
  */
  float2 sq;
  sq[0] = v[0]*v[0] + v[1]*v[1];
  sq[1] = SQUARE( v[2] );
  float alpha = sqrtf( 1.0f / f2dot( sq, spheroid->radii_invsq )  );
  f3copy( p, spheroid->c );
  f3madd( p, alpha, v );
}

void spheroid_calcnormal( const spheroid_t *spheroid, const float3 v, float3 n, int32 centered ){
  /*
      gradient = d(x^2/a^2)/dx, d(y^2/b^2)/dy
                 2x/a^2, 2y/b^2
        normal = normalize( 2x/a^2, 2y/b^2 )
   */

  float3 u;

  f3zero( n );
  if( !centered )
    f3make( u, spheroid->c, v );
  else
    f3copy( u, v );
  n[0] = u[0] * spheroid->radii_invsq[0];
  n[1] = u[1] * spheroid->radii_invsq[0];
  n[2] = u[2] * spheroid->radii_invsq[1];
  float len = f3mag( n );
  if( len > 0.0f )
    f3muls( n, 1.0f/len );
}

void spheroid_make_bounding_sphere( const spheroid_t *spheroid, sphere_t *sphere ){
  f3copy( sphere->c, spheroid->c );
  sphere->radius = spheroid->radii[0] > spheroid->radii[1] ?
                   spheroid->radii[0] : spheroid->radii[1];
}

int32 spheroid_lseg_hit( const spheroid_t *spheroid, const lseg_t *lseg, float3 norm, float3 pos ){
  float3   ps[2], v;
  lseg_t   lseg0;
  sphere_t sphere;
  float    dist;

  f3zero( norm );

  for( int32 i = 0; i < 2; i++ ){
    f3make( ps[i], spheroid->c, lseg->ps[i] );
    ps[i][0] /= spheroid->radii[0];
    ps[i][1] /= spheroid->radii[0];
    ps[i][2] /= spheroid->radii[1];
  }
  lseg_calc( &lseg0, ps[0], ps[1] );

  f3zero( sphere.c );
  sphere.radius=1.0f;

  //lifted (somewhat) from sphere_lseg_hit()

  //calculate (potential) normal
  f3make( v, lseg0.ps[0], sphere.c ); //this is really an invert
  dist = f3dot( v, lseg0.dir );
  f3madd( v,-dist, lseg0.dir );

  float rad_sq = sphere.radius * sphere.radius;

  int32 hit = 0;
  if( dist >= 0.0f && dist <= lseg0.length )
    if( f3dot( v, v ) < rad_sq )
      hit++;

  if( !hit ){
    for( int32 i = 0; i < 2; i++ )
      //corner check - don't need f3make() because c is { 0, 0, 0 }
      if( f3dot( lseg0.ps[i], lseg0.ps[i] ) <= rad_sq ){
        hit++;
        break;
      }
  }

  if( hit ){
    //flip direction (point away from sphere center)
    v[0] *= -spheroid->radii[0];
    v[1] *= -spheroid->radii[0];
    v[2] *= -spheroid->radii[1];
    spheroid_calcnormal( spheroid, v, norm, 1 );
    f3cadd( pos, v, spheroid->c );
  }

  return hit;
}

int32 spheroid_lseg_isect( const spheroid_t *spheroid, const lseg_t *lseg, float2 alpha ){
  float3   ps[2];
  lseg_t   lseg0;
  sphere_t sphere;

  for( int32 i = 0; i < 2; i++ ){
    f3make( ps[i], spheroid->c, lseg->ps[i] );
    ps[i][0] /= spheroid->radii[0];
    ps[i][1] /= spheroid->radii[0];
    ps[i][2] /= spheroid->radii[1];
  }
  lseg_calc( &lseg0, ps[0], ps[1] );

  f3zero( sphere.c );
  sphere.radius=1.0f;

  return sphere_lseg_isect( &sphere, &lseg0, alpha  );
}

int32 spheroid_testinside_tri( const spheroid_t *spheroid, const float3 tri_ps[3], spheroid_insidetest_t *test ){
  memcpy( &test->params.spheroid, spheroid, sizeof(spheroid_t) );
  f3copy( test->params.ps[0], tri_ps[0] );
  f3copy( test->params.ps[1], tri_ps[1] );
  f3copy( test->params.ps[2], tri_ps[2] );

  f3zero( test->sphere.c );
  test->sphere.radius=1.0f;
  for( int32 i = 0; i < 3; i++ ){
    f3make( test->ps[i], test->params.spheroid.c, test->params.ps[i] );
    test->ps[i][0] /= test->params.spheroid.radii[0];
    test->ps[i][1] /= test->params.spheroid.radii[0];
    test->ps[i][2] /= test->params.spheroid.radii[1];
  }

  plane_calc( &test->plane, test->ps[0], test->ps[1], test->ps[2] );
  if( fabsf( test->plane.dist ) > test->sphere.radius )
    return test->inside = 0;

  if( bcs3_calc( &test->bcs, test->ps[0], test->ps[1], test->ps[2] ) ){
    test->inside = bcs3_find( &test->bcs, test->sphere.c, &test->st[0], &test->st[1] );
    if( test->inside ){
      f3copy( test->hitp, test->bcs.o );
      f3madd( test->hitp, test->st[0], test->bcs.u );
      f3madd( test->hitp, test->st[1], test->bcs.v );
      test->hitp[0] *= test->params.spheroid.radii[0];
      test->hitp[1] *= test->params.spheroid.radii[0];
      test->hitp[2] *= test->params.spheroid.radii[1];
      f3add( test->hitp, test->params.spheroid.c );
    }
    return test->inside;
  }
  return 0;
}




