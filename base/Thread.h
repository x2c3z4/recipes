#ifndef BASE_THREAD_H_
#define BASE_THREAD_H_

#include <stdint.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#include <functional>

#include <boost/function.hpp>

#include "Condition.h"
#include "Errors.h"
#include "Mutex.h"
#include "Timers.h"

// ---------------------------------------------------------------------------
namespace base {
// ---------------------------------------------------------------------------
typedef boost::function<bool ()> LoopFunc;
typedef pthread_t thread_id_t;
class Thread {
public:
  Thread(const LoopFunc&);
  virtual             ~Thread();
  status_t    run();
  // Ask this object's thread to exit. This function is asynchronous, when the
  // function returns the thread might still be running. Of course, this
  // function can be called from a different thread.
  void        requestExit();

  // Call requestExit() and wait until this object's thread exits.
  // BE VERY CAREFUL of deadlocks. In particular, it would be silly to call
  // this function from this object's thread. Will return WOULD_BLOCK in
  // that case.
  status_t    requestExitAndWait();

  // Wait until this object's thread exits. Returns immediately if not yet running.
  // Do not call from this object's thread; will return WOULD_BLOCK in that case.
  status_t    join();

  // Indicates whether this thread is running or not.
  bool        isRunning() const;

  // Return the thread's kernel ID, same as the thread itself calling gettid() or
  // androidGetTid(), or -1 if the thread is not running.
  pid_t       getTid() const;
  LoopFunc        mThreadLoop;
protected:
  // exitPending() returns true if requestExit() has been called.
  bool        exitPending() const;
private:
  // Derived class must implement threadLoop(). The thread starts its life
  // here. There are two ways of using the Thread object:
  // 1) loop: if threadLoop() returns true, it will be called again if
  //          requestExit() wasn't called.
  // 2) once: if threadLoop() returns false, the thread will exit upon return.
  int _createThread(thread_id_t* threadId);

private:
  Thread& operator=(const Thread&);
  static void*            _threadLoop(void* user);
  // always hold mLock when reading or writing
  thread_id_t     mThread;
  mutable Mutex           mLock;
  Condition       mThreadExitedCondition;
  status_t        mStatus;
  // note that all accesses of mExitPending and mRunning need to hold mLock
  volatile bool           mExitPending;
  volatile bool           mRunning;
};


}; // namespace base

// ---------------------------------------------------------------------------
#endif
// ---------------------------------------------------------------------------
