#include <iostream>
#include <vector>
#include <algorithm>

#include <stdio.h>

using namespace std;

string cmd1(string& rst, int i, int j, char x) {
  while (i <= j) {
    rst[i - 1] = x;
    ++i;
  }
  return rst;
}

string cmd2(string& rst, int i, int j, int k) {
  while (i <= j) {
    rst[i - 1] = rst[i - 1] + k;
    if (rst[i - 1] > 'Z')
      rst[i - 1] = 'A' + rst[i - 1] % 'Z' - 1;
    ++i;
  }
  return rst;
}

string cmd3(string& str, int k) {
  string out;
  out = str.substr(k, str.size() - k) + str.substr(0, k);
  return out;
}

string cmd4(string& str, int i, int j) {
  if (i > j)
    return str;
  else {
    str = cmd4(str, i + 1, j);
    str = cmd2(str, i ,j, 1);
  }
  return str;
}

int main() {
  int n, m;
  string rst;

  cin >> n >> m;
  cin >> rst;

  for (int t = 1; t <= m; t++) {
    string dummy;
    int cmd;
    cin >> dummy >> cmd;
    int i, j, k;
    char x;
    switch (cmd) {
      case 1:
        cin >> i >> j >> x;
        rst = cmd1(rst, i, j, x);
        // cout << rst << endl;
        break;
      case 2:
        cin >> i >> j >> k;
        rst = cmd2(rst, i, j, k);
        // cout << rst << endl;
        break;
      case 3:
        cin >> k;
        rst = cmd3(rst, k);
        // cout << rst << endl;
        break;
      case 4:
        cin >> i >> j;
        rst = cmd4(rst, i, j);
        // cout << rst << endl;
        break;
    }
  }
  cout << rst << endl;
  return 0;
}
