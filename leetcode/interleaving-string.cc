#include <string>
#include <iostream>

#include "thirdparty/gtest/gtest.h"

using namespace std;

class Solution {
public:
    bool isInterleave(string s1, string s2, string s3) {
      const int n1 = s1.size();
      const int n2 = s2.size();
      const int n3 = s3.size();
      if (n1 == 0){
        if (s2 == s3) {
          return true;
        } else {
          return false;
        }
      }

      if (n2 == 0){
        if (s1 == s3) {
          return true;
        } else {
          return false;
        }
      }

      if (n1 + n2 != n3) {
        return false;
      }
      vector<vector<bool>> f(n1 + 1, vector<bool>(n2 + 1, false));
      f[0][0] = true;

      for(int i = 0; i <= n1; ++i) {
        for(int j = 0; j <= n2; ++j) {
          if (i > 0)
            f[i][j] = (s1[i - 1] == s3[i + j - 1]) && f[i - 1][j];

          if (j > 0)
            f[i][j] = f[i][j] || ( (s2[j - 1] == s3[i + j - 1]) && f[i][j - 1]);

          // cout << "[" << i << " , " << j << "] = " << f[i][j] << endl;
        }
      }
      return f[n1][n2];
    }
};

TEST(InterleavingString, try) {
  Solution s;
  string s1, s2, s3;
  s1 = "aabcc";
  s2 = "dbbca";
  s3 = "aadbbcbcac";
  EXPECT_TRUE(s.isInterleave(s1, s2, s3));
  s3 = "aadbbbaccc";
  EXPECT_FALSE(s.isInterleave(s1, s2, s3));

  s1 = "aa";
  s2 = "ab";
  s3 = "abaa";
  EXPECT_TRUE(s.isInterleave(s1, s2, s3));

  s1 = "aacaac";
  s2 = "aacaaeaac";
  s3 = "aacaaeaaeaacaac";
  EXPECT_FALSE(s.isInterleave(s1, s2, s3));

}
