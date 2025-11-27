#ifndef _RENDER_H_
#define _RENDER_H_

#include <iostream>
#include "Model/model.h"
#include "geometry.h"
#include "TGAreader/tgaimage.h"
#include <vector>

extern Matrix ModelView;
extern Matrix ViemPort;
extern Matrix Projection;


void viemport(int x, int y, int w, int h);
void projection(float c = 0.f);
void lookat(Vec3f eye, Vec3f center, Vec3f up);

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
Vec3f cross_product(Vec3f a, Vec3f b);

struct IShader {
    //virtual ~IShader();
    virtual Vec3f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

void triangle(Vec3f* p, IShader& shader, TGAImage& image, float* zbuffer);

#endif