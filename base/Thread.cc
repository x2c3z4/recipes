#include "Thread.h"
#include "Errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include <pthread.h>
#include <sched.h>
#include <sys/prctl.h>

/*
 * ===========================================================================
 *      Thread wrappers
 * ===========================================================================
 */

using namespace base;


thread_id_t getThreadId() {
  return (thread_id_t)pthread_self();
}

namespace base {

/*
 * This is our thread object!
 */

Thread::Thread(const LoopFunc& func)
  : mThreadLoop(func),
    mThread(thread_id_t(-1)),
    mStatus(NO_ERROR),
    mExitPending(false), mRunning(false) {
}

Thread::~Thread() {
}

status_t Thread::run() {
  Mutex::Autolock _l(mLock);
  if (mRunning) {
    // thread already started
    return INVALID_OPERATION;
  }

  // reset status and exitPending to their default value, so we can
  // try again after an error happened (either below, or in readyToRun())
  mStatus = NO_ERROR;
  mExitPending = false;
  mThread = thread_id_t(-1);
  mRunning = true;

  bool res;
  // create pthread
  res = _createThread(&mThread);
  if (res == false) {
    mStatus = UNKNOWN_ERROR;   // something happened!
    mRunning = false;
    mThread = thread_id_t(-1);

    return UNKNOWN_ERROR;
  }
  // Do not refer to mStatus here: The thread is already running (may, in fact
  // already have exited with a valid mStatus result). The NO_ERROR indication
  // here merely indicates successfully starting the thread and does not
  // imply successful termination/execution.
  return NO_ERROR;

  // Exiting scope of mLock is a memory barrier and allows new thread to run
}

// static
void* Thread::_threadLoop(void* user) {
  Thread* const self = static_cast<Thread*>(user);
  bool result = true;

  do {
    result = self->mThreadLoop();
    // establish a scope for mLock
    Mutex::Autolock _l(self->mLock);
    if (result == false || self->mExitPending) {
      self->mExitPending = true;
      self->mRunning = false;
      // clear thread ID so that requestExitAndWait() does not exit if
      // called by a new thread using the same thread ID as this one.
      self->mThread = thread_id_t(-1);
      // note that interested observers blocked in requestExitAndWait are
      // awoken by broadcast, but blocked on mLock until break exits scope
      self->mThreadExitedCondition.broadcast();
      break;
    }
  } while (1);
  return NULL;
}

void Thread::requestExit() {
  Mutex::Autolock _l(mLock);
  mExitPending = true;
}

status_t Thread::requestExitAndWait() {
  Mutex::Autolock _l(mLock);
  if (mThread == getThreadId()) {
    printf(
      "Thread (this=%p): don't call waitForExit() from this "
      "Thread object's thread. It's a guaranteed deadlock!",
      this);
    return WOULD_BLOCK;
  }
  mExitPending = true;

  while (mRunning == true) {
    mThreadExitedCondition.wait(mLock);
  }
  mExitPending = false;

  return mStatus;
}

status_t Thread::join() {
  Mutex::Autolock _l(mLock);
  if (mThread == getThreadId()) {
    printf(
      "Thread (this=%p): don't call join() from this "
      "Thread object's thread. It's a guaranteed deadlock!",
      this);

    return WOULD_BLOCK;
  }

  while (mRunning == true) {
    mThreadExitedCondition.wait(mLock);
  }

  return mStatus;
}

bool Thread::isRunning() const {
  Mutex::Autolock _l(mLock);
  return mRunning;
}


bool Thread::exitPending() const {
  Mutex::Autolock _l(mLock);
  return mExitPending;
}

int Thread::_createThread(thread_id_t* threadId) {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  errno = 0;
  pthread_t thread;
  int result = pthread_create(&thread, &attr, Thread::_threadLoop, this);
  pthread_attr_destroy(&attr);
  if (result != 0) {
    printf("createThread failed (res=%d, errno=%d)\n", result, errno);
    return 0;
  }

  // Note that *threadID is directly available to the parent only, as it is
  // assigned after the child starts.  Use memory barrier / lock if the child
  // or other threads also need access.
  if (threadId != NULL) {
    *threadId = (thread_id_t)thread; // XXX: this is not portable
  }
  return 1;
}
};  // namespace android
