#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "utils/util_structs.h"
#include "bitboards/bitboard.h"
#include "bitboards/minimax.h"

#define TOP_COLOR PieceBlack
#define BOTTOM_COLOR PieceWhite

typedef struct gui_board {
    U64* held_bit_piece;
    board_move_pos held_piece_pos;
    bitboard bitboard;
    bishop_rays bishop_rays;
    rook_rays rook_rays;
   // board_point last_click_loc;

    Color top_color;
    Color bottom_color;

    GLuint piece_VAO;
    GLuint piece_texts[12]; // first 6 are bottom, second 6 are top
} gui_board;

gui_board init_board(bishop_rays* b_rays, rook_rays* r_rays);

bool move_piece_on_board(gui_board* board, board_move_pos m);

bool is_board_move_valid(gui_board* board, board_point start, board_point end);

#endif