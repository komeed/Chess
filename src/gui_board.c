//
// Created by Omeed on 2/5/26.
//

#include "gui_board.h"

#include <stdbool.h>

gui_board init_board(bishop_rays* b_rays, rook_rays* r_rays) {
    gui_board board;
    board.bitboard_ptr = init_board_bitboards(b_rays, r_rays, 1);
    //board.held_piece = NULL;
    board.held_bit_piece = NULL;
    board.held_piece_pos = (board_move_pos) {-1, -1, 0};
    board.top_color = COLOR_BLACK;
    board.bottom_color = COLOR_WHITE;
    return board;
}

bool move_piece_on_board(gui_board* board, board_move_pos m) {
    if (!board) {
        throw_exception(NULL_POINTER, "Board is NULL");
        return false;
    }
    board->bitboard_ptr->p = m;
    if (board->bitboard_ptr->waiting_for_pawn_promote || !make_move(board->bitboard_ptr)) {
        board->held_bit_piece = NULL;
        return false;
    }
    if (board->bitboard_ptr->waiting_for_pawn_promote) {
        board->bitboard_ptr->flags ^= TURN_FLAG;
        return true;
    }
   // board_move_trace trace = mm_find_next_move_trace(&board->bitboard);
   // print_board_move_trace(&trace);
    if (board->bitboard_ptr->use_minimax_on_black) {
        bm_pos_w_score ps = mm_find_next_pos(board->bitboard_ptr);
        print_bm_w_score(ps);
        board->bitboard_ptr->p = ps.p;
        make_move(board->bitboard_ptr);
    }
    print_bitboard(board->bitboard_ptr);
    board->held_bit_piece = NULL;
   /* U64* temp_piece = board->held_bit_piece;


    if (!is_board_move_valid(board, start, end)) {
        board->held_piece = NULL;
        return false;
    }
    Piece temp_piece = *board->held_piece;
    board->board[start.row][start.col].type = Blank;
    board->last_click_loc = end;
    board->held_piece = NULL;
    board->board[end.row][end.col] = temp_piece;
    board->is_bottom_turn = !board->is_bottom_turn;*/
    return true;
}
/*
static bool check_rook_move(gui_board* board, board_point start, board_point end) {
    int dr = end.row - start.row;
    int dc = end.col - start.col;

    // must move in a straight line
    if (dr != 0 && dc != 0)
        return false;

    int step_r = (dr > 0) - (dr < 0); // 1, 0, or -1
    int step_c = (dc > 0) - (dc < 0); // 1, 0, or -1

    int steps = (dr != 0) ? abs(dr) : abs(dc);

    // check intermediate squares
    for (int i = 1; i < steps; i++) {
        if (board->board[start.row + i * step_r]
                        [start.col + i * step_c].type != Blank) {
            return false;
                        }
    }

    return true;
}

static bool check_bishop_move(gui_board* board, board_point start, board_point end) {
    int dr = end.row - start.row;
    int dc = end.col - start.col;

    // must be diagonal
    if (abs(dr) != abs(dc))
        return false;

    int step_r = (dr > 0) ? 1 : -1;
    int step_c = (dc > 0) ? 1 : -1;

    int steps = abs(dr);

    // check intermediate squares (exclude start and end)
    for (int i = 1; i < steps; i++) {
        if (board->board[start.row + i * step_r]
                        [start.col + i * step_c].type != Blank) {
            return false;
                        }
    }

    return true;
}

bool is_board_point_in_board(board_point p) {
    return !(p.row < 0 || p.row >= 8 || p.col < 0 || p.col >= 8);
}

bool is_board_move_valid(gui_board* board, board_point start, board_point end) {
    if (!is_board_point_in_board(start) || !is_board_point_in_board(end)) {
        fprintf(stderr, "error, board point is outside of boudns");
        return false;
    }
    Piece* start_piece = &board->board[start.row][start.col];
    Piece* end_piece = &board->board[end.row][end.col];
    if (board->is_bottom_turn == 0 && start_piece->side == 0) {
        return false;
    }
    if (board->is_bottom_turn == 1 && start_piece->side == 1) {
        return false;
    }
    if (start.row == end.row && start.col == end.col) {
        return false;
    }
    if (board->board[end.row][end.col].type != Blank && end_piece->side == start_piece->side) {
        return false; // if they are the esame color
    }
    if (start_piece->type == Knight) {
        if (abs(start.row - end.row) == 2 && abs(start.col - end.col) == 1 ||
            abs(start.row - end.row) == 1 && abs(start.col - end.col) == 2) {
            return true;
        }
    }
    else if (start_piece->type == Rook) {
        return check_rook_move(board, start, end);
    }
    else if (start_piece->type == Pawn) {
        int dr = end.row - start.row;
        int dc = end.col - start.col;

        int dir = (start_piece->side == 1) ? -1 : 1;
        int start_row = (dir == -1) ? 6 : 1;

        // forward move
        if (dc == 0) {
            // 1 step
            if (dr == dir && end_piece->type == Blank)
                return true;

            // 2 steps from starting rank
            if (dr == 2*dir &&
                start.row == start_row &&
                end_piece->type == Blank &&
                board->board[start.row + dir][start.col].type == Blank)
                return true;
        }

        // capture
        if (abs(dc) == 1 &&
            dr == dir &&
            end_piece->type != Blank)
        {
            return true;
        }
    }
    else if (start_piece->type == Bishop) {
        return check_bishop_move(board, start, end);
    }
    else if (start_piece->type == Queen) {
        return (check_rook_move(board, start, end) || check_bishop_move(board, start, end));
    }
    else if (start_piece->type == King) {
        int dr = abs(start.row - end.row);
        int dc = abs(start.col - end.col);
        if (dr <= 1 && dc <= 1 && (dr + dc) > 0) {
            return true;
        }
    }
    return false;
}*/