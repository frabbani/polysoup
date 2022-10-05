#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

#define SAFE_FREE(p)  { if( NULL != p ){ free(p); p = NULL; }   }

#define PRINT_HERE  printf( "%s:%d - here...\n", __FUNCTION__, __LINE__ );

