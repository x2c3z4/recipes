#ifndef BASE_LOOPER_H_
#define BASE_LOOPER_H_

#include <pthread.h>
#include <sys/epoll.h>

#include <vector>

#include <boost/function.hpp>

#include "base/Mutex.h"

namespace net {
#define MAX_EVENTS (1024*10)
#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

#define AE_NOMORE -1
typedef boost::function<void ()> Functor;

class FileEvent {
 public:
  FileEvent():
    fd(-1) {

  }
  FileEvent(int fd, int mask, void* clientData) :
    fd(fd),
    mask(mask),
    clientData(clientData) {

  }
  FileEvent(const FileEvent& fe) :
    fd(fe.fd),
    mask(fe.mask),
    rfileProc(fe.rfileProc),
    wfileProc(fe.wfileProc),
    clientData(clientData) {

  }
  int fd;
  int mask;

  Functor rfileProc;
  Functor wfileProc;
  void* clientData;
};

class Looper {
 public:
  explicit Looper();
  ~Looper();
  int createFileEvent(int fd, int mask, const Functor& func, void* clientData);
  void removeFileEvent(int fd, int mask);
  void loop();
 private:
  int processEvent_();

  int epollfd_;
  bool looping_;
  bool quit_;
  // const pid_t threadId_;
  base::Mutex mutex_;

  FileEvent fileEvents_[MAX_EVENTS]; // use fd as index
};
} // namespace base
#endif
