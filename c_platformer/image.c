#include "image.h"

#include "libs.h"

color_t color_( uint8 r, uint8 g, uint8 b, uint8 a ) {
  color_t c;
  c.r = r;
  c.g = g;
  c.b = b;
  c.a = a;
  return c;
}

void image_free( image_t *image ) {
  image->w = image->h = 0;
  if( image->pixels ){
    free( image->pixels );
    image->pixels = NULL;
  }
}

image_t image_init( uint32 w, uint32 h ) {
  image_t image = { 0, 0, NULL };
  if( w == 0 || h == 0 )
    return image;
  image.w = w;
  image.h = h;
  image.pixels = calloc( image.w * image.h, sizeof(color_t) );
  return image;
}

image_t image_dupe( const image_t image ) {
  image_t image_out = { 0, 0, NULL };
  if( !image.w || !image.h || !image.pixels )
    return image_out;

  image_out.w = image.w;
  image_out.h = image.h;
  image_out.pixels = malloc( image_out.w * image_out.h * sizeof(color_t) );
  memcpy( image_out.pixels, image.pixels, image_out.w * image_out.h * sizeof(color_t) );
  return image_out;
}

color_t image_multisample( const image_t image, uint32 x, uint32 y, uint32 w, uint32 h ) {
  color_t c;
  c.value = 0;

  float count = 0;
  float r, g, b, a;

  if( x >= image.w || y >= image.h )
    return c;

  uint32 xmin = x;
  uint32 ymin = y;
  uint32 xmax = x + w - 1;
  uint32 ymax = y + h - 1;
  CLAMP( xmax, 0, image.w - 1 );
  CLAMP( ymax, 0, image.h - 1 );

  for( y = ymin; y <= ymax; y++ )
    for( x = xmin; x <= xmax; x++ ){
      int32 index = y * image.w + x;
      r += (float)image.pixels[index].r;
      g += (float)image.pixels[index].g;
      b += (float)image.pixels[index].b;
      a += (float)image.pixels[index].a;
      count += 1.0f;
    }

  r /= count;
  g /= count;
  b /= count;
  a /= count;
  CLAMP( r, 0.0f, 255.0f );
  CLAMP( g, 0.0f, 255.0f );
  CLAMP( b, 0.0f, 255.0f );
  CLAMP( a, 0.0f, 255.0f );

  c.r = (uint8)r;
  c.g = (uint8)g;
  c.b = (uint8)b;
  c.a = (uint8)a;
  return c;
}

image_t image_nextmip( const image_t image ) {
  image_t image_mip = { 0, 0, NULL };

  if( NULL == image.pixels || image.w <= 0 || image.h <= 0 ){
    printf( "%s:error - next level failed, invalid source\n", __FUNCTION__ );
    return image_mip;
  }
  if( 1 == image.w && 1 == image.h ){
    return image_mip;
  }

  if( 1 == image.w ){
    image_mip = image_init( 1, image.h / 2 );

    int32 oddh = (image.h & 0x01);
    for( int32 y = 0; y < image_mip.w; y++ ){
      int32 endy = y == (image_mip.h - 1);
      int32 h = oddh && endy ? 3 : 2;
      image_mip.pixels[y] = image_multisample( image, 0, y * 2, 1, h );
    }
    return image_mip;
  }

  if( 1 == image.h ){
    image_mip = image_init( image.w / 2, 1 );

    int32 oddw = (image.w & 0x01);
    for( int32 x = 0; x < image_mip.w; x++ ){
      int32 endx = x == (image_mip.w - 1);
      int32 w = oddw && endx ? 3 : 2;
      image_mip.pixels[x] = image_multisample( image, x * 2, 0, w, 1 );
    }
    return image_mip;
  }

  image_mip = image_init( image.w / 2, image.h / 2 );

  int32 oddw = (image.w & 0x01);
  int32 oddh = (image.h & 0x01);
  for( int32 y = 0; y < image_mip.h; y++ )
    for( int32 x = 0; x < image_mip.w; x++ ){
      int32 endx = x == (image_mip.w - 1);
      int32 endy = y == (image_mip.h - 1);
      int32 w = oddw && endx ? 3 : 2;
      int32 h = oddh && endy ? 3 : 2;
      image_mip.pixels[y * image_mip.w + x] = image_multisample( image, x * 2, y * 2, w, h );
    }

  return image_mip;
}

void image_mipchain_free( image_mipchain_t *chain ) {
  if( chain->levels ){
    for( uint32 i = 0; i < chain->levels; i++ )
      image_free( &chain->mips[i] );
    chain->levels = 0;
  }
}

int32 image_mipchain_init( image_mipchain_t *chain, const image_t image, int free_first ) {
  const uint32 maxsize = 256 * 1024;

  if( free_first )
    image_mipchain_free( chain );
  else
    memset( chain, 0, sizeof(image_mipchain_t) );

  if( !image.w || !image.h || !image.pixels ){
    printf( "%s:error - invalid source\n", __FUNCTION__ );
    return 0;
  }
  if( image.w >= maxsize || image.h >= maxsize ){
    printf( "%s:error - invalid dimensions\n", __FUNCTION__ );
    return 0;
  }

  chain->mips[chain->levels++] = image_dupe( image );
  while( 1 ){
    image_t mip = image_nextmip( chain->mips[chain->levels - 1] );
    if( !mip.pixels )
      break;
    chain->mips[chain->levels++] = mip;
  }

  return 1;
}

