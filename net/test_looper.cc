#include "Looper.h"

#include <stdio.h>
#include <unistd.h>

#include <vector>
#include <memory>

#include <boost/bind.hpp>

#include "thirdparty/gtest/gtest.h"

#include "base/Thread.h"

void readMessage() {
  printf("read done!\n");
}

class MyLooper {
 public:
  bool foo() {
    for (int i = 0; i < 10; ++i) {
      sleep(1);
      printf("sleeping...\n");
    }
    loop.removeFileEvent(1, AE_READABLE);
    for (int i = 0; i < 10; ++i) {
      sleep(1);
      printf("xxxx...\n");
    }
    loop.createFileEvent(1, AE_READABLE, readMessage, NULL);
    return true;
  }
  net::Looper loop;
};

TEST(Looper, createFileEvent) {

  MyLooper mylooper;
  mylooper.loop.createFileEvent(1, AE_READABLE, readMessage, NULL);

  base::Thread t1(boost::bind(&MyLooper::foo, &mylooper));
  t1.run();

  mylooper.loop.loop();
  t1.join();
}
