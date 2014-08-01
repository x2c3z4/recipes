/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include "BlockingQueue.h"

BlockingQueue::BlockingQueue(void** buffer):
  buffer(buffer),
  capacity(sizeof(buffer) / sizeof(buffer[0])),
  size(0),
  in(0),
  out(0),
  mutex(PTHREAD_MUTEX_INITIALIZER),
  cond_full(PTHREAD_COND_INITIALIZER),
  cond_empty(PTHREAD_COND_INITIALIZER) {
}
BlockingQueue::~BlockingQueue() {
  pthread_cond_destroy(&cond_full);
  pthread_cond_destroy(&cond_empty);
  pthread_mutex_destroy(&(mutex));
}
void BlockingQueue::Enqueue(void* value) {
  pthread_mutex_lock(&(mutex));
  /*enter critical area*/
  while (size == capacity) {
    pthread_cond_wait(&(cond_full), &(mutex));
  }
  printf("enqueue %d\n", *(int*)value);
  buffer[in] = value;
  ++size;
  ++in;
  in %= capacity;
  /* leave critical area*/
  pthread_mutex_unlock(&(mutex));
  pthread_cond_broadcast(&(cond_empty));
}

void* BlockingQueue::Dequeue() {
  pthread_mutex_lock(&(mutex));
  /*enter critical area*/
  while (size == 0) {
    pthread_cond_wait(&(cond_empty), &(mutex));
  }
  void* value = buffer[out];
  printf("dequeue %d\n", *(int*)value);
  --size;
  ++out;
  out %= capacity;
  /* leave critical area*/
  pthread_cond_broadcast(&(cond_full));
  pthread_mutex_unlock(&(mutex));
  return value;
}


int BlockingQueue::Size() {
  pthread_mutex_lock(&mutex);
  int size = this->size;
  pthread_mutex_unlock(&(mutex));
  return size;
}

