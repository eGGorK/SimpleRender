#ifndef __IMAGE_H_
#define __IMAGE_H_

#include <fstream>

#pragma pack(push,1)
struct TGA_Header {
	char idlength;
	char colormaptype;
	char datatypecode;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char  bitsperpixel;
	char  imagedescriptor;
};
#pragma pack(pop)

struct TGAColor {
    union 
    {
        struct 
        {
            unsigned char b, g, r, a;
        };
        unsigned char raw[4];
        unsigned int val;
    };
    int bytespp;
    
    TGAColor();

    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A);

    TGAColor(int v, int bpp);

    TGAColor(const unsigned char* praw, int bpp);

    TGAColor(const TGAColor& other);

    TGAColor& operator =(const TGAColor& other);
};

class TGAImage {
protected:
    unsigned char* data;
    int width;
    int height;
    int bytespp;

    bool load_RLE_data(std::ifstream& in);
    bool unload_RLE_data(std::ofstream& out);
public:
    enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4};
    TGAImage();
    TGAImage(int w, int h, int bpp);
    TGAImage(const TGAImage& other);
    TGAImage& operator =(const TGAImage &other);
    ~TGAImage();
    TGAColor get(int x, int y);
    bool set (int x, int y, TGAColor p);
    bool swap (TGAColor p1, TGAColor p2);
    bool read_TGA_file(const char* fname);
    bool write_TGA_file(const char* fname, bool rle = true);
    bool flip_vertically();
    bool flip_horizontally();
    void clear();
};

#endif