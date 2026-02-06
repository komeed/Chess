#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "utils/util_structs.h"

typedef struct Board {
    Piece board[8][8]; // i is row, j is column
    Piece* held_piece;
    i8_point last_click_loc;
} Board;

Board init_board();

#endif