#ifndef NET_TCPSERVER_H_
#define NET_TCPSERVER_H_

#include "base/Looper.h"

namespace net {
// class base::ThreadPool;

class TcpServer {
public:
  TcpServer();
  explicit TcpServer(int port);
  ~TcpServer();


  void start();
private:
  base::Looper* loop_; // listen sock
  int port_;
  // base::ThreadPool *pool_;
};
} // namespace net
#endif
