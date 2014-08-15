#include "LooperThread.h"

#include <unistd.h>

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

TEST(LooperThread,ctor_and_dtor) {
  net::LooperThread looperThread;
}

TEST(LooperThread,start) {
  net::LooperThread looperThread;
  looperThread.start();

  LOG(INFO) << "after start()";

  sleep(3);
}
