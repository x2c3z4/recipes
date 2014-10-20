#include <iostream>
#include <stack>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

int time2int(string &time) {
  int i;
  int a[3];
  int j =0;
  stringstream ss(time);
  while (ss >> i)
  {
      a[j++] = i;
      if (ss.peek() == ':')
          ss.ignore();
  }
  int sum = a[0] * 3600 + a[1] * 60 + a[2];
  // cout << sum << endl;
  return sum;
}

string int2time(int time) {
  int h = 0, m = 0, s = 0;
  h = time / 3600;
  m = (time - h * 3600) / 60;
  s = time % 60;
  char buf[256];
  snprintf(buf, 256, "%02d:%02d:%02d", h ,m, s);
  return buf;
}


typedef pair<string, int> func_t;
struct node_t{
  func_t fun;
  node_t *left;
  node_t *right;
  node_t *parent;
  bool used_left;
  int elasp;
  node_t():left(NULL), right(NULL), parent(NULL), used_left(false), elasp(-1) {};
};

void dfs(node_t *node, string& out) {
  if (node) {
    out += node->fun.first + " " + int2time(node->elasp) + '\n';
    // cout << out << endl;
    dfs(node->left, out);
    dfs(node->right, out);
  }
}

int main() {
  int T;
  cin >> T;

  stringstream out;
  vector<string> rst;
  bool flag = false;

  stack<func_t> func_stack;
  node_t *tail; // tail is dummy
  node_t dummy;
  tail = &dummy;

  for (int t = 1; t <= T; t++) {
    string s1, s2, s3;
    cin >> s1 >> s2 >> s3;

    if (s3 == "START") {
      func_t f(s1, time2int(s2));

      node_t *node = new node_t;
      node->fun = f;
      node->parent = tail;
      if (!tail->used_left)
        tail->left = node;
      else
        tail->right = node;
      tail = node;

      func_stack.push(f);
      // cout << "push " << f.first << endl;
    } else {
      if (func_stack.top().first != s1) {
        cout <<  "\nIncorrect performance log" << endl;
        flag = true;
        break;
      } else {
        func_t f = func_stack.top();
        func_stack.pop();
        // cout << "pop " << f.first << endl;
        int elasp = time2int(s2) - f.second;
        tail->elasp = elasp;

        tail = tail->parent; // free
        // assert(tail != NULL);
        tail->used_left = true;
        // cout << "pop " << f.first << endl;

        // char buf[256];
        // snprintf(buf, 256, "%s %d\n", f.first.c_str(), int2time(elasp));
        // out << f.first << " " << int2time(elasp) << endl;
        // rst.push_back(out.str());
        // out.str("");
        // rst.push_back(buf);
      }
    }
    // cout << s1 << " ," << s2 << ", " << s3 << endl;
  }

  if (!flag) {
    string out;
    dfs(tail->left, out);
    cout << out;
    // reverse(rst.begin(), rst.end());
    // for (size_t i = 0; i < rst.size(); ++i)
    //   cout << rst[i];
  }
  return 0;
}
