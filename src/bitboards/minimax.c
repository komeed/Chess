//
// Created by Omeed on 2/16/26.
//

#include "minimax.h"

#define SCORE_PIECES(bbus, bbthem, value)        \
do {                                         \
score += __builtin_popcountll(bbus) * (value); \
score -= __builtin_popcountll(bbthem) * (value); \
} while (0)

#define SCORE_PIECE_PST(bb, pst, sign, mirror)          \
do {                                                     \
U64 tmp = (bb);                                      \
while (tmp) {                                        \
int sq = __builtin_ctzll(tmp);                  \
tmp &= tmp - 1;                                  \
score += ((mirror) ? (pst)[MIRROR_PST[sq]] : (pst)[sq]) * (sign); \
}                                                    \
} while(0)

#define CLEAR_MM_BOARD_MOVES(a) ((a)->size = 0)

static bm_pos_w_score find_all_possible_board_moves(bitboard* b, piece_bitboards* us, piece_bitboards* them,
    int alpha, int beta, U8 is_max, U8 depth) {
    find_all_attacking_pieces(b);

    U64* moving_piece = NULL;
    U64 result = 0;
    bm_pos_w_score max_pos;
    if (is_max) {
        max_pos.s = INT32_MIN;
    }
    else {
        max_pos.s = INT32_MAX;
    }
    scores_for_legal_moves* l_moves = &b->legal_moves_array[depth - 1];
    clear_legal_board_move_with_score(l_moves);
    for (I8 i = 0; i < 64; i++) {
        moving_piece = find_valid_moves_for_piece(b, us, i, &result);
        if (moving_piece != NULL) {
            result &= ~us->all;
            while (result) {
                I8 end_sq = (I8) __builtin_ctzll(result);
                board_move_pos move_pos = {i, end_sq, 0};
                I32 light_score = lightweight_eval(us, them, move_pos, moving_piece);
                if (moving_piece == &us->pawns && ((end_sq < 8 && us == &b->black) || (end_sq > 55 && us == &b->white))) {
                    for (int j = 1; j < 5; j++) {
                        move_pos.pp_flag = j;
                        I32 score = GET_PP_SCORE(j) + light_score;
                        add_legal_board_move_with_score(l_moves, move_pos, score, moving_piece);
                    }
                }
                else {
                    add_legal_board_move_with_score(l_moves, move_pos, light_score, moving_piece);
                }
                result &= result - 1;
            }
        }
    }
    //lazy selection sort
    for (int i = 0; i < l_moves->size; i++) {
        int best = i;
        for (int j = i + 1; j < l_moves->size; j++) {
            if (l_moves->ps[j].s > l_moves->ps[best].s) {
                best = j;
            }
        }
//swap them
        bmps_with_mp temp = l_moves->ps[best];
        l_moves->ps[best] = l_moves->ps[i];
        l_moves->ps[i] = temp;
        //then run the typical processing on best
        bmps_with_mp* pos = &l_moves->ps[i];
        test_move_w_flags(b, us, them, pos->p, pos->moving_piece);
        reset_board_all(b);
        if (!is_attacking_our_king(b, us, them)) { // if they are not still attacking our king (legal move)
           // b->count++;
            bm_pos_w_score ps = mm_recurse_helper(b, them, us, alpha, beta, !is_max, depth - 1);
            if (is_max) {
                if (ps.s > max_pos.s) {
                    alpha = ps.s;
                    max_pos.s = ps.s;
                    max_pos.p = pos->p;
                    if (alpha >= beta) {
                        unmove_piece(b, us);
                        reset_board_all(b);
                        break;
                    }
                }
            }
            else {
                if (ps.s < max_pos.s) {
                    beta = ps.s;
                    max_pos.s = ps.s;
                    max_pos.p = pos->p;
                    if (beta <= alpha) {
                        //prune the rest
                        unmove_piece(b, us);
                        reset_board_all(b);
                        break;
                    }
                }
            }
        }
        unmove_piece(b, us);
        reset_board_all(b);
    }
    // also means we are max
    if (max_pos.s == INT32_MIN) {
        //if they are currently attacking us, we got checkmated
        if (us->kings & them->attacking) {
            return (bm_pos_w_score) { .s = -CHECKMATE_SCORE + (MAX_PLY - depth)};
        }
        //otherwise, it's stalemate
        else {
            return (bm_pos_w_score) {.s = 0};
        }
    }
    else if (max_pos.s == INT32_MAX) {
            if (us->kings & them->attacking) {
                return (bm_pos_w_score) {.s = CHECKMATE_SCORE - (MAX_PLY - depth)};
            }
            else {
                return (bm_pos_w_score) {.s = 0};
            }
        }
    return max_pos;

   // return max_pos;
}

static board_move_trace find_board_move_trace(bitboard* b, piece_bitboards* us, piece_bitboards* them,
    int alpha, int beta, U8 is_max, U8 depth) {
    find_all_attacking_pieces(b);
    U64* moving_piece = NULL;
    U64 result = 0;
    bm_pos_w_score max_pos;
    board_move_trace trace;
    trace.size = 0;
    if (is_max) {
        trace.score = INT32_MIN;
        max_pos = (bm_pos_w_score) {.s = INT32_MIN};
    }
    else {
        trace.score = INT32_MAX;
        max_pos = (bm_pos_w_score) {.s = INT32_MAX};
    }

    scores_for_legal_moves* l_moves = &b->legal_moves_array[depth - 1];
    clear_legal_board_move_with_score(l_moves);
    for (I8 i = 0; i < 64; i++) {
        moving_piece = find_valid_moves_for_piece(b, us, i, &result);
        if (moving_piece != NULL) {
            result &= ~us->all;
            while (result) {
                I8 end_sq = (I8) __builtin_ctzll(result);
                const board_move_pos move_pos = {i, end_sq};
                I32 score = lightweight_eval(us, them, move_pos, moving_piece);
                add_legal_board_move_with_score(l_moves, move_pos, score, moving_piece);
                result &= result - 1;
            }
        }
    }

    for (int i = 0; i < l_moves->size; i++) {
        int best = i;
        for (int j = i + 1; j < l_moves->size; j++) {
            if (l_moves->ps[j].s > l_moves->ps[best].s) {
                best = j;
            }
        }
        //swap them
        bmps_with_mp temp = l_moves->ps[best];
        l_moves->ps[best] = l_moves->ps[i];
        l_moves->ps[i] = temp;
        //then run the typical processing on best
        bmps_with_mp* pos = &l_moves->ps[i];
        test_move_w_flags(b, us, them, pos->p, pos->moving_piece);
        reset_board_all(b);
        if (!(is_attacking_our_king(b, us, them))) { // if they are not still attacking our king (legal move)
            b->count++;
            board_move_trace their_trace = mm_recurse_helper_trace(b, them, us, alpha, beta, !is_max, depth - 1);
            bm_pos_w_score ps = get_top_move(&their_trace);
            if (is_max) {
                if (their_trace.score > max_pos.s) {
                    alpha = ps.s;
                    max_pos.s = ps.s;
                    max_pos.p = pos->p;
                    if (beta <= alpha) {
                        //prune the rest
                        unmove_piece(b, us);
                        reset_board_all(b);
                        break;
                    }
                    trace = their_trace;
                }
            }
            else {
                if (their_trace.score < max_pos.s) {
                    beta = ps.s;
                    max_pos.s = ps.s;
                    max_pos.p = pos->p;
                    if (beta <= alpha) {
                        //prune the rest
                        unmove_piece(b, us);
                        reset_board_all(b);
                        break;
                    }
                    trace = their_trace;
                }
            }
        }
        unmove_piece(b, us);
        reset_board_all(b);
    }
    // also means we are max
    if (max_pos.s == INT32_MIN) {
        //if they are currently attacking us, we got checkmated
        if (us->kings & them->attacking) {
            add_legal_board_move(&trace, (board_move_pos) {},
                -CHECKMATE_SCORE + (MAX_PLY - depth));
        }
        //otherwise, it's stalemate
        else {
            add_legal_board_move(&trace, (board_move_pos) {},
                0);
        }
    }
    else if (max_pos.s == INT32_MAX) {
        if (us->kings & them->attacking) {
            add_legal_board_move(&trace, (board_move_pos) {},
                CHECKMATE_SCORE - (MAX_PLY - depth));
        }
        else {
            add_legal_board_move(&trace, (board_move_pos) {},
                0);
        }
    }
    else {
        add_legal_board_move(&trace, max_pos.p, max_pos.s);
    }
    return trace;
}

bm_pos_w_score mm_find_next_pos(bitboard* b) {
    piece_bitboards* us = &b->white;
    piece_bitboards* them = &b->black;
    if (b->flags & TURN_FLAG) {
        us = &b->black;
        them = &b->white;
    }
   // b->l_moves = init_scores_w_legal_moves();
    bm_pos_w_score pos = mm_recurse_helper(b, us, them, INT32_MIN, INT32_MAX, 1, MAX_PLY);
   // SAFE_FREE(b->l_moves);
    return pos;
}

board_move_trace mm_find_next_move_trace(bitboard* b) {
    piece_bitboards* us = &b->white;
    piece_bitboards* them = &b->black;
    if (b->flags & TURN_FLAG) {
        us = &b->black;
        them = &b->white;
    }
    return mm_recurse_helper_trace(b, us, them, INT32_MIN, INT32_MAX, 1, MAX_PLY);
}

void print_board_move_pos(board_move_pos p) {
    int start_file = p.start_square % 8;
    int start_rank = p.start_square / 8;

    int end_file = p.end_square % 8;
    int end_rank = p.end_square / 8;

    printf("%c%c%c%c\n",
        'a' + start_file,
        '1' + start_rank,
        'a' + end_file,
        '1' + end_rank
    );
}

void print_bm_w_score(bm_pos_w_score ps) {
    print_board_move_pos(ps.p);
    printf("score: %d\n", ps.s);
}

void print_board_move_trace(board_move_trace* ps) {
    printf("score of trace: %d", ps->score);
    for (int i = 0; i < ps->size; i++) {
        print_board_move_pos(ps->p[i]);
    }
}

bm_pos_w_score mm_recurse_helper(bitboard* b, piece_bitboards* us, piece_bitboards* them,
    int alpha, int beta, U8 is_max, U8 depth) {
    if (depth == 0) {
        int score = 0;
        if (is_max) {
            score = compute_board_score(b, us, them);
        }
        else {
            score = compute_board_score(b, them, us);
        }
        return (bm_pos_w_score) {.s = score};
    }
    return find_all_possible_board_moves(b, us, them, alpha, beta, is_max, depth);
}

board_move_trace mm_recurse_helper_trace(bitboard* b, piece_bitboards* us, piece_bitboards* them,
    int alpha, int beta, U8 is_max, U8 depth) {
    if (depth == 0) {
        int score = 0;
        if (is_max) {
            score = compute_board_score(b, us, them);
        }
        else {
            score = compute_board_score(b, them, us);
        }
        board_move_trace trace = {.size = 0};
        add_legal_board_move(&trace, (board_move_pos) {}, score);
        return trace;
    }
    return find_board_move_trace(b, us, them, alpha, beta, is_max, depth);
}

I32 lightweight_eval(const piece_bitboards* us, const piece_bitboards* them, board_move_pos p, const U64* moving_piece) {
    U64 end = 1ULL << p.end_square;
    I32 victim_score = GET_PIECE_SCORE(end, them);
    I32 attacker_score = GET_PIECE_SCORE_FROM_PTR(moving_piece, us);
    I32 final_score = (victim_score * 10) - attacker_score;
    return final_score;
}

static I32 compute_PST(const bitboard* b, const piece_bitboards* us, const piece_bitboards* them) {
    I32 score = 0;
    int mirror_flag = b->flags & TURN_FLAG;
    SCORE_PIECE_PST(us->pawns, PAWN_PST, +1, mirror_flag);
    SCORE_PIECE_PST(them->pawns, PAWN_PST, -1, !mirror_flag);
    SCORE_PIECE_PST(us->knights, KNIGHT_PST, +1, mirror_flag);
    SCORE_PIECE_PST(them->knights, KNIGHT_PST, -1, !mirror_flag);
    SCORE_PIECE_PST(us->rooks, ROOK_PST, +1, mirror_flag);
    SCORE_PIECE_PST(them->rooks, ROOK_PST, -1, !mirror_flag);
    SCORE_PIECE_PST(us->queens, QUEEN_PST, +1, mirror_flag);
    SCORE_PIECE_PST(them->queens, QUEEN_PST, -1, !mirror_flag);
    SCORE_PIECE_PST(us->bishops, BISHOP_PST, +1, mirror_flag);
    SCORE_PIECE_PST(them->bishops, BISHOP_PST, -1, !mirror_flag);
    return score;
}

static I32 compute_HCE(const piece_bitboards* us, const piece_bitboards* them) {
    I32 score = 0;
    SCORE_PIECES(us->pawns,   them->pawns,   PAWN_SCORE);
    SCORE_PIECES(us->knights, them->knights, KNIGHT_SCORE);
    SCORE_PIECES(us->bishops, them->bishops, BISHOP_SCORE);
    SCORE_PIECES(us->rooks,   them->rooks,   ROOK_SCORE);
    SCORE_PIECES(us->queens,  them->queens,  QUEEN_SCORE);
    return score;
}

I32 compute_board_score(const bitboard* b, const piece_bitboards* us, const piece_bitboards* them) {
    return compute_HCE(us, them) + compute_PST(b, us, them);
}