#pragma once

#include "defs.h"

size_t string_split ( char str[],  const char delims[], char *toks[], size_t tokc );
size_t string_split2( char str[],  const char delims[], char *toks[], size_t tokc, char openclose );
size_t string_split3( char str[],  const char delims[], char *toks[], size_t tokc, char open, char close );

