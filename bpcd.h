#pragma once

#include "bound.h"
#include "arraytypes.h"

struct bpcd_grid_s;

typedef struct bpcd_grid_cell_s{
  size_t index;
  union{
    struct{ size_t c, r, l; };
    size_t lrc[3];
  };
  aabb_t      aabb;
  sizearray_t indices;

}bpcd_grid_cell_t;

void bpcd_grid_cell_add( bpcd_grid_cell_t *cell, size_t index );

typedef struct bpcd_grid_s{
  //level = z axis
  //row = y axis
  //column = x axis
  union{
    struct{ size_t ncols, nrows, nlevs; };
    size_t counts[3];
  };
  struct{
    struct{
      struct{
        size_t cell;
      } *cols;
    }*rows;
  }*levs;

  aabb_t aabb;
  float  cellsize;

  array_t cells;
}bpcd_grid_t;


void bpcd_grid_term( bpcd_grid_t *grid );
void bpcd_grid_init( bpcd_grid_t *grid, const float3 min, const float3 max, float cellsize, int32 term );
bpcd_grid_cell_t  *bpcd_grid_add_cell( bpcd_grid_t *grid, size_t l, size_t r, size_t c );
const bpcd_grid_cell_t *bpcd_grid_get_cell( const bpcd_grid_t *grid, size_t l, size_t r, size_t c );  //will be stale if array resizes
void bpcd_grid_aabb_for_cell( const bpcd_grid_t *grid, size_t l, size_t r, size_t c, aabb_t *aabb );

typedef struct bpcd_grid_sector_s{
  const bpcd_grid_t *grid;
  size_t ls[2], rs[2], cs[2];
}bpcd_grid_sector_t;

/*
typedef struct bpcd_grid_sector_iter_s{
  bpcd_grid_sector_t sector;
  size_t r, l, c;
  bpcd_grid_cell_t *cell;
}bpcd_grid_sector_iter_t;
*/

typedef struct bpcd_grid_sector_const_iter_s{
  bpcd_grid_sector_t sector;
  size_t r, l, c;
  const bpcd_grid_cell_t *cell;
}bpcd_grid_sector_const_iter_t;

bpcd_grid_sector_t bpcd_grid_sector_make( const bpcd_grid_t *grid, float3 p, float3 halfsize );
//bpcd_grid_sector_iter_t bpcd_grid_sector_iter_init( bpcd_grid_sector_t *sector );
//bpcd_grid_sector_const_iter_t bpcd_grid_sector_const_iter_init( bpcd_grid_sector_t sector );
int32 bpcd_grid_sector_iterate_const( bpcd_grid_sector_const_iter_t *kit, int32 notempty );
