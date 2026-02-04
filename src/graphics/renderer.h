//
// Created by Omeed on 2/3/26.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

#include "graphic_libs.h"

#define DEFAULT_WIDTH 800 // used for reference frame in order to scale correctly with window size
#define DEFAULT_HEIGHT 800

typedef struct RenderCommand {
    GLuint shaderProgram;
    GLuint VAO;
    GLuint indexCount;
    float color[3];
    float pos[2];
    float scale[2];
} RenderCommand;

typedef struct RenderQueue {
    RenderCommand* commands;
    size_t size;
    size_t capacity;
} RenderQueue;

void init_render_queue(RenderQueue* q, size_t capacity);

void push_render_queue(RenderQueue* q, RenderCommand command);

void clear_render_queue(RenderQueue* q);

void free_render_queue(RenderQueue* q);

RenderCommand* render_queue_get(RenderQueue* q, int index);

void render(RenderQueue* q);

#endif //RENDERER_H
