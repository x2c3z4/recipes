#ifndef NET_TCPSERVER_H_
#define NET_TCPSERVER_H_

#include <boost/scoped_ptr.hpp>

#include "net/Socket.h"
#include "net/Looper.h"

namespace net {
// class base::ThreadPool;

class TcpServer {
public:
  TcpServer(Looper* loop, int port = 8888);
  ~TcpServer();

  void start();
private:
  void newConnection();

  Looper* loop_; // listen sock
  int port_;
  boost::scoped_ptr<Socket> listenSocket_;
  // base::ThreadPool *pool_;
};
} // namespace net
#endif
