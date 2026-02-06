//
// Created by Omeed on 2/3/26.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

#include "graphic_libs.h"
#include "rectangle.h"
#include "image.h"
#include "../board.h"

typedef struct RenderCommand {
    GLuint shaderProgram;
    GLuint VAO;
    GLuint indexCount;
    Color color;

    bool hasTexture;
    GLuint texture;
} RenderCommand;

typedef struct RenderQueue {
    RenderCommand* commands;
    size_t size;
    size_t capacity;
    SizeInt windowSize;
} RenderQueue;

void init_rq(RenderQueue* q, size_t capacity, SizeInt windowSize);

void push_rq(RenderQueue* q, RenderCommand command);
void push_shape_to_rq(RenderQueue* q, float* vertices, size_t vertices_size, unsigned int* indices, size_t indices_size, Color color, GLuint shaderProgram);

void clear_rq(RenderQueue* q);

void free_rq(RenderQueue* q);

RenderCommand* get_rq(RenderQueue* q, int index);

void render(RenderQueue* q, SizeInt windowSize);

void add_rect_to_rq(RenderQueue *q, const Rectangle rect, const GLuint shaderProgram);
void add_rects_to_rq(RenderQueue *q, Rectangle rects[], size_t rect_size, Color color, unsigned int shaderProgram);

void add_tiles_to_rq(RenderQueue *q, GLuint shaderProgram);

void add_image_to_rq(RenderQueue *q, Image *image, const GLuint shaderProgram);

void add_board_to_rq(RenderQueue *q, Board *board, const GLuint shaderProgram);

SizeInt get_board_pos(RenderQueue *q, double x, double y);

#endif //RENDERER_H
