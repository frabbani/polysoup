#include "obj.h"

#include "libs.h"
#include <polysoup/utils.h>
#include <polysoup/strn.h>
#include <polysoup/floatn.h>

ARRAYFUNC( vec3, vec3_t );
ARRAYFUNC( vec2, vec2_t );

vec3_t vec3( const float3 f3 ) {
  vec3_t v;
  v.x = f3[0];
  v.y = f3[1];
  v.z = f3[2];
  return v;
}

vec2_t vec2( const float2 f2 ) {
  vec2_t v;
  v.x = f2[0];
  v.y = f2[1];
  return v;
}

void WAVE_obj_term( WAVE_obj_t *obj ) {
  vec3array_term( &obj->vps );
  vec3array_term( &obj->vns );
  vec2array_term( &obj->vts );
  array_term( &obj->fs );
}

int32 WAVE_obj_load( WAVE_obj_t *obj, const char objname[], float scale, int32 term ) {
  if( term )
    WAVE_obj_term( obj );
  else
    memset( obj, 0, sizeof(WAVE_obj_t) );

  if( scale <= 0.0f )
    scale = 1.0f;

  FILE *fp = NULL;
  char fn[256], line[256], *toks[24];

  obj->vps = vec3array_make( 1024 );
  obj->vts = vec2array_make( 1024 );
  obj->vns = vec3array_make( 1024 );
  obj->fs = array_make( sizeof(WAVE_obj_face_t), 1024 );

  str80_t s = str80fmt( "%s.obj", objname );
  fp = fopen( s.chars, "r" );
  if( !fp ){
    printf( "%s - error: invalid OBJ file '%s'\n", __FUNCTION__, fn );
    return 0;
  }

  size_t pos = -1;
  while( 1 ){
    memset( line, 0, sizeof(line) );
    char tag[8];
    if( NULL == fgets( line, sizeof(line), fp ) )
      break;

    memcpy( tag, line, sizeof(tag) );
    if( 0 == string_split( line, " \n", toks, 24 ) )
      continue;

    if( 'v' == tag[0] && ' ' == tag[1] ){
      vec3_t p;
      p.x = atof( toks[1] );
      p.y = atof( toks[2] );
      p.z = atof( toks[3] );
      f3muls( p.xyz, scale );
      vec3array_add( &obj->vps, p );
    }
    else if( 'v' == tag[0] && 'n' == tag[1] ){
      vec3_t n;
      n.x = atof( toks[1] );
      n.y = atof( toks[2] );
      n.z = atof( toks[3] );
      f3norm( n.xyz );
      vec3array_add( &obj->vns, n );
    }
    else if( 'v' == tag[0] && 't' == tag[1] ){
      vec2_t t;
      t.x = atof( toks[1] );
      t.y = atof( toks[2] );
      vec2array_add( &obj->vts, t );
    }
    else if( 'f' == tag[0] && ' ' == tag[1] ){
      WAVE_obj_face_t *face = array_add( &obj->fs, NULL, &pos );

      for( size_t i = 0; i < 3; i++ ){
        char *toktoks[3];
        string_split( toks[i + 1], "/", toktoks, 3 );
        face->vs[i] = (size_t)atoi( toktoks[0] ) - 1;
        face->ts[i] = (size_t)atoi( toktoks[1] ) - 1;
        face->ns[i] = (size_t)atoi( toktoks[2] ) - 1;
      }
    }
  }
  fclose( fp );
  fp = NULL;

  printf( "%s - OBJ '%s' loaded: %zu verts, %zu normals, %zu texcoords, %zu faces\n", __FUNCTION__, s.chars, obj->vps.size, obj->vns.size, obj->vts.size, obj->fs.size );
  return 1;
}
