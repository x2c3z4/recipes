#ifndef NET_TCPSERVER_H_
#ifndef NET_TCPSERVER_H_
namespace net {

class base::Looper;
class base::ThreadPool;

class TcpServer {
  public:
  explict TcpServer();
  ~TcpServer();
  private:
  base::Looper *loop_; // listen sock
  //base::ThreadPool *pool_;
};
}
#endif
