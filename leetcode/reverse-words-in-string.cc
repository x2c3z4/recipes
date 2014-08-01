#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>

#include "thirdparty/gtest/gtest.h"

using namespace std;

class Solution{
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

  void reverseWords2(string& s) {
    istringstream is(s);
    string tmp = "";
    string out = "";
    while (is >> tmp) {
      tmp += ' ';
      tmp += out;
      out = tmp;
    }
    s = out.substr(0, out.length() - 1);
  }
private:
  vector<string> container;
};


TEST(reverseWords, reverseWords) {
  string test = " I am a hero ";
  string expected = "hero a am I";
  Solution *s = new Solution();
  s->reverseWords(test);
  ASSERT_TRUE(test == expected);
  delete s;
}

TEST(ReverseWords, reverseWords2) {
  string test = " I am a hero ";
  string expected = "hero a am I";
  Solution *s = new Solution();
  s->reverseWords2(test);
  ASSERT_STREQ(expected.c_str(), test.c_str());
  delete s;
}
