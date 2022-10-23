#pragma once

#include "defs.h"

#define HASH_TABLE_SIZE 256

typedef struct hash_s {
  size_t val;
  struct hash_s *next;
}hash_t;


size_t hash_fnv1a( size_t val );
size_t hash( size_t v, size_t maxval );

typedef struct hashtable_s{
  size_t maxval;
  size_t bool;
  size_t test[ HASH_TABLE_SIZE ];
  hash_t hashes[ HASH_TABLE_SIZE ];
}hashmap_t;

void   hashmap_init  ( hashmap_t *map, size_t maxval );
void   hashmap_reset ( hashmap_t *map );
int32  hashmap_add   ( hashmap_t *map, size_t val );  //returns 1 if collision occurred, ignores dups
//int32  hashmap_remove( hashmap_t *map, size_t val );
int32  hashmap_found ( const hashmap_t *map, size_t val );
float hash_collisions_rate();
