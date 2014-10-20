#include <iostream>
#include <vector>
#include <algorithm>

#include <stdio.h>

using namespace std;

template <typename T>
void printVector(const vector<T>& val) {
  for (auto& v : val) {
    cout << v << " " << endl;
  }
  cout << endl;
}

string solve(int n, string& s) {
  return "YES";
  // return "NO";
}

int main() {
  int T;
  cin >> T;
  for (int t = 1; t <= T; t++) {
    int n;
    string s;
    cin >> n;
    cin >> s;

    cout << solve(n, s) << endl;
  }
  return 0;
}
