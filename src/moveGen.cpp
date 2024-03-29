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

#include "moveGen.hpp"
#include "attacks.hpp"

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <iostream>

MoveList::MoveList(const Board& board) {
    // information used in both captures and quiets move generation
    this->allPieces = board.pieceSets.get(ALL);
    this->emptySquares = ~this->allPieces;
    this->isWhiteTurn = board.isWhiteTurn();

    // gather piece bitboards
    this->kings   = board.pieceSets.get(KING, board.isWhiteTurn());
    this->knights = board.pieceSets.get(KNIGHT, board.isWhiteTurn());
    this->bishops = board.pieceSets.get(BISHOP, board.isWhiteTurn());
    this->rooks   = board.pieceSets.get(ROOK, board.isWhiteTurn());
    this->queens  = board.pieceSets.get(QUEEN, board.isWhiteTurn());
    
    this->pawns   = board.pieceSets.get(PAWN, board.isWhiteTurn());
    this->promotingPawns = board.isWhiteTurn() ? this->pawns & RANK_7 : this->pawns & RANK_2;
    this->pawns ^= promotingPawns;
}

void MoveList::generateAllMoves(const Board& board) {
    this->generateCaptures(board);
    this->generateQuiets(board);
}

void MoveList::generateCaptures(const Board& board) {
    // helper information for captures
    uint64_t validDests = board.pieceSets.get(ALL, !isWhiteTurn);
    this->enPassSquare = board.enPassSquare();

    const auto knightMovesFunc = [this](Square piece, uint64_t vd) {return this->knightMoves(piece, vd);};
    const auto bishopMovesFunc = [this](Square piece, uint64_t vd) {return this->bishopMoves(piece, vd);};
    const auto rookMovesFunc = [this](Square piece, uint64_t vd) {return this->rookMoves(piece, vd);};
    const auto kingMovesFunc = [this](Square piece, uint64_t vd) {return this->kingMoves(piece, vd);};
    const auto pawnCapturesFunc = [this](Square piece, uint64_t vd) {return this->pawnCaptures(piece, vd);};
    const auto pawnPushesFunc = [this](Square piece, uint64_t vd) {return this->pawnPushes(piece, vd);};

    // regular captures
    this->generatePieceMoves(this->knights, validDests, knightMovesFunc, board);
    this->generatePieceMoves(this->bishops, validDests, bishopMovesFunc, board);
    this->generatePieceMoves(this->rooks, validDests, rookMovesFunc, board);
    this->generatePieceMoves(this->queens, validDests, bishopMovesFunc, board);
    this->generatePieceMoves(this->queens, validDests, rookMovesFunc, board);
    this->generatePieceMoves(this->kings, validDests, kingMovesFunc, board);

    // non-promotion pawn captures including en passant
    if (this->enPassSquare != NULLSQUARE) {
        validDests |= c_u64(1) << this->enPassSquare;
    }
    this->generatePieceMoves(pawns, validDests, pawnCapturesFunc, board);

    // pawn captures promotions
    this->generatePawnPromotions(this->promotingPawns, validDests, pawnCapturesFunc, board, true);
    this->generatePawnPromotions(this->promotingPawns, validDests, pawnCapturesFunc, board, false);

    // pawn pushes promotions to queens
    this->generatePawnPromotions(this->promotingPawns, this->emptySquares, pawnPushesFunc, board, true);
}

void MoveList::generateQuiets(const Board& board) {
    // helper information for quiets
    const uint64_t validDests = this->emptySquares;
    this->pawnStartRank   = board.isWhiteTurn() ? RANK_2 : RANK_7;
    this->pawnJumpRank    = board.isWhiteTurn() ? RANK_4 : RANK_5;
    this->castlingRights  = board.castlingRights();
    this->castlingRights &= board.isWhiteTurn() ? W_Castle : B_Castle;
    
    const auto knightMovesFunc = [this](Square piece, uint64_t vd) {return this->knightMoves(piece, vd);};
    const auto bishopMovesFunc = [this](Square piece, uint64_t vd) {return this->bishopMoves(piece, vd);};
    const auto rookMovesFunc = [this](Square piece, uint64_t vd) {return this->rookMoves(piece, vd);};
    const auto kingMovesFunc = [this](Square piece, uint64_t vd) {return this->kingMoves(piece, vd);};
    const auto pawnPushesFunc = [this](Square piece, uint64_t vd) {return this->pawnPushes(piece, vd);};

    // regular quiets
    this->generatePieceMoves(this->pawns, validDests, pawnPushesFunc, board);
    this->generatePieceMoves(this->knights, validDests, knightMovesFunc, board);
    this->generatePieceMoves(this->bishops, validDests, bishopMovesFunc, board);
    this->generatePieceMoves(this->rooks, validDests, rookMovesFunc, board);
    this->generatePieceMoves(this->queens, validDests, bishopMovesFunc, board);
    this->generatePieceMoves(this->queens, validDests, rookMovesFunc, board);
    this->generatePieceMoves(this->kings, validDests, kingMovesFunc, board);

    // castling
    this->generateKingCastles(board);

    // non-queen promotions
    this->generatePawnPromotions(this->promotingPawns, this->emptySquares, pawnPushesFunc, board, false);
}

template<typename Func>
void MoveList::generatePieceMoves(uint64_t pieces, uint64_t validDests, Func pieceMoves, const Board& board) {
    while (pieces) {
        const Square piece = popLsb(pieces);
        uint64_t dests = pieceMoves(piece, validDests);

        while (dests) {
            const int target = popLsb(dests);
            const Move move(piece, target);
            if (board.isLegalMove(move)) {
                this->moves.push_back(move);
            }
        }
    }
}

template<typename Func>
void MoveList::generatePawnPromotions(uint64_t pieces, uint64_t validDests, Func pieceMoves, const Board& board, const bool QUEENS) {
    const pieceTypes allyKnight = board.isWhiteTurn() ? WKnight : BKnight;
    const pieceTypes allyBishop = board.isWhiteTurn() ? WBishop : BBishop;
    const pieceTypes allyRook = board.isWhiteTurn() ? WRook : BRook;
    const pieceTypes allyQueen = board.isWhiteTurn() ? WQueen : BQueen;

    while (pieces) {
        const Square piece = popLsb(pieces);
        uint64_t dests = pieceMoves(piece, validDests);

        while (dests) {
            const int target = popLsb(dests);
            const Move move(piece, target);
            if (!board.isLegalMove(move)) {
                continue;
            }

            if (QUEENS) {
                this->moves.push_back(Move(piece, target, allyQueen));
            } else {
                this->moves.push_back(Move(piece, target, allyKnight));
                this->moves.push_back(Move(piece, target, allyRook));
                this->moves.push_back(Move(piece, target, allyBishop));
            }
        }
    }
}

void MoveList::generateKingCastles(const Board& board) {
    // assumes one king
    const Square king = lsb(this->kings);
    uint64_t dests = this->kingCastles(board.pieceSets);
    while (dests) {
        const int target = popLsb(dests);
        const Move move(king, target);
        if (board.isLegalMove(move)) {
            this->moves.push_back(move);
        }
    }
}

uint64_t MoveList::knightMoves(int square, uint64_t validDests) const {
    return Attacks::knightAttacks(square) & validDests;
}

uint64_t MoveList::bishopMoves(int square, uint64_t validDests) const {
    return Attacks::bishopAttacks(square, this->allPieces) & validDests;
}

uint64_t MoveList::rookMoves(int square, uint64_t validDests) const {
    return Attacks::rookAttacks(square, this->allPieces) & validDests;
}

uint64_t MoveList::kingMoves(int square, uint64_t validDests) const {
    return Attacks::kingAttacks(square) & validDests;
}

uint64_t MoveList::pawnCaptures(int square, uint64_t validDests) const {
    return Attacks::pawnAttacks(square, this->isWhiteTurn) & validDests;
}

uint64_t MoveList::pawnPushes(int square, uint64_t validDests) const {
    uint64_t dests{};
    const uint64_t pawn = c_u64(1) << square;
    const uint64_t currFile = FILES_MASK[getFile(square)];

    // one space forward
    if (this->isWhiteTurn) {
        dests = (pawn >> 8) & validDests;
    } else {
        dests = (pawn << 8) & validDests;
    } 
    // two spaces forward
    if (pawn & this->pawnStartRank && dests) {
        dests |= (currFile & this->pawnJumpRank) & validDests;
    }
    return dests;
}

uint64_t MoveList::kingCastles(PieceSets pieceSets) {
    // indexes based on castle rights defined in "types.hpp"
    constexpr std::array<uint64_t, 4> rookPaths = {
        0x6000000000000000, 0x0E00000000000000, 0x0000000000000060, 0x000000000000000E};
    constexpr std::array<uint64_t, 4> kingPaths = {
        0x2000000000000000, 0x0800000000000000, 0x0000000000000020, 0x0000000000000008};
    constexpr std::array<uint64_t, 4> castleDestination = {
        0x4000000000000000, 0x0400000000000000, 0x0000000000000040, 0x0000000000000004};

    uint64_t dests{};
    // test if the king is in check; castling is illegal in check
    if (!currKingInAttack(pieceSets, this->isWhiteTurn)) {
        const int kingIndex = this->isWhiteTurn ? WKing : BKing;

        while (this->castlingRights) {
            const int currRight = popLsb(this->castlingRights);

            // check for emptiness between rook and king
            if (rookPaths[currRight] & this->allPieces) {
                continue;
            }

            // check for king path being attacked
            pieceSets[kingIndex] = kingPaths[currRight];
            if (currKingInAttack(pieceSets, this->isWhiteTurn)) {
                continue;
            }

            // perform castle
            dests |= castleDestination[currRight];
        }
    }

    return dests;
}
