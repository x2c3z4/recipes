#include "TcpServer.h"

#include <boost/bind.hpp>

#include "thirdparty/glog/logging.h"

#include "net/Looper.h"
#include "net/Socket.h"

namespace net {
TcpServer::TcpServer(Looper* loop, int port):
  loop_(CHECK_NOTNULL(loop)),
  port_(port),
  listenSocket_(new Socket(sockets::createNonblockingOrDie())) {
  listenSocket_->setPort(port);
}

TcpServer::~TcpServer() {
}

void TcpServer::start() {
  listenSocket_->setReuseAddr(true);
  listenSocket_->setReusePort(true);
  listenSocket_->bindAddress(false); // listen 0.0.0.0
  listenSocket_->listen();
  loop_->createFileEvent(listenSocket_->fd(), AE_READABLE, boost::bind(&TcpServer::newConnection, this), NULL);
}

void TcpServer::newConnection() {
  LOG(INFO) << "new connection";
  listenSocket_->accept();
  // Loop* ioLoop = threadPool_->getNextLoop();
}

} // namespace net
