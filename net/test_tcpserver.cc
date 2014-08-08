#include <stdio.h>
#include <unistd.h>

#include <vector>
#include <memory>

#include <boost/bind.hpp>

#include "thirdparty/gtest/gtest.h"

#include "base/Looper.h"
#include "base/Thread.h"

#include "net/TcpServer.h"

using namespace net;
using namespace base;

void readMessage() {
  printf("read done!\n");
}

TEST(TcpServer, listen) {
  Looper loop;
  TcpServer server(8000);
  server.start(); // nonblocking
  loop.loop();
}
