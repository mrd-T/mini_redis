
#include <gtest/gtest.h>
int add(int x, int y) { return x + y; }
TEST(AddTest, PositiveNumbers) { EXPECT_EQ(add(1, 2), 3); }

TEST(AddTest, NegativeNumbers) { EXPECT_EQ(add(-1, -2), -3); }

TEST(AddTest, MixedNumbers) { EXPECT_EQ(add(5, -3), 2); }

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}