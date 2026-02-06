//
// Created by Omeed on 2/5/26.
//

#include "board.h"

Board init_board() {
    Board board;
    Piece white_back_rank[] = {W_Rook, W_Knight, W_Bishop, W_Queen, W_King, W_Bishop, W_Knight, W_Rook};
    Piece black_back_rank[] = {B_Rook, B_Knight, B_Bishop, B_Queen, B_King, B_Bishop, B_Knight, B_Rook};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (i == 0) {
                board.board[i][j] = white_back_rank[j];
            }
            else if (i == 7) {
                board.board[i][j] = black_back_rank[j];
            }
            else if (i == 1) {
                board.board[i][j] = W_Pawn;
            }
            else if (i == 6) {
                board.board[i][j] = B_Pawn;
            }
            else {
                board.board[i][j] = 0;
            }
        }
    }
    return board;
}