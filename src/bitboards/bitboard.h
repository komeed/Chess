//
// Created by Omeed on 2/7/26.
//

#ifndef BITBOARD_H
#define BITBOARD_H
//for pawn promotion, assume everything

#include "bishop_rays.h"
#include "rook_rays.h"
#include "macro_utils.h"
#include <stdlib.h>

#include "dynamic_array.h"

#define TURN_FLAG              0b00000001
#define DOUBLE_MOVE_SQUARE_IS_NOT_NULL      0b00000010 // if 1, its not null, if 0, it is null
//#define EP_FLAG                0b00000010
//#define DOUBLE_MOVE_FLAG       0b00000100
// for rook flags, if on, that means they are moved
#define R_WK_FLAG              0b00000100
#define R_WQ_FLAG              0b00001000
#define R_BK_FLAG              0b00010000
#define R_BQ_FLAG              0b00100000
#define M_CASTLE_FLAG       0b01000000
#define M_EN_PASSANT_FLAG   0b10000000
//#define DOUBLE_MOVE_ONOFF_FLAG 0b10000000

//#define MOVE_CASTLE_FLAG 0b1
//#define MOVE_EN_PASSANT_FLAG 0b10
//#define MOVE_COLOR_FLAG 0b100 // if 1, black, if 0, white (same as turn)
//#define MOVE_IS_BLANK_FLAG 0b1000

//whichever one is on is the one that it's promoting to
#define M_PP_Q 0b00000001
#define M_PP_R 0b00000010
#define M_PP_B 0b00000100
#define M_PP_N 0b00001000

// CHECKED 0b1

#define R_WK_LOC 0x0000000000000080ULL
#define R_WQ_LOC 0x0000000000000001ULL
#define R_BK_LOC 0x8000000000000000ULL
#define R_BQ_LOC 0x0100000000000000ULL

#define R_WK_IND 7
#define R_WQ_IND 0
#define R_BK_IND 63
#define R_BQ_IND 56

#define WK_IND 4
#define BK_IND 60

#define CLEAR_MOVE_STACK(b) ((b)->ply_size = 0)

#define SWITCH_TURN(b) ((b)->flags ^= TURN_FLAG)

typedef struct {
    U64 pawns;
    U64 rooks;
    U64 knights;
    U64 bishops;
    U64 queens;
    U64 kings;

    U64 all; // unified pieces
    U64 attacking; // pieces that are attacking (able to attack)
} piece_bitboards;

typedef struct {
    U64* moved_piece;
    U64* destroyed_piece; // if castle, this is us->rooks
    U8 ep_square; // for en passant (if it's en passant, this is the destroyed piece square)
    //also acts as the double move square when it's not en passant
    board_move_pos p;
    U8 old_flags;
    U8 pawn_promote_flags;
} move;

typedef struct {
    piece_bitboards white;
    piece_bitboards black;

    U64 all; //all pieces in total
    bishop_rays* b_rays;
    rook_rays* r_rays;
    U64 knight_attacks[64];
    U64 king_attacks[64];

    //movement flags
    board_move_pos p;
    U8 double_move_square; // square that previously held double up
    U8 flags;
    move move_stack[MAX_PLY];
    U8 ply_size;
    U64 attacking;

    //dummy matrix of legal moves that corresponds to each depth (works without multithreading)
    scores_for_legal_moves legal_moves_array[MAX_PLY];

    //minimax
    //scores_for_legal_moves* l_moves;
    U8 use_ab_pruning;
    I32 count;
} bitboard;

//TODO: don't pass them, make it optimized
void find_all_attacking_pieces(bitboard* b);

void find_all_attacking_pieces_and_reset_all(bitboard* b);

U64* find_piece_on_pb(piece_bitboards* pb, U8 pos);
U64* find_piece_on_board(bitboard* board, U8 pos);

void reset_board_all(bitboard* b);

U8 boards_are_equal(bitboard* b1, bitboard* b2);

U64* find_valid_moves_for_piece(bitboard* b, piece_bitboards* us, U8 s, U64* result);

U8 is_attacking_our_king(bitboard* b, piece_bitboards* us, piece_bitboards* them);

int8_t set_board_new_pos(bitboard* b, const char input[5]);

bitboard init_board_bitboards(bishop_rays* b_rays, rook_rays* r_rays, U8 use_minimax);

void add_to_move_stack(bitboard* b, U64* moved_piece, U64* destroyed_piece, U8 ep_square, U8 start_square,
    U8 end_square, U8 flags, U8 pawn_promote_flags);

void unmove_piece(bitboard* b);

U64* test_move_w_flags(bitboard* b, piece_bitboards* us, piece_bitboards* them, board_move_pos p, U64* moving_piece);

void print_bitboard(bitboard* b);

void print_bitboard_values(bitboard* b);

void print_all_valid_moves(bitboard* b, U8 s);

void print_u64(U64 x);

U8 make_move(bitboard* b);

inline U8 is_white_turn(bitboard* b) { return !(b->flags & TURN_FLAG);}
#endif //BITBOARD_H