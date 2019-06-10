#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <cstddef>
#include <vector>

template <typename T>
class Array2D {

    std::vector<T> data;
    size_t cols;

public:

    Array2D() : data(0), cols(0) {}

    Array2D(size_t rows, size_t cols, T init) :
        data(std::vector<T>(rows*cols, init)),
        cols(cols) {}

    inline T* operator[](size_t row) {
        return &data[row*cols];
    }

};

#endif
