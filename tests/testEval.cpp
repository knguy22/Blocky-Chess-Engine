#include "eval.hpp"

#include <gtest/gtest.h>

using namespace Eval;

TEST(ScoreTest, Construction) {
    S score(1, 2);
    EXPECT_EQ(score.opScore, 1);
    EXPECT_EQ(score.egScore, 2);
};

TEST(ScoreTest, PlusEquals) {
    S score(1, 2);
    S score2(1, 2);
    score += score2;
    EXPECT_EQ(score.opScore, 2);
    EXPECT_EQ(score.egScore, 4);
};

TEST(ScoreTest, MinusEquals) {
    S score(1, 2);
    S score2(1, 3);
    score += score2;
    EXPECT_EQ(score.opScore, 0);
    EXPECT_EQ(score.egScore, -1);
};

TEST(ScoreTest, Plus) {
    S score(1, 2);
    S score2(1, 3);
    S score3 = score + score2;
    EXPECT_EQ(score3.opScore, 2);
    EXPECT_EQ(score3.egScore, 5);
};

TEST(ScoreTest, Minus) {
    S score(1, 6);
    S score2(6, 3);
    S score3 = score - score2;
    EXPECT_EQ(score3.opScore, -5);
    EXPECT_EQ(score3.egScore, 3);
};

TEST(checkIsPassedPawn, whiteTrue) {
    int square = toSquare("d4");
    uint64_t enemyPawns = 1ull << toSquare("e4") | 1ull << toSquare("c3");
    ASSERT_TRUE(isPassedPawn(square, enemyPawns, true));
}

TEST(checkIsPassedPawn, whiteTrue2) {
    int square = toSquare("d4");
    uint64_t enemyPawns = 1ull << toSquare("d3") | 1ull << toSquare("c4");
    ASSERT_TRUE(isPassedPawn(square, enemyPawns, true));
}

TEST(checkIsPassedPawn, whiteFalse) {
    int square = toSquare("d4");
    uint64_t enemyPawns = 1ull << toSquare("e4") | 1ull << toSquare("c5");
    ASSERT_FALSE(isPassedPawn(square, enemyPawns, true));
}

TEST(checkIsPassedPawn, whiteFalse2) {
    int square = toSquare("d4");
    uint64_t enemyPawns = 1ull << toSquare("d5") | 1ull << toSquare("c3");
    ASSERT_FALSE(isPassedPawn(square, enemyPawns, true));
}

TEST(checkIsPassedPawn, blackTrue) {
    int square = toSquare("d4");
    uint64_t enemyPawns = 1ull << toSquare("e4") | 1ull << toSquare("c5");
    ASSERT_TRUE(isPassedPawn(square, enemyPawns, false));
}

TEST(checkIsPassedPawn, blackFalse) {
    int square = toSquare("d4");
    uint64_t enemyPawns = 1ull << toSquare("e3") | 1ull << toSquare("c5");
    ASSERT_FALSE(isPassedPawn(square, enemyPawns, false));
}

TEST(checkIsPassedPawn, blackFalse2) {
    int square = toSquare("b7");
    uint64_t enemyPawns = 1ull << toSquare("a4") | 1ull << toSquare("c4");
    ASSERT_FALSE(isPassedPawn(square, enemyPawns, false));
}

TEST(checkIsPassedPawn, blackFalse3) {
    int square = toSquare("a7");
    uint64_t enemyPawns = 1ull << toSquare("a2") | 1ull << toSquare("b2") | 1ull << toSquare("c2");
    ASSERT_FALSE(isPassedPawn(square, enemyPawns, false));
}