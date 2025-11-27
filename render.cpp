#include <iostream>
#include "Model/model.h"
#include "geometry.h"
#include "TGAreader/tgaimage.h"
#include <algorithm>
#include <random>
#include "render.h"

Matrix ModelView;
Matrix Projection;
Matrix ViemPort;

void viemport(int x, int y, int w, int h) {
    ViemPort = Matrix::identity(4);
    ViemPort[0][3] = x+w/2.f;
    ViemPort[1][3] = y+h/2.f;
    ViemPort[2][3] = 255.f/2.f;

    ViemPort[0][0] = w/2.f;
    ViemPort[1][1] = h/2.f;
    ViemPort[2][2] = 255.f/2.f;
}

void projection(float c) {
    Projection = Matrix::identity(4);
    Projection[3][2] = c;
}

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye - center).normalize();
    Vec3f x = cross(up,z).normalize();
    Vec3f y = cross(z,x).normalize();
    ModelView = Matrix::identity(4);
    for (int i = 0; i < 3; i++) {
        ModelView[0][i] = x[i];
        ModelView[1][i] = y[i];
        ModelView[2][i] = z[i];
        ModelView[i][3] = -center[i];
    }
}

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f v0 = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x);
    Vec3f v1 = Vec3f(B.y - A.y, C.y - A.y, A.y - P.y);
    Vec3f crss = cross(v0, v1);
    if (std::abs(crss.z) < 1)
        return Vec3f(-1, 1, 1);
    Vec3f result = Vec3f(1.f - (crss.x + crss.y) / crss.z, crss.x / crss.z, crss.y / crss.z);
    return result;
}

void triangle(Vec3f* p, IShader& shader, TGAImage& image, float* zbuffer) {
    int width = image._width();
    int height = image._height();
    int minX = std::max(0, (int)std::floor(std::min({p[0].x, p[1].x, p[2].x})));
    int minY = std::max(0, (int)std::floor(std::min({p[0].y, p[1].y, p[2].y})));
    int maxX = std::min(width - 1, (int)std::ceil(std::max({p[0].x, p[1].x, p[2].x})));
    int maxY = std::min(height - 1, (int)std::ceil(std::max({p[0].y, p[1].y, p[2].y})));
    
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            Vec2f p0 = proj<2>(p[0]);
            Vec3f bar = barycentric(proj<2>(p[0]), proj<2>(p[1]), proj<2>(p[2]), Vec2f(x, y));
            if (bar.x < 0 || bar.y < 0 || bar.z < 0) continue;
            
            float z = p[0].z * bar.x + p[1].z * bar.y + p[2].z * bar.z;
            int index = x + y * width;
            
            if (z >= zbuffer[index]) {
                zbuffer[index] = z;
                //float u = t[0].x*bar.x + t[1].x*bar.y + t[2].x*bar.z;
                //float v = (t[0].y*bar.x + t[1].y*bar.y + t[2].y*bar.z);
                //float light = ( inty[0] *bar.x + inty[1] *bar.y + inty[2] *bar.z);
                //TGAColor color = model.textur_map.get(u,v);
                TGAColor color(255,255,255,255);
                shader.fragment(bar, color);
                image.set(x, y, color);
            }
        }
    }
}