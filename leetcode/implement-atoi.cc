#include <stdlib.h>
#include <stdio.h>
#include "thirdparty/gtest/gtest.h"

class Solution {
public:
  // const int INT_MAX = 2147483647;
  // const int INT_MIN = -2147483648;
  #define INT_MAX  2147483647
  #define INT_MIN  -2147483648
  bool isOverflow(int v) {
    return !!(v >> 31 & 0x1);
  }
    int convertToDigital(char c) {
      if(c < '0' || c > '9')
        return -1;
      return (c - '0');
    }
    int atoi(const char *str) {
      const char *start = str;
      int len = strlen(str);
      const char *end = start + len;
      while(*start == ' ') start++;
      if(start == end) {
        return 0;
      }
      int sign = 1;
      if(*start == '+') {
        sign = 1;
        start++;
      } else if(*start == '-') {
        sign = -1;
        start++;
      }
      int rest = 0;
      while(start != end) {
        int v = convertToDigital(*start);
        if(v < 0) {
          break;
        }
        else {
          // rest = rest * 10 + v;
          if(!!(rest >> 28 & 0xf)) {
            if(sign == 1) {
              return INT_MAX;
            } else {
              return INT_MIN;
            }
          } else {
            rest = (rest << 3) + (rest << 1) + v;
          }
        }
        if(isOverflow(rest)) {
          if(sign == 1) {
            return INT_MAX;
          } else {
            return INT_MIN;
          }
        }
        start++;
      }
      return rest * sign;
    }
};

TEST(ImplementAtoi, atoi) {
  Solution s;
  int rest, real;

  char a1[] = "   - 1234b ";
  rest = s.atoi(a1);
  real = atoi(a1);
  printf("%d\n", real);
  EXPECT_EQ(rest, real);

  char a2[] = "   +-1234b ";
  rest = s.atoi(a2);
  real = atoi(a2);
  printf("%d\n", real);
  EXPECT_EQ(rest, real);

  char a3[] = "2147483648";
  rest = s.atoi(a3);
  real = atoi(a3);
  printf("%d\n", real);
  EXPECT_EQ(rest, real);

  char a4[] = "    10522545459";
  rest = s.atoi(a4);
  real = atoi(a4);
  printf("%d\n", real);
  EXPECT_EQ(rest, real);

  char a5[] = "-2147483648";
  rest = s.atoi(a5);
  real = atoi(a5);
  printf("%d\n", real);
  EXPECT_EQ(rest, real);
}
