/*
 * Copyright (c) 2014
 * Author: Feng,Li (lifeng1519@gmail.com)
 */

#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "thirdparty/gtest/gtest.h"

#include "blocking_queue.h"
typedef struct {
  char video[88];
} Parcel;

const int NUM_FRAMES = 450;
const int producers = 1;
const int consumers = 1;

void* producer(void* queue) {
  unsigned int i;
  BlockingQueue *q = static_cast<BlockingQueue*>(queue);
  for (i = 0; i < 100; ++i) {
    Parcel* value = (Parcel*)malloc(sizeof(*value));
    char ch = 'A' + (random() % 26);
    printf("ch:%c\n", ch);
    memset(value->video, ch, sizeof(value->video));
    q->Enqueue(static_cast<void*>(value));
  }
  return NULL;
}

void* consumer(void* queue) {
  unsigned int i;
  BlockingQueue *q = static_cast<BlockingQueue*>(queue);
  for (i = 0; i < 100; ++i) {
    Parcel* value = static_cast<Parcel*>(q->Dequeue());
    printf("value = %s\n", value->video);
    free(value);
  }
  return NULL;
}

TEST(BlockingQueue, enqueue_and_dequeue) {
  unsigned int i;
  Parcel* buffer[NUM_FRAMES];
  BlockingQueue* queue = new BlockingQueue((void**)buffer);
  pthread_t producers[producers];
  for (i = 0; i < sizeof(producers) / sizeof(producers[0]); ++ i) {
    pthread_create(&producers[i], NULL, producer, queue);
  }
  pthread_t consumers[consumers];
  for (i = 0; i < sizeof(consumers) / sizeof(consumers[0]); ++ i) {
    pthread_create(&consumers[i], NULL, consumer, queue);
  }
  for (i = 0; i < sizeof(producers) / sizeof(producers[0]); ++ i) {
    pthread_join(producers[i], NULL);
  }
  for (i = 0; i < sizeof(consumers) / sizeof(consumers[0]); ++ i) {
    pthread_join(consumers[i], NULL);
  }
}
