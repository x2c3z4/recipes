/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */

/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LIBS_UTILS_CONDITION_H
#define _LIBS_UTILS_CONDITION_H

#include <stdint.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#include "Errors.h"
#include "Timers.h"
#include "Mutex.h"

// ---------------------------------------------------------------------------
namespace base {
// ---------------------------------------------------------------------------

/*
 * Condition variable class.  The implementation is system-dependent.
 *
 * Condition variables are paired up with mutexes.  Lock the mutex,
 * call wait(), then either re-wait() if things aren't quite what you want,
 * or unlock the mutex and continue.  All threads calling wait() must
 * use the same mutex for a given Condition.
 */
class Condition {
public:
  enum {
    PRIVATE = 0,
    SHARED = 1
  };

  enum WakeUpType {
    WAKE_UP_ONE = 0,
    WAKE_UP_ALL = 1
  };

  Condition();
  Condition(int type);
  ~Condition();
  // Wait on the condition variable.  Lock the mutex before calling.
  status_t wait(Mutex& mutex);
  // same with relative timeout
  status_t waitRelative(Mutex& mutex, nsecs_t reltime);
  // Signal the condition variable, allowing one thread to continue.
  void signal();
  // Signal the condition variable, allowing one or all threads to continue.
  void signal(WakeUpType type) {
    if (type == WAKE_UP_ONE) {
      signal();
    } else {
      broadcast();
    }
  }
  // Signal the condition variable, allowing all threads to continue.
  void broadcast();

private:
  pthread_cond_t mCond;
};

// ---------------------------------------------------------------------------
inline Condition::Condition() {
  pthread_cond_init(&mCond, NULL);
}
inline Condition::Condition(int type) {
  if (type == SHARED) {
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&mCond, &attr);
    pthread_condattr_destroy(&attr);
  } else {
    pthread_cond_init(&mCond, NULL);
  }
}
inline Condition::~Condition() {
  pthread_cond_destroy(&mCond);
}
inline status_t Condition::wait(Mutex& mutex) {
  return -pthread_cond_wait(&mCond, &mutex.mMutex);
}
inline status_t Condition::waitRelative(Mutex& mutex, nsecs_t reltime) {
  struct timespec ts;
  struct timeval t;
  gettimeofday(&t, NULL);
  ts.tv_sec = t.tv_sec;
  ts.tv_nsec = t.tv_usec * 1000;
  ts.tv_sec += reltime / 1000000000;
  ts.tv_nsec += reltime % 1000000000;
  if (ts.tv_nsec >= 1000000000) {
    ts.tv_nsec -= 1000000000;
    ts.tv_sec  += 1;
  }
  return -pthread_cond_timedwait(&mCond, &mutex.mMutex, &ts);
}
inline void Condition::signal() {
  pthread_cond_signal(&mCond);
}
inline void Condition::broadcast() {
  pthread_cond_broadcast(&mCond);
}
// ---------------------------------------------------------------------------
}; // namespace base
// ---------------------------------------------------------------------------
#endif // _LIBS_UTILS_CONDITON_H
