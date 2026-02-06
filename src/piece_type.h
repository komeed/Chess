//
// Created by Omeed on 2/5/26.
//

#ifndef PIECE_TYPE_H
#define PIECE_TYPE_H

typedef enum {
    Blank, // blank is 0
    W_Pawn,
    B_Pawn,
    W_Knight,
    B_Knight,
    W_Bishop,
    B_Bishop,
    W_Queen,
    B_Queen,
    W_King,
    B_King,
    W_Rook,
    B_Rook
} PieceType;

char* print_piece_path(PieceType piece);

#endif //PIECE_TYPE_H
