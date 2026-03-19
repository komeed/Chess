//
// Created by Omeed on 3/3/26.
//

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#define MAX_LEGAL_MOVES 218
#define SAFE_FREE(ptr) do { free(ptr); (ptr) = NULL; } while(0)

#define MAX_PLY 7

#include "stdio.h"
#include "typedefs.h"

typedef struct {
    U8 start_square : 6;
    U8 end_square : 6;
    U8 pp_flag : 4;
} board_move_pos;

typedef struct {
    board_move_pos p;
    I32 s;
} bm_pos_w_score;

typedef struct {
    board_move_pos p;
    I32 s;
    U64* moving_piece;
} bmps_with_mp;

typedef struct {
    board_move_pos p[MAX_PLY + 1];
    I32 score;
    U8 size;
} board_move_trace;

typedef struct {
    bmps_with_mp ps[MAX_LEGAL_MOVES];
    U8 size;
} scores_for_legal_moves;

board_move_trace* init_legal_board_moves();

void add_legal_board_move(board_move_trace* moves, board_move_pos p, I32 score);

bm_pos_w_score get_top_move(board_move_trace* moves);

//scores_for_legal_moves* init_scores_w_legal_moves();

void clear_scores_for_legal_moves_arr(scores_for_legal_moves* legal_moves_arr);

void add_legal_board_move_with_score(scores_for_legal_moves* moves, board_move_pos p, I32 s, U64* moving_piece);
void clear_legal_board_move_with_score(scores_for_legal_moves* moves);

#endif //DYNAMIC_ARRAY_H
