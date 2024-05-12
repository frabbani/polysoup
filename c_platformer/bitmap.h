#pragma once

#include "defs.h"
#include "image.h"

#define BI_RGB        0
#define BI_RLE8       1
#define BI_RLE4       2
#define BI_BITFIELDS  3
#define BI_JPEG       4
#define BI_PNG        5

#pragma pack(push, 1)

typedef struct {
  uint8 num0, num1;
} BMP_file_magic_t;      //2 bytes

typedef struct {
  uint32 filesize;
  uint16 creators[2];
  uint32 dataoffset;
} BMP_file_header_t;     //12 bytes

typedef struct {
  uint32 headersize;
  int32 width, height;
  uint16 numplanes, bitsperpixel;
  uint32 compression;
  uint32 datasize;
  int32 hpixelsper, vpixelsper;  //horizontal and vertical pixels-per-meter
  uint32 numpalcolors, numimpcolors;

} BMP_dib_header_t;   //40 bytes, all windows versions since 3.0

typedef struct {
  unsigned int headersize;
  int width, height;
  unsigned short numplanes, bitsperpixel;
  unsigned int compression;
  unsigned int datasize;
  int hpixelsper, vpixelsper;
  unsigned int numpalcolors, numimpcolors;
  unsigned int redmask, greenmask, bluemask;

} BMP_dib_header_v2_t;   //52 bytes, 40 + RGB double word masks (added by adobe)

typedef struct {
  unsigned int headersize;
  int width, height;
  unsigned short numplanes, bitsperpixel;
  unsigned int compression;
  unsigned int datasize;
  int hpixelsper, vpixelsper;
  unsigned int numpalcolors, numimpcolors;
  unsigned int redmask, greenmask, bluemask, alphamask;

} BMP_dib_header_v3_t;   //56 bytes, 40 + RGBA double word masks (added by adobe)

#pragma pack(pop)

extern void BMP_write( const color_t *pixels, uint32 w, uint32 h, const char name[] );

extern image_t BMP_init_image( const char bmpfile[] );
extern void BMP_save_image( const image_t image, const char bmpfile[] );
