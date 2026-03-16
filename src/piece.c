//
// Created by Omeed on 2/6/26.
//
#include "piece.h"

bool color_equal(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

 char* print_piece_path(PieceType piece, Color color) {
    if (color_equal(color, COLOR_WHITE)) {
        switch (piece) {
            case Pawn: return "assets/chess-pieces/wP.png";
            case Knight: return "assets/chess-pieces/wN.png";
            case Bishop: return "assets/chess-pieces/wB.png";
            case Queen: return "assets/chess-pieces/wQ.png";
            case King: return "assets/chess-pieces/wK.png";
            case Rook: return "assets/chess-pieces/wR.png";
            case Blank: return "";
            default:
                return "";
        }
    }
    else {
        switch (piece) {
            case Pawn: return "assets/chess-pieces/bP.png";
            case Knight: return "assets/chess-pieces/bN.png";
            case Bishop: return "assets/chess-pieces/bB.png";
            case Queen: return "assets/chess-pieces/bQ.png";
            case King: return "assets/chess-pieces/bK.png";
            case Rook: return "assets/chess-pieces/bR.png";
            case Blank: return "";
            default:
                return "";
        }
    }
}
char* print_piece_path_from_board(bitboard* b, I8 p) {

    U64* piece = find_piece_on_board(b, p);

    if (!piece) return "";

    /* -------- WHITE PIECES -------- */
    if (*piece & b->white.all) {

        if (piece == &b->white.pawns)   return "assets/chess-pieces/wP.png";
        if (piece == &b->white.knights) return "assets/chess-pieces/wN.png";
        if (piece == &b->white.bishops) return "assets/chess-pieces/wB.png";
        if (piece == &b->white.rooks)   return "assets/chess-pieces/wR.png";
        if (piece == &b->white.queens)  return "assets/chess-pieces/wQ.png";
        if (piece == &b->white.kings)    return "assets/chess-pieces/wK.png";

        return "";
    }

    /* -------- BLACK PIECES -------- */
    if (*piece & b->black.all) {

        if (piece == &b->black.pawns)   return "assets/chess-pieces/bP.png";
        if (piece == &b->black.knights) return "assets/chess-pieces/bN.png";
        if (piece == &b->black.bishops) return "assets/chess-pieces/bB.png";
        if (piece == &b->black.rooks)   return "assets/chess-pieces/bR.png";
        if (piece == &b->black.queens)  return "assets/chess-pieces/bQ.png";
        if (piece == &b->black.kings)    return "assets/chess-pieces/bK.png";

        return "";
    }

    return "";
}
char* print_piece_path_from_board_piece(bitboard* b, U64* piece) {
    if (!piece) return "";

    /* -------- WHITE PIECES -------- */
    if (*piece & b->white.all) {

        if (piece == &b->white.pawns)   return "assets/chess-pieces/wP.png";
        if (piece == &b->white.knights) return "assets/chess-pieces/wN.png";
        if (piece == &b->white.bishops) return "assets/chess-pieces/wB.png";
        if (piece == &b->white.rooks)   return "assets/chess-pieces/wR.png";
        if (piece == &b->white.queens)  return "assets/chess-pieces/wQ.png";
        if (piece == &b->white.kings)    return "assets/chess-pieces/wK.png";

        return "";
    }

    /* -------- BLACK PIECES -------- */
    if (*piece & b->black.all) {

        if (piece == &b->black.pawns)   return "assets/chess-pieces/bP.png";
        if (piece == &b->black.knights) return "assets/chess-pieces/bN.png";
        if (piece == &b->black.bishops) return "assets/chess-pieces/bB.png";
        if (piece == &b->black.rooks)   return "assets/chess-pieces/bR.png";
        if (piece == &b->black.queens)  return "assets/chess-pieces/bQ.png";
        if (piece == &b->black.kings)    return "assets/chess-pieces/bK.png";

        return "";
    }

    return "";
}

Piece get_piece_from_board(bitboard* b, U64* piece) {
    if (!piece) {
        fprintf(stderr, "piece is null! EXITTING!");
        exit(1);
    }

    /* -------- WHITE PIECES (side = 0) -------- */
    if (piece == &b->white.pawns)   return (Piece){Pawn,   0};
    if (piece == &b->white.knights) return (Piece){Knight, 0};
    if (piece == &b->white.bishops) return (Piece){Bishop, 0};
    if (piece == &b->white.rooks)   return (Piece){Rook,   0};
    if (piece == &b->white.queens)  return (Piece){Queen,  0};
    if (piece == &b->white.kings)   return (Piece){King,   0};

    /* -------- BLACK PIECES (side = 1) -------- */
    if (piece == &b->black.pawns)   return (Piece){Pawn,   1};
    if (piece == &b->black.knights) return (Piece){Knight, 1};
    if (piece == &b->black.bishops) return (Piece){Bishop, 1};
    if (piece == &b->black.rooks)   return (Piece){Rook,   1};
    if (piece == &b->black.queens)  return (Piece){Queen,  1};
    if (piece == &b->black.kings)   return (Piece){King,   1};

    fprintf(stderr, "piece doesn't exist inside board! must be bad.\n");
    exit(1);
}
