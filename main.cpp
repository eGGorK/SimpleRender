#include "TGAreader/tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line_Bresenham(int x1, int y1, int x2, int y2, TGAColor color, TGAImage& image) {
    bool steep = false;
    if (std::abs(x1 - x2) < std::abs(y1 - y2)) {
        std::swap(x1, y1);
        std::swap(x2, y2);
        steep = true;
    }
    if (x1 > x2) { 
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    int dx = x2 - x1;
    int dy = std::abs(y2 - y1);

    int error = dx / 2;
    int ystep = (y1 < y2) ? 1 : -1; 

    int y = y1;
    for (int x = x1; x <= x2; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}



int main() {
    TGAImage image(100, 100, TGAImage::RGB);
    image.set(50,50,red);
    line_Bresenham(0, 100, 100, 0, red, image);
    line_Bresenham(0, 0, 100, 100, red, image);
    image.flip_vertically();
    image.write_TGA_file("output.tga");
    return 0;
}