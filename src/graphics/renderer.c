//
// Created by Omeed on 2/3/26.
//

#include "renderer.h"

#include <string.h>
#include <secure/_string.h>

#include "../utils/errorhandler.h"

#define SAFE_FREE(p) do { free(p); p = NULL; } while(0)

void init_rq(RenderQueue* q, size_t capacity, SizeInt windowSize) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    q->windowSize = windowSize;

    SAFE_FREE(q->commands);  // SAFE_FREE should handle NULL

    q->commands = malloc(sizeof(RenderCommand) * capacity);
    if (!q->commands) {
        throw_exception(MEMORY_ALLOC_ERROR,
                        "Failed to allocate memory for render queue!");
        return;
    }

    q->size = 0;
    q->capacity = capacity;
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

void push_shape_to_rq(RenderQueue* q, float* vertices, size_t vertices_size, unsigned int* indices, size_t indices_size, Color color, GLuint shaderProgram) {
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

    RenderCommand command = {.shaderProgram = shaderProgram, .VAO = VAO, .indexCount = indices_size,
    .color = color, .hasTexture = false};
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

//assumes that all commands already initialized with vao
void render(RenderQueue* q, SizeInt windowSize) {
    glClear(GL_COLOR_BUFFER_BIT);
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    q->windowSize = windowSize;
    if (!q->commands) {
        throw_exception(NULL_POINTER, "commands is null!");
        return;
    }
    const float newBoxLen = MIN(windowSize.width, windowSize.height);
    const float scaleX = (newBoxLen/(float)windowSize.width);
    const float scaleY = (newBoxLen/(float)windowSize.height);
    for (int i = 0; i < q->size; i++) {
        RenderCommand* cmd = get_rq(q, i);
        if (!cmd) {
            throw_exception(NULL_POINTER, "command is NULL");
            return;
        }
        if (cmd->hasTexture) { // if image
            glUseProgram(cmd->shaderProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cmd->texture);

            glUniform1i(glGetUniformLocation(cmd->shaderProgram, "uTexture"), 0);

            glUniform2f(glGetUniformLocation(cmd->shaderProgram, "uScale"),
                scaleX, scaleY);

            glBindVertexArray(cmd->VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        } else {
            int vertexScaleLocation = glGetUniformLocation(cmd->shaderProgram, "uScale");
            int fragColorLocation   = glGetUniformLocation(cmd->shaderProgram, "uColor");

            glUseProgram(cmd->shaderProgram);
            glUniform3f(fragColorLocation, cmd->color.r/255.0f, cmd->color.g/255.0f, cmd->color.b/255.0f);
            glUniform2f(vertexScaleLocation, scaleX, scaleY);
            // Bind the vertex array
            glBindVertexArray(cmd->VAO);
            // Draw elements
            glDrawElements(GL_TRIANGLES, cmd->indexCount, GL_UNSIGNED_INT, 0);
        }
    }
}

//based on default 800x800 frame (scale appropriately
void add_rect_to_rq(RenderQueue *q, const Rectangle rect, const GLuint shaderProgram) {
    // Convert from pixel coordinates to Normalized Device Coordinates (-1 to 1)
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    float left, right, bottom, top;
    compute_ndc_for_rect(rect, q->windowSize, &left, &right, &bottom, &top);

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
    push_shape_to_rq(q, vertices, 8, indices, 6, (Color) {255, 255, 255}, shaderProgram);

    // Draw rectangle
   // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
/* // don't delete these because we're reusing
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);*/
}

void add_rects_to_rq(RenderQueue *q, Rectangle rects[], size_t rect_size, Color color, unsigned int shaderProgram) {
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
        compute_ndc_for_rect(rect, q->windowSize, &left, &right, &bottom, &top);
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
    push_shape_to_rq(q, vertices, vertices_size, indices, indices_size, color, shaderProgram);
    // after we're done, free eveything
    SAFE_FREE(vertices);
    SAFE_FREE(indices);
}

void add_tiles_to_rq(RenderQueue *q, GLuint shaderProgram) {
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }

    Tiles tiles = create_tiles(q->windowSize);

    size_t tiles_size = 32; // size of each tileset

    add_rects_to_rq(q, tiles.white_tiles, tiles_size, (Color) {255, 255, 255}, shaderProgram);
    add_rects_to_rq(q, tiles.black_tiles, tiles_size, (Color) {0, 0, 0}, shaderProgram);
}

void add_image_to_rq(RenderQueue *q, Image *image, const GLuint shaderProgram) {
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
    compute_ndc_for_rect(image->rect, q->windowSize, &left, &right, &bottom, &top);
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

    //push to render queue
    RenderCommand command = {.shaderProgram = shaderProgram, .VAO = VAO, .indexCount = 6, .color = (Color) {255, 255, 255},
    .hasTexture = true, .texture = texture};
    push_rq(q, command);
}

void add_board_to_rq(RenderQueue *q, Board *board, const GLuint shaderProgram) {
    float dx = q->windowSize.width/8;
    float dy = q->windowSize.height/8;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board->board[i][j] != Blank) {
                char* path = print_piece_path(board->board[i][j]);
                if (path[0] == '\0') {
                    throw_exception(NULL_POINTER, "piece is invalid");
                    return;
                }

                Image image = create_image(path,
                    (Rectangle) {j*dx, i*dy, dx, dy, false});
                add_image_to_rq(q, &image, shaderProgram);
            }
        }
    }
}

SizeInt get_board_pos(RenderQueue *q, double x, double y) {
    //y is 0 at top and max height at bottom, x is 0 left max width at right
    SizeInt windowSize = q->windowSize;
    printf("x: %f, y: %f", x, y);
}