//
// Created by Omeed on 2/3/26.
//

#include "renderer.h"

#include <string.h>
#include <tgmath.h>
#include <secure/_string.h>

#include "../utils/errorhandler.h"

#define SAFE_FREE(p) do { free(p); p = NULL; } while(0)

void init_rq(RenderQueue* q, size_t capacity, size_int window_size, ShaderBuffers shaders) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    q->window_size = window_size;

    SAFE_FREE(q->commands);  // SAFE_FREE should handle NULL

    q->commands = malloc(sizeof(RenderCommand) * capacity);
    if (!q->commands) {
        throw_exception(MEMORY_ALLOC_ERROR,
                        "Failed to allocate memory for render queue!");
        return;
    }

    q->size = 0;
    q->capacity = capacity;
    q->new_box_len = MIN(window_size.width, window_size.height);

    q->rect_sp = shaders.rectShaderProgram;
    q->image_sp = shaders.imageShaderProgram;
}

void push_rq(RenderQueue* q, RenderCommand command) {
    if (q->capacity == 0) {
        throw_exception(MEMORY_ALLOC_ERROR, "empty capacity!");
        return;
    }
    if (!q->commands) {
        throw_exception(MEMORY_ALLOC_ERROR, "commands is null!");
        return;
    }
    if (q->size >= q->capacity) { // if capacity exceeded
        q->capacity *= 2;
        RenderCommand* temp = realloc(q->commands, sizeof(RenderCommand) * q->capacity);
        if (!temp) {
            throw_exception(MEMORY_ALLOC_ERROR, "Failed to allocate memory for render queue!");
            return; // original q->commands is still valid
        }
        q->commands = temp; // only assign if realloc succeeded
    }
    q->commands[q->size] = command;
    q->size++;
}

void push_shape_to_rq(RenderQueue* q, float* vertices, size_t vertices_size, unsigned int* indices,
    size_t indices_size, Color color, bool is_switch) {
    unsigned int VAO, VBO, EBO;
    if (!glGenVertexArrays) {
        fprintf(stderr, "glGenVertexArrays is NULL!\n");
        exit(1);
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_size, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_size, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    RenderCommand command = {.shader_program = q->rect_sp, .VAO = VAO, .ind_count = indices_size,
    .color = color, .is_switch = is_switch};
    push_rq(q, command);
}

RenderCommand* get_rq(RenderQueue* q, int index) {
    if (!q) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return NULL;
    }
    if (index >= q->size) {
        throw_exception(INDEX_OUT_OF_BOUNDS, "index out of bounds!");
        return NULL;
    }
    return &q->commands[index];
}

void clear_rq(RenderQueue* q) {
    q->size = 0;
}

void free_rq(RenderQueue* q) {
    SAFE_FREE(q->commands); // safe even if q->commands is NULL
    q->size = 0;
    q->capacity = 0;
}

static void draw_chess_piece(RenderQueue *q, Piece* p, board_point bp, f_point scale) {
    float y = (bp.row + 0.07f) / 4.0f;
    float x = (bp.col - 0.07f) / 4.0f;
    //printf("path: %s for x: %.2f, y: %.2f", print_piece_path(cmd->piece->type), x, y);

    glUseProgram(q->image_sp);
    glActiveTexture(GL_TEXTURE0);
    if (p->type - 1 < 0 || p->type - 1 >= 6) {
        throw_exception(INDEX_OUT_OF_BOUNDS, "index out of bounds!");
        return;
    }
    glBindTexture(GL_TEXTURE_2D, q->board->piece_texts[p->type - 1 + p->side * 6]);

    glUniform1i(glGetUniformLocation(q->image_sp, "uTexture"), 0);

    glUniform2f(glGetUniformLocation(q->image_sp, "uPos"),
        x, y);

    glUniform2f(glGetUniformLocation(q->image_sp, "uScale"),
        scale.x, scale.y);

    glBindVertexArray(q->board->piece_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void render_board(RenderQueue *q, GLFWwindow* window, f_point scale) {
    if (!q) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    if (!q->board) {
        throw_exception(NULL_POINTER, "Board is NULL");
        return;
    }
    if (!window) {
        throw_exception(NULL_POINTER, "Window is NULL");
        return;
    }
    for (int8_t i = 0; i < 8; i++) {
        for (int8_t j = 0; j < 8; j++) {
            Piece* piece = &q->board->board[i][j];
            if (piece->type != Blank && piece != q->board->held_piece) {
                draw_chess_piece(q, piece, (board_point) {i, j}, scale);
            }
        }
    }
    Piece* piece = q->board->held_piece;
    if (piece) { // draw the held one last
        if (piece->type != Blank) {
            board_point bp;
            double xd, yd;
            glfwGetCursorPos(window, &xd, &yd);
            get_board_pos(q, xd, yd, &bp);
            draw_chess_piece(q, piece, bp, scale);
        }
    }
}

//assumes that all commands already initialized with vao
void render(RenderQueue* q, GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    if (!window) {
        throw_exception(NULL_POINTER, "window is NULL");
        return;
    }
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    size_int window_size = {fbWidth / 2, fbHeight / 2};
    q->window_size = window_size;
    if (!q->commands) {
        throw_exception(NULL_POINTER, "commands is null!");
        return;
    }
    const float newBoxLen = MIN(window_size.width, window_size.height);
    const float scaleX = (newBoxLen/(float)window_size.width);
    const float scaleY = (newBoxLen/(float)window_size.height);
    //float scaleX = 1;
    //float scaleY = 1;
    q->new_box_len = newBoxLen;
    int vertexScaleLocation = glGetUniformLocation(q->rect_sp, "uScale");
    int fragColorLocation = glGetUniformLocation(q->rect_sp, "uColor");
    int vertexPosLocation = glGetUniformLocation(q->rect_sp, "uPos");
    for (int i = 0; i < q->size; i++) {
        RenderCommand* cmd = get_rq(q, i);
        if (!cmd) {
            throw_exception(NULL_POINTER, "command is NULL");
            return;
        }

        glUseProgram(cmd->shader_program);
        glUniform3f(fragColorLocation, cmd->color.r/255.0f, cmd->color.g/255.0f, cmd->color.b/255.0f);
        glUniform2f(vertexScaleLocation, scaleX, scaleY);
        if (cmd->is_switch == true) {
            GLfloat offset = 0;
            if (!q->board->is_bottom_turn) {
                offset = 1.0f;
            }
            glUniform2f(vertexPosLocation, 0.0f, offset);
        }
        else {
            glUniform2f(vertexPosLocation, 0.0f, 0);
        }
        // Bind the vertex array
        glBindVertexArray(cmd->VAO);
        // Draw elements
        glDrawElements(GL_TRIANGLES, cmd->ind_count, GL_UNSIGNED_INT, 0);
    }
    render_board(q, window, (f_point) {scaleX, scaleY});
}

//based on default 800x800 frame (scale appropriately
void add_rect_to_rq(RenderQueue *q, const Rectangle rect, Color color, bool is_switch) {
    // Convert from pixel coordinates to Normalized Device Coordinates (-1 to 1)
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    float left, right, bottom, top;
    compute_ndc_for_rect(rect, q->window_size, &left, &right, &bottom, &top);

    printf("Rect vertices: %f %f %f %f %f %f %f %f\n", left,bottom,right,bottom,right,top,left,top);

    float vertices[] = {
        left,  bottom,  // bottom-left
        right, bottom,  // bottom-right
        right, top,     // top-righta
        left,  top      // top-left
    };

    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };
    push_shape_to_rq(q, vertices, 8, indices, 6, color, is_switch);

    // Draw rectangle
   // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
/* // don't delete these because we're reusing
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);*/
}

void add_rects_to_rq(RenderQueue *q, Rectangle rects[], size_t rect_size, Color color) {
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }

    size_t vertices_size = 8 * rect_size;
    size_t indices_size = 6 * rect_size;
    float* vertices = malloc(sizeof(float) * vertices_size); // 8 vertices for each
    unsigned int* indices = malloc(sizeof(unsigned int) * indices_size); // 6 indices (2 triangles)

    size_t vertices_index = 0;
    size_t indices_index = 0;

    for (int i = 0; i < rect_size; i++) {
        Rectangle rect = rects[i];
        //Rectangle rect = rect_array_get(rects, rect_size, i);
        //compute ndc
        float left, right, bottom, top;
        compute_ndc_for_rect(rect, q->window_size, &left, &right, &bottom, &top);
        //copy vertices
        if (vertices_index + 8 > vertices_size) {
            throw_exception(INDEX_OUT_OF_BOUNDS, "vertices index out of bounds!");
            return;
        }
        memcpy(&vertices[vertices_index],
       (float[]){ left, bottom, right, bottom, right, top, left, top },
       8 * sizeof(float));

        vertices_index += 8;
        //copy indices
        if (indices_index + 6 > indices_size) {
            throw_exception(INDEX_OUT_OF_BOUNDS, "indices index out of bounds!");
        }
        unsigned int vert_offset = i * 4;
        memcpy(&indices[indices_index],
               (unsigned int[]){ vert_offset, vert_offset+1, vert_offset+2, vert_offset+2, vert_offset+3, vert_offset },
               6 * sizeof(unsigned int));

        indices_index += 6;
    }
    push_shape_to_rq(q, vertices, vertices_size, indices, indices_size, color, false);
    // after we're done, free eveything
    SAFE_FREE(vertices);
    SAFE_FREE(indices);
}

void add_tiles_to_rq(RenderQueue *q) {
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }

    Tiles tiles = create_tiles(q->window_size);

    size_t tiles_size = 32; // size of each tileset

    add_rects_to_rq(q, tiles.white_tiles, tiles_size, (Color) {255, 255, 255});
    add_rects_to_rq(q, tiles.black_tiles, tiles_size, (Color) {0, 0, 0});
}

void add_image_to_rq(RenderQueue *q, Image *image, Piece *piece) {
    if (!q) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    if (!image) {
        throw_exception(NULL_POINTER, "Image is NULL");
        return;
    }
    if (!image->data) {
        throw_exception(NULL_POINTER, "Image data is NULL");
        return;
    }
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGBA;   // change to GL_RGB if 3 channels
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,         // how GPU stores it
        image->orig_width,
        image->orig_height,
        0,
        format,         // how CPU data is laid out
        GL_UNSIGNED_BYTE,
        image->data     // your image bytes
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    //generate vertices and indices array (given the image is a square)
    float left, right, bottom, top;
    compute_ndc_for_rect(image->rect, q->window_size, &left, &right, &bottom, &top);
    float vertices[] = {
        left,  bottom, 0.0f, 1.0f,
right, bottom, 1.0f, 1.0f,
right, top,    1.0f, 0.0f,
left,  top,    0.0f, 0.0f

    };
    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };
    //generate vao
    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
/*
    //push to render queue
    RenderCommand command = {.shader_program = shaderProgram, .VAO = VAO, .ind_count = 6, .color = (Color) {255, 255, 255},
    .has_tex = true, .texture = texture, .piece = (piece ? piece : NULL)};
    push_rq(q, command);*/
    if (piece) {
        if (piece->type - 1 < 0 || piece->type - 1 >= 6) {
            throw_exception(INDEX_OUT_OF_BOUNDS, "piece type out of bounds!");
            return;
        }
        q->board->piece_texts[piece->type - 1 + piece->side * 6] = texture;
        q->board->piece_VAO = VAO;
    }
    // else, not needed yet bcs no other images
}

void add_board_to_rq(RenderQueue *q, Board *board) {
    if (!q) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    q->board = board;
    float dx = q->window_size.width/8;
    float dy = q->window_size.height/8;
    for (int8_t i = 0; i < 8; i++) {
        for (int8_t j = 0; j < 8; j++) {
            Piece* piece = &board->board[i][j];
            if (!piece) {
                throw_exception(NULL_POINTER, "Piece is NULL");
                return;
            }
            if (piece->type != Blank) {
                char* path = print_piece_path(piece->type, (piece->side == 0) ? board->bottom_color : board->top_color);
                if (path[0] == '\0') {
                    throw_exception(NULL_POINTER, "piece is invalid");
                    return;
                }
                Image image = create_image(path,
                    (Rectangle) {0, 0, dx, dy, false});
                add_image_to_rq(q, &image, piece);
            }
        }
    }
    Rectangle switch_rect = {-MARGIN, 0, MARGIN, q->window_size.height/2, false};
    add_rect_to_rq(q, switch_rect, (Color) {0, 255, 0}, true);
}

bool get_board_pos(RenderQueue *q, double x, double y, board_point *bp) {
    //y is 0 at top and max height at bottom, x is 0 left max width at right
    if (!q) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return false;
    }
    //first flip y (for some reason)
    size_int window_size = q->window_size;
    y = window_size.height - y;
    const float start_x = (q->window_size.width - q->new_box_len)/2.0f;
    const float start_y = (q->window_size.height - q->new_box_len)/2.0f;
    if (!rect_contains_point((Rectangle) {start_x, start_y, q->new_box_len, q->new_box_len, false}, (f_point) {x, y})) {
       // fprintf(stderr, "oops, mouse clicked outside of board! at x: %f, y: %f\n", x, y);
        q->board->held_piece = NULL;
        return false;
    }
    const float dl = q->new_box_len/8.0f;
    bp->col = floor((x-start_x)/dl);
    bp->row = floor((y-start_y)/dl);
    //printf("mouse clicked at row: %d, col: %d\n", *row, *col);
    return true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        RenderQueue* queue = glfwGetWindowUserPointer(window);
        if (!queue ) {
            throw_exception(NULL_POINTER, "queue is NULL!");
            return;
        }
        if (!queue->board) {
            throw_exception(NULL_POINTER, "board is NULL!");
            return;
        }
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        board_point bp;
        if (get_board_pos(queue, xpos, ypos, &bp) == true) {
            Piece* piece = &queue->board->board[bp.row][bp.col];
            queue->board->held_piece = piece;
            queue->board->last_click_loc = bp;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        RenderQueue* queue = glfwGetWindowUserPointer(window);
        if (!queue ) {
            throw_exception(NULL_POINTER, "queue is NULL!");
            return;
        }
        if (!queue->board) {
            throw_exception(NULL_POINTER, "board is NULL!");
            return;
        }
        Piece* held_piece = queue->board->held_piece;
        if (held_piece) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            board_point bp;
            if (get_board_pos(queue, xpos, ypos, &bp)) {
                board_point new_loc = bp;
                board_point last_click_loc = queue->board->last_click_loc;
                move_piece_on_board(queue->board, last_click_loc, new_loc);
            }
        }
    }
}