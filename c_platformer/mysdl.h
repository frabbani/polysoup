#pragma once

#include "defs.h"
#include <SDL\SDL.h>

#define DT_SECS   0.01666f
#define DT_MS     (DT_SECS*1000.)

typedef struct MySDL_Keyval_s {
  int16 down;
  int16 press;
} MySDL_KeyVal;

typedef struct MYSDL_hatval_s {
  MySDL_KeyVal keys[4];
} MYSDL_hatval_t;

typedef struct MYSDL_mouse_s {
  MySDL_KeyVal buttons[8];
  int32 x, y;
} MYSDL_mouse_t;

typedef struct MYSDL_keyboard_s {
  int32 numkeys;
  MySDL_KeyVal keys[512];
} MYSDL_keyboard_t;

#define MYSDL_MAXJOYS         8
#define MYSDL_MAXJOYAXES      16
#define MYSDL_MAXJOYHATS      4
#define MYSDL_MAXJOYBUTTONS   32

#define MYSDL_HAT_U  0
#define MYSDL_HAT_D  1
#define MYSDL_HAT_R  2
#define MYSDL_HAT_L  3

typedef struct MYSDL_joyval_s {
  float axes[MYSDL_MAXJOYAXES];  //up to 16 axes
  MySDL_KeyVal buttons[MYSDL_MAXJOYBUTTONS];  //up to 32 buttons
  MYSDL_hatval_t hats[MYSDL_MAXJOYHATS];  //up to 4 hats

} MYSDL_joyval_t;

#define  MYSDL_TIME_FORMAT_MS   0
#define  MYSDL_TIME_FORMAT_SECS 1

extern const MYSDL_keyboard_t* MySDL_keyboard();
extern const MYSDL_mouse_t* MySDL_mouse();
extern const MYSDL_joyval_t* MySDL_joysticks( int32 no );

#define KEY_DOWN(k)  ( MySDL_keyboard()->keys[ (k) & 511 ].down  )
#define KEY_PRESS(k) ( MySDL_keyboard()->keys[ (k) & 511 ].press )

#define MOUSE_BUTTON_DOWN( b ) ( mysdl_mouse()->buttons[ (b) & 7 ].down )
#define MOUSE_BUTTON_PRESS( b ) ( mysdl_mouse()->buttons[ (b) & 7 ].press )

extern float MySDL_time( int32 time_fmt );
extern uint32 mysdl_ticks();

extern int32 mysdl_gl_init( const char *title, int32 w, int32 h, int32 center, int32 full, int32 joy, int32 sound );
extern void mysdl_pump();
extern int32 mysdl_focused( int32 input, int32 mouse );

extern int32 mysdl_running();
extern int32 mysdl_resumed();
extern int32 mysdl_paused();
extern void mysdl_unpause();

extern int32 mysdl_init();
extern void mysdl_step();
extern void mysdl_draw();
extern void mysdl_term();
extern void mysdl_run();

