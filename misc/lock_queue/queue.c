/*
c-pthread-queue - c implementation of a bounded buffer queue using posix threads
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#include "queue.h"



void queue_enqueue(queue_t *queue, void *value)
{
	#ifdef CONFIG_SEMAPHORE
		sem_wait(&queue->nempty);
		sem_wait(&queue->mutex);
	#else
		pthread_mutex_lock(&(queue->mutex));
	#endif
		/*enter critical area*/

        while (queue->size == queue->capacity)
		{
		#ifdef CONFIG_SEMAPHORE
			sem_post(&queue->nempty);
		#else
            pthread_cond_wait(&(queue->cond_full), &(queue->mutex));
		#endif
		}
		/*dead lock
		printf("enqueue %d\n", *(int *)value);
		fflush(0);
		queue_size(queue);
		*/
        printf("enqueue %d\n", *(int *)value);
        queue->buffer[queue->in] = value;
        ++ queue->size;
        ++ queue->in;
        queue->in %= queue->capacity;
		/* leave critical area*/
	#ifdef CONFIG_SEMAPHORE
		sem_post(&queue->mutex);
		sem_post(&queue->nstored);
	#else
        pthread_mutex_unlock(&(queue->mutex));
        pthread_cond_broadcast(&(queue->cond_empty));
	#endif
}

void *queue_dequeue(queue_t *queue)
{
	#ifdef CONFIG_SEMAPHORE
		sem_wait(&queue->nstored);
		sem_wait(&queue->mutex);
	#else
        pthread_mutex_lock(&(queue->mutex));
	#endif
		/*enter critical area*/
        while (queue->size == 0)
		{
		#ifdef CONFIG_SEMAPHORE
			sem_post(&queue->nempty);
		#else
            pthread_cond_wait(&(queue->cond_empty), &(queue->mutex));
		#endif
		}
        void *value = queue->buffer[queue->out];
        printf("dequeue %d\n", *(int *)value);
        -- queue->size;
        ++ queue->out;
        queue->out %= queue->capacity;
		/* leave critical area*/
	#ifdef CONFIG_SEMAPHORE
		sem_post(&queue->mutex);
		sem_post(&queue->nempty);
	#else
	    pthread_cond_broadcast(&(queue->cond_full));
        pthread_mutex_unlock(&(queue->mutex));
	#endif
        return value;
}

void queue_destroy(queue_t *queue)
{
	#ifdef CONFIG_SEMAPHORE
		sem_destroy(&queue->mutex);
		sem_destroy(&queue->nempty);
		sem_destroy(&queue->nstored);
	#else
		pthread_cond_destroy(&queue->cond_full);
		pthread_cond_destroy(&queue->cond_empty);
		pthread_mutex_destroy(&(queue->mutex));
	#endif
}

int queue_size(queue_t *queue)
{
	#ifdef CONFIG_SEMAPHORE
		sem_wait(&queue->mutex);
		int size = queue->size;
		sem_wait(&queue->mutex);
		return size;
	#else
		pthread_mutex_lock(&queue->mutex);
		int size = queue->size;
		pthread_mutex_unlock(&(queue->mutex));
		return size;
	#endif
}

