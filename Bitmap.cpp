/*
 * As a note, I'm not really sure where this came from, but I did not write
 * this. This was given to me by my professor.
 */

#include "Bitmap.h"
#include <cstdio>
#include <cstdlib>

using namespace std;

/* BMP file loader loads a 24-bit bmp file only */

/*
* getint and getshort are help functions to load the bitmap byte by byte
*/
static unsigned int getint(FILE *fp) {
  int c, c1, c2, c3;

  /*  get 4 bytes */
  c = getc(fp);
  c1 = getc(fp);
  c2 = getc(fp);
  c3 = getc(fp);

  return ((unsigned int) c) +
    (((unsigned int) c1) << 8) +
    (((unsigned int) c2) << 16) +
    (((unsigned int) c3) << 24);
}

static unsigned int getshort(FILE *fp){
  int c, c1;

  /* get 2 bytes*/
  c = getc(fp);
  c1 = getc(fp);

  return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

/*  quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  */
Bitmap::Bitmap(char *filename) {
   FILE *file;
   unsigned long size;                 /*  size of the image in bytes. */
   unsigned long i;                    /*  standard counter. */
   unsigned short int planes;          /*  number of planes in image (must be 1)  */
   unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
   char temp;                          /*  used to convert bgr to rgb color. */

   if ((file = fopen(filename, "rb"))==NULL)
      throw "file not found";

   /*  seek through the bmp header, up to the width height: */
   fseek(file, 18, SEEK_CUR);
   sizeX = getint (file);
   sizeY = getint (file);

   size = sizeX * sizeY * 3; // assuming 24 bit color

   planes = getshort(file);
   if (planes != 1) throw "number of planes is not 1";

   bpp = getshort(file);
   if (bpp != 24) throw "Bpp is not 24";

   fseek(file, 24, SEEK_CUR);

   /*  read the data.  */
   data = (char *) malloc(size);
   if (data == NULL) throw "Error allocating memory for color-corrected image data";

   if ((i = fread(data, size, 1, file)) != 1) throw "Error reading image data";

   for (i=0;i<size;i+=3) { /*  reverse all of the colors. (bgr -> rgb) */
      temp = data[i];
      data[i] = data[i+2];
      data[i+2] = temp;
   }

   fclose(file); /* Close the file and release the filedes */
}

Bitmap::~Bitmap() {
}

