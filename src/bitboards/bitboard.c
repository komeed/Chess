//
// Created by Omeed on 2/7/26.
//

#include "bitboard.h"

#include <stdlib.h>

#include "stdio.h"
#include "masks.h"

#define GET_FLAG_FROM_ROOK_MOV(start_square) \
((start_square) == R_WK_IND ? R_WK_FLAG : \
(start_square) == R_BK_IND ? R_BK_FLAG : \
(start_square) == R_WQ_IND ? R_WQ_FLAG : \
(start_square) == R_BQ_IND ? R_BQ_FLAG : \
0)

//masks

void init_piece_bitboard_promo_table(piece_bitboards* side) {
    side->promo_table[M_PP_Q - 1] = &side->queens;
    side->promo_table[M_PP_N - 1] = &side->knights;
    side->promo_table[M_PP_B - 1] = &side->bishops;
    side->promo_table[M_PP_R - 1] = &side->rooks;
}

void reset_board_all(bitboard* b) {
    piece_bitboards* white = &b->white;
    piece_bitboards* black = &b->black;
    white->all = white->bishops | white->pawns | white->queens | white->kings | white->rooks | white->knights;
    black->all = black->bishops | black->pawns | black->queens | black->kings | black->rooks | black->knights;
    b->all = white->all | black->all;
}

static void init_knight_attacks(bitboard* b) {
    for (int sq = 0; sq < 64; sq++) {
        U64 attacks = 0ULL;
        int rank = sq / 8;
        int file = sq % 8;

        // Knight moves relative to current square
        int knight_offsets[8][2] = {
            {2,1}, {1,2}, {-1,2}, {-2,1},
            {-2,-1}, {-1,-2}, {1,-2}, {2,-1}
        };

        for (int i = 0; i < 8; i++) {
            int r = rank + knight_offsets[i][0];
            int f = file + knight_offsets[i][1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8)
                attacks |= 1ULL << (r*8 + f);
        }
        b->knight_attacks[sq] = attacks;
    }
}

static void init_king_attacks(bitboard* b) {
    for (int sq = 0; sq < 64; sq++) {
        U64 attacks = 0ULL;
        int rank = sq / 8;
        int file = sq % 8;

        // Iterate all possible king move offsets
        int offsets[8][2] = {
            {1, 0},  {1, 1}, {0, 1}, {-1, 1},
            {-1, 0}, {-1,-1}, {0,-1}, {1,-1}
        };

        for (int i = 0; i < 8; i++) {
            int r = rank + offsets[i][0];
            int f = file + offsets[i][1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8) {
                attacks |= 1ULL << (r*8 + f);
            }
        }
        b->king_attacks[sq] = attacks;
    }
}

bitboard* init_board_bitboards(bishop_rays* b_rays, rook_rays* r_rays, U8 use_minimax) {
    bitboard *board = malloc(sizeof(bitboard));

    if (!board) {
        fprintf(stderr, "Failed to allocate memory for bitboards\n");
        exit(1);
    }

    *board = (bitboard) {
        .white = {
            .pawns   = 0x000000000000FF00ULL,
            .rooks   = 0x0000000000000081ULL,
            .knights = 0x0000000000000042ULL,
            .bishops = 0x0000000000000024ULL,
            .queens  = 0x0000000000000008ULL,
            .kings   = 0x0000000000000010ULL
        },
        .black = {
            .pawns   = 0x00FF000000000000ULL,
            .rooks   = 0x8100000000000000ULL,
            .knights = 0x4200000000000000ULL,
            .bishops = 0x2400000000000000ULL,
            .queens  = 0x0800000000000000ULL,
            .kings   = 0x1000000000000000ULL
        }
    };
    board->flags = 0;
    board->count = 0;
    /**board = (bitboard) {
        .white = {
            .pawns   = 0x000000000000FB00ULL,
            .rooks   = 0x0000000000000080ULL,
            .knights = 0,
            .bishops = 0x0000000000000020ULL,
            .queens  = 0,
            .kings   = 0x0000000000000010ULL
        },
        .black = {
            .pawns   = 0x00FF000000000400ULL,
            .rooks   = 0x8100000000000000ULL,
            .knights = 0x4200000000000000ULL,
            .bishops = 0x2400000000000000ULL,
            .queens  = 0x0800000000000000ULL,
            .kings   = 0x1000000000000000ULL
        }
    };
    board->count = 0;
    board->flags = 0; // default turns (white turn first, every other flag is off)*/
    //board.flags |= (R_WK_FLAG | R_WQ_FLAG | R_BK_FLAG | R_BQ_FLAG);
    //board.flags |= TURN_FLAG;
    board->flags &= ~TURN_FLAG;
    board->b_rays = b_rays;
    board->r_rays = r_rays;
    board->ply_size = 0;
    board->use_ab_pruning = use_minimax;
    init_knight_attacks(board);
    init_king_attacks(board);

    board->waiting_for_pawn_promote = 0;
    board->use_minimax_on_black = 1;

    init_piece_bitboard_promo_table(&board->white);
    init_piece_bitboard_promo_table(&board->black);

    clear_scores_for_legal_moves_arr(board->legal_moves_array);

    reset_board_all(board);
    find_all_attacking_pieces(board);
    return board;
}

void print_bitboard(bitboard* b) {
    char unified_board[64];
    piece_bitboards* wp = &b->white;
    piece_bitboards* bp = &b->black;
    for (int8_t row = 7; row >= -2; row--) {
        for (unsigned char col = 0; col < 8; col++) {
            putchar(' ');
            if (row == -1) {
                if (col == 0) {
                    putchar(' ');
                    putchar(' ');
                }
                putchar('_');
                if (col == 7) {
                    putchar('\n');
                }
                continue;
            }
            if (row == -2) {
                if (col == 0) {
                    putchar(' ');
                    putchar(' ');
                }
                putchar('a' + col);
                continue;
            }

            unsigned char i = row * 8 + col;
            if (i % 8 == 0) {
                putchar((row + 1) + '0');
                putchar('|');
            }

            U64 temp = 1ULL << i;
            if (wp->pawns & temp) { unified_board[i] = 'P'; }
            else if (wp->rooks & temp) { unified_board[i] = 'R'; }
            else if (wp->knights & temp) { unified_board[i] = 'N'; }
            else if (wp->bishops & temp) { unified_board[i] = 'B'; }
            else if (wp->queens & temp) { unified_board[i] = 'Q'; }
            else if (wp->kings & temp) { unified_board[i] = 'K'; }

            else if (bp->pawns & temp) { unified_board[i] = 'p'; }
            else if (bp->rooks & temp) { unified_board[i] = 'r'; }
            else if (bp->knights & temp) { unified_board[i] = 'n'; }
            else if (bp->bishops & temp) { unified_board[i] = 'b'; }
            else if (bp->queens & temp) { unified_board[i] = 'q'; }
            else if (bp->kings & temp) { unified_board[i] = 'k'; }

            else { unified_board[i] = '.'; }

            putchar(unified_board[i]);
            if (i % 8 == 7) { putchar('\n'); }
        }
    }
    putchar('\n');
    if (b->flags & TURN_FLAG) {
        printf("black turn\n");
    }
    else {
        printf("white turn\n");
    }
}

void print_bitboard_values(bitboard* b) {
    printf("White Pawn:   %llu\n", b->white.pawns);
    printf("White Rook:   %llu\n", b->white.rooks);
    printf("White Knight: %llu\n", b->white.knights);
    printf("White Bishop: %llu\n", b->white.bishops);
    printf("White Queen:  %llu\n", b->white.queens);
    printf("White King:   %llu\n", b->white.kings);

    printf("Black Pawn:   %llu\n", b->black.pawns);
    printf("Black Rook:   %llu\n", b->black.rooks);
    printf("Black Knight: %llu\n", b->black.knights);
    printf("Black Bishop: %llu\n", b->black.bishops);
    printf("Black Queen:  %llu\n", b->black.queens);
    printf("Black King:   %llu\n", b->black.kings);
}

static U8 piece_bitboards_are_equal(piece_bitboards* b1, piece_bitboards* b2) {
    if (b1->pawns != b2->pawns) { return 0; }
    if (b1->rooks != b2->rooks) { return 0; }
    if (b1->knights != b2->knights) { return 0; }
    if (b1->bishops != b2->bishops) { return 0; }
    if (b1->queens != b2->queens) { return 0; }
    if (b1->kings != b2->kings) { return 0; }
   // if (b1->attacking != b2->attacking) { return 0; }
    if (b1->all !=  b2->all) { return 0; }
    return 1;
}

U8 boards_are_equal(bitboard* b1, bitboard* b2) {
    return (piece_bitboards_are_equal(&b1->white, &b2->white) && piece_bitboards_are_equal(&b1->black, &b2->black)
         && b1->double_move_square == b2->double_move_square && b1->all == b2->all);
}

static inline U64 pawn_attack_mask_from_square(U64 square, U8 is_black) {
    U64 diag_left = (square >> 9 & NOT_H);
    U64 diag_right = (square >> 7 & NOT_A);

    if (!is_black) { // if we are in white's turn
        diag_left = (square << 7 & NOT_H);
        diag_right = (square << 9 & NOT_A);
    }
    return diag_left | diag_right;
}

static void validate_pawn_moves(bitboard* b, piece_bitboards* us, U8 s, U64* result) { // need turn for this one
    if (us == NULL) {
        fprintf(stderr, "validate_pawn_moves: us is null!");
        exit(1);
    }

    U64 start = 1ULL << s;

    U64 single_up = start >> 8;
    U64 double_up = start >> 16;
    U64 diag_left = (start >> 9 & NOT_H);
    U64 diag_right = (start >> 7 & NOT_A);
    U64 pawn_row = 0x00FF000000000000ULL;

    if (us == &b->white) {
        single_up = start << 8;
        double_up = start << 16;
        diag_left = (start << 7 & NOT_H);
        diag_right = (start << 9 & NOT_A);
        pawn_row = 0x000000000000FF00ULL;
    }
    if (diag_left & b->all) {
        *result |= diag_left;
    }
    else {
        if (diag_left) {
            if ((b->flags & DOUBLE_MOVE_SQUARE_IS_NOT_NULL) && ((1ULL << b->double_move_square) & ((start) >> 1))) { // if ep square is not null and ep square is left
                //  b->flags |= EP_FLAG;
                *result |= diag_left;
            }
        }
    }
    if (diag_right & b->all) {
        *result |= diag_right;
    }
    else {
        if (diag_right) {
            if ((b->flags & DOUBLE_MOVE_SQUARE_IS_NOT_NULL) && ((1ULL << b->double_move_square) & ((start) << 1))) {
                // b->flags |= EP_FLAG;
                *result |= diag_right;
            }
        }
    }

    if (!(single_up & b->all)) {
        *result |= single_up;
        if (!(double_up & b->all) && (start & pawn_row)) {
            *result |= double_up;
        }
    }
}

static void validate_knight_moves(bitboard* b, int s, U64* result) {
    *result |= b->knight_attacks[s];
}

static U64 get_bishop_rays(bitboard* b, U8 s) {
    U64 result = 0;

    U64 b_ray_ne = b->b_rays->bishop_rays_NE[s];
    U64 b_ray_se = b->b_rays->bishop_rays_SE[s];
    U64 b_ray_nw = b->b_rays->bishop_rays_NW[s];
    U64 b_ray_sw = b->b_rays->bishop_rays_SW[s];

    // NE
    U64 blocker_ne = b_ray_ne & b->all;
    if (blocker_ne) {
        int blocker_sq = __builtin_ctzll(blocker_ne); // LSB
        result |= b_ray_ne ^ b->b_rays->bishop_rays_NE[blocker_sq];
    } else {
        result |= b_ray_ne;
    }

    // SE
    U64 blocker_se = b_ray_se & b->all;
    if (blocker_se) {
        int blocker_sq = 63 - __builtin_clzll(blocker_se); // MSB
        result |= b_ray_se ^ b->b_rays->bishop_rays_SE[blocker_sq];
    } else {
        result |= b_ray_se;
    }

    // NW
    U64 blocker_nw = b_ray_nw & b->all;
    if (blocker_nw) {
        int blocker_sq = __builtin_ctzll(blocker_nw); // LSB
        result |= b_ray_nw ^ b->b_rays->bishop_rays_NW[blocker_sq];
    } else {
        result |= b_ray_nw;
    }

    // SW
    U64 blocker_sw = b_ray_sw & b->all;
    if (blocker_sw) {
        int blocker_sq = 63 - __builtin_clzll(blocker_sw); // MSB
        result |= b_ray_sw ^ b->b_rays->bishop_rays_SW[blocker_sq];
    } else {
        result |= b_ray_sw;
    }
    return result;
}

static void validate_bishop_moves(bitboard* b, U8 s, U64* result) {
    *result |= get_bishop_rays(b, s);
}

static U64 get_rook_rays(bitboard* b, U8 s) {
    U64 result = 0;
    U64 ray_n = b->r_rays->r_rays_N[s];
    U64 ray_s = b->r_rays->r_rays_S[s];
    U64 ray_e = b->r_rays->r_rays_E[s];
    U64 ray_w = b->r_rays->r_rays_W[s];

    // NORTH
    U64 blocker_n = ray_n & b->all;
    if (blocker_n) {
        int blocker_sq = __builtin_ctzll(blocker_n); // LSB = closest north blocker
        result |= ray_n ^ b->r_rays->r_rays_N[blocker_sq];
    } else {
        result |= ray_n;
    }

    // SOUTH
    U64 blocker_s = ray_s & b->all;
    if (blocker_s) {
        int blocker_sq = 63 - __builtin_clzll(blocker_s); // MSB = closest south blocker
        result |= ray_s ^ b->r_rays->r_rays_S[blocker_sq];
    } else {
        result |= ray_s;
    }

    // EAST
    U64 blocker_e = ray_e & b->all;
    if (blocker_e) {
        int blocker_sq = __builtin_ctzll(blocker_e); // LSB = closest east blocker
        result |= ray_e ^ b->r_rays->r_rays_E[blocker_sq];
    } else {
        result |= ray_e;
    }

    // WEST
    U64 blocker_w = ray_w & b->all;
    if (blocker_w) {
        int blocker_sq = 63 - __builtin_clzll(blocker_w); // MSB = closest west blocker
        result |= ray_w ^ b->r_rays->r_rays_W[blocker_sq];
    } else {
        result |= ray_w;
    }
    return result;
}

static void validate_rook_moves(bitboard* b, U8 s, U64* result) {
    *result |= get_rook_rays(b, s);
}

static void validate_king_moves(bitboard* b, piece_bitboards* us, U8 s, U64* result) { // return 1 if castle, return 0 if don't castle
    U64 start = 1ULL << s;
    *result |= b->king_attacks[s];
    piece_bitboards* them = &b->white;
    if (us == them) {
        them = &b->black;
    }
    if (!(them->attacking & start)) {
        if (us == &b->black) { // if turn is black
            if (!(b->flags & R_BK_FLAG)) { // if rook near bk isn't flagged (top right)
                // make sure that king hasn't moved and is still in the correct spot
                if (!(b->all & (start << 1)) && !(b->all & (start << 2))) { // if the squares right are empty
                    if (!(s == BK_IND)) {
                        fprintf(stderr, "OOPS! somehow black king isn't where it's suppsoed to be");
                        exit(1);
                    }
                 //   printf("bk does have ability to castle rightwards!");
                    *result |= start << 2;
                }
            }
            if (!(b->flags & R_BQ_FLAG)) {
                if (!(b->all & (start >> 1)) && !(b->all & (start >> 2)) && !(b->all & (start >> 3))) {
                    if (!(s == BK_IND)) {
                        fprintf(stderr, "OOPS! somehow black king isn't where it's suppsoed to be");
                        exit(1);
                    }
                 //   printf("bk does have ability to castle leftwards!");
                    *result |= start >> 2;
                }
            }
        }
        else {
            if (!(b->flags & R_WK_FLAG)) { // if rook near wk isn't flagged (top right)
                // make sure that king hasn't moved and is still in the correct spot
                if (!(b->all & (start << 1)) && !(b->all & (start << 2))) { // if the squares right are empty
                    if (!(s == WK_IND)) {
                        fprintf(stderr, "OOPS! somehow white king isn't where it's suppsoed to be");
                        exit(1);
                    }
                  //  printf("wk does have ability to castle rightwards!");
                    *result |= start << 2;
                   // b->flags |= CASTLE_MOVE_ONOFF_FLAG;
                }
            }
            if (!(b->flags & R_WQ_FLAG)) {
                if (!(b->all & (start >> 1)) && !(b->all & (start >> 2)) && !(b->all & (start >> 3))) {
                    if (!(s == WK_IND)) {
                        fprintf(stderr, "OOPS! somehow white king isn't where it's suppsoed to be");
                        exit(1);
                    }
                   // printf("wk does have ability to castle leftwards!");
                    *result |= start >> 2;
                   // b->flags |= CASTLE_MOVE_ONOFF_FLAG;
                }
            }
        }
    }
}

static void accumulate_attacks_for_side(bitboard* b, piece_bitboards* us) {
    U64 result = 0;
    U64 pawns = us->pawns;
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        pawns &= pawns - 1;
        result |= pawn_attack_mask_from_square(1ULL << sq, (us == &b->black));
    }
    U64 knights = us->knights;
    while (knights) {
        int sq = __builtin_ctzll(knights);
        knights &= knights - 1;
        result |= b->knight_attacks[sq];
    }
    U64 rooks = us->rooks;
    while (rooks) {
        int sq = __builtin_ctzll(rooks);
        rooks &= rooks - 1;
        result |= get_rook_rays(b, sq);
    }
    U64 bishops = us->bishops;
    while (bishops) {
        int sq = __builtin_ctzll(bishops);
        bishops &= bishops - 1;
        result |= get_bishop_rays(b, sq);
    }
    U64 queens = us->queens;
    while (queens) {
        int sq = __builtin_ctzll(queens);
        queens &= queens - 1;
        result |= get_bishop_rays(b, sq) | get_rook_rays(b, sq);
    }
    U64 kings = us->kings;
    while (kings) {
        int sq = __builtin_ctzll(kings);
        kings &= kings - 1;
        result |= b->king_attacks[sq];
    }
    result &= ~us->all;
    us->attacking = result;
    /*
    U64 pos = 1ULL << sq;
    if (!(us->all & pos)) {
        return;
    }
    b->p.start_square = sq;
    if (pos & us->pawns) {
        U64 temp = 0;
        validate_pawn_moves(b, us, sq, &temp);
        temp &= ~(0x0101010101010101ULL << (sq % 8));
        us->attacking |= temp;
    }
    else if (pos & us->knights) validate_knight_moves(b, sq, &us->attacking);
    else if (pos & us->bishops) validate_bishop_moves(b, sq, &us->attacking);
    else if (pos & us->rooks)   validate_rook_moves(b, sq, &us->attacking);
    else if (pos & us->queens) {
        validate_bishop_moves(b, sq, &us->attacking);
        validate_rook_moves(b, sq, &us->attacking);
    }
    else if (pos & us->kings) {
        U64 temp = 0;
        validate_king_moves(b, us, sq, &temp);
        temp &= ~((pos << 2) | (pos >> 2));
        us->attacking |= temp;
    }*/
}

void find_all_attacking_pieces(bitboard* b) {
    accumulate_attacks_for_side(b, &b->white);
    accumulate_attacks_for_side(b, &b->black);
    b->attacking = b->white.attacking | b->black.attacking;
}

void find_all_attacking_pieces_and_reset_all(bitboard* b) {
    piece_bitboards* white = &b->white;
    piece_bitboards* black = &b->black;
    white->all = white->bishops | white->pawns | white->queens | white->kings | white->rooks | white->knights;
    black->all = black->bishops | black->pawns | black->queens | black->kings | black->rooks | black->knights;
    b->all = white->all | black->all;
    accumulate_attacks_for_side(b, white);
    accumulate_attacks_for_side(b, black);
    b->attacking = black->attacking | white->attacking;

}

U64* find_valid_moves_for_piece(bitboard* b, piece_bitboards* us, U8 s, U64* result) {
    U64 start = 1ULL << s;
    if (!(us->all & start)) {
        return NULL;
    }
    *result = 0;
    U64* moving_piece = NULL;
    if (us->pawns & start) {
        validate_pawn_moves(b, us, s, result);
        moving_piece = &us->pawns;
    }
    else if (us->knights & start) {
        validate_knight_moves(b, s, result);
        moving_piece = &us->knights;
    }
    else if (us->bishops & start) {
        validate_bishop_moves(b, s, result);
        moving_piece = &us->bishops;
    }
    else if (us->rooks & start) {
        validate_rook_moves(b, s, result);
        moving_piece = &us->rooks;
    }
    else if (us->queens & start) {
        validate_bishop_moves(b, s, result);
        validate_rook_moves(b, s, result);
        moving_piece = &us->queens;
    }
    else if (us->kings & start) {
        validate_king_moves(b, us, s, result);
        moving_piece = &us->kings;
    }
    else {
        moving_piece = NULL;
    }
    return moving_piece;
}

static U8 does_them_have_legal_moves(bitboard* b, piece_bitboards* us, piece_bitboards* them) {
    U64 result = 0;
    U64* moving_piece = NULL;
    bitboard temp_b = *b;
    for (I8 i = 0; i < 64; i++) {
        moving_piece = find_valid_moves_for_piece(b, them, i, &result);
        result &= ~them->all;
        if (moving_piece != NULL) {
            while (result) {
                I8 end_sq = (I8) __builtin_ctzll(result);
                board_move_pos move_pos = {i, end_sq, 0};
                if (moving_piece == &them->pawns && ((end_sq < 8 && them == &b->black) || (end_sq > 55 && them == &b->white))) {
                    for (int j = 1; j < 5; j++) {
                        move_pos.pp_flag = j;
                        if (j == 1) {
                            j = 1;
                        }
                        test_move_w_flags(b, them, us, (board_move_pos) {i, end_sq, j}, moving_piece);
                        reset_board_all(b);
                        find_all_attacking_pieces(b);
                        if (!(them->kings & us->attacking)) { // if WE are not still attacking their king (they protected successfully)
                            unmove_piece(b, them); //
                            reset_board_all(b);
                            if (!boards_are_equal(&temp_b, b)) {
                                fprintf(stderr, "inside we are not still attacking king: boards are not equal!");
                                exit(1);
                            }
                            return 1;
                        }
                        else { // otherwise, unmove. (move invalid)
                            unmove_piece(b, them);
                            reset_board_all(b);
                            if (!boards_are_equal(&temp_b, b)) {
                                fprintf(stderr, "inside otherwise unmove: boards are not equal!");
                                exit(1);
                            }
                        }
                    }
                }
                else {
                    test_move_w_flags(b, them, us, (board_move_pos) {i, end_sq, 0}, moving_piece);
                    reset_board_all(b);
                    find_all_attacking_pieces(b);
                    if (!(them->kings & us->attacking)) { // if WE are not still attacking their king (they protected successfully)
                        unmove_piece(b, them); //
                        reset_board_all(b);
                        if (!boards_are_equal(&temp_b, b)) {
                            fprintf(stderr, "inside we are not still attacking king: boards are not equal!");
                            exit(1);
                        }
                        return 1;
                    }
                    else { // otherwise, unmove. (move invalid)
                        unmove_piece(b, them);
                        reset_board_all(b);
                        if (!boards_are_equal(&temp_b, b)) {
                            fprintf(stderr, "inside otherwise unmove: boards are not equal!");
                            exit(1);
                        }
                    }
                }
                result &= result - 1;
            }
        }
    }
    return 0;
}

//ignores en passant

U8 is_attacking_our_king(bitboard* b, piece_bitboards* us, piece_bitboards* them) {
    U64 king = us->kings;
    if (king == 0) {
        fprintf(stderr, "no king found!!! something  bad hpapened");
        exit(1);
    }
    U8 king_square = __builtin_ctzll(king);
    if (pawn_attack_mask_from_square(king, !(b->flags & TURN_FLAG)) & them->pawns) {
        return 1;
    }
    if (them->knights & b->knight_attacks[king_square]) {
        return 1;
    }
    //TODO: this can be further opitmized by creating separate function that only stores blockers
    if (get_bishop_rays(b, king_square) & (them->queens | them->bishops)) {
        return 1;
    }
    if (get_rook_rays(b, king_square) & (them->queens | them->rooks)) {
        return 1;
    }
    if (them->kings & b->king_attacks[king_square]) {
        return 1;
    }
    return 0;
}


U64* find_piece_on_pb(piece_bitboards* pb, U8 pos) {
    U64 u64_pos = 1ULL << pos;
    if (!(pb->all & u64_pos)) {
       // printf("ate blank square!\n");
        return NULL;
    }
    if (pb->pawns & u64_pos)
        return &(pb->pawns);
    if (pb->rooks & u64_pos)
        return &(pb->rooks);
    if (pb->knights & u64_pos)
        return &(pb->knights);
    if (pb->bishops & u64_pos)
        return &(pb->bishops);
    if (pb->queens & u64_pos)
        return &(pb->queens);
    if (pb->kings & u64_pos)
        return &(pb->kings);
    fprintf(stderr, "SOMETHING  BAD HAPPENED!");
    exit(1);
    return NULL;
}
U64* find_piece_on_board(bitboard* board, U8 pos) {
    U64 u64_pos = 1ULL << pos;
    if (!(board->all & u64_pos)) {
        return NULL;
    }
    U64* piece;
    piece = find_piece_on_pb(&board->white, pos);
    if (piece == NULL) {
        piece = find_piece_on_pb(&board->black, pos);
    }
    return piece;
}

static void remove_opponent_piece(U64* piece, U8 pos) {
    if (piece == NULL) {
        fprintf(stderr, "piece is null in remove_opponent_piece! returning.");
        exit(1);
    }
    U64 u64_pos = 1ULL << pos;
    if (!(*piece & u64_pos)) {
        fprintf(stderr, "remove_opponent_piece: piece didn't have position to begin with! uncalled for.");
    }
    *piece &= ~u64_pos;
}

static void unremove_opponent_piece(U64* piece, U8 pos) {
    if (piece == NULL) {
        fprintf(stderr, "piece is null in unremove_opponent_piece! returning.");
        exit(1);
    }
    U64 u64_pos = 1ULL << pos;
    *piece |= u64_pos;
}

static void move_piece_direct(U64* piece, U8 s, U8 e) {
    U64 start = 1ULL << s;
    if (!(*piece & start)) {
        fprintf(stderr, "piece doesn't contain start! returning.");
        return;
    }
    *piece &= ~start;
    *piece |= (1ULL << e);
    //print_u64(*piece);
}

static void move_piece(move* m, piece_bitboards* us) {
    U8 start_square = m->p.start_square;
    U8 end_square = m->p.end_square;
    if (m->old_flags & M_CASTLE_FLAG) { // if its castle
        if (m->old_flags & TURN_FLAG) {
            if (start_square < end_square) { // if castling to the right
                move_piece_direct(m->destroyed_piece, R_BK_IND, end_square - 1); // move rook
            }
            else {
                move_piece_direct(m->destroyed_piece, R_BQ_IND, end_square + 1);
            }
        }
        else {
            if (start_square < end_square) {
                move_piece_direct(m->destroyed_piece, R_WK_IND, end_square - 1); // move rook
            }
            else {
                move_piece_direct(m->destroyed_piece, R_WQ_IND,  end_square + 1);
            }
        }
    }
    else if (m->old_flags & M_EN_PASSANT_FLAG) {
        if (m->destroyed_piece != NULL) {
            remove_opponent_piece(m->destroyed_piece, m->ep_square);
        }
        else {
            fprintf(stderr, "move_piece: move destroyed piece for en passant is null! we're eating blank space");
            exit(1);
        }
    }
    else {
        U64* piece = GET_PP_PIECE(us, m->p.pp_flag);
        if (piece != NULL) {
            if (m->destroyed_piece != NULL) {
                remove_opponent_piece(m->destroyed_piece, end_square);
            }
            remove_opponent_piece(m->moved_piece, start_square); // first remove the pawn
            *piece |= 1ULL << end_square;
            return;
        }
        if (m->destroyed_piece != NULL) {
            remove_opponent_piece(m->destroyed_piece, end_square);
        }
    }
    move_piece_direct(m->moved_piece, start_square, end_square);
}

void add_to_move_stack(bitboard* b, U64* moved_piece, U64* destroyed_piece, U8 ep_square, board_move_pos move_pos, U8 flags) {
    //if our size exceeds max_ply
    if (b->ply_size == MAX_PLY + 1) {
        fprintf(stderr, "b->ply_size exceeded! this shoudln't hpapen!!");
        exit(1);
    }
    b->move_stack[b->ply_size] = (move){
        .moved_piece = moved_piece,
        .destroyed_piece = destroyed_piece,
        .ep_square = ep_square,
        move_pos,
        flags
    };
    b->ply_size++;
}

static void do_flags_for_move(bitboard* b, piece_bitboards* us,
    board_move_pos pos) {
    U64 start = 1ULL << pos.start_square;
    SWITCH_TURN(b);
    if (us->pawns & start) {
        if (ABS(pos.end_square - pos.start_square) == 16) {
            b->double_move_square = pos.end_square;
            b->flags |= DOUBLE_MOVE_SQUARE_IS_NOT_NULL;
        }
        else {
            b->flags &= ~DOUBLE_MOVE_SQUARE_IS_NOT_NULL;
        }
    }
    else {
        b->flags &= ~DOUBLE_MOVE_SQUARE_IS_NOT_NULL;
    }
  /*  if (!(us->pawns & start && (ABS(pos.end_square - pos.start_square) == 16))) {
        b->flags &= ~DOUBLE_MOVE_SQUARE_IS_NOT_NULL;
    }*/
    if (us->kings & start) {
        if (us == &b->white) { // if white king, do flag
            b->flags |= R_WK_FLAG | R_WQ_FLAG; // FLAG CHANGE
        }
        else {
            b->flags |= R_BK_FLAG | R_BQ_FLAG; // FLAG CHANGE
        }
    }
    U8 f = GET_FLAG_FROM_ROOK_MOV(pos.end_square);
    if (f > 0) {
        b->flags |= f;
    }
    if (us->rooks & start) {
        // if its rooks, make sure that since they moved the rook they can't castle
        f = GET_FLAG_FROM_ROOK_MOV(pos.start_square);
        if (f > 0) {
            b->flags |= f;
        }
        else {
          //  printf("rook moved already");
        }
    }
}

U64* test_move_w_flags(bitboard* b, piece_bitboards* us, piece_bitboards* them, board_move_pos p, U64* moving_piece) {
    U64* destroy_piece = NULL;
    if ((1ULL << p.end_square) & us->all) {
        fprintf(stderr, "we are hitting ourselves!!!");
        exit(1);
    }
    if (&us->pawns == moving_piece) {

        if (b->flags & DOUBLE_MOVE_SQUARE_IS_NOT_NULL && p.start_square != b->double_move_square) {
            U8 is_en_passant = 0;
            if (&b->white == us) {
                if ((b->double_move_square + 8) == p.end_square) { // if our final position is above the double move
                    is_en_passant = 1;
                }
            }
            else {
                if (b->double_move_square - 8 == p.end_square) {
                    is_en_passant = 1;
                }
            }
            if (is_en_passant) {
                destroy_piece = find_piece_on_pb(them, b->double_move_square);
                if (destroy_piece == NULL) {
                    fprintf(stderr, "oops! how could this happen! in is_en_passant, destroy_piece is null");
                    exit(1);
                }
                add_to_move_stack(b, moving_piece, destroy_piece, b->double_move_square,
                    p, b->flags | M_EN_PASSANT_FLAG);
                goto do_flags;
            }
        }
        /* else {
             if (ABS(p.end_square - p.start_square) == 16) { // if they are 16 apart (double move)
                 b->double_move_square = p.end_square;
                 b->flags |= DOUBLE_MOVE_SQUARE_IS_NOT_NULL;
             }
         }*/
    }
    if ((&us->kings == moving_piece) && (ABS(p.end_square - p.start_square) == 2)) { // meaning its castling
            add_to_move_stack(b, &us->kings, &us->rooks, b->double_move_square,
            p, b->flags | M_CASTLE_FLAG);
    }
    else {
        destroy_piece = find_piece_on_pb(them, p.end_square);
        add_to_move_stack(b, moving_piece, destroy_piece, b->double_move_square,
            p, b->flags);
    }
    do_flags:
        do_flags_for_move(b, us, p);
        if (b->ply_size == 0) {
            printf("oops! no move added to move_stack! what happened?");
            exit(1);
        }
        move* m = &b->move_stack[b->ply_size-1];
        if (m == NULL) {
            printf("oops! move is null! what do we do?");
            exit(1);
        }
        move_piece(m, us);
        return destroy_piece;
}

void unmove_piece(bitboard* b, piece_bitboards* old_us) {
    move* m = &(b->move_stack[b->ply_size-1]);
    if (m == NULL) {
        fprintf(stderr, "M IS NULL!! WHAT DO WE DO?");
        exit(1);
    }
    U8 start_square = m->p.start_square;
    U8 end_square = m->p.end_square;
    if (m->old_flags & M_CASTLE_FLAG) { // if its castle
        if (m->old_flags & TURN_FLAG) {
            if (start_square < end_square) { // if castling to the right
                move_piece_direct(m->destroyed_piece, end_square - 1, R_BK_IND); // move rook
            }
            else {
                move_piece_direct(m->destroyed_piece,  end_square + 1, R_BQ_IND);
            }
        }
        else {
            if (start_square < end_square) {
                move_piece_direct(m->destroyed_piece, end_square - 1, R_WK_IND); // move rook
            }
            else {
                move_piece_direct(m->destroyed_piece, end_square + 1, R_WQ_IND);
            }
        }
    }
    else if (m->old_flags & M_EN_PASSANT_FLAG) {
        if (m->destroyed_piece != NULL) {
            unremove_opponent_piece(m->destroyed_piece, m->ep_square);
        }
        else {
            fprintf(stderr, "unmove: move destroyed piece for en passant is null! we're eating blank space");
            exit(1);
        }
    }
    else {
        U64* piece = GET_PP_PIECE(old_us, m->p.pp_flag);
        if (piece != NULL) {
            if (m->destroyed_piece != NULL) {
                //printf("unmoving piece!\n");
                unremove_opponent_piece(m->destroyed_piece, end_square);
            }
            unremove_opponent_piece(m->moved_piece, start_square);
            *piece &= ~(1ULL << end_square);
            b->ply_size--;
            b->flags = (m->old_flags & ~(M_EN_PASSANT_FLAG | M_CASTLE_FLAG)); // reset the flags to whatever was set before the move
            return;
        }
        if (m->destroyed_piece != NULL) {
            //printf("unmoving piece!\n");
            unremove_opponent_piece(m->destroyed_piece, end_square);
        }
        b->double_move_square = m->ep_square; // set the double move square back to what it originally was
    }
    move_piece_direct(m->moved_piece, m->p.end_square, m->p.start_square);
    b->ply_size--;
    b->flags = (m->old_flags & ~(M_EN_PASSANT_FLAG | M_CASTLE_FLAG)); // reset the flags to whatever was set before the move
}

//at this point, why don't we then do all this turn processing in these results?
U8 make_move(bitboard* b) {
    piece_bitboards* us;
    piece_bitboards* them;

    I8 start_square = b->p.start_square;
    I8 end_square = b->p.end_square;

    //first set the move to be invalid
   // pos.is_valid_move = 0;

    if (b->flags & TURN_FLAG) { // if black
        us = &b->black;
        them = &b->white;
    }
    else {
        us = &b->white;
        them = &b->black;
    }
    U64 end = 1ULL << end_square;

    if (end & us->all) { // if hitting our own pieces, invalid
        fprintf(stderr, "INVALID MOVE!!\n");
        return 0;
    }

    U64 result = 0;
    U64* moving_piece = NULL;

    moving_piece = find_valid_moves_for_piece(b, us, start_square, &result);

    if (result & end) {
        if (moving_piece == NULL) {
            fprintf(stderr, "MOVING PIECE IS NULL! ABORTING!!!\n");
            exit(1);
            return 0;
        }
        ///
        /// TEMPORARILY MOVE THIS DIRECTION BEFORE CEHCKING IF CHECKED
        ///
        if (moving_piece == &us->pawns && ((end_square < 8 && us == &b->black) || (end_square > 55 && us == &b->white))) {
            if (b->p.pp_flag < 1 || b->p.pp_flag > 4) {
                b->waiting_for_pawn_promote = 1;
            }
        }
        U64* destroy_piece = test_move_w_flags(b, us, them, b->p, moving_piece);
        // temporary reset the board alls and attack moves to cehck if we are checked
        reset_board_all(b);
        find_all_attacking_pieces(b);
        printf("valid move!\n");

        //DO SOME PROCESSING, THAT CHECKS IF WE'RE CHECKED
        U8 bool_is_checked = 0;
        if (them->attacking & us->kings) {
            bool_is_checked = 1;
        }
        //if we are checked, print that and unmove
        if (bool_is_checked) {
           // print_u64(result);
            fprintf(stderr, "Can't move there, as that puts your king in danger!\n");
            unmove_piece(b, us);
            reset_board_all(b);
            return 0;
        }
        else {
            if (!does_them_have_legal_moves(b, us, them)) {
                if (us->attacking & them->kings) {
                    fprintf(stderr, "CHECKMATE! turn %d wins\n", (b->flags & TURN_FLAG));
                   // exit(1);
                }
                else {
                    fprintf(stderr, "STALEMATE! it's a tie!");
                    exit(1);
                }
            }
            printf("we aren't checked! let's continue with the flags.\n");
            CLEAR_MOVE_STACK(b);
           // do_flags_for_move(b, us, them, (board_move_pos) {start_square, end_square});
        }
    }
    else {
        fprintf(stderr, "INVALID MOVE!!\n");
        return 0;
    }
    return 1;
}

void print_u64(U64 x) {
    for (int r = 7; r >= 0; r--) {       // print rank 8 down to 1 (top to bottom)
        for (int f = 0; f < 8; f++) {    // files a-h
            int sq = r * 8 + f;          // compute bit index
            U64 mask = 1ULL << sq;
            printf("%d ", (x & mask) ? 1 : 0);
        }
        printf("\n");
    }
}

int8_t set_board_new_pos(bitboard* b, const char input[5]) {
    int x = input[0] - 'a' + ((input[1] - '1') << 3);
    if (x > INT8_MAX || x < INT8_MIN) {
        fprintf(stderr, "error! Make sure your values are in bounds!");
        return 0;
    }
    b->p.start_square = (I8) x;
    if (input[2] == 'z') {
        print_all_valid_moves(b, b->p.start_square);
        return 0;
    }
    if (input[0] - 'a' < 0 || input[0] - 'a' > 7 || input[1] - '1' < 0 || input[1] - '1' > 7 ||
        input[2] - 'a' < 0 || input[2] - 'a' > 7 || input[3] - '1' < 0 || input[3] - '1' > 7 ) {
        fprintf(stderr, "INVALID LOCATION\n");
        return 0;
    }
    x = input[2] - 'a' + ((input[3] - '1') << 3);
    if (x > INT8_MAX || x < INT8_MIN) {
        fprintf(stderr, "error! Make sure your values are in bounds!");
        return 0;
    }
    b->p.end_square = (I8) x;
    return 1;
}

void promote_pawn(U64* pawn, U64* target_piece, board_move_pos move) {
    remove_opponent_piece(pawn, move.end_square); // first remove the pawn
    *target_piece |= 1ULL << move.end_square;
}

void print_all_valid_moves(bitboard* b, U8 s) {
    piece_bitboards* us;

    if (b->flags & TURN_FLAG) { // if black
        us = &b->black;
    }
    else {
        us = &b->white;
    }
    U64 result = 0;
    find_valid_moves_for_piece(b, us, s, &result);
    result &= ~us->all;
    print_u64(result);
}