//
// Created by Omeed on 2/3/26.
//
#include "../shader/shader.h"

#include "../utils/files.h"

ShaderBuffers init_shaders() {
    ShaderBuffers shaders;
    shaders.rectShaderProgram = init_shader_from_path("shaders/rect.vert", "shaders/rect.frag");
    shaders.imageShaderProgram = init_shader_from_path("shaders/image.vert", "shaders/image.frag");
    return shaders;
}

unsigned int init_shader_from_path(const char* vertexPath, const char* fragmentPath) {
    char* vertexSource = read_file(vertexPath);
    char* fragmentSource = read_file(fragmentPath);
    // 1. Create shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader error:\n%s\n", infoLog);
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader error:\n%s\n", infoLog);
    }


    // 2. Create program and link shaders
    unsigned int rectShaderProgram = glCreateProgram();
    glAttachShader(rectShaderProgram, vertexShader);
    glAttachShader(rectShaderProgram, fragmentShader);
    glLinkProgram(rectShaderProgram);

    glGetProgramiv(rectShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(rectShaderProgram, 512, NULL, infoLog);
        printf("Shader link error:\n%s\n", infoLog);
    }


    // 3. Delete individual shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //free the vertexSource and fragmentSource malloc buffers
    free(vertexSource);
    free(fragmentSource);

    return rectShaderProgram;
}