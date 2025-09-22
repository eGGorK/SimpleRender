#include "TGAreader/tgaimage.h"
#include "Model/model.h"
#include <algorithm>
#include <random>
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

struct TGAColorInt{
    int r,g,b;
    TGAColorInt() = default;
    TGAColorInt(int x, int y, int z): r(x), g(y), b(z) {}
    TGAColorInt(float x, float y, float z): r(x), g(y), b(z) {}
    TGAColorInt(const TGAColor& other): r(other.r), g(other.g), b(other.b) {}
    TGAColorInt(TGAColorInt&& other): r(other.r), g(other.g), b(other.b) {}
    TGAColorInt(const TGAColorInt& other): r(other.r), g(other.g), b(other.b) {} 
    TGAColorInt& operator= (TGAColorInt& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }
    TGAColorInt operator+ (const TGAColorInt& other) {
        TGAColorInt res;
        res.r = r + other.r;
        res.g = g + other.g;
        res.b = b + other.b;
        return res;
    }
    TGAColorInt operator- (const TGAColorInt& other) {
        TGAColorInt res;
        res.r = r - other.r;
        res.g = g - other.g;
        res.b = b - other.b;
        return res;
    }
    TGAColorInt operator* (const float other) {
        TGAColorInt res;
        res.r = r * other;
        res.g = g * other;
        res.b = b * other;
        return res;
    }
};

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

Vec3f cross_product(Vec3f a, Vec3f b) {
    return Vec3f(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
}

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f v0 = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x);
    Vec3f v1 = Vec3f(B.y - A.y, C.y - A.y, A.y - P.y);
    Vec3f cross = cross_product(v0, v1);
    if (std::abs(cross.z) < 1)
        return Vec3f(-1, 1, 1);
    Vec3f result = Vec3f(1.f - (cross.x + cross.y) / cross.z, cross.x / cross.z, cross.y / cross.z);
    return result;
}

void triangle(Vec3f p0, Vec3f p1, Vec3f p2, TGAColor c0, TGAColor c1, TGAColor c2,  TGAImage& image, float* zbuffer) {
    if (p0 == p1 || p1 == p2 || p0 == p2) {
        return;
    }
    int width = image._width();
    int height = image._height();
    
    int minX = std::max(0, (int)std::floor(std::min({p0.x, p1.x, p2.x})));
    int minY = std::max(0, (int)std::floor(std::min({p0.y, p1.y, p2.y})));
    int maxX = std::min(width - 1, (int)std::ceil(std::max({p0.x, p1.x, p2.x})));
    int maxY = std::min(height - 1, (int)std::ceil(std::max({p0.y, p1.y, p2.y})));
    
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            Vec3f bar = barycentric(Vec2f(p0.x, p0.y), Vec2f(p1.x, p1.y), Vec2f(p2.x, p2.y), Vec2f(x, y));
            if (bar.x < 0 || bar.y < 0 || bar.z < 0) continue;
            float z = p0.z * bar.x + p1.z * bar.y + p2.z * bar.z;
            int index = x + y * width;
            if (z > zbuffer[index]) {
                zbuffer[index] = z;
                TGAColor color = TGAColor(
                    c0.r * bar.x + c1.r * bar.y + c2.r *bar.z,
                    c0.g * bar.x + c1.g * bar.y + c2.g *bar.z,
                    c0.b * bar.x + c1.b * bar.y + c2.b *bar.z,
                    255
                );
                TGAColor cl = TGAColor(
                    (c0.r +c1.r + c2.r)/3,
                    (c0.g +c1.g + c2.g)/3,
                    (c0.b +c1.b + c2.b)/3,
                    255
                );
                image.set(x, y, color);
            }
        }
    }
}

void quadrilateral(Vec3f p0, Vec3f p1, Vec3f p2, Vec3f p3, TGAColor c0,TGAColor c1,TGAColor c2,TGAColor c3, TGAImage& image, float* zbuffer) {
    triangle(p0, p1, p2, c0, c1,c2, image, zbuffer);
    triangle(p2, p3, p0, c2, c3, c0, image, zbuffer);
}

 
int main() {
    Model model("obj/Madara_Uchiha.obj");
    model.scaling(1024,1024);
    TGAImage image(1024, 1024, TGAImage::RGB);
    float* zbuffer = new float[1024*1024];
    model.load_textur_map("_Madara_texture_main_mAIN.tga");
    for (int i = 0; i < image._width()*image._height(); i++) {
        zbuffer[i] = -10000;
    }
    for (int i = 0; i < model.faces.size(); i++) {
            Vec3f p0 = model.vertices[model.faces[i].v[0]];
            Vec3f p1 = model.vertices[model.faces[i].v[1]];
            Vec3f p2 = model.vertices[model.faces[i].v[2]];
            Vec3f p3 = model.vertices[model.faces[i].v[3]];
            Vec3f n0 = model.normals[model.faces[i].vn[0]];
            Vec3f n1 = model.normals[model.faces[i].vn[1]];
            Vec3f n2 = model.normals[model.faces[i].vn[2]];
            Vec3f n3 = model.normals[model.faces[i].vn[3]];
            Vec3f norm(0, 0, 1.0);
            float intensivity0 = n0*norm;
            float intensivity1 = n1*norm;
            float intensivity2 = n2*norm;
            float intensivity3 = n3*norm;
            TGAColorInt ci0(255*intensivity0, 255*intensivity0, 255*intensivity0);
            TGAColorInt ci1(255*intensivity1, 255*intensivity1, 255*intensivity1);
            TGAColorInt ci2(255*intensivity2, 255*intensivity2, 255*intensivity2);
            TGAColorInt ci3(255*intensivity3, 255*intensivity3, 255*intensivity3);            
            TGAColor c0(ci0.r, ci0.g, ci0.b, 255);
            TGAColor c1(ci1.r, ci1.g, ci1.b, 255);
            TGAColor c2(ci2.r, ci2.g, ci2.b, 255);
            TGAColor c3(ci3.r, ci3.g, ci3.b, 255);
            
            //TGAColor c0 = model.textur_map.get(model.texturs[model.faces[i].vt[0]].x * 1024, model.texturs[model.faces[i].vt[0]].y*1024)*intensivity0;
            //TGAColor c1 = model.textur_map.get(model.texturs[model.faces[i].vt[1]].x*1024, model.texturs[model.faces[i].vt[1]].y*1024)*intensivity1;
            //TGAColor c2 = model.textur_map.get(model.texturs[model.faces[i].vt[2]].x*1024, model.texturs[model.faces[i].vt[2]].y*1024)*intensivity2;
            //TGAColor c3 = model.textur_map.get(model.texturs[model.faces[i].vt[3]].x*1024, model.texturs[model.faces[i].vt[3]].y*1024)*intensivity3;
            quadrilateral(p0, p1, p2, p3, c0,c1,c2,c3, image, zbuffer);
            //triangle(p0, p1, p2, c0, c1,c2, image, zbuffer);
    }
    image.flip_vertically();
    image.write_TGA_file("output.tga");
       model.textur_map.write_TGA_file("test.tga");
    delete zbuffer;
    return 0;
}