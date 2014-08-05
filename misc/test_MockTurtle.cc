#include "thirdparty/gtest/gtest.h"
#include "thirdparty/gmock/gmock.h"

#include "MockTurtle.h"

using ::testing::AtLeast;                     // #1

TEST(PainterTest, CanDrawSomething) {
  misc::MockTurtle turtle;                          // #2
  EXPECT_CALL(turtle, PenDown())              // #3
    .Times(AtLeast(1));
}

TEST(Turtle, ctor) {
  misc::MockTurtle turtle;
  EXPECT_CALL(turtle, PenDown())
    .Times(AtLeast(1));
}
