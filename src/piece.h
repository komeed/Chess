//
// Created by Omeed on 2/5/26.
//

#ifndef PIECE_H
#define PIECE_H

#include "piece_type.h"
#include "utils/util_structs.h"
#include "stdbool.h"
#include "graphics/graphic_libs.h"

typedef struct {
    PieceType type;
    i8_point pos;
    bool is_pressed;

    //ui stuff
    GLuint VAO;
    GLuint texture;
} Piece;

#endif //PIECE_H
