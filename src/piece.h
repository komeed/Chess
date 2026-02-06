//
// Created by Omeed on 2/5/26.
//

#ifndef PIECE_H
#define PIECE_H

#include "piece_type.h"
#include "utils/util_structs.h"
#include "stdbool.h"
#include "graphics/graphic_libs.h"

typedef struct board_point {
    int8_t row;
    int8_t col;
} board_point;

typedef struct {
    PieceType type;
    Color color;
    board_point pos;

    //ui stuff
    GLuint VAO;
    GLuint texture;
} Piece;

bool color_equal(Color a, Color b);

char* print_piece_path(PieceType piece, Color color);

#endif //PIECE_H
