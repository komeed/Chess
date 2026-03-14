#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "utils/util_structs.h"

#define TOP_COLOR PieceBlack
#define BOTTOM_COLOR PieceWhite

typedef struct Board {
    Piece board[8][8]; // i is row, j is column
    Piece* held_piece;
    board_point last_click_loc;

    Color top_color;
    Color bottom_color;
    bool is_bottom_turn;

    GLuint VAO;
} Board;

Board init_board();

bool move_piece_on_board(Board* board, board_point start, board_point end);

bool is_board_move_valid(Board* board, board_point start, board_point end);

#endif