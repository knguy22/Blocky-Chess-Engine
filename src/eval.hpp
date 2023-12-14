#pragma once

#include "move.hpp"
#include "types.hpp"

namespace Eval {

constexpr int totalPhase = 24;

class Info {
    public:
        Info() = default;
        int getRawEval(const PieceSets& pieceSets) const;
        void addPiece(Square square, pieceTypes piece);
        void removePiece(Square square, pieceTypes piece);

    private:
        template<bool ISOPENING>
        int evalPawns(const PieceSets& pieceSets) const;
        template<bool ISOPENING, bool ISWHITE>
        int evalPassedPawns(const PieceSets& pieceSets) const;
        int mopUpScore(const PieceSets& pieceSets, int score) const;

        int opScore{};
        int egScore{};
        int phase{};
};

template<bool IS_OPENING>
int getPlacementScore(Square square, pieceTypes currPiece);
int getPiecePhase(pieceTypes piece);

constexpr std::array<int, NUM_FILES> passedPawnOp = {1};
constexpr std::array<int, NUM_FILES> passedPawnEg = {6};

constexpr std::array<int, NUM_PIECES> pieceValsOp = {0, 856, 309, 361, 391, 67, };
constexpr std::array<int, NUM_PIECES> pieceValsEg = {0, 793, 276, 282, 473, 120, };

constexpr auto tablesOp = [] {
    std::array<std::array<int, BOARD_SIZE>, 6> tables{};

    tables[WKing] = {
    45, 399, -22, -143, 556, 853, -107, -29,
    1021, 205, 116, 407, 42, 28, 227, -359,
    364, 175, -168, -25, 48, 115, -18, -186,
    -185, -243, -4, -191, -69, -23, -199, -308,
    -349, -204, -181, -123, -173, -216, -183, -278,
    -173, -122, -141, -228, -303, -215, -75, -108,
    -10, -210, -67, -139, -138, -126, -19, 39,
    -12, -7, -27, -136, 4, -112, 49, 48,
    };

    tables[WQueen] = {
    344, 423, 399, 392, 440, 416, 513, 481,
    372, 366, 393, 373, 337, 515, 380, 495,
    390, 385, 373, 419, 444, 486, 480, 418,
    385, 384, 389, 396, 400, 407, 382, 398,
    395, 383, 390, 385, 377, 386, 398, 370,
    379, 398, 386, 382, 387, 390, 405, 401,
    360, 380, 404, 403, 403, 407, 423, 380,
    380, 347, 384, 395, 401, 314, 257, 353,
    };

    tables[WBishop] = {
    111, 79, -4, 13, -20, -102, -86, 94,
    119, 126, 121, 88, 93, 177, 85, 99,
    102, 126, 147, 154, 188, 295, 152, 163,
    114, 112, 137, 179, 141, 154, 108, 90,
    100, 112, 114, 143, 158, 116, 107, 113,
    106, 113, 108, 123, 112, 124, 123, 119,
    124, 109, 120, 98, 107, 59, 146, 89,
    33, 43, 82, 72, 29, 72, -120, 120,
    };

    tables[WKnight] = {
    -205, -99, -109, 64, 254, -93, 31, -179,
    -24, 7, 67, 106, 129, 185, -41, -16,
    23, 69, 69, 121, 184, 243, 117, 44,
    33, 36, 75, 98, 67, 139, 46, 93,
    23, 27, 49, 30, 46, 51, 53, 21,
    -5, 15, 28, 55, 33, 31, 49, -15,
    -29, -24, -11, 18, 22, -6, 26, 8,
    -121, 2, -41, -30, -21, -4, 1, -31,
    };

    tables[WRook] = {
    157, 191, 220, 221, 260, 239, 234, 282,
    148, 157, 197, 203, 216, 344, 367, 313,
    131, 175, 189, 192, 269, 274, 292, 268,
    88, 131, 156, 165, 175, 196, 138, 141,
    87, 143, 117, 135, 147, 120, 164, 130,
    73, 101, 114, 113, 123, 127, 181, 125,
    59, 77, 117, 97, 94, 132, 112, 38,
    106, 116, 125, 125, 134, 124, 76, 106,
    };

    tables[WPawn] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    155, 138, 170, 182, 123, 11, -12, -85,
    -10, 31, 52, 88, 91, 118, 76, 21,
    1, 22, 8, 37, 44, 38, 39, 14,
    -4, 21, 6, 19, 22, 22, 52, 32,
    -8, 20, -3, 5, 28, 3, 90, 50,
    -21, 8, -14, -19, 9, 55, 124, 37,
    0, 0, 0, 0, 0, 0, 0, 0,
    };

    for (int piece = 0; piece < NUM_PIECES; ++piece) {
        for (auto& sqr: tables[piece]) {
            sqr += pieceValsOp[piece];
        }
    }

    return tables;
}();

// Endgame tables

constexpr auto tablesEg = [] {
    std::array<std::array<int, BOARD_SIZE>, 6> tables{};

    tables[WKing] = {
    -273, -311, 22, 62, -280, -579, 78, -64,
    -577, -81, -108, -175, -42, 29, -175, 225,
    -162, -62, 115, 44, 34, 16, 110, 132,
    51, 117, 15, 113, 94, 62, 111, 127,
    120, 87, 94, 76, 105, 121, 100, 100,
    83, 77, 78, 112, 133, 116, 57, 48,
    1, 100, 68, 80, 81, 63, 28, -16,
    -68, -12, 2, 17, -82, 5, -36, -71,
    };

    tables[WQueen] = {
    526, 529, 576, 581, 596, 622, 504, 549,
    500, 549, 562, 615, 682, 578, 671, 538,
    465, 498, 600, 551, 618, 633, 644, 606,
    468, 524, 558, 594, 614, 660, 659, 557,
    453, 520, 515, 603, 587, 560, 542, 557,
    456, 432, 516, 504, 505, 528, 526, 429,
    469, 465, 427, 445, 441, 399, 358, 365,
    444, 459, 406, 416, 391, 419, 329, 392,
    };

    tables[WBishop] = {
    145, 127, 187, 198, 177, 242, 153, 68,
    107, 140, 168, 165, 190, 127, 179, 132,
    123, 172, 163, 155, 137, 165, 160, 148,
    143, 174, 173, 170, 192, 150, 166, 173,
    103, 171, 176, 185, 174, 170, 152, 80,
    124, 140, 191, 162, 203, 157, 90, 133,
    66, 104, 94, 140, 152, 153, 116, 34,
    90, 106, 90, 121, 149, 94, 160, 28,
    };

    tables[WKnight] = {
    103, 106, 154, 59, 6, 131, 88, 20,
    83, 79, 100, 109, 45, 48, 81, 62,
    70, 107, 129, 118, 88, 66, 74, 65,
    86, 118, 136, 141, 136, 93, 115, 66,
    72, 74, 140, 133, 133, 132, 95, 62,
    37, 70, 93, 99, 125, 98, 83, 46,
    16, 27, 80, 87, 80, 112, 95, 19,
    20, -37, 34, 59, 60, 35, -26, -33,
    };

    tables[WRook] = {
    279, 290, 276, 278, 267, 268, 263, 228,
    307, 307, 294, 306, 283, 227, 221, 244,
    294, 283, 283, 276, 244, 251, 240, 228,
    293, 291, 286, 279, 265, 263, 289, 245,
    278, 256, 280, 279, 248, 258, 221, 231,
    245, 253, 249, 244, 258, 253, 219, 213,
    242, 242, 250, 258, 253, 244, 229, 217,
    244, 244, 266, 268, 256, 247, 257, 175,
    };

    tables[WPawn] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    365, 400, 339, 247, 312, 305, 344, 348,
    233, 215, 206, 151, 141, 74, 161, 158,
    111, 89, 72, 40, 27, 32, 55, 61,
    75, 63, 35, 34, 25, 36, 30, 24,
    70, 58, 41, 49, 55, 48, 29, 13,
    74, 66, 55, 20, 53, 56, 20, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    };

    for (int piece = 0; piece < NUM_PIECES; ++piece) {
        for (auto& sqr: tables[piece]) {
            sqr += pieceValsEg[piece];
        }
    }

    return tables;
}();

} // namespace eval