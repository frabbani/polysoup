#pragma once

#include <GL\glew.h>
#include "image.h"

#include <polysoup/floatn.h>
#include <polysoup/strn.h>

extern str256 mygl_texture_info( GLuint tex );

extern int32 mygl_texture_from_image( GLuint *tex, const image_t image, int mipmapped, int bpp, int filtered );

extern void mygl_setup();

extern void mygl_load_matrix( const float4x4 f4x4 );

extern void mygl_mult_matrix( const float4x4 f4x4 );

extern void mygl_vertex_pc( float x, float y, float z, float r, float g, float b, float a );

extern void mygl_vertex_pt( float x, float y, float z, float s, float t );

extern void mygl_vertex_ptc( float x, float y, float z, float s, float t, float r, float g, float b, float a );

extern void mygl_transform( const float3 p, const float3 b0, const float3 b1, const float3 b2 );

extern void mygl_camera( const float3 p, const float3 b0, const float3 b1, const float3 b2 );

extern void mygl_persp( int w, int h, float fov, float min, float max );

extern void mygl_ortho( float w, float h, float min, float max );

extern void mygl_bind_tex_2d( GLsizei unit, GLuint tex );

extern void mygl_enable( GLenum cap, int32 enable );

extern void mygl_wireframe_tri( const float3 p0, const float3 p1, const float3 p2, float r, float g, float b, float a );

extern void mygl_wireframe_quad( const float3 p0, const float3 p1, const float3 p2, const float3 p3, float r, float g, float b, float a );

