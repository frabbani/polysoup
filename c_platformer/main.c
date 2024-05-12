#define DISP_W  1280 //640 //800
#define DISP_H  720 //360 //450
#define CENTERED
//#define FULLSCREEN

#define NOFRAME

#include "mysdl.h"
#include "mygl.h"
#include "timer.h"
#include "camera.h"
#include "bitmap.h"
#include "font.h"
#include "map.h"
#include "obj.h"
#include "actor.h"

#include <polysoup/trace.h>

int32 debug = 0;
camera_t cam;
WAVE_obj_t sph;
actor_t player;
font_t font = { 0 };

void player_logic() {
  const float speed = MySDL_keyboard()->keys[SDLK_LSHIFT].down ? 1.5f : 6.0f; // : 2.5f;
  float turnspeed = 360.0f / 3.0f;  //full turn in 3 seconds
  const float jumpaccel = 300.0f;

  player.movespeed = speed;
  player.yawspeed = turnspeed;
  player.jumpaccel = jumpaccel;
  player.movetype = MOVETYPE_SLIDE;

  float turn = 0.0f;
  if( KEY_DOWN( 'd' ) )
    turn = -1.0f;
  else if( KEY_DOWN( 'a' ) )
    turn = +1.0f;

  float move = 0.0f;
  if( KEY_DOWN( 'w' ) )
    move = +1.0f;
  else if( KEY_DOWN( 's' ) )
    move = -1.0f;

  float side = 0.0f;
  if( KEY_DOWN( 'e' ) )
    side = +1.0f;
  else if( KEY_DOWN( 'q' ) )
    side = -1.0f;

  float jump = 0.0f;
  if( KEY_PRESS( ' ' ) )
    jump = 1.0f;

  actor_move( &player, move, side, turn, jump );
}

void cam_logic( const actor_t *follow ) {
  static float yaw = 0.0f, pitch = 0.0f;

  float dx = -(float)(MySDL_mouse()->x - DISP_W / 2) * 2.0f / (float)DISP_W;
  float dy = -(float)(MySDL_mouse()->y - DISP_H / 2) * 2.0f / (float)DISP_H;

  if( MySDL_mouse()->buttons[2].down ){
    pitch += dy * 360;
    yaw += dx * 360;
  }
  else{
    const float ideal = -30.0f;

    float diff = ideal - pitch;
    diff *= 0.9f;
    pitch += diff;
    if( fabsf( pitch - ideal ) < 0.01f )
      pitch = ideal;

    yaw *= 0.9f;
    if( fabsf( yaw ) < 0.01f )
      yaw = 0.0f;
  }

  CLAMP( pitch, -89.0f, 89.0f );
  yaw = fmodf( yaw, 360.0f );
  if( yaw < 0.0f )
    yaw += 360.0f;

  float3x3 YPR;
  f3x3ypr( YPR, yaw, pitch, 0.0f, 1 );

  f3set( cam.bs[0], 1.0f, 0.0f, 0.0f );
  f3set( cam.bs[1], 0.0f, 1.0f, 0.0f );
  f3set( cam.bs[2], 0.0f, 0.0f, 1.0f );
  f3transf( YPR, cam.bs[0] );
  f3transf( YPR, cam.bs[1] );
  f3transf( YPR, cam.bs[2] );

  f3copy( cam.p, cam.bs[1] );
  f3muls( cam.p, -follow->radius * 30.0f );
  //f3madd( cam.p, -3.0f, cam.bs[1] );

  float4x4 W;
  f4x4world( W, follow->transform.p, follow->transform.b0, follow->transform.b1, follow->transform.b2 );
  f3wtransf( W, cam.p, 1.0f );
  f3wtransf( W, cam.b0, 0.0f );
  f3wtransf( W, cam.b1, 0.0f );
  f3wtransf( W, cam.b2, 0.0f );

}

int32 mysdl_init() {
  mygl_setup();

  font_view( DISP_W, DISP_H );
  font_load( &font, "calibri32" );

  WAVE_obj_load( &sph, "sphere", 1.0f, 0.0f );

  f3set( cam.p, -5.0f, 0.0f, 3.0f );
  f3set( cam.b0, 1.0f, 0.0f, 0.0f );
  f3set( cam.b1, 0.0f, 1.0f, 0.0f );
  f3set( cam.b2, 0.0f, 0.0f, 1.0f );
  cam.D_n = 0.01f;
  cam.D_f = 1000.0f;
  cam.FOV = 90.0f;

  f3set( player.transform.p, 0.0f, 0.0f, 5.0f );
  f3set( player.transform.b0, 1.0f, 0.0f, 0.0f );
  f3set( player.transform.b1, 0.0f, 1.0f, 0.0f );
  f3set( player.transform.b2, 0.0f, 0.0f, 1.0f );
  player.transform.D_n = 0.01f;
  player.transform.D_f = 3000.0f;
  player.transform.FOV = 90.0f;
  player.radius = 0.3f;
  player.yaw = 0.0f;
  player.zvel = 0.0f;
  player.movetype = MOVETYPE_SLIDE;

  map_load( "bobomb" );
  f3copy( player.transform.p, map_spawn().xyz );
  printf( "player spawned @ { %f %f %f }\n", player.transform.p[0], player.transform.p[1], player.transform.p[2] );
  return 1;
}

void mysdl_step() {
  static float t = 0;

  if( KEY_PRESS('.') || player.transform.p[2] < player.radius * 0.7f ){
    DEBUG = !DEBUG;
    printf( "%s - DEBUG: %d\n", __FUNCTION__, DEBUG );
  }

  if( KEY_PRESS( 'p' ) )
    actors_pause_movement = !actors_pause_movement;

  player_logic();
  cam_logic( &player );
  t += DT_SECS;
  if( t >= 5.0f ){
    t -= 5.0f;
    printf( "%s - collision rate = %.2f%%\n", __FUNCTION__, hash_collisions_rate() * 100.0f );
    printf( " - player x/y: { %f, %f }\n", player.transform.p[0], player.transform.p[1] );
  }
}

void draw_sphere( float s ) {
  WAVE_obj_face_t *fs = sph.fs.data;
  glBegin( GL_LINES );
  for( size_t i = 0; i < sph.fs.size; i++ ){
    vec3_t p0 = sph.vps.elems[fs[i].vs[0]];
    vec3_t p1 = sph.vps.elems[fs[i].vs[1]];
    vec3_t p2 = sph.vps.elems[fs[i].vs[2]];
    f3muls( p0.xyz, s );
    f3muls( p1.xyz, s );
    f3muls( p2.xyz, s );
    mygl_wireframe_tri( p0.xyz, p1.xyz, p2.xyz, 1.0f, 0.0f, 0.0f, 1.0f );
  }
  glEnd();
}

void draw_aabb( const aabb_t *aabb, float r, float g, float b, float a ) {
  glBegin( GL_LINES );
  mygl_wireframe_quad( aabb->ps[0], aabb->ps[1], aabb->ps[3], aabb->ps[2], r, g, b, a );
  mygl_wireframe_quad( aabb->ps[4], aabb->ps[5], aabb->ps[7], aabb->ps[6], r, g, b, a );
  mygl_wireframe_quad( aabb->ps[0], aabb->ps[1], aabb->ps[5], aabb->ps[4], r, g, b, a );
  mygl_wireframe_quad( aabb->ps[2], aabb->ps[3], aabb->ps[7], aabb->ps[6], r, g, b, a );
  glEnd();
}

void mysdl_draw() {
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glClearDepth( 1.0f );
  glClearStencil( 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

  size_t numselects = 0;
  size_t selections[256];

  mygl_persp( DISP_W, DISP_H, cam.FOV, cam.D_n, cam.D_f );
  //gl_ortho( 5.0f, 5.0f, 0.01f, 1000.0f );

  glMatrixMode( GL_MODELVIEW );

  glLoadIdentity();
  mygl_camera( cam.p, cam.b0, cam.b1, cam.b2 );
  mygl_transform( player.transform.p, player.transform.b0, player.transform.b1, player.transform.b2 );
  draw_sphere( player.radius );

  glLoadIdentity();
  mygl_camera( cam.p, cam.b0, cam.b1, cam.b2 );
  map_draw();

  {
    float3 h = { player.radius, player.radius, player.radius };
    //aabb_t aabb;
    //aabb_make( &aabb, actor->transform.p, h, 0.0f, 1 );
    //draw_aabb( &aabb, 0.0f, 0.0f, 1.0f );
    bpcd_grid_sector_t sector = bpcd_grid_sector_make( map_grid(), player.transform.p, h );
    for( size_t l = sector.ls[0]; l <= sector.ls[1]; l++ ){
      for( size_t r = sector.rs[0]; r <= sector.rs[1]; r++ ){
        for( size_t c = sector.cs[0]; c <= sector.cs[1]; c++ ){
          const bpcd_grid_cell_t *cell = bpcd_grid_get_cell( map_grid(), l, r, c );
          if( !cell ){
            aabb_t aabb;
            bpcd_grid_aabb_for_cell( map_grid(), l, r, c, &aabb );
            draw_aabb( &aabb, 0.25f, 0.0f, 0.5f, 1.0f );
            continue;
          }
          draw_aabb( &cell->aabb, 0.0f, 1.0f, 1.0f, 1.0f );
          float3 p;
          f3copy( p, cell->aabb.sphere.c );
          p[2] += cell->aabb.halfsize[2];
          font_draw_3d( &font, cam, p, 1.0f, color_( 128, 0, 128, 128 ), str32fmt( "%zu (%zu) {%zu.%zu,%zu}", cell->index, cell->indices.size, cell->l, cell->r, cell->c ).chars );
          array_const_indirect_iter_t kiit = array_const_indirect_iter_init( &map_geom()->faces.array, cell->indices.elems, cell->indices.size );

          glBegin( GL_LINES );
          while( array_iterate_indirect_const( &kiit ) ){
            const coll_face_t *face = kiit.data;
            selections[numselects++] = face->index;
            mygl_wireframe_tri( face->ps[0], face->ps[1], face->ps[2], 0.0f, 1.0f, 0.0f, 1.0f );
          }
          glEnd();
        }
      }
    }
  }

  for( size_t i = 0; i < numselects; i++ ){
    const coll_face_t *f = array_data_ro( &map_geom()->faces.array, selections[i] );
    float4 p;
    f3copy( p, f->ps[0] );
    f3add( p, f->ps[1] );
    f3add( p, f->ps[2] );
    f3muls( p, 0.333f );
    str32_t s = str32fmt( "%zu", f->index );
    font_draw_3d( &font, cam, p, 1.0f, color_( 0, 0, 255, 128 ), s.chars );
  }

  str64_t s = str64fmt( "{%+.3f,%+.3f, %+.3f | %+.3f}", player.transform.p[0], player.transform.p[1], player.transform.p[2], player.zvel );
  font_draw( &font, DISP_W - 550, DISP_H - 50, 1.8f, color_( 255, 0, 255, 255 ), s.chars );

  SDL_GL_SwapBuffers();
}

void mysdl_term() {

  printf( "terminating...\n" );
  map_term();
  font_term( &font );

  printf( "done!\n" );

}

int32 main( int argc, char *args[] ) {
  if( mysdl_gl_init( args[0], DISP_W, DISP_H, 1, 0, 0, 0 ) ){
    mysdl_run();
  }

  return 0;
}
