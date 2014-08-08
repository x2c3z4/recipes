#include <stdio.h>
#include <unistd.h>

#include <vector>
#include <memory>

#include <boost/bind.hpp>

#include "thirdparty/gtest/gtest.h"

#include "Looper.h"
#include "Thread.h"

void readMessage() {
  printf("read done!\n");
}

using namespace net;
using namespace base;
TEST(TcpServer, listen) {
  Looper loop;
  TcpServer server(8000);
  server.start(); // nonblocking
  loop.loop();
}
