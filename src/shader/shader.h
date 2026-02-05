//
// Created by Omeed on 2/3/26.
//

#ifndef SHADER_H
#define SHADER_H

#include "../graphics/graphic_libs.h"

typedef struct ShaderBuffers {
    unsigned int rectShaderProgram;
    unsigned int imageShaderProgram;
} ShaderBuffers;

ShaderBuffers init_shaders();

unsigned int init_shader_from_path(const char* vertexPath, const char* fragmentPath);

#endif //SHADER_H
