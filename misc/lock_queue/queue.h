/*
c-pthread-queue - c implementation of a bounded buffer queue using posix threads
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#ifndef _QUEUE_H
#define _QUEUE_H

#ifdef CONFIG_SEMAPHORE
#define QUEUE_INITIALIZER(name,buffer) \
		queue_t name={			\
		.buffer = buffer, \
		.capacity = sizeof(buffer) / sizeof(buffer[0]), \
		.size = 0,\
		.in = 0, \
		.out=0};\
		sem_init(&(name.mutex),0,1);	\
		sem_init(&(name.nempty),0,name.capacity);	\
		sem_init(&(name.nstored),0,0)

#else
#define QUEUE_INITIALIZER(name,buffer) \
	queue_t name={ buffer, \
	sizeof(buffer) / sizeof(buffer[0]), \
	0, \
	0, \
	0, \
	PTHREAD_MUTEX_INITIALIZER, \
	PTHREAD_COND_INITIALIZER,\
	PTHREAD_COND_INITIALIZER }
#endif

typedef struct queue
{
        void **buffer;
        const int capacity;
        int size;
        int in;
        int out;
	#ifdef CONFIG_SEMAPHORE
		sem_t mutex,nempty,nstored;
	#else
        pthread_mutex_t mutex;
        pthread_cond_t cond_full;
        pthread_cond_t cond_empty;
	#endif
} queue_t;

void queue_enqueue(queue_t *queue, void *value);

void *queue_dequeue(queue_t *queue);

void queue_destroy(queue_t *queue);

int queue_size(queue_t *queue);

#endif
