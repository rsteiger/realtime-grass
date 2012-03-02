#ifndef BITMAP_H_
#define BITMAP_H_

/* class to store the image data*/
class Bitmap {
  public:
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
    Bitmap(char *filename);
    Bitmap(): sizeX(0), sizeY(0), data(0) { };
    ~Bitmap();
};

#endif
