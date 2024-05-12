#include "mysdl.h"
#include "timer.h"

#include "libs.h"
#define NO_SDL_GLEXT
#include <GL\glew.h>
#include <SDL\SDL.h>
#include <SDL\SDL_audio.h>
#include <SDL\SDL_opengl.h>

int32 disp_w = 1280;
int32 disp_h = 720;
int32 disp_centered = 0;
int32 disp_frame = 0;
int32 disp_fullscreen = 0;

MYSDL_keyboard_t keyboard;
MYSDL_mouse_t mouse;
MYSDL_joyval_t joysticks[MYSDL_MAXJOYS];

SDL_Surface *video;
SDL_Joystick *sdl_joys[MYSDL_MAXJOYS];

int32 running = 0;
int32 paused = 0;
int32 resumed = 1;
int32 joyful = 0;

float ms_tot = 0.0f;
float ms_run = 0.0f;

void term() {
  if( joyful ){
    for( int32 i = 0; i < SDL_NumJoysticks(); i++ ){
      if( i >= MYSDL_MAXJOYS )
        break;
      if( SDL_JoystickOpened( i ) )
        SDL_JoystickClose( sdl_joys[i] );
    }
  }
}

int32 mysdl_resumed() {
  return 1 == resumed;
}

int32 mysdl_paused() {
  return paused;
}

void mysdl_unpause() {
  if( -1 == resumed )
    resumed = 0;
}

int32 mysdl_gl_init( const char *title, int32 w, int32 h, int32 center, int32 full, int32 joy, int32 sound ) {

  running = 0;
  printf( "initializing SDL Video..." );

  uint32 flags = SDL_INIT_VIDEO | SDL_INIT_TIMER;

  joyful = 0;
  if( joy ){
    flags |= SDL_INIT_JOYSTICK;
    joyful = 1;
  }

  if( sound )
    flags |= SDL_INIT_AUDIO;

  if( SDL_Init( flags ) < 0 ){
    printf( "FAILED! aborted\n" );
    return 0;
  }

  printf( "done\n" );
  atexit( SDL_Quit );
  atexit( term );

  printf( "creating OpenGL window..." );
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  if( w > 0 && h > 0 ){
    disp_w = w;
    disp_h = h;
  }

  if( center ){
    disp_centered = 1;
    SDL_putenv( "SDL_VIDEO_WINDOW_POS=center" );
  }
  if( full )
    video = SDL_SetVideoMode( disp_w, disp_h, 0, SDL_OPENGL | SDL_FULLSCREEN );
  else
    video = SDL_SetVideoMode( disp_w, disp_h, 0, SDL_OPENGL );

  if( NULL == video ){
    printf( "FAILED! aborted\n" );
    return 0;
  }
  printf( "done\n" );

  printf( "initializing GLEW (v %s)...", glewGetString( GLEW_VERSION ) );
  if( GLEW_OK != glewInit() || !GL_VERSION_3_1 ){
    printf( "FAILED! aborted\n" );
    return 0;
  }
  printf( "done\n" );

  if( NULL != title )
    SDL_WM_SetCaption( title, NULL );

  keyboard.numkeys = SDLK_LAST;

  if( joyful ){
    for( int32 i = 0; i < SDL_NumJoysticks(); i++ )
      if( i >= MYSDL_MAXJOYS )
        break;
      else
        sdl_joys[i] = SDL_JoystickOpen( i );
  }

  FILE *fp = fopen( "extensions.txt", "w" );

  const char *ptr = (const void*)glGetString( GL_EXTENSIONS );
  int32 len = strlen( ptr );
  for( int32 i = 0; i < len; i++ )
    if( ptr[i] == ' ' )
      fputc( '\n', fp );
    else
      fputc( ptr[i], fp );

  fprintf( fp, "*************************\n" );

  int32 v;
  glGetIntegerv( GL_MAX_VERTEX_UNIFORM_COMPONENTS, &v );
  fprintf( fp, "GL_MAX_VERTEX_UNIFORM_COMPONENTS...: %d\n", v );

  glGetIntegerv( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &v );
  fprintf( fp, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS..: %d\n", v );

  glGetIntegerv( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &v );
  fprintf( fp, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS.: %d\n", v );

  glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &v );
  fprintf( fp, "GL_MAX_TEXTURE_IMAGE_UNITS.........: %d\n", v );

  glGetIntegerv( GL_MAX_VARYING_VECTORS, &v );
  fprintf( fp, "GL_MAX_VARYING_VECTORS.............: %d\n", v );

  glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &v );
  fprintf( fp, "GL_MAX_COLOR_ATTACHMENTS...........: %d\n", v );

  glGetIntegerv( GL_MAX_DRAW_BUFFERS, &v );
  fprintf( fp, "GL_MAX_DRAW_BUFFERS................: %d\n", v );

  glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &v );
  fprintf( fp, "GL_MAX_VERTEX_ATTRIBS..............: %d\n", v );

  glGetIntegerv( GL_MAX_TEXTURE_BUFFER_SIZE, &v );
  fprintf( fp, "GL_MAX_TEXTURE_BUFFER_SIZE.........: %d\n", v );

  float f;
  glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f );
  fprintf( fp, "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT..: %f\n", f );

  fclose( fp );

  return (running = 1);
}

int32 mysdl_running() {
  return running;
}

void mysdl_pump() {
  const float tol = 0.05f;
  int32 x, y, n;

  SDL_PumpEvents();

  const uint8 *keys = SDL_GetKeyState( &n );
  int32 button = SDL_GetMouseState( &x, &y );

  for( int32 i = 0; i < n; i++ ){
    if( keys[i] )
      keyboard.keys[i].down++;
    else
      keyboard.keys[i].down = 0;

    keyboard.keys[i].press = 1 == keyboard.keys[i].down ? 1 : 0;
  }
  keyboard.numkeys = n;

  for( int32 i = 0; i < 8; i++ ){
    if( button & SDL_BUTTON( i + 1 ) )
      mouse.buttons[i].down++;
    else
      mouse.buttons[i].down = 0;

    mouse.buttons[i].press = 1 == mouse.buttons[i].down ? 1 : 0;
  }
  mouse.x = x;
  mouse.y = y;

  if( joyful ){
    SDL_JoystickUpdate();
    for( int32 i = 0; i < SDL_NumJoysticks(); i++ ){
      if( i >= MYSDL_MAXJOYS )
        break;
      if( !SDL_JoystickOpened( i ) )
        continue;

      MYSDL_joyval_t *stick = &joysticks[i];
      n = SDL_JoystickNumAxes( sdl_joys[i] );
      for( int32 j = 0; j < n; j++ ){
        if( j >= MYSDL_MAXJOYAXES )
          break;

        float v = (float)SDL_JoystickGetAxis( sdl_joys[i], j );
        if( v > 0.0f ){
          v /= 32767.0f;
          v = (v - tol) / (1.0f - tol);
          v = v < 0.0f ? 0.0f : v > 1.0f ? 1.0f : v;
        }
        if( v < 0.0f ){
          v /= 32768.0f;
          v = (v + tol) / (1.0f - tol);
          CLAMP( v, -1.0f, 0.0f );
        }
        stick->axes[j] = v;
        //if( v != 0.0f )
        //  printf( "%d - %+.3f\n", j, v );
      }
      //printf( "\n" );

      n = SDL_JoystickNumHats( sdl_joys[i] );
      for( int32 j = 0; j < n; j++ ){
        MYSDL_hatval_t *hat = &stick->hats[j];

        int v = SDL_JoystickGetHat( sdl_joys[i], j );

        if( v & SDL_HAT_UP )
          hat->keys[ MYSDL_HAT_U].down++;
        else
          hat->keys[ MYSDL_HAT_U].down = 0;

        if( v & SDL_HAT_DOWN )
          hat->keys[ MYSDL_HAT_D].down++;
        else
          hat->keys[ MYSDL_HAT_D].down = 0;

        if( v & SDL_HAT_RIGHT )
          hat->keys[ MYSDL_HAT_R].down++;
        else
          hat->keys[ MYSDL_HAT_R].down = 0;

        if( v & SDL_HAT_LEFT )
          hat->keys[ MYSDL_HAT_L].down++;
        else
          hat->keys[ MYSDL_HAT_L].down = 0;

        hat->keys[0].press = 1 == hat->keys[0].down ? 1 : 0;
        hat->keys[1].press = 1 == hat->keys[1].down ? 1 : 0;
        hat->keys[2].press = 1 == hat->keys[2].down ? 1 : 0;
        hat->keys[3].press = 1 == hat->keys[3].down ? 1 : 0;
      }

      n = SDL_JoystickNumButtons( sdl_joys[i] );
      for( int32 j = 0; j < n; j++ ){
        if( SDL_JoystickGetButton( sdl_joys[i], j ) )
          stick->buttons[j].down++;
        else
          stick->buttons[j].down = 0;

        stick->buttons[j].press = 1 == stick->buttons[j].down ? 1 : 0;
      }
    }
  }

  if( keyboard.keys[SDLK_ESCAPE].down )
    running = 0;

  if( !paused && 0 == resumed )
    resumed = 1;

  if( keyboard.keys[SDLK_HOME].press ){
    paused = !paused;
    if( paused )
      resumed = -1;
  }
}

int32 mysdl_focused( int32 input, int32 mouse ) {
  uint8 flags = SDL_APPACTIVE;
  if( input )
    flags |= SDL_APPINPUTFOCUS;
  if( mouse )
    flags |= SDL_APPMOUSEFOCUS;
  uint8 state = SDL_GetAppState();
  return (state & flags) == flags;
}

const MYSDL_keyboard_t* MySDL_keyboard() {
  return &keyboard;
}
const MYSDL_mouse_t* MySDL_mouse() {
  return &mouse;
}
const MYSDL_joyval_t* MySDL_joysticks( int32 no ) {
  if( no < 0 || no >= MYSDL_MAXJOYS )
    return NULL;
  if( no >= SDL_NumJoysticks() )
    return NULL;
  if( !SDL_JoystickOpened( no ) )
    return NULL;
  return &joysticks[no];
}

extern float MySDL_time( int32 time_fmt ) {
  switch( time_fmt){
    case MYSDL_TIME_FORMAT_SECS:
      return ms_run * 1e-3f;
    case MYSDL_TIME_FORMAT_MS:
    default: {
      return ms_run;
    }
  }
  return ms_run;
}

extern uint32 mysdl_ticks() {
  return (uint32)(ms_run / DT_MS);
}

void mysdl_run() {
  usec_timer_t loop_timer, draw_timer;
  uint32 n;
  int32 cur;

  printf( "*** INIT ***\n" );
  mysdl_init();

  printf( "*** LOOP ***\n" );
  usec_timer_init( &loop_timer );
  usec_timer_init( &draw_timer );

  cur = SDL_ShowCursor( 0 );
  n = 0;
  ms_tot = 0.0f;
  usec_timer_start( &loop_timer );
  running = 1;
  while( mysdl_running() ){
    usec_timer_stop( &loop_timer );
    usec_timer_start( &loop_timer );

    double dt = (double)loop_timer.diff_usecs * 0.001;
    CLAMP( dt, 0.0, DT_MS );
    ms_tot = ms_tot + (float)dt;

    uint32 new_n = ms_tot / DT_MS;
    if( new_n > n ){
      n = new_n;

      mysdl_pump();
      if( mysdl_paused() ){
        if( 0 == SDL_ShowCursor( -1 ) )
          SDL_ShowCursor( 1 );
      }
      else{
        if( 1 == SDL_ShowCursor( -1 ) )
          SDL_ShowCursor( 0 );
        if( (SDL_GetAppState() & SDL_APPINPUTFOCUS) ){
          SDL_WarpMouse( disp_w / 2, disp_h / 2 );
          mysdl_unpause();
        }
      }
      if( mysdl_resumed() )
        mysdl_step();
      ms_run = n * DT_MS;
    }

    usec_timer_start( &draw_timer );
    mysdl_draw();
    usec_timer_stop( &draw_timer );
  }
  SDL_ShowCursor( cur );

  printf( "************\n" );

  mysdl_term();
  printf( "%s: average loop FPS: %f\n", __FUNCTION__, USEC_TIMER_AVG_SECS( loop_timer ) );
  printf( "%s: average draw FPS: %f\n", __FUNCTION__, USEC_TIMER_AVG_SECS( draw_timer ) );

}
