/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */

#ifndef BASE_BLOCKINGQUEUE_H_
#define BASE_BLOCKINGQUEUE_H_

#include <deque>
#include <assert.h>

#include "Condition.h"
#include "Mutex.h"

namespace base
{

template<typename T>
class BlockingQueue
{
 public:
  BlockingQueue()
    : mutex_(),
      notEmpty_(),
      queue_()
  {
  }

  void put(const T& x)
  {
    AutoMutex lock(mutex_);
    queue_.push_back(x);
    notEmpty_.signal(); // wait morphing saves us
    // http://www.domaigne.com/blog/computing/condvars-signal-with-mutex-locked-or-not/
  }

  T take()
  {
    AutoMutex lock(mutex_);
    // always use a while-loop, due to spurious wakeup
    while (queue_.empty())
    {
      notEmpty_.wait(mutex_);
    }
    assert(!queue_.empty());
    T front(queue_.front());
    queue_.pop_front();
    return front;
  }

  size_t size() const
  {
    AutoMutex lock(mutex_);
    return queue_.size();
  }

 private:
  mutable Mutex mutex_;
  Condition         notEmpty_;
  std::deque<T>     queue_;
};

}

#endif  // BASE_BLOCKINGQUEUE_H_
