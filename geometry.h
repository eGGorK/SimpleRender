#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cmath>

template<typename T>
class Vec3{
public:
    T x, y, z;
public:
    Vec3<T>() = default;
    Vec3<T>(T x, T y, T z):x(x), y(y), z(z){}
    Vec3<T>(Vec3<T>&& other) : x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)) {}
    Vec3<T>(const Vec3<T>& other): x(other.x), y(other.y), z(other.z) {}
    Vec3<T> & operator =(const Vec3<T>& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }
    Vec3<T> operator- (const Vec3<T>& other) const {
        return Vec3<T>(x - other.x, y - other.y, z - other.z);
    }
    Vec3<T> operator+ (const Vec3<T>& other) const {
        return  Vec3<T>(x + other.x, y + other.y, z + other.z);
    }
    bool operator== (const Vec3<T>& other) const {
        if ( this->x == other.x && this->y == other.y &&  this->z == other.z) {
            return true;
        }   
        return false;
    }
    Vec3<T> operator* (float f) const { 
        return Vec3<T>(x*f, y*f, z*f);
    }
    T operator* (const Vec3<T>& other) const {
        return (other.x*x + other.y*y + other.z*z);
    }
    float norm () const {
        return sqrt(x*x + y*y + z*z);
    }
    Vec3<T> normalize(T l = 1) {
        *this = (*this)* (1/norm());
        return (*this);
    }
    Vec3<T> cross_product(Vec3<T> a, Vec3<T> b) {
    return Vec3<T>(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
}
};

template<typename T>
class Vec2{
public:
    T x,y;
public:
    Vec2<T>() = default;
    Vec2<T>(T x, T y):x(x), y(y) {}
    Vec2<T>(const Vec2<T>& other): x(other.x), y(other.y) {}
    Vec2<T>(Vec2<T>&& other): x(other.x), y(other.y) {}
    Vec2<T>& operator= (const Vec2<T>& other) {
        if (this != other) {
            x = other.x;
            y = other.x;
        }
        return *this;
    }
    
    Vec2<T> operator+ (const Vec2<T>& other) const {return Vec2<T>(other.x +x , other.y + y);}
    Vec2<T> operator- (const Vec2<T>& other) const {return Vec2<T>( -other.x + x , - other.y + y);}
    T operator* (const Vec2<T>& other) const {return x*other.x + y.other*y;}
    float norm () const {return sqrt(x*x + y*y);}
    Vec2<T> normilize(T l = 1) {
        *this = (*this)*(l/norm());
        return *this;
    }

};

template<typename T> 
class Matrix2{
    T* data[2][2];
public:
    Matrix2<T>() = default;
    Matrix2<T>(const Vec2<T>& p0, const Vec2<T>& p1) {
        data[0][0] = new T(p0.x); 
        data[0][1] = new T(p0.y);
        data[1][0] = new T(p1.x);
        data[1][1] = new T(p1.y);
    }
    
};

typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
#endif