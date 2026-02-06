#include "piece_type.h"

char* print_piece_path(PieceType piece) {
    switch (piece) {
        case B_Pawn: return "assets/chess-pieces/bP.png";
        case B_Knight: return "assets/chess-pieces/bN.png";
        case B_Bishop: return "assets/chess-pieces/bB.png";
        case B_Queen: return "assets/chess-pieces/bQ.png";
        case B_King: return "assets/chess-pieces/bK.png";
        case B_Rook: return "assets/chess-pieces/bR.png";
        case W_Pawn: return "assets/chess-pieces/wP.png";
        case W_Knight: return "assets/chess-pieces/wN.png";
        case W_Bishop: return "assets/chess-pieces/wB.png";
        case W_Queen: return "assets/chess-pieces/wQ.png";
        case W_King: return "assets/chess-pieces/wK.png";
        case W_Rook: return "assets/chess-pieces/wR.png";
        case Blank: return "";
        default:
            return "";
    }
}