#include "map.h"

#include "libs.h"
#include "mygl.h"
#include "bitmap.h"
#include <polysoup/trace.h>

#include "font.h"

static bpcd_grid_t grid;
static coll_geom_t geom;
static WAVE_obj_t obj;
static GLuint tex;
static coll_geom_trace_test_t test;
vec3_t spawn = { { { 0.0f, 0.0f, 0.0f } } };

const bpcd_grid_t* map_grid() {
  return &grid;
}
const coll_geom_t* map_geom() {
  return &geom;
}
coll_geom_trace_test_t* map_sphere_trace_test() {
  return &test;
}
vec3_t map_spawn() {
  return spawn;
}

void map_term() {
  WAVE_obj_term( &obj );
  coll_geom_term( &geom );
  bpcd_grid_term( &grid );
  coll_geom_trace_test_term( &test );
  glDeleteTextures( 1, &tex );
}

void map_load( const char name[] ) {
  image_t image = BMP_init_image( str64fmt( "assets/maps/%s/tex.bmp", name ).chars );
  mygl_texture_from_image( &tex, image, 0, 1, 1 );
  image_free( &image );

  printf( "%s - loading map '%s'\n", __FUNCTION__, name );
  WAVE_obj_load( &obj, str64fmt( "assets/maps/%s/map", name ).chars, 1.0f, 0 );
  coll_geom_load( &geom, str64fmt( "assets/maps/%s/coll.obj", name ).chars, 0 );
  coll_geom_load_bpcd_grid( &geom, &grid, 2.0f );
  printf( " * geometry extents: {%f %f %f} -> {%f %f %f}\n", geom.aabb.min[0], geom.aabb.min[1], geom.aabb.min[2], geom.aabb.max[0], geom.aabb.max[1], geom.aabb.max[2] );
  printf( " * # of grid levels/rows/columns: %zu/%zu/%zu\n", grid.nlevs, grid.nrows, grid.ncols );
  coll_geom_trace_test_init( &test, &geom, &grid, COLL_GEOM_TRACE_SPHERE, 0 );

  f3zero( spawn.xyz );
  FILE *fp = fopen( str64fmt( "assets/maps/%s/spawn.txt", name ).chars, "r" );
  if( fp ){
    char line[128];
    if( fgets( line, sizeof(line), fp ) ){
      char *tok = strtok( line, " \n" );
      spawn.x = (float)atof( tok );
      tok = strtok( NULL, " \n" );
      spawn.y = (float)atof( tok );
      tok = strtok( NULL, " \n" );
      spawn.z = (float)atof( tok );
    }
    fclose( fp );
  }
}

void map_draw() {
  {
    mygl_enable( GL_BLEND, 0 );
    mygl_bind_tex_2d( 0, tex );
    WAVE_obj_face_t *faces = obj.fs.data;
    size_t num = obj.fs.size;
    glBegin( GL_TRIANGLES );
    for( size_t i = 0; i < num; i++ ){
      vec3_t p0 = obj.vps.elems[faces[i].vs[0]];
      vec3_t p1 = obj.vps.elems[faces[i].vs[1]];
      vec3_t p2 = obj.vps.elems[faces[i].vs[2]];

      vec2_t t0 = obj.vts.elems[faces[i].ts[0]];
      vec2_t t1 = obj.vts.elems[faces[i].ts[1]];
      vec2_t t2 = obj.vts.elems[faces[i].ts[2]];

      mygl_vertex_pt( p0.x, p0.y, p0.z, t0.x, t0.y );
      mygl_vertex_pt( p1.x, p1.y, p1.z, t1.x, t1.y );
      mygl_vertex_pt( p2.x, p2.y, p2.z, t2.x, t2.y );
    }
    glEnd();
    mygl_bind_tex_2d( 0, 0 );
    mygl_enable( GL_BLEND, 1 );
  }
  /*
   draw_aabb( &grid.aabb, 1.0f, 0.0f, 1.0f, 1.0f );

   {
   for( size_t l = 0; l < grid.nlevs; l++ ){
   for( size_t r = 0; r < grid.nrows; r++ ){
   for( size_t c = 0; c < grid.ncols; c++ ){
   //aabb_t aabb;
   //bpcd_grid_aabb_for_cell( &grid, l, r, c, &aabb );
   //draw_aabb( &aabb, 0.0f, 0.0f, 1.0f );
   const bpcd_grid_cell_t *cell = bpcd_grid_get_cell( &grid, l, r, c );
   if( !cell )
   continue;
   draw_aabb( &cell->aabb, 0.0f, 0.0f, 1.0f, 0.1f );
   }
   }
   }
   }


   {

   float3 h = { actor->radius, actor->radius, actor->radius };
   //aabb_t aabb;
   //aabb_make( &aabb, actor->transform.p, h, 0.0f, 1 );
   //draw_aabb( &aabb, 0.0f, 0.0f, 1.0f );
   bpcd_grid_sector_t sector = bpcd_grid_sector_make( &grid, actor->transform.p, h );
   for( size_t l = sector.ls[0]; l <= sector.ls[1]; l++ ){
   for( size_t r = sector.rs[0]; r <= sector.rs[1]; r++ ){
   for( size_t c = sector.cs[0]; c <= sector.cs[1]; c++ ){

   const bpcd_grid_cell_t *cell = bpcd_grid_get_cell( &grid, l, r, c );
   if( !cell ){
   aabb_t aabb;
   bpcd_grid_aabb_for_cell( &grid, l, r, c, &aabb );
   draw_aabb( &aabb, 0.25f, 0.0f, 0.5f, 1.0f );
   continue;
   }
   draw_aabb( &cell->aabb, 0.0f, 1.0f, 1.0f, 1.0f );
   array_const_indirect_iter_t kiit = array_const_indirect_iter_init( &geom.faces.array,
   cell->indices.elems,
   cell->indices.size );
   glBegin( GL_LINES );
   while( array_iterate_indirect_const( &kiit ) ){
   const coll_face_t *face = kiit.data;
   mygl_wireframe_tri( face->ps[0], face->ps[1], face->ps[2], 0.0f, 1.0f, 0.0f, 1.0f );
   }
   glEnd();
   }
   }
   }
   / *
   bpcd_grid_sector_const_iter_t kit = bpcd_grid_sector_const_iter_init( sector );
   glBegin( GL_LINES );
   while( bpcd_grid_sector_iterate_const( &kit, 1 ) ){
   draw_aabb( &kit.cell->aabb, 0.0f, 0.0f, 1.0f );
   array_const_indirect_iter_t kiit = array_const_indirect_iter_init( &geom.faces.array,
   kit.cell->indices.elems,
   kit.cell->indices.size );
   while( array_iterate_indirect_const( &kiit ) ){
   const coll_face_t *face = kiit.data;
   mygl_wireframe_tri( face->ps[0], face->ps[1], face->ps[2], 0.0f, 1.0f, 0.0f );
   }
   }
   glEnd();
   * /
   }
   */
  /*
   {
   coll_face_t *faces =  geom.faces.elems;
   size_t num = geom.faces.size;
   glBegin( GL_LINES );
   for( size_t i = 0; i < num; i++ ){
   vec3_t p0 = vec3( faces[i].ps[0] );
   vec3_t p1 = vec3( faces[i].ps[1] );
   vec3_t p2 = vec3( faces[i].ps[2] );
   mygl_wireframe_tri( p0.xyz, p1.xyz, p2.xyz, 1.0f, 1.0f, 0.0f );
   }
   glEnd();
   }
   */

}
