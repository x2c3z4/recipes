#include <stdio.h>

#include <vector>
#include <memory>

#include <boost/bind.hpp>

#include "thirdparty/gtest/gtest.h"
#include "Thread.h"

void readMessage() {
  printf("read done!\n");
}
bool runloop() {
  printf("runLoop\n");
  return false;
}
bool runExit() {
  //base::Thread *t = static_cast<base::Thread>(user);
  printf(" runExit\n");
  //sleep(100);
  //t.requestExit();
  return false;
}

class Bench
{
  public:
    bool threadFunc() {
      printf("class threadFunc\n");
      return false;
    }
};

TEST(Thread, cdor) {
  base::Thread t1(runloop);
  base::Thread t2(runExit);

  Bench bench;
  base::Thread t3(boost::bind(&Bench::threadFunc,&bench));
  t1.run();
  t2.run();
  t3.run();

  t1.join();
  t2.join();
  t3.join();
}
