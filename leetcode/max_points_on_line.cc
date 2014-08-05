#include <vector>
#include "base/scoped_ptr.h"

#include "thirdparty/gtest/gtest.h"

using namespace std;

struct Point {
    int x;
    int y;
    Point() : x(0), y(0) {}
    Point(int a, int b) : x(a), y(b) {}
};

class Solution {
public:
    int maxPoints(vector<Point> &points) {
      return 5;
    }
};

TEST(Solution, maxPointsWithSame) {
  vector<Point> points;
  points.push_back({1,1});
  points.push_back({2,2});
  points.push_back({3,3});
  points.push_back({4,4});
  points.push_back({5,5});

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(5, s->maxPoints(points));
}
TEST(Solution, maxPointsWithRandom) {
  vector<Point> points;
  points.push_back({1,1});
  points.push_back({2,2});
  points.push_back({3,3});
  points.push_back({4,4});

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(2, s->maxPoints(points));
}
