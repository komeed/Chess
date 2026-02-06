//
// Created by Omeed on 2/3/26.
//
#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "graphic_libs.h"
#include "../shader/shader.h"
#include "../utils/errorhandler.h"
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 700
#define MARGIN 50
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct Rectangle {
    float x;
    float y;
    float width;
    float height;
    bool is_ndc;
} Rectangle;

Rectangle create_rectangle_centered_at(float x, float y, float width, float height, bool is_ndc);

void draw_rectangle(Rectangle rect, size_int windowSize, ShaderBuffers shaderBuffers);

Rectangle rect_array_get(Rectangle rects[], size_t rect_size, size_t index);

void compute_ndc_for_rect(Rectangle rect, size_int windowSize, float* left, float* right, float* bottom, float* top);

typedef struct {
    Rectangle white_tiles[32];
    Color white_color;
    Rectangle black_tiles[32];
    Color black_color;
} Tiles;

Tiles create_tiles(size_int windowSize);

bool rect_contains_point(Rectangle rect, f_point point);

#endif //RECTANGLE_H
