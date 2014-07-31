#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>

#include "thirdparty/gtest/gtest.h"

using namespace std;

class Solution {
public:
  void reverseWords(string& s) {
    uint32_t i = 0;
    uint32_t j = 0;
    while (i < s.length()) {
      if (s[i] != ' ') {
        ++i;
        ++j;
      } else if (j > 0) {
        container.push_back(s.substr(i - j, j));
        j = 0;
      } else {
        ++i;
      }
    }

    ostringstream os;
    for (auto it = container.crbegin(); it != container.crend(); ++it) {
      os << *it << ' ';
    }
    s = os.str();
    s = s.substr(0, s.length() - 1);
  }
private:
  vector<string> container;
};

class Solution2 {
public:
  void reverseWords(string& s) {
    istringstream is(s);
    string tmp = "";
    string out = "";
    while (is >> tmp) {
      tmp += ' ';
      tmp += out;
      out = tmp;
    }
  }
};



string test = " I am a hero ";
string expected = "hero a am I";

TEST(Solution, reverse1) {
  Solution* s = new Solution();
  s->reverseWords(test);
  ASSERT_TRUE(test == expected);
}

TEST(Solution2, reverse2) {
  Solution2* s = new Solution2();
  s->reverseWords(test);
  ASSERT_TRUE(test == expected);
  delete s;
}
