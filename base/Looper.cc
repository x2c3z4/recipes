#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>

#include "Looper.h"

namespace base{
  Looper::Looper():
  quit_(0){
    epollfd_ = epoll_create(MAX_EVENTS);
    if (epollfd_ == -1) {
      perror("epoll create error");
    }
  }
  Looper::~Looper() {
  }

  int Looper::createFileEvent(int fd, int mask, const Functor& func, void* clientData) {
    printf("add fd %d mask %d\n",fd, mask);
    if (mask & AE_READABLE) fileEvents_[fd].rfileProc = func;
    if (mask & AE_WRITABLE) fileEvents_[fd].wfileProc = func;
    fileEvents_[fd].clientData = clientData;

    struct epoll_event ee;
    /*  If the fd was already monitored for some event, we need a MOD
     *       * operation. Otherwise we need an ADD operation. */
    int op = fileEvents_[fd].mask == AE_NONE ?  EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    ee.events = 0;
    mask |= fileEvents_[fd].mask; /*  Merge old events */
    fileEvents_[fd].mask = mask;
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0; /*  avoid valgrind warning */
    ee.data.fd = fd;
    if (epoll_ctl(epollfd_, op, fd, &ee) == -1)
      return -1;
    return 0;
  }
  void Looper::removeFileEvent(int fd, int delmask) {
    printf("rm fd %d mask %d\n",fd, delmask);
    struct epoll_event ee;
    int mask = fileEvents_[fd].mask & (~delmask);
    ee.events = 0;
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0; /*  avoid valgrind warning */
    ee.data.fd = fd;
    if (mask != AE_NONE) {
      epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ee);
    } else {
      epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ee);
    }
  }
  int Looper::processEvent_() {
    struct epoll_event events_[MAX_EVENTS];
    int nfds;
    nfds = epoll_wait(epollfd_, events_, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      exit(-1);
    }
    for (int j = 0; j < nfds; ++j) {
      struct epoll_event* e = events_ + j;
      int mask = 0;
      int fd  = e->data.fd;

      if (e->events & EPOLLIN) mask |= AE_READABLE;
      if (e->events & EPOLLOUT) mask |= AE_WRITABLE;
      if (e->events & EPOLLERR) mask |= AE_WRITABLE;
      if (e->events & EPOLLHUP) mask |= AE_WRITABLE;

      if (mask & AE_READABLE) {
        fileEvents_[fd].rfileProc();
      }
      if (mask & AE_WRITABLE) {
        fileEvents_[fd].wfileProc();
      }
    }
    return nfds;
  }

  void Looper::loop() {
   quit_ = 0;
   int processed = 0;
    while (!quit_) {
      processed = processEvent_();
      printf("processed %d ... \n", processed);
    }
  }
}
