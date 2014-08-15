#include "LooperThread.h"

#include <boost/bind.hpp>

#include "thirdparty/glog/logging.h"
#include "Looper.h"

namespace net {
  LooperThread::LooperThread() :
    thread_(boost::bind(&LooperThread::worker, this)) {
  }

  void LooperThread::start() {
    thread_.run();
  }

  LooperThread::~LooperThread() {
    // TODO: need request quit
    LOG(INFO) << "dtor LooperThread...";
    loop_.requestQuit();
    thread_.requestExitAndWait();
  }

  bool LooperThread::worker() {
    LOG(INFO) << "LooperThread run in loop...";
    loop_.loop();
    return true; // not exit
  }

} // namespace net
