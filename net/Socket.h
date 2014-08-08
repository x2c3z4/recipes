#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_

#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <boost/noncopyable.hpp>
namespace net {

class Socket : boost::noncopyable {
public:
  explicit Socket(int sockfd)
    : sockfd_(sockfd) {
  }

  // Socket(Socket&&) // move constructor in C++11
  ~Socket();

  int fd() const {
    return sockfd_;
  }
  // return true if success.
  bool getTcpInfo(struct tcp_info*) const;
  bool getTcpInfoString(char* buf, int len) const;

  /// abort if address in use
  void bindAddress(const sockaddr_in& localaddr);
  /// abort if address in use
  void listen();

  /// On success, returns a non-negative integer that is
  /// a descriptor for the accepted socket, which has been
  /// set to non-blocking and close-on-exec. *peeraddr is assigned.
  /// On error, -1 is returned, and *peeraddr is untouched.
  int accept(sockaddr_in* peeraddr);

  void shutdownWrite();

  ///
  /// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
  ///
  void setTcpNoDelay(bool on);

  ///
  /// Enable/disable SO_REUSEADDR
  ///
  void setReuseAddr(bool on);

  ///
  /// Enable/disable SO_REUSEPORT
  ///
  void setReusePort(bool on);

  ///
  /// Enable/disable SO_KEEPALIVE
  ///
  void setKeepAlive(bool on);

private:
  const int sockfd_;
};

namespace sockets {

inline uint64_t hostToNetwork64(uint64_t host64)
{
    return htobe64(host64);
}

inline uint32_t hostToNetwork32(uint32_t host32)
{
    return htobe32(host32);
}

inline uint16_t hostToNetwork16(uint16_t host16)
{
    return htobe16(host16);
}

inline uint64_t networkToHost64(uint64_t net64)
{
    return be64toh(net64);
}

inline uint32_t networkToHost32(uint32_t net32)
{
    return be32toh(net32);
}

inline uint16_t networkToHost16(uint16_t net16)
{
    return be16toh(net16);
}


int createNonblockingOrDie();

int  connect(int sockfd, const struct sockaddr_in& addr);
ssize_t read(int sockfd, void* buf, size_t count);
ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
ssize_t write(int sockfd, const void* buf, size_t count);
void close(int sockfd);

void toIpPort(char* buf, size_t size,
              const struct sockaddr_in& addr);
void toIp(char* buf, size_t size,
          const struct sockaddr_in& addr);
void fromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in* addr);

int getSocketError(int sockfd);

struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);
bool isSelfConnect(int sockfd);
} // sockets
} // net

#endif
