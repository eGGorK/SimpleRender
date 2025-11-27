#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

//VEC

template <size_t DIM, typename T> 
struct vec {
    T data[DIM];
    vec() { for (size_t i = DIM; i--; data[i] = T()); }
    T& operator[](const size_t i) {assert(i < DIM); return data[i]; }
    const T& operator[](const size_t i ) const {assert(i < DIM); return data[i];}
};

template <typename T> 
struct vec<2,T> {
    T x, y;
    vec(): x(T()), y(T()) {}
    vec(T x, T y): x(x), y(y) {}
    T& operator[](const size_t i) {assert(i < 2); return i<=0 ? x : y; }
    const T& operator[](const size_t i) const {assert(i < 2); return i<=0 ? x : y; }
};

template <typename T> 
struct vec<3,T> {
    T x, y, z;
    vec(): x(T()), y(T()) {}
    vec(T x, T y, T z): x(x), y(y), z(z) {}
    T& operator[](const size_t i) { assert(i < 3); return i<=0 ? x : (i == 1 ? y : z); }
    const T& operator[](const size_t i) const { assert(i < 3); return i<=0 ? x : (i == 1 ? y : z); }
    float norm() { return std::sqrt(x*x+y*y+z*z); }
    vec<3,T> & normalize(T l=1) { *this = (*this)*(l/norm()); return *this; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
//operators
//////////////////////////////////////////////////////////////////////////////////////////////

template<size_t DIM, typename T> 
T operator*(const vec<DIM,T>& f, const vec<DIM,T>& s) {
    T res = T();
    for (size_t i = 0; i < DIM; i++) {
        res += f[i]*s[i];
    }
    return res;
}

template<size_t DIM, typename T> 
vec<DIM, T> operator+ (const vec<DIM,T>& f, const vec<DIM,T>& s) {
    vec<DIM, T> res;
    for (size_t i = 0; i < DIM; i++) {
        res[i] = f[i] + s[i];
    }
    return res;
}

template<size_t DIM, typename T> 
vec<DIM, T> operator- (const vec<DIM,T>& f, const vec<DIM,T>& s) {
    vec<DIM, T> res;
    for (size_t i = 0; i < DIM; i++) {
        res[i] = f[i] - s[i];
    }
    return res;
}

template<size_t DIM, typename T>
vec<DIM, T> operator* (const vec<DIM, T>& f, const T c) {
    vec<DIM, T> res;
    for (size_t i = 0; i < DIM; i++) {
        res[i] = f[i] * c;
    }
    return res;
}

template<size_t DIM, typename T>
vec<DIM, T> operator/ (const vec<DIM, T>& f, const T c) {
    vec<DIM, T> res;
    for (size_t i = 0; i < DIM; i++) {
        res[i] = f[i]/c;
    }
    return res;
}

template<size_t LEN,size_t DIM, typename T>
vec<LEN, T> proj(const vec<DIM, T>& v, T c = 1) {
    vec<LEN, T> res;
    for (size_t i = 0; i < LEN; i++) {
        res[i] = v[i]/c;
    }
    return res;
}

template<size_t LEN,size_t DIM, typename T>
vec<LEN, T> expan(const vec<DIM, T> &v, T cnst = 1) {
    vec<LEN, T> res;
    for (size_t i = 0; i < DIM; i++) {
        res[i] = v[i];
    }
    for (size_t i = DIM; i < LEN; i++) {
        res[i] = cnst;
    }
    return res;
}

template <typename T> 
vec<3,T> cross(vec<3,T> v1, vec<3,T> v2) {
    return vec<3,T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

typedef vec<2,float> Vec2f;
typedef vec<2,int> Vec2i;
typedef vec<3,float> Vec3f;
typedef vec<3,int> Vec3i;
typedef vec<4,float> Vec4f;
typedef vec<4,int> Vec4i;

const int DEFAULT_ALLOC=4;

class Matrix {
    std::vector<std::vector<float>> m;
    int rows;
    int cols;
public:
    Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC);
    Matrix(const Matrix& other);
    Matrix operator= (const Matrix& other);

    std::vector<float>& operator[](const int indx);
    Matrix operator*(const Matrix& other);
    Matrix operator*(const float c);
    Matrix operator+(const Matrix& other);
    Matrix operator-(const Matrix& other);

    Matrix transpose();
    Matrix inverse();
    float determinant();
    static Matrix identity(int size);
};

template<size_t DIM, typename T> 
vec<DIM, T> operator*(Matrix m, vec<DIM, T> v) {
    vec<DIM, T> res;
    for(size_t i = 0; i < DIM; i++) {
        T tmp = 0;
        for (size_t j = 0; j < DIM; j++) {
            tmp += v[j]*m[i][j];
        }
        res[i] = tmp;
    }
    return res;
}
#endif