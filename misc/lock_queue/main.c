/*
c-pthread-queue - c implementation of a bounded buffer queue using posix threads
*/

#include <stdlib.h>
#include <pthread.h>
#include "queue.h"

// Size of the shared queue buffer
// Must be greater than zero
const int buffer_len = 10;
// Number of producer threads to start
// Must be greater than zero
const int producers = 3;
// Number of integers to be produced by each producer thread
// Must be greater than zero
const int producer_amount = 5;
// Number of consumer threads to start
// Must be greater than zero
const int consumers = 5;
// producers * producer_amount / consumers must be a whole integer

void *producer(void *arg)
{
        int i;
        for (i = 0; i < producer_amount; ++ i)
        {
                int *value = malloc(sizeof(*value));
                *value = i;
                queue_enqueue(arg, value);
				sleep(0);
        }
}

void *consumer(void *arg)
{
        int i;
        for (i = 0; i < producer_amount * producers / consumers; ++ i)
        {
                int *value = queue_dequeue(arg);
                free(value);
				sleep(0);
        }
}

int main()
{
        void *buffer[buffer_len];
        QUEUE_INITIALIZER(queue,buffer);
        
        int i;
        pthread_t producers[producers];
        for (i = 0; i < sizeof(producers) / sizeof(producers[0]); ++ i)
        {
                pthread_create(&producers[i], NULL, producer, &queue);
        }
        pthread_t consumers[consumers];
        for (i = 0; i < sizeof(consumers) / sizeof(consumers[0]); ++ i)
        {
                pthread_create(&consumers[i], NULL, consumer, &queue);
        }
        for (i = 0; i < sizeof(producers) / sizeof(producers[0]); ++ i)
        {
                pthread_join(producers[i], NULL);
        }
        for (i = 0; i < sizeof(consumers) / sizeof(consumers[0]); ++ i)
        {
                pthread_join(consumers[i], NULL);
        }
		
		queue_destroy(&queue);
		return 0;
}
