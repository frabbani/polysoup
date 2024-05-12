#include "actor.h"

#include "mysdl.h"
#include "defs.h"

#include <polysoup/trace.h>

int32 actors_pause_movement = 0;

void actor_adjust_zaccel( actor_t *actor ) {
  const float slope = sinf( GROUNDSLOPE * PI / 180.0f );
  const float tol = 0.01f; //0.01 units / second
  const coll_geom_trace_test_t *test = map_sphere_trace_test();
  const coll_geom_trace_info_t *infos = test->traceinfos.elems;
  for( size_t i = 0; i < test->traceinfos.size; i++ ){
    if( infos[i].n[2] > +slope && actor->zvel < 0.0f ){
      actor->zvel -= actor->zvel * infos[i].n[2];
      if( actor->zvel >= -tol )
        actor->zvel = 0.0f;
    }
    if( infos[i].n[2] < -slope && actor->zvel > 0.0f ){
      actor->zvel += actor->zvel * infos[i].n[2];
      if( actor->zvel <= tol )
        actor->zvel = 0.0f;
    }
  }
}
void actor_move( actor_t *actor, float move, float side, float turn, float jump ) {

  int32 steps = 1;
  CLAMP( turn, -1.0f, 1.0f );
  CLAMP( move, -1.0f, 1.0f );
  CLAMP( side, -1.0f, 1.0f );
  CLAMP( jump, 0.0f, 1.0f );

  actor->yaw += turn * fabsf( actor->yawspeed ) * DT_SECS;
  actor->yaw = fmodf( actor->yaw, 360.0f );
  if( actor->yaw < 0.0f )
    actor->yaw += 360.0f;

  float3x3 Y;
  f3x3ypr( Y, actor->yaw, 0.0f, 0.0f, 1 );
  f3set( actor->transform.b0, 1.0f, 0.0f, 0.0f );
  f3set( actor->transform.b1, 0.0f, 1.0f, 0.0f );
  f3set( actor->transform.b2, 0.0f, 0.0f, 1.0f );
  f3transf( Y, actor->transform.b0 );
  f3transf( Y, actor->transform.b1 );
  f3transf( Y, actor->transform.b2 );

  sphere_t sphere;
  float3 p, v;
  coll_geom_trace_test_t *test = map_sphere_trace_test();

  if( !actors_pause_movement ){
    actor->zvel += GRAVITY * DT_SECS;
    actor->zvel += actor->jumpaccel * jump * DT_SECS;
  }
  else
    actor->zvel = 0.0f;

  f3copy( p, actor->transform.p );
  f3madd( p, side * actor->movespeed * DT_SECS, actor->transform.b0 );
  f3madd( p, move * actor->movespeed * DT_SECS, actor->transform.b1 );
  p[2] += actor->zvel * DT_SECS;

  f3make( v, actor->transform.p, p );
  f3copy( sphere.c, actor->transform.p );
  sphere.radius = actor->radius;

  steps = 0;
  if( f3dot( v, v ) > 1e-9f ){
    float len = f3mag( v );
    steps = (int32)ceilf( len / (4.0f * sphere.radius) );
  }
  else
    return;

  steps = 4;
  f3muls( v, 1.0f / (float)steps );

  if( actor->movetype == MOVETYPE_SLIDE ){
    for( int32 i = 0; i < steps; i++ ){
      coll_geom_trace_test_sphere( test, sphere );
      coll_geom_trace( test, v );
      actor_adjust_zaccel( actor );
      if( test->stuck )
        break;
      f3add( sphere.c, v );
    }
    if( test->stuck ){
      if( actor->zvel < 0.0f )
        actor->zvel = 0.0f;
      printf( "%s: stuck!\n", __FUNCTION__ );
    }
    if( !actors_pause_movement )
      f3copy( actor->transform.p, sphere.c );
  }

}
