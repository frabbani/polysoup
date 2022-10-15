#include "bpcd.h"

#include "libs.h"


void bpcd_grid_cell_add( bpcd_grid_cell_t *cell, size_t index ){
  for( size_t i = 0; i < cell->indices.size; i++ )
    if( cell->indices.elems[i] == index )
      return;
  sizearray_add( &cell->indices, index );
}

void bpcd_grid_term( bpcd_grid_t *grid ){
  if( grid->levs ){
    for( size_t l = 0; l < grid->nlevs; l++ )
      if( grid->levs[l].rows ){
        for( size_t r = 0; r < grid->nrows; r++ )
          if( grid->levs[l].rows[r].cols )
            free( grid->levs[l].rows[r].cols );
        free( grid->levs[l].rows );
      }
    free( grid->levs );
  }
  for( size_t i = 0; i < grid->cells.size; i++ ){
    bpcd_grid_cell_t *cell = array_data( &grid->cells, i );
    sizearray_term( &cell->indices );
  }
  array_term( &grid->cells );
  grid->nlevs = grid->nrows = grid->ncols = 0;
  grid->cellsize = 0.0f;
}


void bpcd_grid_init( bpcd_grid_t *grid, const float3 min, const float3 max, float cellsize, int32 term ){
  if( term )
    bpcd_grid_term( grid );
  else
    memset( grid, 0, sizeof(bpcd_grid_t) );

  grid->cellsize = cellsize < 0.1f ? 0.1f : cellsize;
  aabb_set( &grid->aabb, min, max, cellsize * 0.5f, 1 );

  grid->nlevs = (int32)ceilf( ( grid->aabb.max[2] - grid->aabb.min[2] ) / grid->cellsize );
  grid->nrows = (int32)ceilf( ( grid->aabb.max[1] - grid->aabb.min[1] ) / grid->cellsize );
  grid->ncols = (int32)ceilf( ( grid->aabb.max[0] - grid->aabb.min[0] ) / grid->cellsize );

  __typeof__(grid->levs[0]) lev;
  __typeof__(lev.rows[0])   row;
  __typeof__(row.cols[0])   col;
  grid->levs = malloc( grid->nlevs * sizeof(lev) );
  for( size_t l = 0; l < grid->nlevs; l++ ){
    grid->levs[l].rows = malloc( grid->nrows * sizeof(row) );
    for( size_t r = 0; r < grid->nrows; r++ ){
      grid->levs[l].rows[r].cols = malloc( grid->ncols * sizeof(col) );
      for( size_t c = 0; c < grid->ncols; c++ )
        grid->levs[l].rows[r].cols[c].cell = -1;
    }
  }

  grid->cells = array_make( sizeof(bpcd_grid_cell_t), 256 );
  grid->cells._typeof = "bpcd_grid_cell_t";

}

bpcd_grid_cell_t *bpcd_grid_add_cell( bpcd_grid_t *grid, size_t l, size_t r, size_t c ){
  if( l >= grid->nlevs || r >= grid->nrows || c >= grid->ncols )
    NULL;
  if( grid->levs[l].rows[r].cols[c].cell != -1 ){
    return array_data( &grid->cells, grid->levs[l].rows[r].cols[c].cell );
  }

  size_t i;
  bpcd_grid_cell_t *cell = array_add( &grid->cells, NULL, &i );
  cell->index = i;
  cell->l = l;
  cell->r = r;
  cell->c = c;

  float3 min, max;
  size_t is[] = { c, r, l };
  for( size_t i = 0; i < 3; i++ ){
    min[i] = grid->aabb.min[i] + is[i] * grid->cellsize;
    max[i] = min[i] + grid->cellsize;
  }

  aabb_set( &cell->aabb, min, max, 0.0f, 1 );
  cell->indices = sizearray_make( 16 );

  grid->levs[l].rows[r].cols[c].cell = cell->index;
  return cell;
}

bpcd_grid_sector_t bpcd_grid_sector_make( const bpcd_grid_t *grid, float3 p, float3 halfsize ){
  float3 min, max;
  for( size_t i = 0; i < 3; i++ ){
    min[i] = floorf( ( p[i] - halfsize[i] - grid->aabb.min[i] ) / grid->cellsize );
    max[i] = floorf( ( p[i] + halfsize[i] - grid->aabb.min[i] ) / grid->cellsize );
  }
  CLAMP( min[2], 0.0f, (float)(grid->nlevs-1) );
  CLAMP( min[1], 0.0f, (float)(grid->nrows-1) );
  CLAMP( min[0], 0.0f, (float)(grid->ncols-1) );

  CLAMP( max[2], 0.0f, (float)(grid->nlevs-1) );
  CLAMP( max[1], 0.0f, (float)(grid->nrows-1) );
  CLAMP( max[0], 0.0f, (float)(grid->ncols-1) );

  bpcd_grid_sector_t sector;
  sector.grid = grid;


  sector.ls[0] = (size_t)min[2];
  sector.ls[1] = (size_t)max[2];

  sector.rs[0] = (size_t)min[1];
  sector.rs[1] = (size_t)max[1];

  sector.cs[0] = (size_t)min[0];
  sector.cs[1] = (size_t)max[0];
  return sector;
}

const bpcd_grid_cell_t *bpcd_grid_get_cell( const bpcd_grid_t *grid, size_t l, size_t r, size_t c ){
  if( l >= grid->nlevs || r >= grid->nrows || c >= grid->ncols )
    NULL;
  if( grid->levs[l].rows[r].cols[c].cell != -1 ){
    return array_data_ro( &grid->cells, grid->levs[l].rows[r].cols[c].cell );
  }
  return NULL;
}

void bpcd_grid_aabb_for_cell( const bpcd_grid_t *grid, size_t l, size_t r, size_t c, aabb_t *aabb ){
  if( l >= grid->nlevs || r >= grid->nrows || c >= grid->ncols )
    return;

  size_t cellno = grid->levs[l].rows[r].cols[c].cell;
  if( cellno != -1 ){
    const bpcd_grid_cell_t *cell = array_data_ro( &grid->cells, cellno );
    *aabb = cell->aabb;
    return;
  }

  float3 min, max;
  size_t is[] = { c, r, l };
  for( size_t i = 0; i < 3; i++ ){
    min[i] = grid->aabb.min[i] + is[i] * grid->cellsize;
    max[i] = min[i] + grid->cellsize;
  }
  aabb_set( aabb, min, max, 0.0f, 1 );  //need to calculate points for SAT test
}

bpcd_grid_sector_const_iter_t bpcd_grid_sector_const_iter_init( bpcd_grid_sector_t sector ){
  bpcd_grid_sector_const_iter_t kit;
  kit.sector = sector;
  kit.l = -1;
  kit.r = -1;
  kit.c = -1;
  kit.cell = NULL;
  return kit;
}

int32 bpcd_grid_sector_iterate_const( bpcd_grid_sector_const_iter_t *kit, int32 notempty ){
  if( kit->l == -1 ){
    kit->l = kit->sector.ls[0];
    kit->r = kit->sector.rs[0];
    kit->c = kit->sector.cs[0];
    if( !notempty ){
      size_t no = kit->sector.grid->levs[kit->l].rows[kit->r].cols[kit->c].cell;
      if( no != -1 )
        kit->cell = array_data_ro( &kit->sector.grid->cells, no );
      return 1;
    }

    for( ; kit->l <= kit->sector.ls[1]; kit->l++ ){
      for( ; kit->r <= kit->sector.rs[1]; kit->r++ ){
        for( ; kit->c <= kit->sector.cs[1]; kit->c++ ){
          size_t no = kit->sector.grid->levs[kit->l].rows[kit->r].cols[kit->c].cell;
          if( no != -1 ){
            kit->cell = array_data_ro( &kit->sector.grid->cells, no );
            return 1;
          }
        }
      }
    }

    return 0;
  }

  if( kit->l > kit->sector.ls[1] )
    return 0;

  if( !notempty ){
    kit->c++;
    if( kit->c > kit->sector.cs[1] ){
      kit->c = 0;
      kit->r++;
      if( kit->r > kit->sector.rs[1] ){
        kit->r = 0;
        kit->l++;
      }
      if( kit->l > kit->sector.ls[1] )
        return 0;
    }
    size_t no = kit->sector.grid->levs[kit->l].rows[kit->r].cols[kit->c].cell;
    kit->cell = array_data_ro( &kit->sector.grid->cells, no );
    return 1;
  }

  do{
    kit->c++;
    if( kit->c > kit->sector.cs[1] ){
      kit->c = 0;
      kit->r++;
      if( kit->r > kit->sector.rs[1] ){
        kit->r = 0;
        kit->l++;
      }
      if( kit->l > kit->sector.ls[1] )
        return 0;
    }
    size_t no = kit->sector.grid->levs[kit->l].rows[kit->r].cols[kit->c].cell;
    kit->cell = array_data_ro( &kit->sector.grid->cells, no );
  }while( kit->cell == NULL );

  return kit->cell != NULL;
}

/*
void bpcd_grid_init( bpcd_grid_t *grid, const collfacearray_t *faces ){
  array_const_iter_t kit = array_const_iter_init( &faces->array );
  float3 min, max;
  f3copy( min, faces->elems[0].ps[0] );
  f3copy( max, faces->elems[0].ps[0] );
  for( size_t i = 0; i < faces->size; i++ ){
    coll_face_t *face = &faces->elems[i];
    for( size_t j = 0; j < 3; j++ ){
      min[i] = face->ps[i] < min[i] ? face->ps[i] : min[i];
      max[i] = face->ps[i] > max[i] ? face->ps[i] : min[i];
    }
  }
  aabb_set( &grid->aabb, min, max, 0.0f, 1 );

  for( size_t i = 0; i < faces->size; i++ ){
    coll_face_t *face = &faces->elems[i];
    aabb_t aabb;
    aabb_fit( &aabb, face->ps, 0.0f, 0 );

    f3copy( min, aabb.min );
    f3sub ( min, grid->aabb.min );
    f3muls( min, 1.0f / grid->cellsize );

    f3copy( max, aabb.max );
    f3sub ( max, grid->aabb.min );
    f3muls( max, 1.0f / grid->cellsize );

    int32 ls[2], rs[2], cs[2];
    ls[0] = floorf( min[2] );
    ls[1] = ceilf ( max[2] );
    CLAMP( ls[0], 0, grid->nlevs-1 );
    CLAMP( ls[1], 0, grid->nlevs-1 );

    rs[0] = floorf( min[1] );
    rs[1] = ceilf ( max[1] );
    CLAMP( rs[0], 0, grid->nrows-1 );
    CLAMP( rs[1], 0, grid->nrows-1 );

    cs[0] = floorf( min[0] );
    cs[1] = ceilf ( max[0] );
    CLAMP( cs[0], 0, grid->ncols-1 );
    CLAMP( cs[1], 0, grid->ncols-1 );

    for( size_t l = ls[0]; l <= ls[1]; l++ ){
      for( size_t r = rs[0]; r <= rs[1]; r++ ){
        for( size_t c = cs[0]; c <= cs[1]; c++ ){
          if( grid->levs[l].rows[r].cols[c].cell){
            bpcd_grid_cell_add_face( grid->levs[l].rows[r].cols[c].cell, face );
          }
          else{

          }
          min[2] = grid->aabb.min[2] + l * grid->cellsize;
          max[2] = min[2] + grid->cellsize;

          min[1] = grid->aabb.min[1] + r * grid->cellsize;
          max[1] = min[1] + grid->cellsize;

          min[0] = grid->aabb.min[0] + c * grid->cellsize;
          max[0] = min[0] + grid->cellsize;

          aabb_make( &aabb, min, max, 0.0f, 0 );
          if( coll_face_aabb_hit( face, &aabb ) )
            continue;
        }
      }
    }

}
*/
