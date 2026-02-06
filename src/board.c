//
// Created by Omeed on 2/5/26.
//

#include "board.h"

#include <stdbool.h>

Board init_board() {
    Board board;
    board.held_piece = NULL;
    board.last_click_loc = (i8_point) {0};
    PieceType white_back_rank[] = {W_Rook, W_Knight, W_Bishop, W_Queen, W_King, W_Bishop, W_Knight, W_Rook};
    PieceType black_back_rank[] = {B_Rook, B_Knight, B_Bishop, B_Queen, B_King, B_Bishop, B_Knight, B_Rook};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (i == 0) {
                board.board[i][j] = (Piece) {.type = white_back_rank[j], .pos = (i8_point) {i, j}, .is_pressed = false};
            }
            else if (i == 7) {
                board.board[i][j] = (Piece) {.type = black_back_rank[j], .pos = (i8_point) {i, j}, .is_pressed = false};
            }
            else if (i == 1) {
                board.board[i][j] = (Piece) {W_Pawn, .pos = (i8_point) {i, j}, .is_pressed = false};
            }
            else if (i == 6) {
                board.board[i][j] = (Piece) {.type = B_Pawn, .pos = (i8_point) {i, j}, .is_pressed = false};
            }
            else {
                board.board[i][j] = (Piece) {.type = 0, .pos = (i8_point) {i, j}, .is_pressed = false};
            }
        }
    }
    return board;
}
