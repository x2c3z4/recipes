#include <vector>
#include <string>
#include "thirdparty/gtest/gtest.h"

using namespace std;

void printf_soduku(vector<vector<char> >& board) {
  cout << "====================" << endl;
  for (int i = 0; i < 9; ++i) {
    string line(board[i].begin(), board[i].end());
    cout << line << endl;
  }
}

class Solution {
public:
  void solveSudoku(vector<vector<char> >& board) {
    solve(board);
  }
  bool solve(vector<vector<char> >& board) {
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 9; ++j) {
        if (board[i][j] == '.') {
          for (int k = 0; k < 9; ++k) {
            board[i][j] = '0' + k;
            if (isValid(board, i, j, '0' + k) && solve(board)) {
              return true;
            } // end if
            board[i][j] = '.';
          } // end for
          return false;
        } //end if .
      } // end for j
    }
    return true;
  }
  bool isValid(vector<vector<char>>& board, int x, int y, char k) {
    // printf_soduku(board);
    // std::cout << "[" << x << " " << y << " " << k << "]" << std::endl;
    for (int i = 0; i < 9; ++i) {
      if (i != y && board[x][i] == k) {
        return false;
      }
      if (i != x && board[i][y] == k) {
        return false;
      }
    }
    for (int i = x/3 * 3; i < x/3 * 3 + 3; ++i) {
      for (int j = y/3 * 3; j < y/3 * 3 + 3; ++j) {
        if ((i != x && j != y) && board[i][j] == k) {
          return false;
        }
      }
    }
    // std::cout << "[" << x << " " << y << " " << k << "]" << std::endl;
    return true;
  }
};


TEST(SodokuSolver, try) {
  Solution s;
  vector<string> data = {
    "..9748...","7........",".2.1.9...","..7...24.",".64.1.59.",".98...3..","...8.3.2.","........6","...2759.."
  };

  vector<vector<char> >board(9, vector<char>(9));
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      board[i][j] = data[i][j];
    }
  }

  s.solveSudoku(board);
  vector<string> rest(9);
  for (int i = 0; i < 9; ++i) {
    rest[i] = string(board[i].begin(), board[i].end());
    cout << rest[i] << endl;
  }
  // vector<string> real = {"519748632","783652419","426139875","357986241","264317598","198524367","975863124","832491756","641275983"};
  // EXPECT_TRUE(restreal);
}
