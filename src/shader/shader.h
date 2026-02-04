//
// Created by Omeed on 2/3/26.
//

#ifndef SHADER_H
#define SHADER_H

#include "../graphics/graphic_libs.h"

typedef struct ShaderBuffers {
    unsigned int rectShaderProgram;
} ShaderBuffers;

ShaderBuffers init_shaders();

unsigned int init_rect_shader();

#endif //SHADER_H
