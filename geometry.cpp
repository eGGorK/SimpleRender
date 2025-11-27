#include <iostream>
#include "geometry.h"
#include <cassert>


//Matrix
Matrix::Matrix(int r, int c): rows(r), cols(c) , m(std::vector<std::vector<float>>(r, std::vector<float>(c, 0.f))) {}

Matrix::Matrix(const Matrix& other): rows(other.rows), cols(other.cols) , m(std::vector<std::vector<float>>(other.m)) {}

Matrix Matrix::operator= (const Matrix& other) {
    if (this == &other){
        return *this;
    }
    rows = other.rows;
    cols = other.cols;
    m = other.m;
    return *this;
}

Matrix Matrix::operator+ (const Matrix& other) {
    assert(rows == other.rows && cols == other.cols);
    Matrix res;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            res.m[i][j] = m[i][j] + other.m[i][j];
        }
    }
    return res;
}
Matrix Matrix::operator- (const Matrix& other) {
    assert(rows == other.rows && cols == other.cols);
    Matrix res;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            res.m[i][j] = m[i][j] - other.m[i][j];
        }
    }
    return res;
}
Matrix Matrix::operator* (const float c) {
    Matrix res;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            res.m[i][j] = m[i][j]*c;
        }
    }
    return res;
}

Matrix Matrix::operator*(const Matrix& other) {
    assert(cols == other.rows);
    Matrix res(rows, other.cols);
    for (int i=0; i<rows; i++) {
        for (int j=0; j<other.cols; j++) {
            res.m[i][j] = 0.f;
            for (int k=0; k<cols; k++) {
                res.m[i][j] += m[i][k]*other.m[k][j];
            }
        }
    }
    return res;
}

std::vector<float>& Matrix::operator[](const int indx) {
    assert(indx>=0 && indx<rows);
    return m[indx];
}

Matrix Matrix::transpose() {
    Matrix res(cols, rows);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; i++) {
            res[j][i] = m[i][j];
        }
    }
    return res;
}

Matrix Matrix::identity(int size) {
    Matrix E(size, size);
    for (int i = 0; i < size; i++) {
        for ( int j = 0; j < size; j++) {
            E[i][j] = (i == j ? 1.f : 0.f);
        }
    }
    return E;
}

