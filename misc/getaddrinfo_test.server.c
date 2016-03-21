#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 500
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void print_port(int sock)
{
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  if (getsockname(sock, (struct sockaddr *)&sin, &len) == -1)
      perror("getsockname");
  else
      printf("port number: %d\n", ntohs(sin.sin_port));
}

int
main(int argc, char *argv[])
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int sfd, s;
  struct sockaddr_storage peer_addr;
  socklen_t peer_addr_len;
  ssize_t nread;
  char buf[BUF_SIZE];

  if (argc != 2) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
  hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
  hints.ai_protocol = 0;          /* Any protocol */
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  s = getaddrinfo(NULL, argv[1], &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(EXIT_FAILURE);
  }

  /* getaddrinfo() returns a list of address structures.
     Try each address until we successfully bind(2).
     If socket(2) (or bind(2)) fails, we (close the socket
     and) try the next address.
family: 2(AF_INET), ai_socktype: 2(SOCK_DGRAM), ai_protocol: 17(IPPRORO_UDP) addr: 0.0.0.0
family: 10(AF_INET6), ai_socktype: 2, ai_protocol: 17 addr: ::
family: 2, ai_socktype: 1(SOCK_STREAM), ai_protocol: 6(IPPROTO_TCP) addr: 0.0.0.0
family: 10, ai_socktype: 1, ai_protocol: 6 addr: ::

     */


  for (rp = result; rp != NULL; rp = rp->ai_next) {
    char s[INET6_ADDRSTRLEN];
    inet_ntop(rp->ai_family, get_in_addr((struct sockaddr *)rp->ai_addr), s, sizeof s);
    printf("family: %d, ai_socktype: %d, ai_protocol: %d addr: %s\n",
        rp->ai_family, rp->ai_socktype, rp->ai_protocol, s);
    sfd = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
    if (sfd == -1)
      continue;

    if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
      break;

    close(sfd);
  }
  print_port(sfd);
  if (rp == NULL) {               /* No address succeeded */
    fprintf(stderr, "Could not bind\n");
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(result);           /* No longer needed */
  /* Read datagrams and echo them back to sender */

  for (;;) {
    peer_addr_len = sizeof(struct sockaddr_storage);
    nread = recvfrom(sfd, buf, BUF_SIZE, 0,
        (struct sockaddr *) &peer_addr, &peer_addr_len);
    if (nread == -1)
      continue;               /* Ignore failed request */

    char host[NI_MAXHOST], service[NI_MAXSERV];

    s = getnameinfo((struct sockaddr *) &peer_addr,
        peer_addr_len, host, NI_MAXHOST,
        service, NI_MAXSERV, NI_NUMERICSERV);
    if (s == 0)
      printf("Received %zd bytes from %s:%s\n",
          nread, host, service);
    else
      fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

    if (sendto(sfd, buf, nread, 0,
          (struct sockaddr *) &peer_addr,
          peer_addr_len) != nread)
      fprintf(stderr, "Error sending response\n");
  }
}

