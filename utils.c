#include "utils.h"


size_t string_split( char str[],  const char delims[], char *toks[], size_t tokc ){

  for( size_t i = 0; i < tokc; i++ )
    toks[i] = NULL;

  uint8 tab[256];
  for( size_t i = 0; i < 256; i++ )
    tab[i] = (uint8)i;

  for( size_t i = 0; delims[i] != '\0'; i++ )
    tab[ (uint8)delims[i] ] = (uint8)'\0';

  uint8 *ptr = (uint8 *)(void *)str;
  size_t len = 0;
  size_t n   = 0;
  for( size_t i = 0; str[i] != '\0'; i++, len++ )
    ptr[i] = tab[ ptr[i] ];


  for( size_t i = 0; i < len; i++ )
    if( str[i] != '\0' )
      if( 0 == i || '\0' == str[i-1] ){
        toks[n++] = &str[i];
        if( n >= tokc )
          break;
      }

  return n;
}

size_t string_split2( char str[],  const char delims[], char *toks[], size_t tokc, char openclose ){

  for( size_t i = 0; i < tokc; i++ )
    toks[i] = NULL;

  uint8 tab[256];
  for( size_t i = 0; i < 256; i++ )
    tab[i] = (uint8)i;

  for( size_t i = 0; delims[i] != '\0'; i++ )
    tab[ (uint8)delims[i] ] = (uint8)'\0';

  uint8 *ptr = (uint8 *)(void *)str;
  size_t len = 0;
  size_t n   = 0;
  int32  ignore = 0;
  for( size_t i = 0; str[i] != '\0'; i++, len++ ){
    if( openclose == str[i] )
      ignore = !ignore;
    else{
      if( !ignore )
        ptr[i] = tab[ ptr[i] ];
    }
  }


  for( size_t i = 0; i < len; i++ )
    if( str[i] != '\0' )
      if( 0 == i || '\0' == str[i-1] ){
        toks[n++] = &str[i];
        if( n >= tokc )
          break;
      }

  return n;
}


size_t string_split3( char str[],  const char delims[], char *toks[], size_t tokc, char open, char close ){

  for( size_t i = 0; i < tokc; i++ )
    toks[i] = NULL;

  uint8 tab[256];
  for( size_t i = 0; i < 256; i++ )
    tab[i] = (uint8)i;

  for( size_t i = 0; delims[i] != '\0'; i++ )
    tab[ (uint8)delims[i] ] = (uint8)'\0';

  uint8 *ptr = (uint8 *)(void *)str;
  size_t len = 0;
  size_t n   = 0;
  int32  ignore = 0;
  for( size_t i = 0; str[i] != '\0'; i++, len++ ){
    if( open  == str[i] )
      ignore++;
    else
    if( close == str[i] )
      ignore--;
    else{
      if( !ignore )
        ptr[i] = tab[ ptr[i] ];
    }
  }


  for( size_t i = 0; i < len; i++ )
    if( str[i] != '\0' )
      if( 0 == i || '\0' == str[i-1] ){
        toks[n++] = &str[i];
        if( n >= tokc )
          break;
      }

  return n;

}


