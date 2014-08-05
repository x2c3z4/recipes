#include <iostream>
#include <assert.h>

#include "thirdparty/gtest/gtest.h"
#include "base/scoped_ptr.h"

using namespace std;

class Solution{
  public:
    int evalRPN(vector<string> &tokens) {
      vector<int> rest;
      for(auto it = tokens.begin(); it != tokens.end(); ++it) {
        if (isOperator(*it)) {
          int num2 = rest.back();
          rest.pop_back();
          int num1 = rest.back();
          rest.pop_back();

          int sum = calculate(num1, num2, *it);
          rest.push_back(sum);
        } else {
          rest.push_back(atoi((*it).c_str()));
        }
      }

      assert(1 == rest.size());
      return rest[0];
    }
    bool isOperator(const string &str) {
      if (str == "+" || str == "-" || str == "*"  || str == "/" ) {
        return true;
      }
      return false;
    }

    int calculate(const int num1, const int num2, const string &str) {
      if (str == "+") return num1 + num2;
      if (str == "-") return num1 - num2;
      if (str == "*") return num1 * num2;
      if (str == "/") return num1 / num2;
      return 0;
    }
};

TEST(ReversePolishNotation, evalRPN) {
  scoped_ptr<Solution> s(new Solution());
  vector<string> test = {"2", "1", "+", "3", "*"};
  EXPECT_EQ(9, s->evalRPN(test));

  vector<string> test2 = {"4", "13", "5", "/", "+"};
  EXPECT_EQ(6, s->evalRPN(test2));
}
