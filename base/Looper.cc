#include <sys/epoll.h>

#include "Looper.h"

namespace base {
explicit Looper() {
  epollfd_ = epoll_create(10240);
  if (epollfd_ == -1) {
    perror("epoll create error");
}
}
  int Looper::createFileEvent(int fd, int mask, const Functor &func, void *clientData){
    FileEvent fe(fd, mask, clientData);
    if (fe.mask & AE_READABLE) fe.rfileProc = func;
    if (fe.mask & AE_WRITABLE) fe.wfileProc = func;
    events[fd] = fe;
    return 0;
  }
  int Looper::processEvent_() {
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    nfds = epoll_wait(epollfd_, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror ("epoll_wait");
      exit(EXIT_FAIURE);
    }
  }

  void loop() {
    stop_ = 0;
    while (!stop_) {
      processEvent_();
    }
  }
  void Looper::removeFileEvent(int fd, int mask) {
    FileEvent *fe = &events[fd];
    if (fe->mask == AE_NONE) return;
    fe->mask = fe->mask & (~mask);
  }

}
