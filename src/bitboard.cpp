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

#include <bit>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>

#include "bitboard.hpp"

// gets index of the least significant bits
int lsb(uint64_t bitboard) {
    assert(bitboard);
    return std::countr_zero(bitboard);
}

// gets index of the most significant bit
int msb(uint64_t bitboard) {
    assert(bitboard);
    return 63 - std::countl_zero(bitboard);
}

int popLsb(uint64_t& bitboard) {
    const int pos = lsb(bitboard);
    bitboard ^= c_u64(1) << pos;
    return pos;
}

int popMsb(uint64_t& bitboard) {
    const int pos = msb(bitboard);
    bitboard ^= c_u64(1) << pos;
    return pos;
}

int popcount(uint64_t bitboard) {
    return std::popcount(bitboard);
}

uint64_t flipVertical(uint64_t bitboard) {
    constexpr uint64_t k1 = 0x00FF00FF00FF00FF;
    constexpr uint64_t k2 = 0x0000FFFF0000FFFF;
    bitboard = ((bitboard >>  8) & k1) | ((bitboard & k1) <<  8);
    bitboard = ((bitboard >> 16) & k2) | ((bitboard & k2) << 16);
    bitboard = ( bitboard >> 32)       | ( bitboard       << 32);
    return bitboard;
}

uint64_t getFileMask(int square) {
    return FILES_MASK[getFile(square)];
}

uint64_t getRankMask(int square) {
    return RANKS_MASK[getRank(square)];
}

uint64_t getDiagMask(int square) {
    return DIAGS_MASK[getRank(square) + getFile(square)];
}

uint64_t getAntiDiagMask(int square) {
    return flipVertical(DIAGS_MASK[7 - getRank(square) + getFile(square)]);
}

// below functions are for debugging and testing

void printHex(uint64_t bitboard) {
    std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex <<  bitboard << "\n" << std::dec;
}

void printBitboard(uint64_t bitboard) {
    std::cout << "------------\n";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            bool bit = c_u64(1) << (8 * i + j) & bitboard;
            std::cout << bit << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "------------\n";
}

uint64_t arrayToBitboardNotEmpty(std::array<pieceTypes, BOARD_SIZE> board) {
    uint64_t result = 0;
    for (size_t i = 0; i < board.size(); i++) {
        if (board.at(i) != EmptyPiece) {
            result |= (c_u64(1) << i);
        }
    }
    return result;
}

uint64_t arrayToBitboardPieceType(std::array<pieceTypes, BOARD_SIZE> board, pieceTypes piece) {
    uint64_t result = 0;
    for (size_t i = 0; i < board.size(); i++) {
        if (board.at(i) == piece) {
            result |= (c_u64(1) << i);
        }
    }
    return result;
}