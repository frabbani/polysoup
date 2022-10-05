#pragma once

#include "defs.h"

extern size_t string_split ( char str[],  const char delims[], char *toks[], size_t tokc );
extern size_t string_split2( char str[],  const char delims[], char *toks[], size_t tokc, char openclose );
extern size_t string_split3( char str[],  const char delims[], char *toks[], size_t tokc, char open, char close );

