#include <vector>
#include <string>

#include "thirdparty/gtest/gtest.h"

using namespace std;

class Solution {
public:
    vector<string> restoreIpAddresses(string s) {
      vector<string> rst;
      string ip;
      dfs(s, 0, 0, ip, rst);
      return rst;
    }

    void dfs(const string& s, size_t curr, size_t step, string ip, vector<string>& rst) {
      if (curr == s.size() && step == 4) {
        ip.resize(ip.size() - 1);
        rst.push_back(ip);
        return;
      }

      if (s.size() - curr < 4 - step) return;
      if (s.size() - curr > 3* (4 - step)) return;
      int num = 0;
      for (size_t i = curr; i < curr + 3; ++i) {
        num = num * 10 + s[i] - '0';
        if (num < 256) {
          ip = ip + s[i];
          dfs(s, i + 1, step + 1, ip + '.', rst);
        }
        if (num == 0) return;
        if (num >=256) return;
      }
    }
};

template <typename T>
void printVector(const vector<T>& val) {
  for (auto& v : val) {
    cout << v << " " << endl;
  }
  cout << endl;
}

TEST(RestoreIP, solve) {
  Solution s;
  vector<string> rst = s.restoreIpAddresses("25525511135");
  printVector(rst);

  rst = s.restoreIpAddresses("010010");
  printVector(rst);
}
