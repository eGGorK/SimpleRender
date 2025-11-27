#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector>
#include "../geometry.h"
#include "../TGAreader/tgaimage.h"

class Model;

class Face { 
public:
    std::vector<int> v;
    std::vector<int> vt;
    std::vector<int> vn;
public:
    Face() = default;
    friend Model;
};

class Model {
public:
    std::vector<Vec3f> vertices;
    std::vector<Vec3f> normals;
    std::vector<Vec3f> texturs;
    std::vector<Face> faces;
    TGAImage textur_map;
    Vec3f box_size[2];
public:
    Model(const char* filename) {
        if (filename != ""){
            parsing(filename);
        }
    }
    bool parsing(const char* filename);
    bool scaling(int widht, int height);
    bool load_textur_map(const char* mapname);
    bool resize(TGAImage& image);
    friend Face;

};

#endif