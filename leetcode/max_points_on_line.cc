#include <vector>
#include <map>
#include <algorithm>
#include <limits>

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
      int max = 1;
      int len = points.size();
      if (len == 0)
        return 0;
      for (int i = 0; i < len - 1; ++i) {
        map<double, int> count;
        int same = 0;
        int verticle = 1;
        for (int j = i + 1; j < len; ++j) {
          if ((points[i].x == points[j].x) && (points[i].y == points[j].y)) {
            ++same;
            continue;
          }
          if (points[i].x == points[j].x) {
            ++verticle;
            continue;
          }
          double v = gradient(points[i], points[j]);
                    printf("v = %f,", v);
          auto i(count.find(v));
          if (i != count.end()) {
            i->second++;
          } else {
            count[v] = 2;
          }
        }
        //for (auto it = count.begin(); it != count.end(); ++it) {
        //  printf("key: %f, value: %d\n", it->first,it->second);
        //}
        //printf("same = %d\n",same);
        if (count.size() != 0) {
          auto pr = std::max_element(count.begin(), count.end(),
              [](const pair<double, int>& a, const pair<double, int>& b) {
              return a.second < b.second;
              }
              );
          max = (max > pr->second + same ? max : pr->second + same);
        }
        max = (max > verticle +same? max : verticle + same);

        // vector<pair<double, int>> pairs(count.begin(), count.end());
        // sort(pairs.begin(), pairs.end(), [](const pair<double, int>& a, const pair<double, int>& b) {
        //    return a.second > b.second;
        //    }
        //    );
        // if (max < pairs[0].second) {
        //  max = pairs[0].second;
        // }
      }
      return max;
    }
    double gradient(const Point& p1, const Point& p2) {
      assert(p2.x != p1.x);
      return (p2.y - p1.y) * 1.0 / (p2.x - p1.x);
    }
};

TEST(Solution, maxPointsWithRandom) {
  vector<Point> points;
  points.push_back( {-4, 1});
  points.push_back( {-7, 7});
  points.push_back( {-1, 5});
  points.push_back( {9, -25});

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(3, s->maxPoints(points));
}
TEST(Solution, maxPointsWithXeqY) {
  vector<Point> points;
  points.push_back( {1, 1});
  points.push_back( {2, 2});
  points.push_back( {3, 3});
  points.push_back( {4, 4});

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(4, s->maxPoints(points));
}

TEST(Solution, maxPointsWithOnePoint) {
  vector<Point> points;
  points.push_back( {1, 1});

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(1, s->maxPoints(points));
}
TEST(Solution, maxPointsWithNon) {
  vector<Point> points;

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(0, s->maxPoints(points));
}
TEST(Solution, maxPointsWithVerticle) {
  vector<Point> points;
  points.push_back( {4, 0});
  points.push_back( {4, -1});
  points.push_back( {4, 5});

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(3, s->maxPoints(points));
}
TEST(Solution, maxPointsWithPointSame) {
  vector<Point> points;
  points.push_back( {0, 0});
  points.push_back( {1, 1});
  points.push_back( {0, 0});

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(3, s->maxPoints(points));
}
TEST(Solution, maxPointsWithAllSame) {
  vector<Point> points;
  points.push_back( {0, 0});
  points.push_back( {0, 0});
  points.push_back( {0, 0});

  scoped_ptr<Solution> s(new Solution());
  EXPECT_EQ(3, s->maxPoints(points));
}
