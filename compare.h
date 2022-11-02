#pragma once

#include "defs.h"

void add_compare_func( const char* name, comparator_t compare_func );
comparator_t get_compare_func( const char *name );
