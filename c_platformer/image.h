#pragma once

#include "defs.h"

color_t color_( uint8 r, uint8 g, uint8 b, uint8 a );

typedef struct image_s {
  uint32 w, h;
  color_t *pixels;
} image_t;

extern void image_free( image_t *image );
extern image_t image_init( uint32 w, uint32 h );
extern image_t image_dup( const image_t image );
extern color_t image_multisample( const image_t image, uint32 x, uint32 y, uint32 w, uint32 h );
extern image_t image_nextmip( const image_t image );

typedef struct {
  uint32 levels;
  image_t mips[32];
} image_mipchain_t;

extern void image_mipchain_free( image_mipchain_t *chain );
extern int32 image_mipchain_init( image_mipchain_t *chain, const image_t image, int free_first );
