#pragma once

#include <polysoup/arraytypes.h>

typedef struct {
  union {
    struct {
      float x, y;
    };
    float2 xy;
  };
} vec2_t;

typedef struct {
  union {
    struct {
      float x, y, z;
    };
    float3 xyz;
  };
} vec3_t;

typedef struct {
  size_t vs[3];
  size_t ts[3];
  size_t ns[3];
} WAVE_obj_face_t;

ARRAYDEF( vec3, vec3_t );
ARRAYDEF( vec2, vec2_t );

#define vec3array_add( a, v )  vec3array_add0( a, v, ARRTAG )
#define vec3array_new( a, v )  vec3array_add1( a, v, ARRTAG )

#define vec2array_add( a, v )  vec2array_add0( a, v, ARRTAG )
#define vec2array_new( a, v )  vec2array_add1( a, v, ARRTAG )

extern vec3_t vec3( const float3 f3 );
extern vec2_t vec2( const float2 f2 );

typedef struct WAVE_obj_s {
  vec3array_t vps;
  vec2array_t vts;
  vec3array_t vns;
  array_t fs;
} WAVE_obj_t;

extern void WAVE_obj_term( WAVE_obj_t *obj );
extern int32 WAVE_obj_load( WAVE_obj_t *obj, const char objname[], float scale, int32 term );

