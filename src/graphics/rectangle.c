//
// Created by Omeed on 2/3/26.
//

#include "rectangle.h"

#include "renderer.h"

static void draw_opengl_shape(float vertices[], unsigned int indices[]);

void draw_rectangle(Rectangle rect, SizeInt windowSize, ShaderBuffers shaderBuffers) {
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