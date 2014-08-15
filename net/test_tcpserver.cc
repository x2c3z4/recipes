#include <stdio.h>
#include <unistd.h>

#include <vector>
#include <memory>

#include <boost/bind.hpp>

#include "thirdparty/gtest/gtest.h"

#include "base/Thread.h"

#include "net/Looper.h"
#include "net/TcpServer.h"

using namespace net;
using namespace base;

void readMessage() {
  printf("read done!\n");
}

TEST(TcpServer, listen) {
  Looper loop;
  TcpServer server(&loop);
  server.start(); // nonblocking
  loop.loop();
}
