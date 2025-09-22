#include "model.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm> 

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
                int v = -1, vt = -1, vn = -1; // Инициализируем значения -1
                
                // Парсим vertex index
                if (std::getline(ss, snum, '/')) {
                    if (!snum.empty()) {
                        try {
                            v = std::stoi(snum);
                            v--; // Индексация начинается с 1 в .obj, а нам нужна с 0
                        } catch (const std::invalid_argument& e) {
                            // Обработка ошибки, если не удалось преобразовать в число
                            std::cerr << "Error parsing vertex index: " << snum << std::endl;
                            v = -1; // Устанавливаем значение по умолчанию
                        }
                    }
                }
                
                // Парсим texture coordinate index
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
                
                // Парсим vertex normal index
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

                // Добавляем индексы в Face, даже если некоторые отсутствуют.
                // Обработка пропущенных индексов должна быть выполнена в вызывающем коде.
                if(v != -1) f.v.push_back(v);
                if(vt != -1) f.vt.push_back(vt);
                if(vn != -1) f.vn.push_back(vn);
            }
            faces.push_back(f);
        }
    }
    box_size[0][0] = std::round(min_x);
    box_size[0][1] = std::round(max_x);
    box_size[1][0] = std::round(min_y);
    box_size[1][1] = std::round(max_y);
    in.close();
    return true;
}

bool Model::scaling(int widht, int height) {
    int box_widht = box_size[0][1] - box_size[0][0];
    int box_height = box_size[1][1] - box_size[1][0];
    float dx = static_cast<float>(widht/box_widht);
    float dy = static_cast<float>(height/box_height);
    for (int i = 0; i < vertices.size(); i++){
        vertices[i].x = (vertices[i].x - box_size[0][0]) * dx;
        vertices[i].y = (vertices[i].y - box_size[1][0]) * dy;
    }
    return true;
}

bool Model::load_textur_map(const char* mapname) {
    return textur_map.read_TGA_file(mapname);
}