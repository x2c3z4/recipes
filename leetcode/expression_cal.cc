#include <stack>
#include <queue>
#include <ostream>

#include <string>
#include <cassert>

#include "thirdparty/gtest/gtest.h"
using namespace std;



template <class Container, class Stream>
Stream& printOneValueContainer
    (Stream& outputstream, const Container& container)
{
    typename Container::const_iterator beg = container.begin();

    outputstream << "[";

    while(beg != container.end())
    {
        outputstream << " " << *beg++;
    }

    outputstream << " ]";

    return outputstream;
}

template < class Type, class Container >
const Container& container
    (const std::stack<Type, Container>& stack)
{
    struct HackedStack : private std::stack<Type, Container>
    {
        static const Container& container
            (const std::stack<Type, Container>& stack)
        {
            return stack.*&HackedStack::c;
        }
    };

    return HackedStack::container(stack);
}

template < class Type, class Container >
const Container& container
    (const std::queue<Type, Container>& queue)
{
    struct HackedQueue : private std::queue<Type, Container>
    {
        static const Container& container
            (const std::queue<Type, Container>& queue)
        {
            return queue.*&HackedQueue::c;
        }
    };

    return HackedQueue::container(queue);
}

template
    < class Type
    , template <class Type, class Container = std::deque<Type> > class Adapter
    , class Stream
    >
Stream& operator<<
    (Stream& outputstream, const Adapter<Type>& adapter)
{
    return printOneValueContainer(outputstream, container(adapter));
}

class Solution {
public:
  /*
  * 1. 如果是操作符：
  *   当操作符stack为空，直接插入。
  *   if 为 ( ， 那么直接入栈。
  *   if 为 )， 那么出栈直到找到 )并弹出。
  *   如果当前优先级大于栈顶优先级，那么入栈。
  *   如果当前优先级小于或者等于栈顶优先级，那么出栈，并计算，再入栈当前操作符。
  */
  int cal(string str) {
    auto isDigit = [](char c) { return c >= '0' && c <= '9';};
    auto isHiger = [](char c1, char c2) {
      if (c2 == '(')
        return true;
      if ( (c1 == '*' || c1 == '/') && (c2 == '+' || c2 == '-') )
        return true;
      return false;
    };
    auto cumul = [](int num1, int num2, char opt) {
      if (opt == '+') return num1 + num2;
      if (opt == '-') return num1 - num2;
      if (opt == '*') return num1 * num2;
      if (opt == '/') return num1 / num2;
      return 0;
    };
    stack<int> nums;
    stack<char> opts;

    size_t i = 0;
    while(i < str.size()) {
      if (str[i] == ' ') {
        ++i;
        continue;
      }
      if (!isDigit(str[i])) { // operator
        if (opts.empty()) {
          opts.push(str[i]);
          cout << "push " << str[i] << endl;
        } else if (str[i] == '(') {
          opts.push(str[i]);
          cout << "push " << str[i] << endl;
        } else if (str[i] == ')') {
          while (opts.top() != '(') {
            char opt = opts.top();opts.pop();
            cout << "pop " << opt << endl;
            int num2 = nums.top(); nums.pop();
            int num1 = nums.top(); nums.pop();
            int rst = cumul(num1, num2, opt);
            nums.push(rst);
            cout << "push " << rst << endl;
          }
          assert(opts.top() == '(');
          opts.pop(); // pop (
        } else {
          while (!isHiger(str[i], opts.top())) { //比栈顶高
            cout << "nums: " << nums << endl;
            cout << "opts: " << opts << endl;
            char opt = opts.top(); opts.pop();
            int num2 = nums.top(); nums.pop();
            int num1 = nums.top(); nums.pop();
            int rst = cumul(num1, num2, opt);
            nums.push(rst);
            cout << "push " << rst << endl;
          }
          opts.push(str[i]);
          cout << "push " << str[i] << endl;
        }
        ++i;
      } else {
        int num = 0;
        while (isDigit(str[i])) {
          num = num * 10 + str[i] - '0';
          ++i;
        }
        nums.push(num);
        cout << "push " << num << endl;
      }
    }
    if (!opts.empty()) {
      char opt = opts.top(); opts.pop();
      int num2 = nums.top(); nums.pop();
      int num1 = nums.top(); nums.pop();
      int rst = cumul(num1, num2, opt);
      nums.push(rst);
    }

    return nums.top();
  }
};

TEST(ExpressionCal, cal) {
  Solution s;
  // EXPECT_EQ(3, s.cal("1+2"));
  // EXPECT_EQ(-66, s.cal("6*(3*(1+2)-4*5)"));
  EXPECT_EQ(-198, s.cal("6*(3*(1+2)-4*5)+6*(3*(1+2)-4*5)+6*(3*(1+2)-4*5)"));
}
