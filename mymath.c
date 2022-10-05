#include "mymath.h"

#include "libs.h"

// these are used to calculate coordinates given the 2 axes
int32 bcs2_calc( bcs2_t *bcs, const float2 p0, const float2 p1, const float2 p2 ){
  f2copy( bcs->o, p0 );
  f2make( bcs->u, p0, p1 );
  f2make( bcs->v, p0, p2 );

  bcs->M[0][0] = f2dot( bcs->u, bcs->u );
  bcs->M[0][1] = bcs->M[1][0] = f2dot( bcs->u, bcs->v );
  bcs->M[1][1] = f2dot( bcs->v, bcs->v );

  bcs->det = ( bcs->M[0][0] * bcs->M[1][1] - bcs->M[1][0] * bcs->M[0][1] );
  if( fabsf( bcs->det ) == 0.0f ){
    bcs->det = 0.0f;
    printf( "%s - error calculating coordinates.\n", __FUNCTION__ );
    return 0;
  }
  float det_inv = 1.0f / bcs->det;
  bcs->M_inv[0][0] = bcs->M[1][1] *  det_inv;
  bcs->M_inv[1][1] = bcs->M[0][0] *  det_inv;
  bcs->M_inv[0][1] = bcs->M[0][1] * -det_inv;
  bcs->M_inv[1][0] = bcs->M[1][0] * -det_inv;

  // u[0] * xaxis + u[1] * yaxis = (p1 - p0)
  // v[0] * xaxis + v[1] * yaxis = (p2 - p0)
  // | u[0], u[1] | * xaxis = | d1 |
  // | v[0], v[1] |   yaxis = | d2 |
  // x-axis:
  // | x | =  | u[0], u[1] | ^ -1 * | d1.x |
  // | y | =  | v[0], v[1] |        | d1.y |
  // y-axis:
  // | x | =  | u[0], u[1] | ^ -1 * | d2.x |
  // | y | =  | v[0], v[1] |        | d2.y |

  f2copy( bcs->A[0], bcs->u );
  f2copy( bcs->A[1], bcs->v );
  //bcs->A[0][0] = bcs->u[0];
  //bcs->A[0][1] = bcs->u[1];
  //bcs->A[1][0] = bcs->v[0];
  //bcs->A[1][1] = bcs->v[1];
  det_inv = 1.0f / ( bcs->A[0][0] * bcs->A[1][1] - bcs->A[1][0] * bcs->A[0][1] );
  bcs->A_inv[0][0] = bcs->A[1][1] *  det_inv;
  bcs->A_inv[1][1] = bcs->A[0][0] *  det_inv;
  bcs->A_inv[0][1] = bcs->A[0][1] * -det_inv;
  bcs->A_inv[1][0] = bcs->A[1][0] * -det_inv;

  return 1;
}


void bcs2_axes( bcs2_t *bcs, const float3 p0, const float3 p1, const float3 p2, float3 axes[3], int ortho ){
  f3zero( axes[0] );
  f3zero( axes[1] );
  f3zero( axes[2] );
  if( fabsf(bcs->det) == 0.0f )
    return;

  float3 u, v;
  f3make( u, p0, p1 );
  f3make( v, p0, p2 );
  f3cross( u, v, axes[2] );

  f3madd( axes[0], bcs->A_inv[0][0], u );
  f3madd( axes[0], bcs->A_inv[0][1], v );

  if( ortho ){
    f3cross( axes[2], axes[0], axes[1] );
    f3norm ( axes[0] );
    f3norm ( axes[1] );
    f3norm ( axes[2] );
  }
  else{
    f3madd( axes[1], bcs->A_inv[1][0], u );
    f3madd( axes[1], bcs->A_inv[1][1], v );
  }

}


int32 bcs2_find( const bcs2_t *bcs, const float2 p, float *uval, float *vval ){
  float2 r;
  float2 b;
  float2 c;

  f2make( r, bcs->o, p );

  //Au + Bv = r
  //A(u.u) + B(v.u) = r . u
  //A(u.v) + B(v.v) = r . v
  //   | A |  =  |  u.u,  v.u  | ^-1  * | r . u |
  //   | B |     |  u.v,  v.v  |        | r . v |
  //0 <= A <= 1,  0 <= B <= 1, 0 <= A+B <= 1

  b[0] = f2dot( r, bcs->u );
  b[1] = f2dot( r, bcs->v );

  c[0] = f2dot( bcs->M_inv[0], b );
  c[1] = f2dot( bcs->M_inv[1], b );

  if( uval )
    *uval = c[0];
  if( vval )
    *vval = c[1];

  if( c[0] < 0.0f || c[1] < 0.0f || ( c[0] + c[1] ) > 1.0f )
    return 0;

  return 1;
}


float bcs2_area( const bcs2_t *bcs ){
  float3 u, v, n;
  f3set( u, bcs->u[0], bcs->u[1], 0.0f );
  f3set( v, bcs->v[0], bcs->v[1], 0.0f );
  f3cross( u, v, n );
  return f3mag( n ) * 0.5f;
}


int32 bcs3_calc( bcs3_t *bcs, const float3 p0, const float3 p1, const float3 p2 ){
  f3copy( bcs->o, p0 );
  f3make( bcs->u, bcs->o, p1 );
  f3make( bcs->v, bcs->o, p2 );

  bcs->M[0][0] = f3dot( bcs->u, bcs->u );
  bcs->M[0][1] = bcs->M[1][0] = f3dot( bcs->u, bcs->v );
  bcs->M[1][1] = f3dot( bcs->v, bcs->v );

  bcs->det = ( bcs->M[0][0] * bcs->M[1][1] - bcs->M[1][0] * bcs->M[0][1] );
  if( fabsf( bcs->det ) == 0.0f ){
    printf( "%s - invalid determinant!\n", __FUNCTION__ );
    bcs->det = 0.0f;
    return 0;
  }
  float det_inv = 1.0f / bcs->det;
  bcs->M_inv[0][0] = bcs->M[1][1] *  det_inv;
  bcs->M_inv[1][1] = bcs->M[0][0] *  det_inv;
  bcs->M_inv[0][1] = bcs->M[0][1] * -det_inv;
  bcs->M_inv[1][0] = bcs->M[1][0] * -det_inv;

  return 1;
}

int32 bcs3_find( const bcs3_t *bcs, const float3 p, float *uval, float *vval ){
  float  usize, vsize;
  float3 r;
  float2 b;
  f3make( r, bcs->o, p );

  //Au + Bv = r
  //A(u.u) + B(v.u) = r . u
  //A(u.v) + B(v.v) = r . v
  //   | A |  =  |  u.u,  v.u  | ^-1  * | r . u |
  //   | B |     |  u.v,  v.v  |        | r . v |
  //0 <= A <= 1,  0 <= B <= 1, 0 <= A+B <= 1

  b[0] = f3dot( r, bcs->u );
  b[1] = f3dot( r, bcs->v );

  usize = f2dot( bcs->M_inv[0], b );
  vsize = f2dot( bcs->M_inv[1], b );

  if( uval )
    *uval = usize;
  if( vval )
    *vval = vsize;

  if( usize < 0.0f || vsize < 0.0f || (usize + vsize) > 1.0f )
    return 0;

  return 1;
}

float bcs3_area( const bcs3_t *bcs ){
  float3 n;
  f3cross( bcs->u, bcs->v, n );
  return f3mag( n ) * 0.5f;
}

INLINE void plane_make( plane_t *plane, const float3 n, const float3 p ){
  if( f3dot( n, n ) > 0.0f ){
    f3copy( plane->n, n );
    f3norm( plane->n );
    plane->dist = f3dot( plane->n, p );
  }
}

INLINE void plane_calc( plane_t *plane, const float3 p0, const float3 p1, const float3 p2 ){
  float3 u, v;
  f3make ( u, p0, p1 );
  f3make ( v, p0, p2 );
  f3cross( u, v, plane->n );
  f3norm ( plane->n );
  plane->dist = f3dot( plane->n, p0 );
}

INLINE void plane_calc_fast( plane_t *plane, const float3 p0, const float3 p1, const float3 p2 ){
  float3 u, v;
  f3make ( u, p0, p1 );
  f3make ( v, p0, p2 );
  f3cross( u, v, plane->n );
  plane->dist = f3dot( plane->n, p0 );
}

INLINE int plane_test( const plane_t *plane, const float3 p )
{
  float proj = f3dot( plane->n, p );
  if( proj > plane->dist )
    return  1;
  if( proj < plane->dist )
    return -1;
  return 0;
}


INLINE float plane_line( const plane_t *plane, const float3 p, const float3 d ){
  return (  plane->dist - f3dot( p, plane->n )  ) / f3dot( d, plane->n );
}

INLINE float plane_line_fast( const plane_t *plane, const float3 p, const float3 d, float ddotn ){
  return (  plane->dist - f3dot( p, plane->n )  ) / ddotn;
}

INLINE float plane_solve( const plane_t *plane, const float3 p ){
  return f3dot( plane->n, p ) - plane->dist;
}

INLINE int32 plane_crosses( const plane_t *plane, const float3 ps[], size_t nps ){
  size_t i;

  int32 side = plane_solve( plane, ps[0] ) > 0.0f;
  for( i = 1; i < nps; i++ ){
    int32 otherside = plane_solve( plane, ps[i] ) >= 0.0f;
    if( otherside != side )
      break;
  }

  return i < nps ? 1 : 0;
}


void sat_init( sat_t *sat, const float3 axis ){
  f3copy( sat->v, axis );
  //if( f3dot( sat->v, sat->v ) != 1.0f )
  //  f3norm( sat->v );
  sat->mins[0] = sat->mins[1] = sat->maxs[0] = sat->maxs[1] = sat->mids[0] = sat->mids[1] = 0.0f;
}

void sat_calc( sat_t *sat, int second, const float3 ps[], size_t nps ){
  int32  i;
  float *min, *max, *mid;

  min = second ? &sat->mins[1] : &sat->mins[0];
  max = second ? &sat->maxs[1] : &sat->maxs[0];
  mid = second ? &sat->mids[1] : &sat->mids[0];
  *min = *max = f3dot( ps[0], sat->v );
  for( i = 1; i < nps; i++ ){
    float dist = f3dot( ps[i], sat->v );
    if( dist < *min )
      *min = dist;
    if( dist > *max )
      *max = dist;
  }
  *mid = 0.5f * ( *min + *max );
}

void sat_make( sat_t *sat, int second, const float3 c, float hdist ){
  float *min, *max, *mid;

  min = second ? &sat->mins[1] : &sat->mins[0];
  max = second ? &sat->maxs[1] : &sat->maxs[0];
  mid = second ? &sat->mids[1] : &sat->mids[0];

  hdist = fabsf(hdist); //half distance.

  *mid = *max = *min = f3dot( c, sat->v );
  *max += hdist;
  *min -= hdist;
}

void sat_init_from_plane( sat_t *sat, const plane_t *plane, int32 second ){
  float *min, *max, *mid;

  min = second ? &sat->mins[1] : &sat->mins[0];
  max = second ? &sat->maxs[1] : &sat->maxs[0];
  mid = second ? &sat->mids[1] : &sat->mids[0];

  f3copy( sat->v, plane->n );
  *mid = *max = *min = plane->dist;
}

INLINE int32 sat_olap( sat_t *sat ){
  // return 1 for overlapping, 0 for separated
  // given lines 1 & 2 on a single axis, (a1,a2) & (b1,b2)
  // lines intersect when: a2 >= b1 and b2 >= a1
  return( sat->maxs[0] >= sat->mins[1] && sat->maxs[1] >= sat->mins[0] );
}



void lseg_calc( lseg_t *lseg, const float3 p0, const float3 p1 ){
  f3copy( lseg->ps[0], p0 );
  f3copy( lseg->ps[1], p1 );
  f3make( lseg->dir, lseg->ps[0], lseg->ps[1] );
  if( f3dot( lseg->dir, lseg->dir ) > 0.0f ){
    lseg->length = f3norm( lseg->dir );
    int i;
    for( i = 0; i < 3; i++ )
      if( 0.0f == fabsf(lseg->dir[i] ) )  //pesky negative zeros...
        lseg->dir[i] = 0.0f;
  }
  else{
    f3zero( lseg->dir );
    lseg->length = 0.0f;
  }
}


void lseg_slide( lseg_t *lseg, float dist, size_t end ){
  if( 0.0f == fabsf(dist) )
    return;

  if( !end ){
    if( dist > lseg->length ){
      printf( "%s - cannot slide beyond end.\n", __FUNCTION__ );
      return;
    }
    f3madd( lseg->ps[0], +dist, lseg->dir );
    lseg->length -= dist;
  }
  else{
    if( dist < -lseg->length ){
      printf( "%s - cannot slide beyond beginning (|d| v.|l|: %f v %f).\n", __FUNCTION__, fabsf(dist), lseg->length );
      return;
    }
    f3madd( lseg->ps[1], +dist, lseg->dir );
    lseg->length += dist;
  }
}

void lseg_perp( const lseg_t *lseg, const float3 p, float3 n ){
  float3 u, v;
  f3make ( u, lseg->ps[0], p );
  f3cross( lseg->dir, u, v  );
  f3cross( v, lseg->dir, n  );
  f3norm ( n );
}


INLINE void circle_init( circle_t *circle, const float2 p, float radius ){
  f2copy( circle->p, p );
  circle->radius = radius < 0.0f ? -radius : radius;
}

void circle_calc( circle_t *circle, const float2 *ps, uint32 nps ){
  f2copy( circle->p, ps[0] );
  for( uint32 i = 1; i < nps; i++ )
    f2add( circle->p, ps[i] );
  f2muls( circle->p, 1.0f/(float)nps );

  float2 u;
  f2make( u, ps[0], circle->p );
  circle->radius = f2dot( u, u );
  for( uint32 i = 1; i < nps; i++ ){
    f2make( u, ps[i], circle->p );
    float s = f2dot( u, u );
    if( s > circle->radius )
      circle->radius = s;
  }

  circle->radius = sqrtf( circle->radius );
}

INLINE int32 circle_touch( const circle_t *circle, const float2 p ){
  float2 v;
  f2make( v, circle->p, p );
  if( f2dot( v, v ) <= SQUARE( circle->radius ) )
    return 1;
  return 0;
}


void lseg2_calc( lseg2_t *lseg, const float2 p0, const float2 p1, int32 normalize ){
  f2copy( lseg->ps[0], p0 );
  f2copy( lseg->ps[1], p1 );
  f2make( lseg->dir, lseg->ps[0], lseg->ps[1] );
  lseg->length    =
  lseg->length_sq = f2dot( lseg->dir, lseg->dir );
  lseg->norm      = normalize ? 1 : 0;
  if( lseg->norm && lseg->length_sq > 0.0f ){
    lseg->length = sqrtf( lseg->length_sq );
    f2muls( lseg->dir, 1.0f / lseg->length );
  }
}


int32 lseg2_isects( const lseg2_t *lseg0, const lseg2_t *lseg1, float *mu, float *nu ){

  float3   u, v;
  float2x2 M, M_inv;
  int32    inside0, inside1;

  // l0.p0 + mu * l0.d = l1.p0 + nu * l1.d
  // mu * l0.d - nu * l1.d = l1.p0 - l0.p0;
  // - let r  := l1.p0 - l0.p0
  // - let d0 := l0.d
  // - let d1 := l1.d
  // solve for <mu,nu>:
  //   | d0.x, -d1.x | * | mu | = | r.x, r.y |
  //   | d0.y, -d1.y |   | nu |

  f2copy( u, lseg1->ps[0] );
  f2sub ( u, lseg0->ps[0] );

  M[0][0] =  lseg0->dir[0];
  M[0][1] = -lseg1->dir[0];
  M[1][0] =  lseg0->dir[1];
  M[1][1] = -lseg1->dir[1];

  float det =  M[0][0] * M[1][1] - M[1][0] * M[0][1];
  if( fabsf(det) == 0.0f )
    return -1;

  float det_inv = 1.0f / det;
  M_inv[0][0] = M[1][1] *  det_inv;
  M_inv[1][1] = M[0][0] *  det_inv;
  M_inv[0][1] = M[0][1] * -det_inv;
  M_inv[1][0] = M[1][0] * -det_inv;

  v[0] = f2dot( M_inv[0], u );
  v[1] = f2dot( M_inv[1], u );

  if( mu )
    *mu = v[0];
  if( nu )
    *nu = v[1];

  inside0 = inside1 = 0;
  if( lseg0->norm )
    inside0 = v[0] >= 0.0f && v[0] <= lseg0->length;
  else
    inside0 = v[0] >= 0.0f && v[0] <= 1.0f;

  if( lseg1->norm )
    inside1 = v[1] >= 0.0f && v[1] <= lseg1->length;
  else
    inside1 = v[1] >= 0.0f && v[1] <= 1.0f;

  return inside0 && inside1;
}

int32 lseg2_circle_hit( const lseg2_t *lseg, const circle_t *circle ){
  float2 v, d;
  float  dist, len;

  if( lseg->length_sq == 0.0f )
    return circle_touch( circle, lseg->ps[0] );

  len = lseg->length;
  f2make( v, lseg->ps[0], circle->p );
  f2copy( d, lseg->dir );
    if( !lseg->norm )
      len = f2norm( d );

  dist = f2dot( v, d );
  if( dist <= 0.0f )
    return circle_touch( circle, lseg->ps[0] );
  else if( dist >= len )
    return circle_touch( circle, lseg->ps[1] );
  else{
    f2madd( v, -dist, d );
    if( f2dot( v, v ) <= SQUARE(circle->radius) )
      return 1;
    //float2 n;
    //f2set( n, -d[1], d[0] );
    //proj = f2dot( v, n );
    //if( fabsf( proj ) <= circle->radius )
    //  return 1;
  }

  return 0;
}

int32 lseg2_circle_isect( const lseg2_t *lseg, const circle_t *circle, float2 alpha ){
  /*
    (p.x + alpha*v.x)^2 + (p.y + alpha*v.y)^2 = r^2

    p.x^2 + 2*p.x*alpha*v.x + alpha^2*v.x^2 + p.y^2 + 2*p.y*alpha*v.y + alpha^2*v.y^2 = r^2

    alpha^2(v.x^2+v.y^2) + alpha(2*p.x*v.x + 2*p.y*v.y) + p.x^2 + p.y^2 = r^2

    A = v.x^2 + v.y^2
    B = 2 * ( p.x*v.x + p.y*v.y )
    C = p.x^2 + p.y^2 - r^2

    A*alpha^2 + B*alpha + C = 0
    alpha one = (-B + sqrt( B^2 - 4AC) )/2A
    alpha two = (-B - sqrt( B^2 - 4AC) )/2A
 */

  int32 touch0, touch1;

  touch0 = circle_touch( circle, lseg->ps[0] );
  if( lseg->length == 0.0f ){
    if( touch0 ){
      alpha[0] = alpha[1] = 0.0f;
      return 1;
    }
    else
      return 0;
  }
  touch1 = circle_touch( circle, lseg->ps[1] );

  if( touch0 && touch1 ){
    alpha[0] = 0.0f;
    alpha[1] = 1.0f;
    return 1;
  }

  float2 p, v;
  f2copy( p, lseg->ps[0] );
  f2sub ( p, circle->p );
  f2make( v, lseg->ps[0], lseg->ps[1] );

  float A = v[0]*v[0] + v[1]*v[1];  // > 0 because line length > 0
  float B = 2.0f*( p[0]*v[0] + p[1]*v[1] );
  float C = p[0]*p[0] + p[1]*p[1] - circle->radius*circle->radius;
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


// calculate bases for a given up vector, using x,y & z axes
void calc_bases_from_up( const float3 u, float3 bs[3] ){
  const float3 axes[3] =  {
      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f }
  };
  f3copy( bs[2], u );
  f3norm( bs[2] );
  if( fabsf( f3dot( bs[2], axes[1] ) ) < 0.95f ){
    f3cross( axes[1], bs[2], bs[0] );
    f3norm ( bs[0] );
  }
  else{
    f3cross( axes[2], bs[2], bs[0] );
    f3norm ( bs[0] );
  }
  f3cross( bs[2], bs[0], bs[1] );
  f3norm ( bs[1] );
}


// calculate bases for a given look vector, using x,y & z axes
void calc_bases_from_look( const float3 l, float3 bs[3] ){
  const float3 axes[3] =  {
      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f }
  };
  //support polar sampling for cubemaps w/ 4k res
  //assume up pixel is <0,0,4096>, right vectors can be
  //calculated for <+1,0,4096>, <-1,0,4096>, <0,+1,4096>
  //& <0,-1,4096> because cross product of all of these
  //with up is not degenerate. Anything closer to up
  //(such as 0.9,0,4096) is considered degenerate and
  // vector <0,1,0> is used as up instead of <0,0,1> (so
  // right vector turns out to be <1,0,0>)

  static float tol = 0.95f;
  if( 0.0f == tol ){
    float3 d0 = { 0.0f, 0.0f, 4096.0f };
    float3 d1 = { 1.0f, 0.0f, 4096.0f };
    f3norm( d0 );
    f3norm( d1 );
    tol = f3dot( d0, d1 );
  }

  f3copy( bs[1], l );
  f3norm( bs[1] );

  //calculate right axis using <0,0,+1> as up
  if( fabsf( f3dot( bs[1], axes[2] ) ) < tol ){
    f3cross( bs[1], axes[2], bs[0] );
    f3norm ( bs[0] );
  }
  //calculate right axis using <0,+1,0> as up
  else{
    f3cross( bs[1], axes[1], bs[0] );
    f3norm ( bs[0] );
  }

  //calculate up axis
  f3cross( bs[0], bs[1], bs[2] );
  f3norm ( bs[2] );
}



float triangle_area( const float3 p0, const float3 p1, const float3 p2 ){
  float3 u, v, n;
  f3make ( u, p0, p1 );
  f3make ( v, p0, p2 );
  f3cross( u, v,  n  );
  return f3mag( n ) * 0.5f;
}

void rect_calc( rect_t *rect, const float3 p, const float3 n, const float3 raxis, float w, float h ){
  rect->size[0] = fabsf( w );
  rect->size[1] = fabsf( h );

  f3copy( rect->p, p );
  plane_make( &rect->plane, n, rect->p );

  f3cross( rect->plane.n, raxis, rect->axes[1] );
  f3cross( rect->axes[1], rect->plane.n, rect->axes[0] );
  f3norm ( rect->axes[0] );
  f3norm ( rect->axes[1] );
}

void rect_calc2( rect_t *rect, const float3 p, const float3 raxis, const float3 uaxis, float w, float h ){
  rect->size[0] = fabsf( w );
  rect->size[1] = fabsf( h );

  float3 n;
  f3cross( raxis, uaxis, n );

  f3copy( rect->p, p );
  plane_make( &rect->plane, n, rect->p );

  f3copy ( rect->axes[0], raxis );
  f3norm ( rect->axes[0] );
  f3cross( rect->plane.n, rect->axes[0], rect->axes[1] );
  f3norm ( rect->axes[1] );
}


int32 rect_isect_ray( const rect_t *rect, const ray_t *ray, float *uval, float *vval ){
  float3      p, r;
  float       dist, uproj, vproj;

  if( fabsf( f3dot( ray->d, rect->plane.n ) ) == 0.0f )
    return 0;

  dist = plane_line( &rect->plane, ray->p, ray->d );
  if( dist < 0.0f )
    return 0;

  f3copy( p, ray->p );
  f3madd( p, dist, ray->d );
  f3make( r, rect->p, p );

  uproj = f3dot( r, rect->axes[0] );
  if( uproj < 0.0f || uproj > rect->size[0] )
    return 0;
  vproj = f3dot( r, rect->axes[1] );
  if( vproj < 0.0f || vproj > rect->size[1] )
    return 0;

  if( uval )
    *uval = uproj / rect->size[0];
  if( vval )
    *vval = vproj / rect->size[1];
  return 1;
}

int32 rect_isect_ray_test( const rect_t *rect, const ray_t *ray, rect_isect_ray_test_t *test ){
  test->hit = test->away = 0;
  float ddotn = f3dot( ray->d, rect->plane.n );
  if( ddotn >= 0.0f ){
    test->away++;
    return 0;
  }

  test->dist = plane_line_fast( &rect->plane, ray->p, ray->d, ddotn );
  f3copy( test->p, ray->p );
  f3madd( test->p, test->dist, ray->d );

  float3 r;
  f3make( r, rect->p, test->p );
  test->uv[0] = f3dot( r, rect->axes[0] ) / rect->size[0];
  test->uv[1] = f3dot( r, rect->axes[1] ) / rect->size[1];
  test->hit =
      ( test->dist  >= 0.0f ) &&
      ( test->uv[0] >= 0.0f && test->uv[0] <= 1.0f ) &&
      ( test->uv[1] >= 0.0f && test->uv[1] <= 1.0f );

  return test->hit;
}


