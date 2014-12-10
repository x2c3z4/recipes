#include <iostream>

using namespace std;

class A {
public:
  A() {cout << "constrcut" << endl;}
  virtual ~A(){}
  virtual void foo() {cout << "foo" << endl;}
  virtual void bar() {cout << "bar" << endl;}
  // void* operator new(size_t size) {cout << "operator new" << endl; return malloc(size);}
// private:
  int a; // 4
  int b; // 4
  double c; // 8
  char d;
};

#define PRINT_SIZE(x) do {cout << #x << ": " << sizeof(x) << endl;} while(0)
int main() {
  PRINT_SIZE(int);
  PRINT_SIZE(double);
  PRINT_SIZE(float);
  PRINT_SIZE(long);
  PRINT_SIZE(long long);
  PRINT_SIZE(A);
  A *a = static_cast<A*>(::operator new(sizeof(A)));
  new (a) A();
  A b;
  cout << "b.foo()" << endl;
  b.foo();
  cout << "a->foo()" << endl;
  a->foo();

  int A::*pa;
  pa = &A::b;
  cout << "pa = " << pa << endl;

  b.*pa = 3;
  cout << "b.b = " << b.b << endl;
  return 0;
}
