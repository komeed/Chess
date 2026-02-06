//
// Created by Omeed on 2/3/26.
//

#include "rectangle.h"

#include "renderer.h"

Rectangle create_rectangle_centered_at(float centerX, float centerY, float width, float height, bool is_ndc) {
    float x = centerX - width / 2;
    float y = centerY - height / 2;
    Rectangle rectangle = {x, y, width, height, is_ndc};
    return rectangle;
}

static void draw_opengl_shape(float vertices[], unsigned int indices[]);

void draw_rectangle(Rectangle rect, size_int windowSize, ShaderBuffers shaderBuffers) {
    glUseProgram(shaderBuffers.rectShaderProgram);

    float left, right, bottom, top;
    compute_ndc_for_rect(rect, windowSize, &left, &right, &bottom, &top);

    float vertices[] = {
        left,  bottom,  // bottom-left
        right, bottom,  // bottom-right
        right, top,     // top-right
        left,  top      // top-left
    };

    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };

    draw_opengl_shape(vertices, indices);
}

static void draw_opengl_shape(float vertices[], unsigned int indices[]) {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Draw rectangle
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

Rectangle rect_array_get(Rectangle rects[], size_t rect_size, size_t index) {
    if (rects == NULL) {
        throw_exception(NULL_POINTER, "rectArray is null!");
    }
    if (index >= rect_size) {
        throw_exception(INDEX_OUT_OF_BOUNDS, "index is out of bounds!");
    }
    return rects[index];
}

void compute_ndc_for_rect(Rectangle rect, size_int windowSize, float* left, float* right, float* bottom, float* top) {
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

Tiles create_tiles(size_int windowSize) {
    float dx = windowSize.width / 8.0f;
    float dy = windowSize.height / 8.0f;
    Tiles tiles = {0};
    int white_tiles_count = 0;
    int black_tiles_count = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            const Rectangle tile = { x * dx, y * dy, dx, dy, false};
            if ((x + y) % 2 == 0) {
                if (white_tiles_count >= 32) {
                    throw_exception(INDEX_OUT_OF_BOUNDS, "white_tiles_count is out of bounds!");
                    return tiles;
                }
                tiles.white_tiles[white_tiles_count++] = tile;
            }
            else {
                if (black_tiles_count >= 32) {
                    throw_exception(INDEX_OUT_OF_BOUNDS, "black_tiles_count is out of bounds!");
                }
                tiles.black_tiles[black_tiles_count++] = tile;
            }
        }
    }
    return tiles;
}

bool rect_contains_point(Rectangle rect, f_point point) {
    if (point.x > rect.x && point.x < rect.x + rect.width && point.y > rect.y && point.y < rect.y + rect.height) {
        return true;
    }
    else {
        return false;
    }
}