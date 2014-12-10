#include <iostream>

using namespace std;

double f(int var)
{
 return double(var);
}

int f(double var)
{
 return -int(var);
}

int main(){
  int n(42);
  std::cout<<f(f(n));
}
