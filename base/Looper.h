#ifndef BASE_LOOPER_H_
#define BASE_LOOPER_H_

#include <vector>
#include <functional>

#include <pthread.h>
#include <sys/epoll.h>

#include "Mutex.h"

namespace base {
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
typedef std::function<void ()> Functor;
class FileEvent {
 public:
  FileEvent(int fd, int mask, void* clientData) :
    fd(fd),
    mask(mask),
    clientData(clientData) {

  }
  int fd;

  Functor rfileProc;
  Functor wfileProc;
  void* clientData;
};

class FiredEvent {
 public:
  int fd;
  int mask;
};
class Looper {
 public:
  explicit Looper():
    quit_(0) {
  }
  ~Looper() {
  }
  int createFileEvent(int fd, int mask, const Functor& func, void* clientData);

  void removeFileEvent(int fd, int mask);
 private:
  int epollfd_;
  bool looping_;
  bool quit_;
  // const pid_t threadId_;
  Mutex mutex_;

  std::vector<FileEvent> events; // use fd as index
  std::vector<FiredEvent> fired;
};
} // namespace base
#endif
