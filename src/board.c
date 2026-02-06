//
// Created by Omeed on 2/5/26.
//

#include "board.h"

#include <stdbool.h>

Board init_board() {
    Board board;
    board.held_piece = NULL;
    board.last_click_loc = (board_point) {0};
    board.top_color = COLOR_BLACK;
    board.bottom_color = COLOR_WHITE;
    if (color_equal(board.bottom_color, COLOR_WHITE)) {
        board.is_bottom_turn = true;
    }
    else {
        board.is_bottom_turn = false;
    }
    PieceType back_rank[] = {Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook};
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (i == 0) {
                board.board[i][j] = (Piece) {.type = back_rank[j], .color = board.bottom_color, .pos = (board_point) {i, j}};
            }
            else if (i == 7) {
                board.board[i][j] = (Piece) {.type = back_rank[j], .color = board.top_color, .pos = (board_point) {i, j}};
            }
            else if (i == 1) {
                board.board[i][j] = (Piece) {Pawn, .color = board.bottom_color, .pos = (board_point) {i, j}};
            }
            else if (i == 6) {
                board.board[i][j] = (Piece) {.type = Pawn, .color = board.top_color, .pos = (board_point) {i, j}};
            }
            else {
                board.board[i][j] = (Piece) {.type = 0, .color = 0, .pos = (board_point) {i, j}};
            }
        }
    }
    return board;
}

bool move_piece_on_board(Board* board, board_point start, board_point end) {
    if (!board) {
        throw_exception(NULL_POINTER, "Board is NULL");
        return false;
    }
    if (!is_board_move_valid(board, start, end)) {
        board->held_piece = NULL;
        return false;
    }
    Piece temp_piece = *board->held_piece;
    board->board[start.row][start.col].type = Blank;
    board->last_click_loc = end;
    board->held_piece = NULL;
    board->board[end.row][end.col] = temp_piece;
    board->is_bottom_turn = !board->is_bottom_turn;
    return true;
}

static bool check_rook_move(Board* board, board_point start, board_point end) {
    if (start.row - end.row == 0 || start.col - end.col == 0) { // if in single line
        //check if there aren't any pieces in the way
        for (int i = start.col + 1; i <= end.col - 1; i++) {
            if (board->board[start.row][i].type != Blank) {
                return false;
            }
        }
        for (int i = start.col - 1; i >= end.col + 1; i--) {
            if (board->board[start.row][i].type != Blank) {
                return false;
            }
        }
        for (int i = start.row + 1; i <= end.row - 1; i++) {
            if (board->board[i][start.col].type != Blank) {
                return false;
            }
        }
        for (int i = start.row - 1; i >= end.row + 1; i--) {
            if (board->board[i][start.col].type != Blank) {
                return false;
            }
        }
        return true;
    }
    return false;
}

static bool check_bishop_move(Board* board, board_point start, board_point end) {
    if (abs(start.row - end.row) == abs(start.col - end.col)) {
        if (start.row - end.row == start.col - end.col) {
            if (start.row < end.row) {
                for (int i = 1; i < end.row - start.row; i++) {
                    if (board->board[i+start.row][i+start.col].type != Blank) {
                        return false;
                    }
                }
            }
            else {
                for (int i = -1; i > end.row - start.row; i--) {
                    if (board->board[i+start.row][i+start.col].type != Blank) {
                        return false;
                    }
                }
            }
        }
        else if (start.row - end.row == end.col - start.col) {
            if (start.row < end.row) {
                for (int i = 1; i < end.row - start.row; i++) {
                    if (board->board[i+start.row][start.col - i].type != Blank) {
                        return false;
                    }
                }
            }
            else {
                for (int i = -1; i > end.row - start.row; i--) {
                    if (board->board[start.row+i][start.col - i].type != Blank) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    else {
        return false;
    }
}

bool is_board_point_in_board(board_point p) {
    return !(p.row < 0 || p.row >= 8 || p.col < 0 || p.col >= 8);
}

bool is_board_move_valid(Board* board, board_point start, board_point end) {
    if (!is_board_point_in_board(start) || !is_board_point_in_board(end)) {
        fprintf(stderr, "error, board point is outside of boudns");
        return false;
    }
    Piece* start_piece = &board->board[start.row][start.col];
    Piece* end_piece = &board->board[end.row][end.col];
    if (board->is_bottom_turn == 0 && color_equal(start_piece->color, board->bottom_color)) {
        return false;
    }
    if (board->is_bottom_turn == 1 && color_equal(start_piece->color, board->top_color)) {
        return false;
    }
    if (start.row == end.row && start.col == end.col) {
        return false;
    }
    if (board->board[end.row][end.col].type != Blank &&
        color_equal(end_piece->color, start_piece->color)) {
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
        if (color_equal(start_piece->color, board->top_color)) {
            if (start.col == end.col
                && ((start.row - end.row == 1 && end_piece->type == Blank) ||
                (start.row - end.row == 2 && start.row == 6 &&
                    end_piece->type == Blank && board->board[start.row - 1][start.col].type == Blank))) {
                return true;
            }
            if (end_piece->type != Blank && ((start.col - end.col == 1 && start.row - end.row == 1) ||
                (start.col - end.col == -1 && start.row - end.row == 1))) {
                return true;
                }
        }
        else if (color_equal(start_piece->color, board->bottom_color)) {
            if (start.col == end.col && ((end.row - start.row == 1 && end_piece->type == Blank)
            || (end.row - start.row == 2 && board->board[start.row + 1][start.col].type == Blank
            && start.row == 1 && end_piece->type == Blank))) {
                return true;
            }
            if (end_piece->type != Blank && ((start.col - end.col == 1 && start.row - end.row == -1) ||
                (start.col - end.col == -1 && start.row - end.row == -1))) {
                return true;
                }
        }
    }
    else if (start_piece->type == Bishop) {
        return check_bishop_move(board, start, end);
    }
    else if (start_piece->type == Queen) {
        return (check_rook_move(board, start, end) || check_bishop_move(board, start, end));
    }
    else if (start_piece->type == King) {
        if (abs(start.row - end.row) == 1 || abs(start.col - end.col) == 1) {
            return true;
        }
    }
    return false;
}