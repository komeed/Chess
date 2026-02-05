//
// Created by Omeed on 2/3/26.
//

#ifndef UTILS_H
#define UTILS_H

#include "errorhandler.h"

typedef struct SizeInt {
    int width;
    int height;
} SizeInt;

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
