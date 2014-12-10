#include <iostream>

using namespace std;

int main() {
  float f1 = 1.1;
  double d1 = 1.1;
  if (f1 == d1) {
    cout << "f1==d1" << endl;
  } else {
    cout << "f1 != d1" << endl;
  }

  float f2 = 1.5;
  double d2 = 1.5;
  if (f2 == d2) {
    cout << "f2 == d2" << endl;
  } else {
    cout << "f2 != d2" << endl;
  }

  float f=0.0f;
  for(int i=0;i<10;++i)
  {
      f+=0.1;
  }
  if(f==1.0)
  {
      std::cout<<"all right"<<std::endl;
  }else{
      std::cout<<"what the fuck:"<< f << std::endl;
  }

  return 0;
}
