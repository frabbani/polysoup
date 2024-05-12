#include "mygl.h"

#include "libs.h"
#include "bitmap.h"

#include <polysoup/arraytypes.h>

void mygl_clear_errors() {
  while( GL_NO_ERROR != glGetError() ){
  }
}

str64 mygl_error_str( GLenum error ) {
  str64 s = { 0 };
  switch( error){
    case GL_NO_ERROR:
      s = str64_( "GL_NO_ERROR" );
      break;
    case GL_INVALID_ENUM:
      s = str64_( "GL_INVALID_ENUM" );
      break;
    case GL_INVALID_VALUE:
      s = str64_( "GL_INVALID_VALUE" );
      break;
    case GL_INVALID_OPERATION:
      s = str64_( "GL_INVALID_OPERATION" );
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      s = str64_( "GL_INVALID_FRAMEBUFFER_OPERATION" );
      break;
    case GL_OUT_OF_MEMORY:
      s = str64_( "GL_OUT_OF_MEMORY" );
      break;
    default:
      s = str64_( "???" );
      break;
  }
  return s;
}

void mygl_print_errors( const char *info ) {
  int32 first = 1;
  while( 1 ){
    GLenum ret = glGetError();
    if( GL_NO_ERROR == ret )
      break;
    if( first )
      printf( "%s:\n", info ? info : __FUNCTION__ );
    first = 0;
    printf( " - %s (%d)\n", mygl_error_str( ret ).chars, (int32)ret );
  }
}

void mygl_setup() {
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );       // Black Background
  glClearDepth( 1.0f );                         // Depth Buffer Setup

  glEnable( GL_DEPTH_TEST );                    // Enables Depth Testing
  glDepthFunc( GL_LEQUAL );                    // The Type Of Depth Testing To Do

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable( GL_CULL_FACE );
  glCullFace( GL_BACK );
  glEnable( GL_TEXTURE_2D );
  //glShadeModel( GL_FLAT );

  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glDisable( GL_LIGHTING );
  glDisable( GL_COLOR_MATERIAL );

}

str256 mygl_renderbuffer_info( GLuint buf ) {
  str256 s;
  int32 w, h, d;

  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, buf );

  glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &w );
  glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &h );
  s = str256fmt( " - width x height: %d x %d\n", w, h );

  glGetRenderbufferParameteriv( GL_RENDERBUFFER,
                                GL_RENDERBUFFER_INTERNAL_FORMAT, &d );
  strcat( s.chars, str64fmt( " - # of bits (%XH format): ", d ).chars );

  glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, &d );
  if( d )
    strcat( s.chars, str32fmt( "r=%d ", d ).chars );

  glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_GREEN_SIZE, &d );
  if( d )
    strcat( s.chars, str32fmt( "g=%d ", d ).chars );

  glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_BLUE_SIZE, &d );
  if( d )
    strcat( s.chars, str32fmt( "b=%d ", d ).chars );

  glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_ALPHA_SIZE, &d );
  if( d )
    strcat( s.chars, str32fmt( "a=%d ", d ).chars );

  glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE, &d );
  if( d )
    strcat( s.chars, str32fmt( "d=%d ", d ).chars );

  glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE, &d );
  if( d )
    strcat( s.chars, str32fmt( "s=%d \n", d ).chars );

  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );

  return s;
}

str256 mygl_texture_info( GLuint tex ) {
  str256 s;
  char tok[16];
  int32 w, h, d, f, r, g, b, a, l, i;

  glBindTexture( GL_TEXTURE_2D, tex );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &f );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &r );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &g );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &b );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &a );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_LUMINANCE_SIZE, &l );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTENSITY_SIZE, &i );
  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH_SIZE, &d );
  glBindTexture( GL_TEXTURE_2D, 0 );

  s = str256fmt( " - width x height: %d x %d\n", w, h );
  strcat( s.chars, str64fmt( " - # of bits (%XH format): ", f ).chars );

  if( r ){
    sprintf( tok, "r=%d ", r );
    strcat( s.chars, tok );
  }
  if( g ){
    sprintf( tok, "g=%d ", g );
    strcat( s.chars, tok );
  }
  if( b ){
    sprintf( tok, "b=%d ", b );
    strcat( s.chars, tok );
  }
  if( a ){
    sprintf( tok, "a=%d ", a );
    strcat( s.chars, tok );
  }
  if( l ){
    sprintf( tok, "l=%d ", l );
    strcat( s.chars, tok );
  }
  if( i ){
    sprintf( tok, "i=%d ", i );
    strcat( s.chars, tok );
  }
  if( d ){
    sprintf( tok, "d=%d ", d );
    strcat( s.chars, tok );
  }

  return s;
}

str256 mygl_texture_2d_array_info( GLuint tex ) {
  str256 s;
  char tok[16];
  int32 w, h, d, f, r, g, b, a, l, i;

  glBindTexture( GL_TEXTURE_2D_ARRAY, tex );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &w );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &h );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &d );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_INTERNAL_FORMAT, &f );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_RED_SIZE, &r );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_GREEN_SIZE, &g );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_BLUE_SIZE, &b );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_ALPHA_SIZE, &a );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_LUMINANCE_SIZE, &l );
  glGetTexLevelParameteriv( GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_INTENSITY_SIZE, &i );
  glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

  s = str256fmt( " - width x height * depth: %d x %d x %d\n", w, h, d );
  strcat( s.chars, str64fmt( " - # of bits (%XH format): ", f ).chars );

  if( r ){
    sprintf( tok, "r=%d ", r );
    strcat( s.chars, tok );
  }
  if( g ){
    sprintf( tok, "g=%d ", g );
    strcat( s.chars, tok );
  }
  if( b ){
    sprintf( tok, "b=%d ", b );
    strcat( s.chars, tok );
  }
  if( a ){
    sprintf( tok, "a=%d ", a );
    strcat( s.chars, tok );
  }
  if( l ){
    sprintf( tok, "l=%d ", l );
    strcat( s.chars, tok );
  }
  if( i ){
    sprintf( tok, "i=%d ", i );
    strcat( s.chars, tok );
  }

  return s;
}

#define MYGL_TEXTURE_CUBEMAP_R  GL_TEXTURE_CUBE_MAP_POSITIVE_X
#define MYGL_TEXTURE_CUBEMAP_L  GL_TEXTURE_CUBE_MAP_NEGATIVE_X

#define MYGL_TEXTURE_CUBEMAP_D  GL_TEXTURE_CUBE_MAP_POSITIVE_Y
#define MYGL_TEXTURE_CUBEMAP_T  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y

#define MYGL_TEXTURE_CUBEMAP_F  GL_TEXTURE_CUBE_MAP_POSITIVE_Z
#define MYGL_TEXTURE_CUBEMAP_B  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z

str256 mygl_cube_texture_info( GLuint tex ) {
  //int base, max;
  //glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, &l );
  //glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &m );
  //printf( "   - base/max level: %i/%i\n", base, max );

  str256 s;
  char tok[16];
  int32 w, h, d, f, r, g, b, a, l, i;

  glBindTexture( GL_TEXTURE_CUBE_MAP, tex );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_WIDTH, &w );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_HEIGHT, &h );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_INTERNAL_FORMAT, &f );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_RED_SIZE, &r );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_GREEN_SIZE, &g );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_BLUE_SIZE, &b );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_ALPHA_SIZE, &a );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_LUMINANCE_SIZE, &l );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_INTENSITY_SIZE, &i );
  glGetTexLevelParameteriv( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                            GL_TEXTURE_DEPTH_SIZE, &d );
  glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );

  s = str256fmt( " - width x height: %d x %d\n", w, h );
  strcat( s.chars, str64fmt( " - # of bits (%XH format): ", f ).chars );

  if( r ){
    sprintf( tok, "r=%d ", r );
    strcat( s.chars, tok );
  }
  if( g ){
    sprintf( tok, "g=%d ", g );
    strcat( s.chars, tok );
  }
  if( b ){
    sprintf( tok, "b=%d ", b );
    strcat( s.chars, tok );
  }
  if( a ){
    sprintf( tok, "a=%d ", a );
    strcat( s.chars, tok );
  }
  if( l ){
    sprintf( tok, "l=%d ", l );
    strcat( s.chars, tok );
  }
  if( i ){
    sprintf( tok, "i=%d ", i );
    strcat( s.chars, tok );
  }
  if( d ){
    sprintf( tok, "d=%d ", d );
    strcat( s.chars, tok );
  }

  return s;
}

str256 mygl_texture1d_info( GLuint tex ) {

  //int base, max;
  //glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, &l );
  //glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &m );
  //printf( "   - base/max level: %i/%i\n", base, max );

  str256 s;
  char tok[16];
  int32 w, h, d, f, r, g, b, a, l, i;

  glBindTexture( GL_TEXTURE_1D, tex );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_WIDTH, &w );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_HEIGHT, &h );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_INTERNAL_FORMAT, &f );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_RED_SIZE, &r );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_GREEN_SIZE, &g );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_BLUE_SIZE, &b );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_ALPHA_SIZE, &a );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_LUMINANCE_SIZE, &l );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_INTENSITY_SIZE, &i );
  glGetTexLevelParameteriv( GL_TEXTURE_1D, 0, GL_TEXTURE_DEPTH_SIZE, &d );
  glBindTexture( GL_TEXTURE_1D, 0 );

  s = str256fmt( " - width x height: %d x %d\n", w, h );
  strcat( s.chars, str64fmt( " - # of bits (%XH format): ", f ).chars );

  if( r ){
    sprintf( tok, "r=%d ", r );
    strcat( s.chars, tok );
  }
  if( g ){
    sprintf( tok, "g=%d ", g );
    strcat( s.chars, tok );
  }
  if( b ){
    sprintf( tok, "b=%d ", b );
    strcat( s.chars, tok );
  }
  if( a ){
    sprintf( tok, "a=%d ", a );
    strcat( s.chars, tok );
  }
  if( l ){
    sprintf( tok, "l=%d ", l );
    strcat( s.chars, tok );
  }
  if( i ){
    sprintf( tok, "i=%d ", i );
    strcat( s.chars, tok );
  }
  if( d ){
    sprintf( tok, "d=%d ", d );
    strcat( s.chars, tok );
  }

  return s;
}

void mygl_texture_2d_wrap( int32 repeat ) {
  if( repeat ){
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  }
  else{
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  }
}

void mygl_texture_2d_array_wrap( int32 repeat ) {
  if( repeat ){
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT );
  }
  else{
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,
                     GL_CLAMP_TO_EDGE );
  }
}

GLuint mygl_texture_2d_init( uint32 unit, int32 filtered, int32 mipmapped, int32 repeat ) {
  GLuint tex = 0;
  glActiveTexture( GL_TEXTURE0 + unit );
  glGenTextures( 1, &tex );
  glBindTexture( GL_TEXTURE_2D, tex );

  if( filtered ){
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    if( mipmapped )
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
          GL_LINEAR_MIPMAP_LINEAR );
    else
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  }
  else{
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    if( mipmapped )
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
          GL_NEAREST_MIPMAP_NEAREST );
    else
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  }
  mygl_texture_2d_wrap( repeat );

  return tex;
}

GLuint mygl_texture_2d_array_init( uint32 unit, int32 filtered, int32 mipmapped, int32 repeat ) {
  GLuint tex = 0;
  glActiveTexture( GL_TEXTURE0 + unit );
  glGenTextures( 1, &tex );
  glBindTexture( GL_TEXTURE_2D_ARRAY, tex );

  if( filtered ){
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    if( mipmapped )
      glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
          GL_LINEAR_MIPMAP_LINEAR );
    else
      glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
          GL_LINEAR );
  }
  else{
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    if( mipmapped )
      glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
          GL_NEAREST_MIPMAP_NEAREST );
    else
      glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
          GL_NEAREST );
  }
  mygl_texture_2d_array_wrap( repeat );

  return tex;
}

GLenum mygl_texture_2d_alloc( uint32 unit, uint32 level, uint32 w, uint32 h, GLenum format ) {
  GLint tex = 0, f = 0;
  glActiveTexture( GL_TEXTURE0 + unit );
  glGetIntegerv( GL_TEXTURE_BINDING_2D, &tex );
  if( !tex ){
    printf( "%s:error - texture not bound to unit %u\n", __FUNCTION__, unit );
    return 0;
  }
  glTexImage2D(
      GL_TEXTURE_2D, level, format, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );

  glGetTexLevelParameteriv( GL_TEXTURE_2D, level, GL_TEXTURE_INTERNAL_FORMAT, &f );
  if( (GLenum)f != format )
    printf( "%s:warning - internal format changed from %Xh to %Xh\n", __FUNCTION__, format, f );

  return (GLenum)f;
}

GLenum mygl_texture_2d_array_alloc( uint32 unit, uint32 level, uint32 w, uint32 h, uint32 d, GLenum format ) {
  GLint tex = 0, f = 0;
  glActiveTexture( GL_TEXTURE0 + unit );
  glGetIntegerv( GL_TEXTURE_BINDING_2D_ARRAY, &tex );
  if( !tex ){
    printf( "%s:error - texture not bound to unit %u\n", __FUNCTION__, unit );
    return 0;
  }

  mygl_clear_errors();
  //glTexStorage3D( GL_TEXTURE_2D_ARRAY, levels, format, w, h, d );
  glTexImage3D(
      GL_TEXTURE_2D_ARRAY, level, format, w, h, d, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
  mygl_print_errors( "glTexImage3D" );

  glGetTexLevelParameteriv( GL_TEXTURE_BINDING_2D_ARRAY, 0,
                            GL_TEXTURE_INTERNAL_FORMAT, &f );
  if( (GLenum)f != format )
    printf( "%s:warning - internal format changed from %Xh to %Xh (%dx%d)\n", __FUNCTION__, format, f, w, h );

  return (GLenum)f;
}

int32 mygl_texture_from_image( GLuint *tex, const image_t image, int mipmapped, int bpp, int filtered ) {
  if( !image.w || !image.h || !image.pixels ){
    printf( "%s:error - failed to create texture, invalid image.\n", __FUNCTION__ );
    return 0;
  }

  int32 filtertype = filtered ? GL_LINEAR : GL_NEAREST;
  int32 texfmt = GL_RGBA8;

  switch( bpp){
    case 32:
      texfmt = GL_RGBA8;
      break;
    case 24:
      texfmt = GL_RGB8;
      break;
    case 16:
      texfmt = GL_RG8;
      break;
    case 8:
      texfmt = GL_R8;
      break;
    default:
      break;
  }

  glGenTextures( 1, tex );
  glBindTexture( GL_TEXTURE_2D, *tex );

  if( mipmapped ){
    image_mipchain_t chain;
    image_mipchain_init( &chain, image, 0 );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_LINEAR );
    for( uint32 i = 0; i < chain.levels; i++ )
      glTexImage2D( GL_TEXTURE_2D, i, texfmt, chain.mips[i].w, chain.mips[i].h, 0, GL_BGRA, GL_UNSIGNED_BYTE, chain.mips[i].pixels );

    image_mipchain_free( &chain );
  }
  else{
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtertype );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtertype );
    glTexImage2D( GL_TEXTURE_2D, 0, texfmt, image.w, image.h, 0, GL_BGRA,
                  GL_UNSIGNED_BYTE, image.pixels );
  }

  //printf( "%s\n", mygl_texture_info( *tex ).chars );

  return 1;
}

int32 mygl_texture_from_bitmap( GLuint *tex, const char *bmpfile, int32 mipmapped, int32 bpp, int32 filtered ) {
  image_t image = BMP_init_image( bmpfile );
  if( !image.pixels ){
    printf( "%s:error - failed\n", __FUNCTION__ );
    return 0;
  }
  int32 ret = mygl_texture_from_image( tex, image, mipmapped, bpp, filtered );
  image_free( &image );
  return ret;
}

int32 mygl_cube_texture_init( GLuint *tex, size_t w, size_t h, GLenum format ) {
  glGenTextures( 1, tex );
  glBindTexture( GL_TEXTURE_CUBE_MAP, *tex );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  for( int32 i = 0; i < 6; i++ )
    glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, w, h, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, NULL );

  mygl_cube_texture_info( *tex );
  return 1;
}

int32 mygl_texture_2d_array_from_images( GLuint *tex, const image_t images[], uint32 size, int32 mipmapped, int32 filtered, int32 repeat ) {
  if( !images[0].pixels || !images[0].w || !images[0].h ){
    printf( "%s - failed to create texture, invalid image(s).\n", __FUNCTION__ );
    return 0;
  }

  *tex = mygl_texture_2d_array_init( 0, filtered, mipmapped, repeat );

  if( mipmapped ){
    image_mipchain_t mipchain = { 0 };

    image_mipchain_init( &mipchain, images[0], 0 );
    for( uint32 level = 0; level < mipchain.levels; level++ ){
      image_t *mip = &mipchain.mips[level];
      mygl_texture_2d_array_alloc( 0, level, mip->w, mip->h, size, GL_RGBA );
    }

    for( uint32 layer = 0; layer < size; layer++ ){
      const image_t image_from = images[layer];

      if( layer > 0 )  //already created mipchain for layer 0
        image_mipchain_init( &mipchain, image_from, 1 );

      for( uint32 level = 0; level < mipchain.levels; level++ ){
        image_t *mip = &mipchain.mips[level];
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY, level, 0, 0, layer, mip->w, mip->h, 1,
            GL_BGRA,
            GL_UNSIGNED_BYTE, mip->pixels );
      }
      image_mipchain_free( &mipchain );
    }
  }
  else{
    mygl_texture_2d_array_alloc( 0, 0, images[0].w, images[0].h, size,
        GL_RGBA );
    for( uint32 layer = 0; layer < size; layer++ ){
      const image_t *image = &images[layer];
      glTexSubImage3D(
          GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, image->w, image->h, 1,
          GL_BGRA,
          GL_UNSIGNED_BYTE, image->pixels );
    }
  }
  return 1;
}

void mygl_load_matrix( const float4x4 f4x4 ) {
  GLfloat m[] = { f4x4[0][0], f4x4[1][0], f4x4[2][0], f4x4[3][0],

      f4x4[0][1], f4x4[1][1], f4x4[2][1], f4x4[3][1],

      f4x4[0][2], f4x4[1][2], f4x4[2][2], f4x4[3][2],

      f4x4[0][3], f4x4[1][3], f4x4[2][3], f4x4[3][3], };
  glLoadMatrixf( m );
}

void mygl_mult_matrix( const float4x4 f4x4 ) {
  GLfloat m[] = { f4x4[0][0], f4x4[1][0], f4x4[2][0], f4x4[3][0],

      f4x4[0][1], f4x4[1][1], f4x4[2][1], f4x4[3][1],

      f4x4[0][2], f4x4[1][2], f4x4[2][2], f4x4[3][2],

      f4x4[0][3], f4x4[1][3], f4x4[2][3], f4x4[3][3], };
  glMultMatrixf( m );
}

void mygl_vertex_pc( float x, float y, float z, float r, float g, float b, float a ) {
  glColor4f( r, g, b, a );
  glVertex3f( x, y, z );
}

void mygl_vertex_pt( float x, float y, float z, float s, float t ) {
  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  glTexCoord2f( s, t );
  glVertex3f( x, y, z );
}

void mygl_vertex_ptc( float x, float y, float z, float s, float t, float r, float g, float b, float a ) {
  glColor4f( r, g, b, a );
  glTexCoord2f( s, t );
  glVertex3f( x, y, z );
}

void mygl_transform( const float3 p, const float3 b0, const float3 b1, const float3 b2 ) {
  float4x4 T;
  f4x4world( T, p, b0, b1, b2 );
  glMatrixMode( GL_MODELVIEW );
  mygl_mult_matrix( T );
}

void mygl_camera( const float3 p, const float3 b0, const float3 b1, const float3 b2 ) {
  float4x4 V;
  f4x4view( V, p, b0, b1, b2 );
  glMatrixMode( GL_MODELVIEW );
  mygl_mult_matrix( V );
}

void mygl_persp( int w, int h, float fov, float min, float max ) {
  glViewport( 0, 0, w, h );
  float4x4 P;
  f4x4persp( P, (float)w / (float)h, fov * PI / 180.0f, min, max );
  glMatrixMode( GL_PROJECTION );
  mygl_load_matrix( P );
}

void mygl_ortho( float w, float h, float min, float max ) {
  // given point <x,y,z,w>, and 4 by 4 matrix mat
  //    | ...,   0,   0,   0 |   | x |   | x' |
  //    |   0,   0, ...,   0 | * | y | = | y' |
  //    |   0,   a,   0,   b |   | z |   | z' |
  //    |   0,   1,   0,   0 |   | 1 |   | w' |
  //
  // x' = x / (w/2)
  // y' = z / (h/2)
  // z' = a * y + b
  // w' = 1
  //
  // NOTES:
  // z-buffer := z'/w, or z'/y. for min/max distances, we want:
  //    i) ( a * min + b ) / min = 0
  //   ii) ( a * max + b ) / max = 1
  // solve for a & b
  //
  // screen coordinates are signed normalized, so the
  // orthographic projection maps x & y values to the -1/1 range, meaning:
  //  -1 <= x'/w <= 1 &
  //  -1 <= y'/w <= 1, where w is 1

  float a = 1.0f / max;
  float b = -min / max;

  float4x4 P;
  memset( P, 0, sizeof(float4x4) );

  P[0][0] = 2.0f / w;
  P[1][2] = 2.0f / h;
  P[2][1] = a;
  P[2][3] = b;
  P[3][3] = 1.0f;

  glMatrixMode( GL_PROJECTION );
  mygl_load_matrix( P );
}

void mygl_bind_tex_2d( GLsizei unit, GLuint tex ) {
  glActiveTexture( GL_TEXTURE0 + unit );
  glBindTexture( GL_TEXTURE_2D, tex );
}

void mygl_enable( GLenum cap, int32 enable ) {
  if( enable )
    glEnable( cap );
  else
    glDisable( cap );
}

void mygl_wireframe_tri( const float3 p0, const float3 p1, const float3 p2, float r, float g, float b, float a ) {
  mygl_vertex_pc( p0[0], p0[1], p0[2], r, g, b, a );
  mygl_vertex_pc( p1[0], p1[1], p1[2], r, g, b, a );

  mygl_vertex_pc( p1[0], p1[1], p1[2], r, g, b, a );
  mygl_vertex_pc( p2[0], p2[1], p2[2], r, g, b, a );

  mygl_vertex_pc( p2[0], p2[1], p2[2], r, g, b, a );
  mygl_vertex_pc( p0[0], p0[1], p0[2], r, g, b, a );
}

void mygl_wireframe_quad( const float3 p0, const float3 p1, const float3 p2, const float3 p3, float r, float g, float b, float a ) {
  mygl_vertex_pc( p0[0], p0[1], p0[2], r, g, b, a );
  mygl_vertex_pc( p1[0], p1[1], p1[2], r, g, b, a );

  mygl_vertex_pc( p1[0], p1[1], p1[2], r, g, b, a );
  mygl_vertex_pc( p2[0], p2[1], p2[2], r, g, b, a );

  mygl_vertex_pc( p2[0], p2[1], p2[2], r, g, b, a );
  mygl_vertex_pc( p3[0], p3[1], p3[2], r, g, b, a );

  mygl_vertex_pc( p3[0], p3[1], p3[2], r, g, b, a );
  mygl_vertex_pc( p0[0], p0[1], p0[2], r, g, b, a );
}

