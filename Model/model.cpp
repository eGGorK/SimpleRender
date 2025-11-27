#include "model.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm> 

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

bool Model::parsing(const char* filename) {
    std::ifstream in;
    in.open(filename);
    if (!in.is_open()) {
        std::cerr << "Can't open .OBJ file:" << filename << std::endl;
        in.close();
        return false;
    }
    std::string line;
    float max_x = 0;
    float max_y = 0;
    float min_x = 0;
    float min_y = 0;
    float min_z = 0;
    float max_z = 0;
    while (std::getline(in, line)) {
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), line.end());
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line == "mtllib 13927_Charging_Bull_Statue_of_Wall_Street_v1_L1.mtl") {
            continue;
        }
        std::istringstream iss(line);
        std::string command;
        int fnum = 0;
        iss >> command;
        if (command == "v") {

            Vec3f vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            if (vertex.x > max_x) max_x = vertex.x;
            if (vertex.y > max_y) max_y = vertex.y;
            if (vertex.x < min_x) min_x = vertex.x;
            if (vertex.y < min_y) min_y = vertex.y;
            if (vertex.z < min_z) min_z = vertex.z;
            if (vertex.z > max_z) max_z = vertex.z;
            vertices.push_back(vertex);
        }
        if (command == "vt") {
            Vec3f text_cord(0.0,0.0,0.0); 
            iss >> text_cord.x >> text_cord.y;
            texturs.push_back(text_cord);
        }
        if (command == "vn") {
            Vec3f normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        if (command == "f") {
            Face f;
            std::string tmp;
            while (iss >> tmp) {
                std::stringstream ss(tmp);
                std::string snum;
                int v = -1, vt = -1, vn = -1;
                if (std::getline(ss, snum, '/')) {
                    if (!snum.empty()) {
                        try {
                            v = std::stoi(snum);
                            v--; 
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Error parsing vertex index: " << snum << std::endl;
                            v = -1;
                        }
                    }
                }
                if (std::getline(ss, snum, '/')) {
                    if (!snum.empty()) {
                        try {
                            vt = std::stoi(snum);
                            vt--;
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Error parsing texture index: " << snum << std::endl;
                            vt = -1;
                        }
                    }
                }
                if (std::getline(ss, snum, '/')) {
                    if (!snum.empty()) {
                        try {
                            vn = std::stoi(snum);
                            vn--;
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Error parsing normal index: " << snum << std::endl;
                            vn = -1;
                        }
                    }
                }
                if(v != -1) f.v.push_back(v);
                if(vt != -1) f.vt.push_back(vt);
                if(vn != -1) f.vn.push_back(vn);
            }
            faces.push_back(f);
        }
    }
    box_size[0].x = (min_x);
    box_size[1].x = (max_x);
    box_size[0].y = (min_y);
    box_size[1].y = (max_y);
    box_size[0].z = (min_z);
    box_size[1].z = (max_z);
    in.close();
    TGAImage tmp(max_x - min_x, max_y - min_y, TGAImage::RGB, white);
    textur_map = tmp;
    return true;
}

bool Model::scaling(int widht, int height) {
    float box_x = box_size[1].x - box_size[0].x;
    float box_y = box_size[1].y - box_size[0].y;
    float box_z = box_size[1].z - box_size[0].z;
    float vx = (std::abs(box_size[1].x) - std::abs(box_size[0].x))/2; 
    float vy = (std::abs(box_size[1].y) - std::abs(box_size[0].y))/2;
    float vz = (std::abs(box_size[1].z) - std::abs(box_size[0].z))/2;
    float dx = 2/box_x;
    float dy = 2/box_y;
    float dz = 1;
    for (int i = 0; i < vertices.size(); i++){
        Vec3f test = vertices[i];
        if ((vertices[i].x - vx) * dx > 1 || (vertices[i].x - vx) * dx < -1 || (vertices[i].y - vy) * dy > 1 || (vertices[i].y - vy) * dy < -1 || (vertices[i].z - vz) * dz > 1 || (vertices[i].z - vz) * dz < -1 ) {
            std::cout << "why?" << std:: endl;
            test.x = ((vertices[i].x - vx) * dx);
            test.y = ((vertices[i].y - vy) * dy);
            test.z = ((vertices[i].z - vz) * dz);
            
        }
        vertices[i].x = ((vertices[i].x - vx) * dx);
        vertices[i].y = ((vertices[i].y - vy) * dy);
        vertices[i].z = ((vertices[i].z - vz) * dz);
    }
    return true;
}

bool Model::load_textur_map(const char* mapname) {
    return textur_map.read_TGA_file(mapname);
}


/*bool Model::resize(TGAImage& image) {
    Matrix2f scaling;
    Matrix2f Rx;
    Matrix2f Ry;
    Matrix2f Rz;
    image.clear();
    for (auto v = vertices.begin(); v != vertices.end(); v++) {
        for (int i =0; i < 3; i++ ) {
             v = scaling * v; 
        }
    }
}
    */