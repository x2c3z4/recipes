#include "Socket.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>  // snprintf
#include <strings.h>  // bzero
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <assert.h>

#include <boost/implicit_cast.hpp>
#include <glog/logging.h>

namespace net {
////////////////////////////////////////////////////////////////////
namespace {

typedef struct sockaddr SA;

const SA* sockaddr_cast(const struct sockaddr_in* addr) {
  return static_cast<const SA*>(boost::implicit_cast<const void*>(addr));
}

SA* sockaddr_cast(struct sockaddr_in* addr) {
  return static_cast<SA*>(boost::implicit_cast<void*>(addr));
}

void setNonBlockAndCloseOnExec(int sockfd) {
  // non-block
  int flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(sockfd, F_SETFL, flags);
  // FIXME check

  // close-on-exec
  flags = ::fcntl(sockfd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(sockfd, F_SETFD, flags);
  // FIXME check

  (void)ret;
}

}

Socket::~Socket() {
  close(sockfd_);
}

bool Socket::getTcpInfo(struct tcp_info* tcpi) const {
  socklen_t len = sizeof(*tcpi);
  bzero(tcpi, len);
  return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool Socket::getTcpInfoString(char* buf, int len) const {
  struct tcp_info tcpi;
  bool ok = getTcpInfo(&tcpi);
  if (ok) {
    snprintf(buf, len, "unrecovered=%u "
             "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
             "lost=%u retrans=%u rtt=%u rttvar=%u "
             "sshthresh=%u cwnd=%u total_retrans=%u",
             tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
             tcpi.tcpi_rto,          // Retransmit timeout in usec
             tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
             tcpi.tcpi_snd_mss,
             tcpi.tcpi_rcv_mss,
             tcpi.tcpi_lost,         // Lost packets
             tcpi.tcpi_retrans,      // Retransmitted packets out
             tcpi.tcpi_rtt,          // Smoothed round trip time in usec
             tcpi.tcpi_rttvar,       // Medium deviation
             tcpi.tcpi_snd_ssthresh,
             tcpi.tcpi_snd_cwnd,
             tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
  }
  return ok;
}

void Socket::bindAddress(const struct sockaddr_in& addr) {
  int ret = ::bind(sockfd_, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof addr));
  if (ret < 0) {
    printf("bindOrDie\n");
  }
}

void Socket::listen() {
  int ret = ::listen(sockfd_, SOMAXCONN);
  if (ret < 0) {
    printf("listenOrDie\n");
  }
}

int Socket::accept(struct sockaddr_in *addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof * addr);
  int connfd = ::accept4(sockfd_, sockaddr_cast(addr),
                         &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd < 0) {
    int savedErrno = errno;
    LOG(ERROR) << "Socket::accept";
    switch (savedErrno) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO: // ???
      case EPERM:
      case EMFILE: // per-process lmit of open file desctiptor ???
        // expected errors
        errno = savedErrno;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        // unexpected errors
        LOG(ERROR) << "unexpected error of ::accept " << savedErrno;
        break;
      default:
        LOG(ERROR) << "unknown error of ::accept " << savedErrno;
        break;
    }
  }
  return connfd;
}

void Socket::shutdownWrite() {
  if (::shutdown(sockfd_, SHUT_WR) < 0) {
    LOG(ERROR) << "shutdownWrite";
  }
}

void Socket::setTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReusePort(bool on) {
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof optval));
  if (ret < 0) {
    LOG(ERROR) << "SO_REUSEPORT failed.";
  }
}

void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
}

namespace sockets {

int createNonblockingOrDie() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  if (sockfd < 0) {
   LOG(ERROR) << "createNonblockingOrDie";
  }
  return sockfd;
}


int connect(int sockfd, const struct sockaddr_in& addr) {
  return ::connect(sockfd, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof addr));
}

ssize_t read(int sockfd, void* buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt) {
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t write(int sockfd, const void* buf, size_t count) {
  return ::write(sockfd, buf, count);
}

void close(int sockfd) {
  if (::close(sockfd) < 0) {
    LOG(ERROR) << "close";
  }
}



void toIpPort(char* buf, size_t size,
                       const struct sockaddr_in& addr) {
  assert(size >= INET_ADDRSTRLEN);
  ::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
  size_t end = ::strlen(buf);
  uint16_t port = networkToHost16(addr.sin_port);
  assert(size > end);
  snprintf(buf + end, size - end, ":%u", port);
}

void toIp(char* buf, size_t size,
                   const struct sockaddr_in& addr) {
  assert(size >= INET_ADDRSTRLEN);
  ::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
}

void fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in* addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
   LOG(ERROR) << "fromIpPort";
  }
}

int getSocketError(int sockfd) {
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;
  } else {
    return optval;
  }
}

struct sockaddr_in getLocalAddr(int sockfd) {
  struct sockaddr_in localaddr;
  bzero(&localaddr, sizeof localaddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
    LOG(ERROR) << "getLocalAddr";
  }
  return localaddr;
}

struct sockaddr_in getPeerAddr(int sockfd) {
  struct sockaddr_in peeraddr;
  bzero(&peeraddr, sizeof peeraddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0) {
    LOG(ERROR) << "getPeerAddr";
  }
  return peeraddr;
}

bool isSelfConnect(int sockfd) {
  struct sockaddr_in localaddr = getLocalAddr(sockfd);
  struct sockaddr_in peeraddr = getPeerAddr(sockfd);
  return localaddr.sin_port == peeraddr.sin_port
         && localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}

} // namespace sockets
} // namespace net
