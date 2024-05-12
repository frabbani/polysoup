#pragma once

#include <polysoup/strn.h>
#include <GL\glew.h>
#include "bitmap.h"
#include "camera.h"

#define NUM_PRINTABLE_CHARS    95
#define PRINTABLE_CHARS_OFFSET 0x20

typedef struct font_s {
  int32 _loaded;
  str64_t name;
  size_t w, h;
  image_t char_images[NUM_PRINTABLE_CHARS];
  GLuint texs[NUM_PRINTABLE_CHARS];
} font_t;

void font_view( float w, float h );
void font_term( font_t *font );
void font_load( font_t *font, const char name[] );
void font_draw( const font_t *font, int32 x, int32 y, float scale, color_t color, const char *text );
void font_draw_3d( const font_t *font, camera_t cam, const float3 p, float scale, color_t color, const char *text );
