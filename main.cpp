#include "TGAreader/tgaimage.h"
#include "Model/model.h"
#include <algorithm>
#include <iostream>
#include "geometry.h"
#include "render.h"


Model model("obj/african_head.obj");

const int width = 1024;
const int height = 1024;
const int depth = 255;


Vec3f light_dir(0.33,0.33,0.33);
Vec3f eye(3,0,3);
Vec3f up(0,1,0);
Vec3f center(0,0,0);


Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m(Vec3f v) {
    Matrix m(4,1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

struct GuroShader: public IShader {
    Vec3f intsvty;
    Vec3f verying_textr[3];
    Vec3f verying_vrtx[3];
    virtual Vec3f vertex(int jvrtx, int iface) {
        Vec4f r_vertex = expan<4>(model.vertices[model.faces[iface].v[jvrtx]]); 
        r_vertex = ViemPort*Projection*ModelView*r_vertex;
        intsvty[jvrtx]= std::max(0.f, model.normals[model.faces[iface].vn[jvrtx]]*light_dir);
        for (int i =0; i < 3; i++) {
            verying_textr[i] = model.texturs[model.faces[iface].vt[i]];
        }
        verying_vrtx[jvrtx] = proj<3>(r_vertex, r_vertex[3]); 
        return proj<3>(r_vertex, r_vertex[3]);
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec3f l = light_dir*(-1.f);
        Vec3f n = cross(verying_vrtx[2] - verying_vrtx[0], verying_vrtx[1] - verying_vrtx[0]);
        n.normalize();
        Vec3f r = cross(n, cross(n,l))* 2.f - l;
        r.normalize();
        Vec3f vv = eye.normalize()*(-1.f);
        int u = 0;
        int v = 0;
        for (int i =0; i < 3; i++) {
            u += bar[i]*verying_textr[i].x*width;
            v += bar[i]*verying_textr[i].y*height;
        }
        float diffuse =std::max(0.f, n * l);
        float specular = std::max(0.f, r*vv);
        float intensivity = intsvty * bar;
        //color = model.textur_map.get(u,v)*diffuse;
        color = color*(specular+diffuse + intensivity);
        return false;
    }
};

int main() {
    lookat(eye, center, up);
    viemport(width/8, height/8, width*3/4, height*3/4);
    projection(-1.f/(eye - center).norm());
    
    TGAImage image(width, height, TGAImage::RGB);
    
    model.textur_map.read_TGA_file("texturs/african_head_diffuse.tga");
    model.textur_map.scale(width, height);
    model.textur_map.flip_vertically();
    
    model.scaling(2,2);
    
    float* zbuffer = new float[width*height];
    for (int i = 0; i < image._width()*image._height(); i++) {
        zbuffer[i] = -10000;
    }

    GuroShader shader;
    
    for (int i = 0; i < model.faces.size(); i++) {
        Vec3f screen_coord[3];
        for (int j = 0; j < model.faces[i].v.size(); j++) {
            screen_coord[j] = shader.vertex(j,i);
        } 
        triangle(screen_coord, shader, image, zbuffer);
    }  
    image.flip_vertically();    
    image.write_TGA_file("output.tga");
    return 0;
}