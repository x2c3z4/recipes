#include "thirdparty/gtest/gtest.h"

class Solution {
public:
  bool isRegural(char ch) {
    if(ch == '?' || ch == '*') {
      return false;
    }
    return true;
  }
    bool isMatch(const char *s, const char *p) {
      if(s == NULL || p == NULL) {
        return false;
      }
      if(strlen(p) == 1 && *p == '*') {
        return true;
      }

      if(strlen(p) == 0 && strlen(s) == 0) {
        return true;
      }

      if(strlen(p) == 0) {
        return false;
      }

      const char *start = s;
      const char *end = s + strlen(s);
      const char *pstart = p;
      const char *pend = p +strlen(p);

      while(start != end && pstart != pend) {
        if(*pstart == '?' || *start == *pstart) {
          pstart++;
          start++;
        } else if(*pstart == '*') {
          //TODO: check
          if(pstart + 1 == pend)
            return true;
          char nc = *(pstart + 1);
          if(isRegural(nc)) {
            while(start != end) {
              if(*start == nc) {
                pstart = pstart +2;
                start++;
                break;
              } else {
                start++;
              }
            } // end while
            if(start == end) {
              return false;
            }
          }  else {// end if
            if(nc == '*') {
              pstart++;
            } else { // ?
              start++;
              pstart = pstart + 2;
            }
          }
        } else {
          return false;
        }
      }
      if(start == end) {
        if(pstart == pend) {
          return true;
        }
        if(*pstart == '*' && pstart + 1 ==pend) {
          return true;
        }
      }
      return false;
    }
};

TEST(WildcardMatching, isMatch) {
  Solution s;
  EXPECT_FALSE(s.isMatch("a","aa"));
  EXPECT_TRUE(s.isMatch("aa","aa"));
  EXPECT_FALSE(s.isMatch("aaa","aa"));
  EXPECT_TRUE(s.isMatch("aa", "*"));
  EXPECT_TRUE(s.isMatch("aa", "a*"));
  EXPECT_TRUE(s.isMatch("ab", "?*"));
  EXPECT_FALSE(s.isMatch("aab", "c*a*b"));
  EXPECT_FALSE(s.isMatch("b", "*?*?"));

  EXPECT_TRUE(s.isMatch("c", "*?*"));
  EXPECT_TRUE(s.isMatch("hi", "*?"));
  EXPECT_TRUE(s.isMatch("hig", "*?g"));
}
