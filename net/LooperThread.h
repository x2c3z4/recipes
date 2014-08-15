#ifndef NET_LOOPER_THREAD_H_
#define NET_LOOPER_THREAD_H_

#include "base/Thread.h"
#include "Looper.h"

namespace net {
class LooperThread {
  public:
    LooperThread();
    ~LooperThread();
    void start();
  private:

    bool worker();
    base::Thread thread_;
    Looper loop_;
};

} // namespace net
#endif
