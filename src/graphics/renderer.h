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
#include "../piece.h"

typedef void (*ClickCallback)(int x, int y, bool pressed);

typedef struct RenderCommand {
    GLuint shader_program;
    GLuint VAO;
    GLuint ind_count;
    Color color;

  //  Rectangle rect; // stores pos and size before rendered (NORMALIZED, INCLUDES SCALING AND DISPLACEMENT)
    //ClickCallback on_click;

    bool has_tex;
    GLuint texture;
    Piece *piece;
} RenderCommand;

typedef struct RenderQueue {
    RenderCommand* commands;
    size_t size;
    size_t capacity;
    size_int window_size;
    float new_box_len;
    Board* board;

    GLuint rect_sp;
    GLuint image_sp;
} RenderQueue;

void init_rq(RenderQueue* q, size_t capacity, size_int window_size, ShaderBuffers shaders);

void push_rq(RenderQueue* q, RenderCommand command);
void push_shape_to_rq(RenderQueue* q, float* vertices, size_t vertices_size, unsigned int* indices, size_t indices_size, Color color);

void clear_rq(RenderQueue* q);

void free_rq(RenderQueue* q);

RenderCommand* get_rq(RenderQueue* q, int index);

void render(RenderQueue* q, GLFWwindow* window);

void add_rect_to_rq(RenderQueue *q, const Rectangle rect);
void add_rects_to_rq(RenderQueue *q, Rectangle rects[], size_t rect_size, Color color);

void add_tiles_to_rq(RenderQueue *q);

void add_image_to_rq(RenderQueue *q, Image *image, Piece *piece);

void add_board_to_rq(RenderQueue *q, Board *board);

void get_board_pos(RenderQueue *q, double x, double y, int8_t* row, int8_t* col);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void poll_left_click(GLFWwindow* window, RenderQueue *q);

void update_board_to_rq(RenderQueue *q);

#endif //RENDERER_H
