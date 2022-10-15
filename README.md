# PolySoup - bounding sphere/spheroid traversal in a Polygon soup environment

 Usage:

    #include <polysoup/collision.h>

    // variables
    bpcd_grid_t grid;
    coll_geom_t geom;
    coll_geom_trace_test_t test;

    // load geometry from OBJ file
    coll_geom_load( &geom, "test.obj", 0 );

    // load BPCD (Broad Phase Collision Detection) grid
    coll_geom_load_bpcd_grid( &geom, &grid, GRID_CELL_SIZE );
    
    // initialize trace test
    coll_geom_trace_test_init( &test, &geom, &grid, COLL_GEOM_TRACE_SPHERE, 0 );

    //given two points p1 and p2, and a vector v going from p1 to p2 
    float3 p1, p2, v;

    f3make( v, p1, p2 );

    sphere_t sphere;
    f3copy( sphere.c, p1 );
    sphere.radius = RADIUS;

    //break trace down into multiple steps to avoid clipping for larger movements
    int32 steps = 8;
    f3muls( v, 1.0f / (float)steps );

    for( int32 i = 0; i < steps; i++ ){
      coll_geom_trace_test_sphere( &test, sphere ); 
      coll_geom_trace( &test, v );  
      if( test.stuck )
        break;
      f3add( sphere.c, v );
    }

    //sphere.c now contains the resolved position
