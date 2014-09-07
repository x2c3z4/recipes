#include <string.h>
#include "thirdparty/gtest/gtest.h"

class Solution {
public:
    char *strStr(char *haystack, char *needle) {
      int n1 = strlen(haystack);
      int n2 = strlen(needle);

      if(haystack == NULL || needle == NULL || n1 < n2) {
        return NULL;
      }
      char* start = haystack;
      char *end = start + n1;
      int j = 0;
      while(start) {
        j = 0;
        if(end - start < n2) {
          return NULL;
        }
        while(*(start + j) == *(needle + j) && j < n2) {
          ++j;
        }
        if(j == n2) {
          return start;
        }
        start++;
      }
      return NULL;
    }
};

TEST(ImplementStrStr, strStr) {
  char haystack[] = "123456";
  char needle[] = "4";
  Solution s;
  char *rest = s.strStr(haystack, needle);
  char *real = strstr(haystack, needle);
  EXPECT_TRUE(rest == real);
}
