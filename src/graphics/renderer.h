//
// Created by Omeed on 2/3/26.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

#include "graphic_libs.h"
#include "rectangle.h"

#define BOX_LENGTH 600

typedef struct RenderCommand {
    GLuint shaderProgram;
    GLuint VAO;
    GLuint indexCount;
    Color color;
} RenderCommand;

typedef struct RenderQueue {
    RenderCommand* commands;
    size_t size;
    size_t capacity;
    SizeInt windowSize;
} RenderQueue;

void init_render_queue(RenderQueue* q, size_t capacity, SizeInt windowSize);

void push_render_queue(RenderQueue* q, RenderCommand command);

void clear_render_queue(RenderQueue* q);

void free_render_queue(RenderQueue* q);

RenderCommand* render_queue_get(RenderQueue* q, int index);

void render(RenderQueue* q, SizeInt windowSize);

void add_rect_to_queue(RenderQueue *q, const Rectangle rect, const GLuint shaderProgram);

#endif //RENDERER_H
