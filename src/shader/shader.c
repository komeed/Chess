//
// Created by Omeed on 2/3/26.
//
#include "../shader/shader.h"

#include "../utils/files.h"

ShaderBuffers init_shaders() {
    ShaderBuffers shaders;
    shaders.rectShaderProgram = init_rect_shader();
    return shaders;
}

unsigned int init_rect_shader() {
    char* vertexSource = read_file("shaders/rect.vert");
    char* fragmentSource = read_file("shaders/rect.frag");
    // 1. Create shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // 2. Create program and link shaders
    unsigned int rectShaderProgram = glCreateProgram();
    glAttachShader(rectShaderProgram, vertexShader);
    glAttachShader(rectShaderProgram, fragmentShader);
    glLinkProgram(rectShaderProgram);

    // 3. Delete individual shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //free the vertexSource and fragmentSource malloc buffers
    free(vertexSource);
    free(fragmentSource);

    return rectShaderProgram;
}