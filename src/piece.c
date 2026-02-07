//
// Created by Omeed on 2/6/26.
//
#include "piece.h"

#include "board.h"

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