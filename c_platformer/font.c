#include "font.h"
#include "libs.h"
#include "mygl.h"

static float disp_w;
static float disp_h;

void font_view( float w, float h ) {
  disp_w = w;
  disp_h = h;
}

void font_term( font_t *font ) {
  for( size_t i = 0; i < NUM_PRINTABLE_CHARS; i++ )
    image_free( &font->char_images[i] );
  glDeleteTextures( NUM_PRINTABLE_CHARS, font->texs );
  font->w = font->h = 0;
  font->name = str64_( "" );
  font->_loaded = 0;
}

void font_load( font_t *font, const char name[] ) {
  if( font->_loaded )
    font_term( font );

  str64_t fn = str64fmt( "assets/fonts/%s.bmp", name );
  image_t atlas = BMP_init_image( fn.chars );
  font->w = atlas.w;
  font->h = atlas.h / NUM_PRINTABLE_CHARS;

  size_t o = 0;
  for( size_t i = 0; i < NUM_PRINTABLE_CHARS; i++ ){
    image_t image = image_init( font->w, font->h );
    for( size_t y = 0; y < font->h; y++ ){
      for( size_t x = 0; x < font->w; x++ ){
        image.pixels[y * font->w + x] = atlas.pixels[(o + y) * atlas.w + x];
        image.pixels[y * font->w + x].a = image.pixels[y * font->w + x].r;
      }
    }
    size_t d = NUM_PRINTABLE_CHARS - i - 1;
    fn = str64fmt( "fonts/%s/%u.bmp", name, (uint32)d );
    font->char_images[d] = image;
    mygl_texture_from_image( &font->texs[d], font->char_images[d], 0, 32, 1 );
    // BMP_save_image( font->char_images[d], fn.chars );
    o += font->h;
  }
  font->name = str64_( name );
  font->_loaded = 1;

  printf( "%s\n", mygl_texture_info( font->texs[0] ).chars );
  image_free( &atlas );
}

void font_draw( const font_t *font, int32 x, int32 y, float scale, color_t color, const char *text ) {
  size_t n = 0;
  const char *c = text;

  if( !font->_loaded )
    return;

  glViewport( 0, 0, disp_w, disp_h );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();

  float r = (float)color.r / 255.0f;
  float g = (float)color.g / 255.0f;
  float b = (float)color.b / 255.0f;
  float a = (float)color.a / 255.0f;

  float w = (float)font->w * scale / disp_w;
  float h = (float)font->h * scale / disp_h;

  float xoff = (float)(2.0f * x - disp_w) / disp_w;
  float yoff = (float)(2.0f * y - disp_h) / disp_h;

  while( *c != '\0' ){
    int32 i = (int32)(*c) - (int32)' ';
    //image_t image = font->char_images[i];
    //glTexSubImage2D(
    //   GL_TEXTURE_2D, 0, 0, 0, image.w, image.h, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels );
    mygl_bind_tex_2d( 0, font->texs[i] );

    glBegin( GL_TRIANGLE_FAN );
    float xmin, xmax;
    float ymin, ymax;
    xmin = xoff;
    xmax = xoff + w;
    ymin = yoff;
    ymax = yoff + h;
    mygl_vertex_ptc( xmin, ymin, -1.0f, 0.0f, 0.0f, r, g, b, a );
    mygl_vertex_ptc( xmax, ymin, -1.0f, 1.0f, 0.0f, r, g, b, a );
    mygl_vertex_ptc( xmax, ymax, -1.0f, 1.0f, 1.0f, r, g, b, a );
    mygl_vertex_ptc( xmin, ymax, -1.0f, 0.0f, 1.0f, r, g, b, a );
    glEnd();
    xoff += 0.5f * w;
    c++;
    n++;
    if( n > 256 )
      break;
  }
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  mygl_bind_tex_2d( 0, 0 );
}

void font_draw_3d( const font_t *font, camera_t cam, const float3 p, float scale, color_t color, const char *text ) {

  if( !font->_loaded )
    return;

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();

  float4x4 P, V;
  f4x4view( V, cam.p, cam.b0, cam.b1, cam.b2 );
  f4x4persp( P, disp_w / disp_h, cam.FOV * PI / 180.0f, cam.D_n, cam.D_f );
  float4 v;
  f4set( v, p[0], p[1], p[2], 1.0f );
  f4transf( V, v );
  if( v[1] > cam.D_n ){
    f4transf( P, v );
    v[0] = v[0] / v[2] * 0.5f + 0.5f;
    v[1] = v[1] / v[2] * 0.5f + 0.5f;
    v[0] *= disp_w;
    v[1] *= disp_h;
    font_draw( font, (int32)v[0], (int32)v[1], scale, color, text );
  }

  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
}
