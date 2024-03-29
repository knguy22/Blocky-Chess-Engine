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

#pragma once

#include <string>

#include "utils/types.hpp"

inline constexpr Square NULLSQUARE = 0xFF;
inline constexpr uint16_t NULLMOVE = 0xFFFF;

class Move {
    public:
        Move() : data(NULLMOVE) {};
        Move(Square square1, Square square2, pieceTypes promotePiece = EmptyPiece);
        Move(std::string input, bool isWhiteTurn);
        std::string toStr() const;

        Square sqr1() const;
        Square sqr2() const;
        pieceTypes promotePiece() const;

        operator bool() const;
        bool operator==(const Move& rhs) const;
        bool operator!=(const Move& rhs) const;
        friend std::ostream& operator<<(std::ostream& os, const Move& obj);
    private:
        static pieceTypes toPieceType(int integer);
        static int toInt(pieceTypes piece);

        uint16_t data{};
};

std::string pieceToStr(pieceTypes piece);
std::string sqrToStr(Square square);
Square toSquare(int rank, int file);
Square toSquare(std::string input);