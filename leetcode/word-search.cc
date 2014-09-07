#include <assert.h>
#include <vector>

#include "thirdparty/gtest/gtest.h"

using namespace std;


class Solution {
public:

    bool exist(vector<vector<char> > &board, string word) {
      int width = static_cast<int>(board.size());
      int len = static_cast<int>(word.length());
      bool ret;
      vector<vector<bool> > res(width, vector<bool>(width));
      ret = checkRec(res, board, width, word, len, 0, 0, 0);
      return ret;
    }

    bool checkRec(vector<vector<bool> >& res, vector<vector<char> > &A, int Width, string& w, int Len, int m ,int n, int i) {
      assert(i >=0 && i <= Len);
      if(m < 0 || m >= Width || n < 0 || n >= Width) {
        return false;
      }

      if(i == Len) {
        return true;
      }

      if(A[m][n] == w[i]) {
        res[m][n] = true;
        ++i;
      }

      if(checkRec(res, A, Width, w, Len, m + 1, n , i) == true) {

      } else if(checkRec(res, A, Width, w, Len, m - 1, n , i) == true) {

      } else if(checkRec(res, A, Width, w, Len, m, n + 1, i) == true) {

      } else if(checkRec(res, A, Width, w, Len, m, n - 1, i) == true) {

      } else {
        if(A[m][n] == w[i]) {
          res[m][n] = false;
          --i;
        }
      }

      return A[m][n] == w[i];
    }
};

TEST(WordSearch, search) {
  Solution s;
  vector<vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'}
  };
  string word("ABCCED");
  EXPECT_TRUE(s.exist(board, word));
  word = "SEE";
  EXPECT_TRUE(s.exist(board, word));
  word = "ABCB";
  EXPECT_FALSE(s.exist(board, word));
}
