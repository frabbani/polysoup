#include "hash.h"

#include <stdio.h>

#define HASH_POOL_SIZE ( HASH_TABLE_SIZE * 4 )

static hash_t hashpool[ HASH_POOL_SIZE ];


static hash_t *hashpool_new( size_t val ){
  static size_t index = 0;
  index = (index+1) % HASH_POOL_SIZE;
  hash_t *hash = &hashpool[index++];
  hash->val  = val;
  hash->next = NULL;
  return hash;
}

size_t hash_fnv1a( size_t v ){
  const size_t offset = 0xcbf29ce484222325;
  const size_t prime  = 0x100000001b3;

  size_t h = offset;

  uint8_t *bytes = (void *)&v;
  for( int32 i = 0; i < 8; i++ ){
    h ^= bytes[i];
    h *= prime;
  }
  return h;
}

size_t hash( size_t val, size_t maxval ){
  if( maxval < HASH_TABLE_SIZE )  //perfect hash, all possible entries can be stored
    return val % ( maxval + 1 );
  return hash_fnv1a( val ) % HASH_TABLE_SIZE;
}

void hashmap_init( hashmap_t *map, size_t maxval ){
  for( size_t i = 0; i < HASH_TABLE_SIZE; i++ ){
    map->hashes[i].next = NULL;
    map->test[i] = 0;
  }
  map->bool = 123;
  map->maxval = maxval;
}

void hashmap_reset( hashmap_t *map ){
  map->bool++;
}

//NOTE: ignore dups
static int32 hashmap_add_with_given_hash( hashmap_t *map, size_t val, size_t index ){
  if( map->test[index] != map->bool ){
    map->hashes[index].val = val;
    map->hashes[index].next = NULL;
    map->test  [index] = map->bool;
    return 0;
  }


  hash_t *hash = &map->hashes[index];
  int32   dup  = 0;

  while( hash ){
    if( hash->val == val ){
      dup++;
      break;
    }
    hash = hash->next;
  }
  if( dup )
    return 0;

  //collision
  hash->next = hashpool_new( val );
  return 1;
}


//NOTE: does not check for dups
int32 hashmap_add( hashmap_t *map, size_t val ){
  size_t i = hash( val, map->maxval );
  if( map->test[i] != map->bool ){
    map->hashes[i].val = val;
    map->hashes[i].next = NULL;
    map->test  [i] = map->bool;
    return 0;
  }

  hash_t *hash = &map->hashes[i];
  hash_t *next = hash->next;

  int32   dup  = 0;
  while( next ){
    if( next->val == val ){
      dup++;
      break;
    }
    hash = next;
    next = next->next;
  }
  if( dup )
    return 0;

  //collision
  hash->next = hashpool_new( val );
  return 1;

}


int32 hashmap_remove( hashmap_t *map, size_t val ){
  size_t i = hash( val, map->maxval );
  if( map->test[i] != map->bool ) //empty
    return 0;

  //collision
  hash_t *hash = &map->hashes[i];
  hash_t *next = hash->next;

  if( hash->val == val ){
    if( !next )
      map->test[i] = map->bool - 1; //empty
    else{
      hash->next = next->next;
    }
   return 1;
  }

  while( next && next->val != val ){
    hash = next;
    next = hash->next;
  }
  if( next ){
    hash->val  = next->val;
    hash->next = next->next;
    return 1;
  }
  else
    hash->next = NULL;

  return 0;
}

int32 hashmap_has( const hashmap_t *map, size_t val ){
  size_t i = hash( val, map->maxval );
  if( map->test[i] != map->bool ) //empty
    return 0;
  if( map->hashes[i].val == val )
    return 1;

  hash_t *hash = map->hashes[i].next;
  while( hash ){
    if( hash->val == val )
      return 1;
    hash = hash->next;
  }

  return 0;
}

int32 hashmap_found( hashmap_t *map, size_t val ){
  size_t i = hash( val, map->maxval );
  if( map->test[i] != map->bool ){ //empty
    return 0;
  }
  if( map->hashes[i].val == val )
    return 1;

  hash_t *hash = map->hashes[i].next;
  while( hash ){
    if( hash->val == val )
      return 1;
    hash = hash->next;
  }

  return 0;
}
