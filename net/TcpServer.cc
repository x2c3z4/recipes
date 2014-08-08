#include "TcpServer.h"

#include "base/Looper.h"

namespace net {
TcpServer::TcpServer(int port):
  port_(port){
}
TcpServer::TcpServer():
  port_(8888) {
  }

void TcpServer::start() {

}
TcpServer::~TcpServer() {
}

} // namespace net
