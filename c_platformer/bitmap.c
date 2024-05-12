#include "bitmap.h"

#include "libs.h"

void BMP_write( const color_t *pixels, uint32 w, uint32 h, const char name[] ) {
  char file[128];

  strcpy( file, name );
  {
    char *p = file;
    for( ; *p != '\0'; p++ ){
    }
    p--;
    if( p[0] != 'p' || p[-1] != 'm' || p[-2] != 'b' || p[-3] != '.' )
      strcat( file, ".bmp" );
  }

  FILE *fp = fopen( file, "wb" );

  BMP_file_magic_t magic;
  magic.num0 = 'B';
  magic.num1 = 'M';
  fwrite( &magic, 2, 1, fp ); //hard coding 2 bytes, (our structure isn't packed).

  BMP_file_header_t fileheader;
  fileheader.filesize = w * h * sizeof(int) + 54;

  fileheader.creators[0] = 0;
  fileheader.creators[1] = 0;
  fileheader.dataoffset = 54;
  fwrite( &fileheader, sizeof(BMP_file_header_t), 1, fp );

  BMP_dib_header_t dibheader;
  dibheader.headersize = 40;
  dibheader.width = (int32)w;
  dibheader.height = (int32)h;
  dibheader.numplanes = 1;
  dibheader.bitsperpixel = 24;
  dibheader.compression = BI_RGB;
  dibheader.datasize = 0;
  dibheader.hpixelsper = dibheader.vpixelsper = 1000;
  dibheader.numpalcolors = dibheader.numimpcolors = 0;
  fwrite( &dibheader, sizeof(BMP_dib_header_t), 1, fp );

  uint32 rem = 0;
  if( (w * 3) & 0x03 )
    rem = 4 - ((w * 3) & 0x03);

  for( uint32 y = 0; y < h; y++ ){
    for( uint32 x = 0; x < w; x++ ){
      fputc( pixels[y * w + x].b, fp );
      fputc( pixels[y * w + x].g, fp );
      fputc( pixels[y * w + x].r, fp );
    }
    for( uint32 i = 0; i < rem; i++ )
      fputc( 0xff, fp );
  }

  fclose( fp );
}

void BMP_save_image( const image_t image, const char bmpfile[] ) {
  BMP_write( image.pixels, image.w, image.h, bmpfile );
}

image_t BMP_init_image( const char bmpfile[] ) {
  image_t image = { 0, 0, NULL };

  if( NULL == bmpfile ){
    printf( "%s:error - invalid bitmap file\n", __FUNCTION__ );
    return image;
  }

  int x, y, i, rem;

  FILE *fp = fopen( bmpfile, "rb" );
  if( NULL == fp ){
    printf( "%s:error - couldn't open file \'%s'\n", __FUNCTION__, bmpfile );
    return image;
  }

  BMP_file_magic_t magic;
  fread( &magic, sizeof(magic), 1, fp );
  if( magic.num0 != 'B' || magic.num1 != 'M' ){
    printf( "%s:error - file '%s' is not a bitmap file\n'", __FUNCTION__, bmpfile );
    fclose( fp );
    return image;
  }
  BMP_file_header_t fileheader;
  fread( &fileheader, sizeof(fileheader), 1, fp );

  BMP_dib_header_t dibheader;
  fread( &dibheader, sizeof(dibheader), 1, fp );
  if( BI_RGB != dibheader.compression && BI_BITFIELDS != dibheader.compression ){
    printf( "%s:error - bitmap '%s' is compressed and not supported\n'", __FUNCTION__, bmpfile );
    fclose( fp );
    return image;
  }

  if( 8 != dibheader.bitsperpixel && 24 != dibheader.bitsperpixel && 32 != dibheader.bitsperpixel ){
    printf( "%s:error - bitmap '%s' bits per pixel not supported\n", __FUNCTION__, bmpfile );
    fclose( fp );
    return image;
  }
  //printf( "size of header: %d\n", dibheader.headersize );

  BMP_dib_header_v3_t dibheaderv3;
  memcpy( &dibheaderv3, &dibheader, sizeof(BMP_dib_header_t) );

  dibheaderv3.redmask = dibheaderv3.greenmask = dibheaderv3.bluemask = dibheaderv3.alphamask = 0;
  if( dibheader.headersize >= 52 ){
    fread( &dibheaderv3.redmask, sizeof(int), 1, fp );
    fread( &dibheaderv3.greenmask, sizeof(int), 1, fp );
    fread( &dibheaderv3.bluemask, sizeof(int), 1, fp );
  }
  if( dibheader.headersize >= 56 )
    fread( &dibheaderv3.alphamask, sizeof(int), 1, fp );

  //printf( "R mask: %p\n", (void *)dibheaderv3.redmask );
  //printf( "G mask: %p\n", (void *)dibheaderv3.greenmask );
  //printf( "B mask: %p\n", (void *)dibheaderv3.bluemask );
  //printf( "A mask: %p\n", (void *)dibheaderv3.alphamask );

  if( !dibheader.width || !dibheader.height ){
    printf( "%s:error - bitmap '%s' invalid dimensions\n", __FUNCTION__, bmpfile );
    fclose( fp );
    return image;
  }

  image.w = dibheader.width;
  image.h = abs( dibheader.height );
  image.pixels = malloc( image.w * image.h * sizeof(color_t) );

  //fseek( fp, fileheader.dataoffset, SEEK_SET );
  //for( i = 0; i < 16; i++ )
  //  printf( "%.2x", fgetc(fp) );
  //printf( "\n" );
  //fseek( fp, fileheader.dataoffset, SEEK_SET );

  if( 8 == dibheader.bitsperpixel ){
    int32 numcols = 0;
    uint32 *pal = NULL;

    if( 0 == dibheader.numpalcolors )
      numcols = 1 << 8;
    else
      numcols = dibheader.numpalcolors;

    //printf( "number of colors in palette: %i\n", 1<<8 );
    //printf( "number of bytes per palette color: %i\n",
    //    (fileheader.dataoffset - 54)/numcols
    //    );

    pal = malloc( numcols * sizeof(int) );
    fread( pal, sizeof(int), numcols, fp );

    rem = 0;
    if( image.w & 0x03 ){
      rem = 4 - (image.w & 0x03);
    }

    fseek( fp, fileheader.dataoffset, SEEK_SET );
    for( y = 0; y < image.h; y++ ){
      for( x = 0; x < image.w; x++ )
        image.pixels[y * image.w + x].value = pal[(int32)fgetc( fp )];
      for( i = 0; i < rem; i++ )
        fgetc( fp );
    }
    free( pal );
  }
  else{
    int bypp = dibheader.bitsperpixel / 8;
    rem = 0;
    if( (image.w * bypp) & 0x03 ){
      rem = 4 - ((image.w * bypp) & 0x03);
    }

    for( y = 0; y < image.h; y++ ){
      for( x = 0; x < image.w; x++ ){
        uint32 v, c;
        color_t pixel;
        pixel.value = 0xFFFFFFFF;
        if( BI_BITFIELDS == dibheader.compression ){
          fread( &v, sizeof(v), 1, fp );

          c = v & dibheaderv3.redmask;
          switch( dibheaderv3.redmask){
            case 0XFF000000:
              pixel.r = c >> 24;
              break;
            case 0X00FF0000:
              pixel.r = c >> 16;
              break;
            case 0X0000FF00:
              pixel.r = c >> 8;
              break;
            case 0X000000FF:
              pixel.r = c;
              break;
            default:
              break;
          }
          c = v & dibheaderv3.greenmask;
          switch( dibheaderv3.greenmask){
            case 0XFF000000:
              pixel.g = c >> 24;
              break;
            case 0X00FF0000:
              pixel.g = c >> 16;
              break;
            case 0X0000FF00:
              pixel.g = c >> 8;
              break;
            case 0X000000FF:
              pixel.g = c;
              break;
            default:
              break;
          }
          c = v & dibheaderv3.bluemask;
          switch( dibheaderv3.bluemask){
            case 0XFF000000:
              pixel.b = c >> 24;
              break;
            case 0X00FF0000:
              pixel.b = c >> 16;
              break;
            case 0X0000FF00:
              pixel.b = c >> 8;
              break;
            case 0X000000FF:
              pixel.b = c;
              break;
            default:
              break;
          }

          c = v & dibheaderv3.alphamask;
          switch( dibheaderv3.alphamask){
            case 0XFF000000:
              pixel.a = c >> 24;
              break;
            case 0X00FF0000:
              pixel.a = c >> 16;
              break;
            case 0X0000FF00:
              pixel.a = c >> 8;
              break;
            case 0X000000FF:
              pixel.a = c;
              break;
            default:
              break;
          }
        }
        else{
          pixel.b = fgetc( fp );
          pixel.g = fgetc( fp );
          pixel.r = fgetc( fp );
          pixel.a = 4 == bypp ? fgetc( fp ) : 0;
        }

        image.pixels[y * image.w + x] = pixel;
      }

      for( i = 0; i < rem; i++ )
        fgetc( fp );
    }
  }

  fclose( fp );

  printf( "%s - bitmap created from file '%s' (%d x %d x %d)\n", __FUNCTION__, bmpfile, dibheader.width, dibheader.height, dibheader.bitsperpixel );

  return image;
}
