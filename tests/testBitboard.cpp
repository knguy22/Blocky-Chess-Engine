#include "attacks.hpp"
#include "bitboard.hpp"
#include "move.hpp"

#include <gtest/gtest.h>
#include <array>
#include <bitset>
#include <cstdint>

class BitboardTest : public testing::Test {
    public:
        static void SetUpTestSuite() {
            Attacks::init();
        }
};

TEST_F(BitboardTest, msbOne) {
    uint64_t bitboard = c_u64(1);
    ASSERT_EQ(msb(bitboard), 0);
}

TEST_F(BitboardTest, msbTrailingOne) {
    uint64_t bitboard = 0x8000000000000000;
    ASSERT_EQ(msb(bitboard), 63);
}

TEST_F(BitboardTest, msbMiddleOne) {
    uint64_t bitboard = 0x0000000000001000;
    ASSERT_EQ(msb(bitboard), 12);
}

TEST_F(BitboardTest, msbMultOnes) {
    uint64_t bitboard = 0x0000080010000000;
    ASSERT_EQ(msb(bitboard), 43);
}

TEST_F(BitboardTest, lsbOne) {
    uint64_t bitboard = 0x8000000000000000;
    ASSERT_EQ(lsb(bitboard), 63);
}

TEST_F(BitboardTest, lsbForwardOne) {
    uint64_t bitboard = 0x0000000000000001;
    ASSERT_EQ(lsb(bitboard), 0);
}

TEST_F(BitboardTest, lsbMiddleOne) {
    uint64_t bitboard = 0x0008000000000000;
    ASSERT_EQ(lsb(bitboard), 51);
}

TEST_F(BitboardTest, lsbMultOnes) {
    uint64_t bitboard = 0x0000001000100000;
    ASSERT_EQ(lsb(bitboard), 20);
}

TEST_F(BitboardTest, popLsb1) {
    uint64_t bitboard = 0x0000001000100000;
    ASSERT_EQ(popLsb(bitboard), 20);
    ASSERT_EQ(bitboard, 0x0000001000000000);
}

TEST_F(BitboardTest, popMsb1) {
    uint64_t bitboard = 0x0000080010000000;
    ASSERT_EQ(popMsb(bitboard), 43);
    ASSERT_EQ(bitboard, 0x0000000010000000);
}

TEST_F(BitboardTest, FlipVerticalEmpty) {
    uint64_t bitboard = 0x0000000000000000;
    ASSERT_EQ(flipVertical(bitboard), 0);
}

TEST_F(BitboardTest, FlipVerticalOne) {
    uint64_t bitboard = 0x0000000000000001;
    uint64_t expected = 0x0100000000000000;
    ASSERT_EQ(flipVertical(bitboard), expected);
}

TEST_F(BitboardTest, FlipVerticalMul) {
    uint64_t bitboard = 0x0000000000008421;
    uint64_t expected = 0x2184000000000000;
    ASSERT_EQ(flipVertical(bitboard), expected);
}

TEST_F(BitboardTest, getFileMaskTest) {
    EXPECT_EQ(getFileMask(0), FILE_A);
    EXPECT_EQ(getFileMask(1), FILE_B);
    EXPECT_EQ(getFileMask(2), FILE_C);
    EXPECT_EQ(getFileMask(3), FILE_D);
    EXPECT_EQ(getFileMask(4), FILE_E);
    EXPECT_EQ(getFileMask(5), FILE_F);
    EXPECT_EQ(getFileMask(6), FILE_G);
    EXPECT_EQ(getFileMask(7), FILE_H);
    EXPECT_EQ(getFileMask(8), FILE_A);
}

TEST_F(BitboardTest, getRankMaskTest) {
    EXPECT_EQ(getRankMask(0), RANK_8);
    EXPECT_EQ(getRankMask(8), RANK_7);
    EXPECT_EQ(getRankMask(16), RANK_6);
    EXPECT_EQ(getRankMask(24), RANK_5);
    EXPECT_EQ(getRankMask(32), RANK_4);
    EXPECT_EQ(getRankMask(40), RANK_3);
    EXPECT_EQ(getRankMask(48), RANK_2);
    EXPECT_EQ(getRankMask(56), RANK_1);
    EXPECT_EQ(getRankMask(5), RANK_8);
}

TEST_F(BitboardTest, getDiagMaskTest) {
    EXPECT_EQ(getDiagMask(0 ), DIAG_0);
    EXPECT_EQ(getDiagMask(8 ), DIAG_1);
    EXPECT_EQ(getDiagMask(16), DIAG_2);
    EXPECT_EQ(getDiagMask(24), DIAG_3);
    EXPECT_EQ(getDiagMask(32), DIAG_4);
    EXPECT_EQ(getDiagMask(40), DIAG_5);
    EXPECT_EQ(getDiagMask(48), DIAG_6);
    EXPECT_EQ(getDiagMask(56), DIAG_7);
    EXPECT_EQ(getDiagMask(57), DIAG_8);
    EXPECT_EQ(getDiagMask(58), DIAG_9);
    EXPECT_EQ(getDiagMask(59), DIAG_10);
    EXPECT_EQ(getDiagMask(60), DIAG_11);
    EXPECT_EQ(getDiagMask(61), DIAG_12);
    EXPECT_EQ(getDiagMask(62), DIAG_13);
    EXPECT_EQ(getDiagMask(63), DIAG_14);
    EXPECT_EQ(getDiagMask(1 ), DIAG_1);
}

TEST_F(BitboardTest, knightAttacks1) {
    uint64_t knightBitboard = Attacks::knightAttacks(toSquare("a8"));
    EXPECT_EQ(knightBitboard, 0x0000000000020400);
}

TEST_F(BitboardTest, knightAttacks2) {
    uint64_t knightBitboard = Attacks::knightAttacks(toSquare("b5"));
    EXPECT_EQ(knightBitboard, 0x0000050800080500);
}

TEST_F(BitboardTest, knightAttacks3) {
    uint64_t knightBitboard = Attacks::knightAttacks(toSquare("b3"));
    EXPECT_EQ(knightBitboard, 0x0508000805000000);
}

TEST_F(BitboardTest, knightAttacks4) {
    uint64_t knightBitboard = Attacks::knightAttacks(toSquare("c2"));
    EXPECT_EQ(knightBitboard, 0x1100110A00000000);
}

TEST_F(BitboardTest, knightAttacks5) {
    uint64_t knightBitboard1 = Attacks::knightAttacks(toSquare("d2"));
    uint64_t knightBitboard2 = Attacks::knightAttacks(toSquare("e1"));
    EXPECT_EQ(knightBitboard1 | knightBitboard2, 0x22442A1400000000);
}

TEST_F(BitboardTest, pawnAttackersTrue1) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t pawnBitboard = Attacks::pawnAttacks(toSquare("c3"), true);
    ASSERT_TRUE(square & pawnBitboard);
}

TEST_F(BitboardTest, pawnAttackersTrue2) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t pawnBitboard = Attacks::pawnAttacks(toSquare("e3"), true);
    ASSERT_TRUE(square & pawnBitboard);
}

TEST_F(BitboardTest, pawnAttackersTrue3) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t pawnBitboard = Attacks::pawnAttacks(toSquare("c5"), false);
    ASSERT_TRUE(square & pawnBitboard);
}

TEST_F(BitboardTest, pawnAttackersTrue4) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t pawnBitboard = Attacks::pawnAttacks(toSquare("e5"), false);
    ASSERT_TRUE(square & pawnBitboard);
}

TEST_F(BitboardTest, pawnAttackersFalse1) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t pawnBitboard = Attacks::pawnAttacks(toSquare("c5"), true);
    ASSERT_FALSE(square & pawnBitboard);
}

TEST_F(BitboardTest, pawnAttackersFalse2) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t pawnBitboard = Attacks::pawnAttacks(toSquare("e5"), true);
    ASSERT_FALSE(square & pawnBitboard);
}

TEST_F(BitboardTest, pawnAttackersFalse3) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t pawnBitboard = Attacks::pawnAttacks(toSquare("c3"), false);
    ASSERT_FALSE(square & pawnBitboard);
}

TEST_F(BitboardTest, pawnAttackersFalse4) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t pawnBitboard = Attacks::pawnAttacks(toSquare("e3"), false);
    ASSERT_FALSE(square & pawnBitboard);
}

TEST_F(BitboardTest, kingAttackersTrue1) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t kingBitboard = Attacks::kingAttacks(toSquare("c5"));
    ASSERT_TRUE(square & kingBitboard);
}

TEST_F(BitboardTest, kingAttackersTrue2) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t kingBitboard = Attacks::kingAttacks(toSquare("c3"));
    ASSERT_TRUE(square & kingBitboard);
}

TEST_F(BitboardTest, kingAttackersFalse) {
    uint64_t square = c_u64(1) << toSquare("d4");
    uint64_t kingBitboard = Attacks::kingAttacks(toSquare("c6"));
    ASSERT_FALSE(square & kingBitboard);
}
