//
// Created by Omeed on 2/14/26.
//

#ifndef MINIMAX_H
#define MINIMAX_H

#include "bitboard.h"
#include "dynamic_array.h"

#define CHECKMATE_SCORE 10000000
#define PAWN_SCORE 10
#define KNIGHT_SCORE 32
#define BISHOP_SCORE 33
#define ROOK_SCORE 50
#define QUEEN_SCORE 90
#define KING_SCORE 0 // edge case

#define GET_PIECE_SCORE(square, side) ( \
((square) & (side)->pawns)   ? PAWN_SCORE : \
((square) & (side)->knights) ? KNIGHT_SCORE : \
((square) & (side)->bishops) ? BISHOP_SCORE : \
((square) & (side)->rooks)   ? ROOK_SCORE : \
((square) & (side)->queens)  ? QUEEN_SCORE : \
((square) & (side)->kings)   ? KING_SCORE : 0 )

#define GET_PIECE_SCORE_FROM_PTR(piece_ptr, side) ( \
(piece_ptr) == &((side)->pawns)   ? PAWN_SCORE : \
(piece_ptr) == &((side)->knights) ? KNIGHT_SCORE : \
(piece_ptr) == &((side)->bishops) ? BISHOP_SCORE : \
(piece_ptr) == &((side)->rooks)   ? ROOK_SCORE : \
(piece_ptr) == &((side)->queens)  ? QUEEN_SCORE : \
(piece_ptr) == &((side)->kings)   ? KING_SCORE : 0 )

#define GET_PP_SCORE(i) ( \
((i) == M_PP_Q) ? QUEEN_SCORE : \
((i) == M_PP_B) ? BISHOP_SCORE : \
((i) == M_PP_N) ? KNIGHT_SCORE : \
ROOK_SCORE )

void add_mm_best_board_moves(board_move_pos p);

board_move_pos mm_find_next_pos(bitboard* b);

I32 compute_HCE(const piece_bitboards* us, const piece_bitboards* them);

void print_board_move_pos(board_move_pos p);

bm_pos_w_score mm_recurse_helper(bitboard* b, piece_bitboards* us, piece_bitboards* them,
    int alpha, int beta, U8 is_max, U8 depth);

board_move_trace mm_recurse_helper_trace(bitboard* b, piece_bitboards* us, piece_bitboards* them,
    int alpha, int beta, U8 is_max, U8 depth);

board_move_trace mm_find_next_move_trace(bitboard* b);
I32 lightweight_eval(const piece_bitboards* us, const piece_bitboards* them, board_move_pos p, const U64* moving_piece);
void print_board_move_trace(board_move_trace* ps);

#endif //MINIMAX_H
