#include <string>
#include <iostream>
#include <vector>
#include <cstdint>

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
  }
  void dumpVector() {
    for (auto it = container.crbegin(); it != container.crend(); ++it) {
      cout << *it << ' ';
    }
  }
private:
  vector<string> container;
};

int main() {
  string test = " I am a hero ";
  Solution* s = new Solution();
  s->reverseWords(test);
  s->dumpVector();
  delete s;
  return 0;
}
