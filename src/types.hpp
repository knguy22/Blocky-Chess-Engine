#pragma once

#include <map>

enum fileVals {nullFile = -1, A, B, C, D, E, F, G, H};

enum pieceTypes {nullPiece = -1, EmptyPiece,
                WKing, WQueen, WBishop, WKnight, WRook, WPawn, 
                BKing, BQueen, BBishop, BKnight, BRook, BPawn,};

enum castleRights {
    noCastle = 0, 
    W_OO = 1, 
    W_OOO = W_OO << 1,
    B_OO = W_OO << 2,
    B_OOO = W_OO << 3,

    W_Castle = W_OO | W_OOO,
    B_Castle = B_OO | B_OOO,
    All_Castle = W_Castle | B_Castle,
};


//not sure if this should be here but it seems utility stuff belongs here
static std::map<pieceTypes, int> pieceValues = {
    {WPawn, 1},
    {BPawn, -1},
    {WBishop, 3}, 
    {BBishop, -3}, 
    {WKnight, 3}, 
    {BKnight, -3}, 
    {WRook, 5}, 
    {BRook, -5}, 
    {WQueen, 9},
    {BQueen, -9}
};


inline castleRights operator&(castleRights lhs, castleRights rhs) {
    return static_cast<castleRights>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

inline castleRights operator^(castleRights lhs, castleRights rhs) {
    return static_cast<castleRights>(static_cast<int>(lhs) ^ static_cast<int>(rhs));
}

inline castleRights& operator&=(castleRights& lhs, castleRights rhs) {
    return lhs = lhs & rhs;
}

inline castleRights& operator^=(castleRights& lhs, castleRights rhs) {
    return lhs = lhs ^ rhs;
}
