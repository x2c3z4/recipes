#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

using namespace std;

static vector<string> number = {"zero", "one", "two", "three", "four", "five", "six", "seven",
                                "eight", "nine"
                               };
static vector<string> successiveNumber = {"", "", "double", "triple", "quadruple",
                                          "quintuple", "sextuple", "septuple", "octuple", "nonuple", "decuple"
                                         };

class PhoneNumber {
 public:
  PhoneNumber() {
  }


  string readFromFile(const string& path) {
    int count = 0;
    int id = 0;
    string line;
    ostringstream os;
    ifstream inf(path);
    getline(inf, line);
    istringstream iss(line);
    iss >> count;
    CHECK(count > 0) << "read count error";
    while (id < count && getline(inf, line)) {
      os << readOne(id + 1, line);
      ++id;
    }
    return os.str();
  }

  string readOne(int id, const string& line) {
    ostringstream oss;
    istringstream iss(line);
    string content, style;
    iss >> content >> style;
    iss.clear();
    iss.str(style);
    int len;
    int index = 0;
    oss << "Case #" << id << ": ";
    while (iss >> len) {
      if (len < 0)
        len = -len;
      string str = content.substr(index, len);
      char last = ' ';
      int succ = 1;
    for (char c : str) {
        if (c == last) {
          ++succ;
        } else {
          if (succ == 1) {
            if (last != ' ')
              oss << number[last - '0'] << " ";
          } else if (succ > 1 && succ <= 10) {
            oss << successiveNumber[succ] << " " << number[last - '0'] << " ";
          } else if (succ > 10) {
            for (int i = 0; i < succ; ++i) {
              oss << number[last - '0'] << " ";
            }
          }
          last = c;
          succ = 1;
        }
      }
      if (succ == 1) {
        if (last != ' ')
          oss << number[last - '0'] << " ";
      } else if (succ > 1 && succ <= 10) {
        oss << successiveNumber[succ] << " " << number[last - '0'] << " ";
      } else if (succ > 10) {
        for (int i = 0; i < succ; ++i) {
          oss << number[last - '0'] << " ";
        }
      }

      index += len;
    }
    string ret = oss.str();
    ret = ret.substr(0, ret.size() - 1);
    // ret.pop_back()
    ret = ret + "\n";
    return ret;
  }
};

TEST(PhoneNumber, readFromFile) {
  PhoneNumber pn;
  string out = pn.readFromFile("test.txt");
  ostringstream oss;
  string expected, line;
  ifstream inf("expected.txt");
  while (getline(inf, line)) {
    oss << line << "\n";
  }
  expected = oss.str();
  cout << "expected:\n" << expected << "out:\n" << out;
  EXPECT_TRUE(out == expected);
}
TEST(PhoneNumber, readSmallFile) {
  PhoneNumber pn;
  string out = pn.readFromFile("A-small-practice.in");
  ofstream ofs("A-small-practice.out");
  ofs << out;
}
TEST(PhoneNumber, readLargeFile) {
  PhoneNumber pn;
  string out = pn.readFromFile("A-large-practice.in");
  ofstream ofs("A-large-practice.out");
  ofs << out;
}
