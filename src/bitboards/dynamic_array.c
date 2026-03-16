//
// Created by Omeed on 3/3/26.
//

#include <stdlib.h>

#include "dynamic_array.h"

board_move_trace* init_legal_board_moves() {
    board_move_trace* moves = malloc(sizeof(board_move_trace));
    moves->size = 0;
    return moves;
}

/*scores_for_legal_moves* init_scores_w_legal_moves() {
    scores_for_legal_moves* scores = malloc(sizeof(scores_for_legal_moves));
    scores->size = 0;
    return scores;
}*/

void clear_scores_for_legal_moves_arr(scores_for_legal_moves* legal_moves_arr) {
    for (int i = 0; i < MAX_PLY; i++) {
        legal_moves_arr[i].size = 0;
    }
}

void add_legal_board_move(board_move_trace* moves, board_move_pos p, I32 score) {
    if (moves == NULL) {
        fprintf(stderr, "add_legal_board_moves: moves is null!");
        return;
    }
    if (moves->size == MAX_PLY + 1) {
        fprintf(stderr, "add_legal_board_moves: hit max size! this should not happen.");
        exit(1);
    }
    moves->p[moves->size] = p;
    moves->score = score;
    moves->size++;
}

void clear_legal_board_moves(board_move_trace* moves) {
    if (moves == NULL) {
        fprintf(stderr, "clear_legal_board_moves: moves is null!");
        return;
    }
    moves->size = 0;
}

bm_pos_w_score get_top_move(board_move_trace* moves) {
    if (moves->size > MAX_PLY + 1) {
        fprintf(stderr, "get_top_move: hit max size! this should not happen.");
        exit(1);
    }
    return (bm_pos_w_score) {moves->p[moves->size - 1], moves->score};
}

void add_legal_board_move_with_score(scores_for_legal_moves* moves, board_move_pos p, I32 s, U64* moving_piece) {
    if (moves == NULL) {
        fprintf(stderr, "add_legal_board_move_with_score: moves is null!");
        return;
    }
    if (moves->size >= MAX_LEGAL_MOVES) {
        fprintf(stderr, "add_legal_board_move_with_score: hit max size!!");
        exit(1);
    }
    if (moving_piece == NULL) {
        fprintf(stderr, "add_legal_board_move_with_score: piece is null!");
        exit(1);
    }
    moves->ps[moves->size] = (bmps_with_mp) {p, s, moving_piece};
    moves->size++;
}

void clear_legal_board_move_with_score(scores_for_legal_moves* moves) {
    if (moves == NULL) {
        fprintf(stderr, "clear_legal_board_move_with_score: moves is null!!");
        return;
    }
    moves->size = 0;
}