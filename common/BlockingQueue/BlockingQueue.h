/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */
/*
   c++ implementation of a bounded buffer queue using posix threads
   */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#ifndef _QUEUE_H
#define _QUEUE_H

class BlockingQueue {
 public:
  BlockingQueue(void**);
  ~BlockingQueue();
  void Enqueue(void* value);
  void* Dequeue();
  void Destroy();
  int Size();
 private:
  void** buffer;
  const int capacity;
  int size;
  int in;
  int out;
  pthread_mutex_t mutex;
  pthread_cond_t cond_full;
  pthread_cond_t cond_empty;
};
#endif
