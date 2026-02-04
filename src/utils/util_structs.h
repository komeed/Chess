//
// Created by Omeed on 2/3/26.
//

#ifndef UTILS_H
#define UTILS_H

#include "rectangle.h"

typedef struct SizeInt {
    int width;
    int height;
} SizeInt;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct RectArray {
    Rectangle rects[];
    size_t size;
} RectArray;

inline Rectangle rect_array_get(RectArray* rectArray, size_t index) {
    if (rectArray == NULL) {
        throw_exception(NULL_POINTER, "rectArray is null!");
        return;
    }
    if (index >= rectArray->size) {
        throw_exception(INDEX_OUT_OF_BOUNDS, "index is out of bounds!");
        return;
    }
    return rectArray->rects[index];
}

inline void rect_array_set(RectArray* rectArray, size_t index, Rectangle rect) {
    if (rectArray == NULL) {
        throw_exception(NULL_POINTER, "rectArray is null!");
        return;
    }
    if (index >= rectArray->size) {
        throw_exception(INDEX_OUT_OF_BOUNDS, "index is out of bounds!");
        return;
    }
    rectArray->rects[index] = rect;
}

inline void compute_ndc_for_rect(Rectangle rect, SizeInt windowSize, float* left, float* right, float* bottom, float* top) {
    *left = rect.x;
    *right = (rect.x + rect.width);
    *bottom = rect.y;
    *top = (rect.y + rect.height);
    if (!rect.is_ndc) {
        *left   =  rect.x / windowSize.width  * 2.0f - 1.0f;
        *right  = ((rect.x + rect.width) / windowSize.width) * 2.0f - 1.0f;
        *bottom =  (float)rect.y / windowSize.height * 2.0f - 1.0f;
        *top    = ((rect.y + rect.height) / windowSize.height) * 2.0f - 1.0f;
    }
}

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
