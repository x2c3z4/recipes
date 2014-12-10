#include <iostream>
#include <cstring>
using namespace std;

int main() {
  int a = 0x12345678;
  char *p = (char*)&a;
  if (*p == 0x78) // p[i]
    cout << "little end" << endl;
  else
    cout << "big end" << endl;
  for (int i = 0; i < 4; i++) {
    cout << *(p+i) << " ";
  }
  int i = 0, j = 0, k = 0, l =0, m = 0;
  m = (i++ && ++j && k++) || l++; // 短路
  cout << i << " " << j << " " << k << " " <<  l << " " << m << endl;

  char s[] = "abc";
  cout << s << " len:" << strlen(s) << endl;
  return 0;
}
