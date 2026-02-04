//
// Created by Omeed on 2/3/26.
//
#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "graphic_libs.h"
#include "../shader/shader.h"
#include "../utils/errorhandler.h"
#include <stdbool.h>

typedef struct Rectangle {
    float x;
    float y;
    float width;
    float height;
    Color color;
    bool is_ndc;
} Rectangle;

void draw_rectangle(Rectangle rect, SizeInt windowSize, ShaderBuffers shaderBuffers);

#endif //RECTANGLE_H
