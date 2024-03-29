/*
* Blocky, a UCI chess engine
* Copyright (C) 2023-2024, Kevin Nguyen
*
* Blocky is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* Blocky is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program;
* if not, see <https://www.gnu.org/licenses>.
*/

#include <array>
#include <cassert>
#include <iostream>

#include "eval.hpp"
#include "pieceSets.hpp"
#include "move.hpp"
#include "bitboard.hpp"
#include "attacks.hpp"
#include "zobrist.hpp"
#include "utils/types.hpp"

namespace Eval {

int Info::getRawEval(const PieceSets& pieceSets, bool isWhiteTurn) {
    // positive values means white is winning, negative means black
    const S pawnScore = this->getPawnInfo(pieceSets).score;
    const S pieceScore = evalPieces(pieceSets, true) - evalPieces(pieceSets, false);
    const S totalScore = this->score + pawnScore + pieceScore;

    const int op = totalScore.opScore;
    const int eg = totalScore.egScore;
    const int eval = (op * this->phase + eg * (TOTAL_PHASE - this->phase)) / TOTAL_PHASE;

    const int tempoBonus = isWhiteTurn ? tempo : -tempo;
    return eval + tempoBonus;
}

void Info::addPiece(Square square, pieceTypes piece) {
    this->score += getPSQTVal(square, piece);
    this->phase += getPiecePhase(piece);
    if (piece == WPawn || piece == BPawn) {
        this->pawnKey ^= Zobrist::pieceKeys[piece][square];
    }
}

void Info::removePiece(Square square, pieceTypes piece) {
    this->score -= getPSQTVal(square, piece);
    this->phase -= getPiecePhase(piece);
    if (piece == WPawn || piece == BPawn) {
        this->pawnKey ^= Zobrist::pieceKeys[piece][square];
    }
}

const PawnHashEntry& Info::getPawnInfo(const PieceSets& pieceSets) {
    // probe pawn hash table for precomputed values
    // if not found, then compute the pawn values and replace the entry
    PawnHashEntry& entry = this->pawnHashTable[this->pawnKey % PAWN_HASH_SIZE];
    if (this->pawnKey != entry.key) {
        entry.score = S();
        entry.score += evalPawns(pieceSets, true);
        entry.score -= evalPawns(pieceSets, false);
        entry.key = this->pawnKey;
    }
    return entry;
}

S evalPawns(const PieceSets& pieceSets, bool isWhite) {
    const auto allyPawnSet = pieceSets.get(PAWN, isWhite);
    const auto enemyPawnSet = pieceSets.get(PAWN, !isWhite);

    int pawn;
    S pawnScore{};

    auto pawns = allyPawnSet;
    while (pawns) {
        pawn = popLsb(pawns);
        if (isPassedPawn(pawn, enemyPawnSet, isWhite)) {
            const int index = isWhite ? getRank(pawn) : getRank(pawn) ^ 7;
            pawnScore += passedPawn[index];
        }
    }

    const auto doubledPawnsMask = getDoubledPawnsMask(allyPawnSet, isWhite);
    pawnScore += doubledPawns * popcount(doubledPawnsMask);

    const auto chainedPawnsMask = getChainedPawnsMask(allyPawnSet, isWhite);
    pawnScore += chainedPawns * popcount(chainedPawnsMask);

    const auto phalanxPawnsMask = getPhalanxPawnsMask(allyPawnSet);
    pawnScore += phalanxPawns * popcount(phalanxPawnsMask);

    return pawnScore;
}

S evalPieces(const PieceSets& pieceSets, bool isWhite) {
    const uint64_t mobilitySquares = getMobilitySquares(pieceSets, isWhite);
    const uint64_t allPieces = pieceSets.get(ALL);
    uint64_t allyKnights = pieceSets.get(KNIGHT, isWhite);
    uint64_t allyBishops = pieceSets.get(BISHOP, isWhite);
    uint64_t allyRooks = pieceSets.get(ROOK, isWhite);

    S score{};
    Square sq;

    if (isBishopPair(allyBishops)) {
        score += bishopPair;
    }

    while (allyKnights) {
        sq = popLsb(allyKnights);
        score += knightMobility[getPieceMobility(KNIGHT, sq, mobilitySquares, allPieces)];
    }
    while (allyBishops) {
        sq = popLsb(allyBishops);
        score += bishopMobility[getPieceMobility(BISHOP, sq, mobilitySquares, allPieces)];
    }
    while (allyRooks) {
        sq = popLsb(allyRooks);
        score += rookMobility[getPieceMobility(ROOK, sq, mobilitySquares, allPieces)];
    }

    return score;
}

int getPiecePhase(pieceTypes piece) {
    switch (piece)
    {
        case WKnight:
        case BKnight:
        case WBishop:
        case BBishop:
            return 1;
        case WRook:
        case BRook:
            return 2;
        case WQueen:
        case BQueen:
            return 4;
        default:
            return 0;
    }
}

// assumes that currPiece is not empty
S getPSQTVal(Square square, pieceTypes currPiece) {
    if (currPiece >= WKing && currPiece <= WPawn) {
        return PSQT[currPiece][square];
    }
    return -PSQT[currPiece - BKing][square ^ 56];
}

bool isPassedPawn(Square pawn, uint64_t enemyPawns, bool isWhite) {
    const int file = getFile(pawn);
    const int rank = getRank(pawn);
    uint64_t adjacentEnemies = ADJ_FILES_AND_FILES_MASK[file] & enemyPawns;
    if (!adjacentEnemies) {
        return true;
    }

    const int backEnemy = isWhite ? popLsb(adjacentEnemies) : popMsb(adjacentEnemies);
    const int enemyRank = getRank(backEnemy);
    return isWhite ? rank <= enemyRank : rank >= enemyRank;
}

// gets all pawns that are in front of other ally pawns
uint64_t getDoubledPawnsMask(uint64_t allyPawnSet, bool isWhite) {
    const uint64_t forwardSquare = isWhite ? allyPawnSet >> 8 : allyPawnSet << 8;
    return allyPawnSet & forwardSquare;
}

// gets all pawns that are defending other ally pawns
uint64_t getChainedPawnsMask(uint64_t allyPawnSet, bool isWhite) {
    // prevent pawns from teleporting to other side of the board with bit shifts
    const uint64_t left  = allyPawnSet & NOT_FILE_A;
    const uint64_t right = allyPawnSet & NOT_FILE_H;

    // find ally pawns in left and right diagonal direction
    const uint64_t leftDiag = isWhite ? left >> 9 : left << 7;
    const uint64_t rightDiag = isWhite ? right >> 7 : right << 9;
    return allyPawnSet & (leftDiag | rightDiag);
}

// get all pawns that have ally pawns to the immediate left
uint64_t getPhalanxPawnsMask(uint64_t allyPawnSet) {
    const uint64_t leftSquare = (allyPawnSet & NOT_FILE_A) >> 1;
    return leftSquare & allyPawnSet;
}

// returns all squares that can be moved to (including captures) that aren't defended by enemy pawns
uint64_t getMobilitySquares(const PieceSets& pieceSets, bool isWhite) {
    const uint64_t nonAllies = ~pieceSets.get(ALL, isWhite);
    const uint64_t enemyPawns = pieceSets.get(PAWN, !isWhite);
    const uint64_t left  = enemyPawns & NOT_FILE_A;
    const uint64_t right = enemyPawns & NOT_FILE_H;
    const uint64_t attackedSquares = isWhite ?
        (left << 7) | (right << 9):
        (left >> 9) | (right >> 7);
    return nonAllies & ~attackedSquares;
}

int getPieceMobility(pieceTypes piece, Square sq, uint64_t mobilitySquares, uint64_t allPieces) {
    uint64_t movementSquares;
    switch (piece)
    {
    case KNIGHT:
        movementSquares = Attacks::knightAttacks(sq);
        break;
    case BISHOP:
        movementSquares = Attacks::bishopAttacks(sq, allPieces);
        break;
    case ROOK:
        movementSquares = Attacks::rookAttacks(sq, allPieces);
        break;
    default:
        assert(false);
    }
    return popcount(movementSquares & mobilitySquares);
}

} // namespace Eval
