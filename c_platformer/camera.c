#include "camera.h"

#include "libs.h"

#include <polysoup/floatn.h>
#include <polysoup/mymath.h>

void camera_move( camera_t *cam, float right, float up, float look ) {
  float3 r, l;

  f3copy( r, cam->b0 );
  f3muls( r, right );
  r[1] = 0;

  f3copy( l, cam->b2 );
  f3muls( l, look );
  l[1] = 0;

  f3add( cam->p, r );
  f3add( cam->p, l );
  cam->p[1] += up;
}

/*
 void camera_eyepos( const camera_t *cam, camera_t *eye, float ipl, float dist, int left )
 {
 ipl  = fabsf(ipl) * 0.5f;
 ipl  = left != 0 ? -ipl : ipl;
 dist = fabsf(dist);

 float3 r, l;

 f3copy( r, cam->b0 );
 f3muls( r, ipl );

 f3copy( l, cam->b2 );
 f3muls( l, dist ); f3sub( l, r );
 f3norm( l );

 f3copy( eye->p, cam->p );
 f3add ( eye->p, r );

 f3copy ( eye->b[1], cam->b[1] );
 f3copy ( eye->b[2], l );
 f3cross( eye->b[1], eye->b[2], eye->b[0] );

 //f3copy( eye->b[0], cam->b[0] );
 //f3copy( eye->b[1], cam->b[1] );
 //f3copy( eye->b[2], cam->b[2] );
 eye->FOV = cam->FOV;
 eye->D_f = cam->D_f;
 eye->D_n = cam->D_n;

 }

 void camera_Vmatrix( const camera_t *cam, float4x4 V ){
 f4x4view( V, cam->p, cam->b[0], cam->b[1], cam->b[2] );
 }

 //centered
 void camera_Vmatrix_centered( const camera_t *cam, float4x4 V ){
 const float3 p = { 0.0f, 0.0f, 0.0f };
 f4x4view( V, p, cam->b[0], cam->b[1], cam->b[2] );
 }


 void camera_Pmatrix( const camera_t *cam, float w, float h, float4x4 P ){
 if( cam->FOV <= 0.0f )
 f4x4ortho( P, w, h, cam->D_n, cam->D_f );
 else
 f4x4persp( P, (float)w/(float)h, cam->FOV * PI / 180.0f, cam->D_n, cam->D_f );
 }


 void camera_PVmatrix( const camera_t *cam, int w, int h, float4x4 PV ){
 float4x4 P, V;
 f4x4view( V, cam->p, cam->b[0], cam->b[1], cam->b[2] );
 if( cam->FOV <= 0.0f )
 f4x4ortho( P, w, h, cam->D_n, cam->D_f );
 else
 f4x4persp( P, (float)w/(float)h, cam->FOV * PI / 180.0f, cam->D_n, cam->D_f );
 f4x4transf( CONSTF4X4(P), CONSTF4X4(V), PV );
 }
 */

