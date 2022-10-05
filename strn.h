#pragma once

#define STRNDEF( len )    \
 typedef struct str##len##_s{  \
  char  chars[len]; \
} str##len##_t; \
typedef str##len##_t str##len;  \
  \
  \
extern str##len str##len##_( const char *str );  \
extern str##len str##len##cat( str##len s, const char *str );  \
extern str##len str##len##fmt( const char* format, ... ); \

#define STRNFUNC( len )    \
\
str##len str##len##_( const char *str ){ \
  str##len s;  \
  memset( &s, 0, sizeof(str##len) ); \
  strncpy( s.chars, str, sizeof(s.chars)-1 ); \
  return s;  \
} \
\
str##len str##len##cat( str##len s, const char *str ){ \
  strncat( s.chars, str, sizeof(s.chars)-1 ); \
  s.chars[ sizeof(s.chars) - 1 ] = '\0'; \
  return s;  \
} \
\
str##len str##len##fmt( const char* format, ... ){  \
  str##len s;  \
  memset( &s, 0, sizeof(str##len) ); \
  va_list args; \
  va_start( args, format ); \
  vsprintf( s.chars, format, args );  \
  va_end( args ); \
  return s; \
} \


STRNDEF(32)
STRNDEF(64)
STRNDEF(80)
STRNDEF(128)
STRNDEF(256)
STRNDEF(1024)


