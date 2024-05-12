#pragma once

#include <polysoup/trace.h>
#include "obj.h"

extern const bpcd_grid_t *map_grid();
extern const coll_geom_t *map_geom();
extern coll_geom_trace_test_t *map_sphere_trace_test();


extern void map_term();
extern void map_load( const char name[] );
extern void map_draw();
extern vec3_t map_spawn();
