#pragma once

#include <stddef.h>
#include <stdint.h>

#include <polysoup/defs.h>

#pragma pack(push,1)
typedef struct color_24bit_s {
  union {
    struct {
      uint8 r, g, b;
    };
    uint8 rgb[3];
  };
} color_24bit_t;
#pragma pack(pop)

typedef struct color_32bit_s {
  union {
    struct {
      uint8 r, g, b, a;
    };
    uint8 rgba[4];
    uint32 value;
  };
} color_32bit_t;

typedef color_32bit_t color_t;

