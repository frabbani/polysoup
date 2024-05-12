#pragma once

#include "defs.h"

#include <polysoup/floatn.h>

typedef struct {
  float3 p;
  union {
    struct {
      float3 r, l, u;
    };
    struct {
      float3 b0, b1, b2;
    };
    float3 bs[3];
  };
  float FOV, D_n, D_f;
} camera_t;

extern void camera_move( camera_t *cam, float right, float up, float look );
//extern void camera_eyepos( const camera_t *cam, camera_t *eye, float ipl, float dist, int left );
//extern void camera_Vmatrix_centered( const camera_t *cam, float4x4 V );
//extern void camera_Pmatrix( const camera_t *cam, float w, float h, float4x4 P );
//extern void camera_PVmatrix( const camera_t *cam, int w, int h, float4x4 PV );
