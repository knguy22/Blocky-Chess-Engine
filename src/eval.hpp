#pragma once

#include "types.hpp"

namespace Eval {

constexpr int totalPhase = 24;

struct Info {
    Info() = default;
    int getRawEval() const;
    void addPiece(int rank, int file, pieceTypes piece);
    void removePiece(int rank, int file, pieceTypes piece);

    uint8_t piecesRemaining = 0;
    int totalMaterial = 0; 
    int materialDifference = 0;
    int opScore = 0;
    int egScore = 0;
    int phase = 0;
};

int getPiecePhase(pieceTypes piece);
int getPlacementScoreOp(int rank, int file, pieceTypes currPiece);
int getPlacementScoreEg(int rank, int file, pieceTypes currPiece);
void init();

// opening tables

constexpr std::array<int, BOARD_SIZE> tableKingOp = {
    0 , -30, -30, -30, -30, -30, -30, 0 ,
    0 , -30, -30, -30, -30, -30, -30, 0 ,
    0 , -30, -30, -30, -30, -30, -30, 0 ,
    0 , -20, -30, -50, -50, -30, -20, 0 ,
    0 , -20, -30, -50, -50, -30, -20, 0 ,
    0 , -15, -30, -30, -30, -30, -15, 0 ,
    10, -10, -15, -15, -15, -15, -15, 10,
    20, 20, 10, 10 , 10, 10, 20, 20 
};

constexpr std::array<int, BOARD_SIZE> tableQueenOp = {
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 ,
    0 , 0 , 3 , 3 , 3 , 3 , 0 , 0 ,
    0 , 3 , 3 , 3 , 3 , 3 , 3 , 0 ,
    0 , 3 , 3 , 3 , 3 , 3 , 3 , 0 ,
    0 , 5 , -3, 5 , 5 , -3, 5 , 0 ,
    0 , 2 , 2 , 1 , 1 , 2 , 2 , 0 ,
    -9, -4, -4, 2 , 2 , -4, -4, -9
};

constexpr std::array<int, BOARD_SIZE> tableBishopOp = {
    -20, -10, -5, -5, -5, -5, -10, -20,
    -10, -5, 5 , 5 , 5 , 5 , -5, -10,
    -5, 5 , 10, 15, 15, 10, 5 , -5,
    -5, 5 , 15, 15, 15, 15, 5 , -5,
    -5, 5 , 15, 15, 15, 15, 5 , -5,
    -5, 15, 15, 15, 15, 15, 15, -5,
    -10, 10, 5 , 5 , 5 , 5 , 10, -10,
    -20, -5, -10, -5, -5, -10, -5, -20,
};

constexpr std::array<int, BOARD_SIZE> tableKnightOp = {
    -20, -10, -5, -5, -5, -5, -10, -20,
    -10, -5, 0 , 5 , 5 , 0 , -5, -10,
    -5, 5 , 15, 15, 15, 15, 5 , -5,
    -5, 5 , 15, 18, 18, 15, 5 , -5,
    -5, 5 , 15, 18, 18, 15, 5 , -5,
    -5, 5 , 15, 15, 15, 15, 5 , -5,
    -10, 0, 5 , 5 , 5 , 5 , 0, -10,
    -25, -20, -10, -10, -10, -10, -20, -25,
};

constexpr std::array<int, BOARD_SIZE> tableRookOp = {
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    20, 20, 20, 20, 20, 20, 20, 20,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 5 , 5 , 5 , 5 , 5 , 5 , 0 ,
    0 , 0 , 10, 10, 10, 10, 0 , 0 
};

constexpr std::array<int, BOARD_SIZE> tablePawnOp = {
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    20, 20, 25, 30, 30, 25, 20, 20,
    10, 10, 15, 25, 25, 15, 10, 10,
    0 , 5 , 10, 28, 28, 10, 5 , 0 ,
    0 , 0 , 6 , 19, 19, 6 , 0 , 0 , 
    0 , 5 , 3 , 5 , 5 , 3 , 5 , 0 ,
    0 , 7 , 0 , -10, -10, 0 , 7,  0 , 
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 
};

// Endgame tables

constexpr std::array<int, BOARD_SIZE> tableKingEg = {
    -20, -15, -7, 0 , 0 , -7, -15, -20,
    -15, -7, 0 , 15 , 15 , 0 , -7, -15,
    0 , 12, 20, 23, 23, 20, 12, 0 ,
    0 , 15, 25, 28, 28, 25, 15, 0 ,
    0 , 15, 25, 28, 28, 25, 15, 0 ,
    -7, 5 , 7 , 15, 15, 7 , 5 , -7,
    -15, -7, 0 , 5 , 5 , 0 , -7, -15,
    -20, -15, -7, 0 , 0 , -7, -15, -20 
};

constexpr std::array<int, BOARD_SIZE> tableQueenEg = {
    -5, -5, -5, -5, -5, -5, -5, -5,
    -5, 0 , 0 , 0 , 0 , 0 , 0 , -5,
    -5, 0 , 5 , 5 , 5 , 5 , 0 , -5,
    -5, 0 , 5 , 15, 15, 5 , 0 , -5,
    -5, 0 , 5 , 15, 15, 5 , 0 , -5,
    -5, 0 , 5 , 5 , 5 , 5 , 0 , -5,
    -5, 0 , 0 , 0 , 0 , 0 , 0 , -5,
    -5, -5, -5, -5, -5, -5, -5, -5
};

constexpr std::array<int, BOARD_SIZE> tableBishopEg = {
    -10, -7, -5, -5, -5, -5, -7, -10,
    -7, 3, 4 , 5 , 5 , 4 , 3, -7,
    -5, 4 , 10, 15, 15, 10, 4 , -5,
    -5, 5 , 15, 15, 15, 15, 5 , -5,
    -5, 5 , 15, 15, 15, 15, 5 , -5,
    -5, 4 , 10, 15, 15, 10, 4 , -5,
    -7, 3 , 4 , 5 , 5 , 4 , 3 , -7,
    -10, -7, -5, -5, -5, -5, -7, -10,
};

constexpr std::array<int, BOARD_SIZE> tableKnightEg = {
    -20, -10, -5, -5, -5, -5, -10, -20,
    -10, -5, 0 , 5 , 5 , 0 , -5, -10,
    -5, 5 , 15, 15, 15, 15, 5 , -5,
    -5, 5 , 15, 18, 18, 15, 5 , -5,
    -5, 5 , 15, 18, 18, 15, 5 , -5,
    -5, 5 , 15, 15, 15, 15, 5 , -5,
    -10, 0, 5 , 5 , 5 , 5 , 0, -10,
    -25, -20, -10, -10, -10, -10, -20, -25,
};

constexpr std::array<int, BOARD_SIZE> tableRookEg = {
    10, 10, 10, 10, 10, 10, 10, 10,
    20, 20, 20, 20, 20, 20, 20, 20,
    10, 10, 10, 10, 10, 10, 10, 10,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    0 , 5 , 5 , 5 , 5 , 5 , 5 , 0 ,
    0 , 0 , 10, 10, 10, 10, 0 , 0 
};

constexpr std::array<int, BOARD_SIZE> tablePawnEg = {
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    120, 110, 100, 100, 100, 100, 110, 120,
    80, 75, 71, 71, 71, 71, 75, 80,
    50, 47, 44, 44, 44, 44, 47, 50,
    31, 29, 27, 27, 27, 27, 29, 31, 
    12, 11, 10, 10, 10, 10, 11, 12,
    2 , 2 , 2 , 2 , 2 , 2 , 2,  2 , 
    0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
};

extern std::array<std::array<int, BOARD_SIZE>, 6> tablesOp;
extern std::array<std::array<int, BOARD_SIZE>, 6> tablesEg; 

} // namespace eval