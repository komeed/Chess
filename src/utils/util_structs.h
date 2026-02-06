//
// Created by Omeed on 2/3/26.
//

#ifndef UTILS_H
#define UTILS_H

#include "errorhandler.h"

typedef struct SizeInt {
    int width;
    int height;
} size_int;

typedef struct f_point {
    float x;
    float y;
} f_point;

typedef struct i8_point {
    int8_t x;
    int8_t y;
} i8_point;

void compute_ndc_for_i8_point(i8_point point, size_int windowSize, float dl, float* x, float* y);

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

inline void float_array_set(float* array, size_t size, size_t index, float value) {
    if (array == NULL) {
        throw_exception(NULL_POINTER, "array is null!");
        return;
    }
    if (index >= size || index < 0) {
        throw_exception(INDEX_OUT_OF_BOUNDS, "index is out of bounds!");
        return;
    }
    array[index] = value;
}

#endif //UTILS_H
