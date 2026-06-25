/*
 * CSC 308 - Operating Systems
 * Practical Session 2: Producer-Consumer Simulation
 * Objective: Implement the Producer-Consumer problem using POSIX semaphores
 *
 * Compile: gcc -o session2_producer_consumer session2_producer_consumer.c -lpthread
 * Run:     ./session2_producer_consumer
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE  5
#define NUM_ITEMS    10

/* Circular buffer */
int buffer[BUFFER_SIZE];
int in  = 0;   /* Producer inserts at 'in'  */
int out = 0;   /* Consumer removes at 'out' */

/* Semaphores:
 *   mutex - ensures only one thread accesses buffer at a time (init = 1)
 *   empty - counts empty slots in buffer                      (init = N)
 *   full  - counts filled slots in buffer                     (init = 0)
 */
sem_t mutex_sem;
sem_t empty_sem;
sem_t full_sem;

void print_buffer() {
    printf("  Buffer: [");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (buffer[i] != -1)
            printf(" %d", buffer[i]);
        else
            printf("  _");
    }
    printf(" ]  (in=%d, out=%d)\n", in, out);
}

/* Producer thread: generates items and puts them in the buffer */
void *producer(void *arg) {
    for (int item = 1; item <= NUM_ITEMS; item++) {
        sleep(1);  /* Simulate time to produce */

        sem_wait(&empty_sem);  /* Wait if buffer is full  */
        sem_wait(&mutex_sem);  /* Lock buffer             */

        /* Critical section: insert item */
        buffer[in] = item;
        printf("[PRODUCER] Produced item %2d -> slot %d\n", item, in);
        in = (in + 1) % BUFFER_SIZE;
        print_buffer();

        sem_post(&mutex_sem);  /* Unlock buffer           */
        sem_post(&full_sem);   /* Signal: one more full   */
    }
    return NULL;
}

/* Consumer thread: takes items from buffer and consumes them */
void *consumer(void *arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        sleep(2);  /* Consumer is slower — watch buffer fill up! */

        sem_wait(&full_sem);   /* Wait if buffer is empty */
        sem_wait(&mutex_sem);  /* Lock buffer             */

        /* Critical section: remove item */
        int item = buffer[out];
        buffer[out] = -1;      /* Mark slot as empty      */
        printf("[CONSUMER] Consumed item %2d <- slot %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;
        print_buffer();

        sem_post(&mutex_sem);  /* Unlock buffer           */
        sem_post(&empty_sem);  /* Signal: one more empty  */
    }
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;

    printf("=== CSC 308 | Practical Session 2: Producer-Consumer ===\n\n");
    printf("Buffer size : %d slots\n", BUFFER_SIZE);
    printf("Total items : %d\n", NUM_ITEMS);
    printf("Producer speed: 1s | Consumer speed: 2s\n");
    printf("Watch the buffer fill and empty...\n\n");

    /* Initialize buffer slots as empty (-1) */
    for (int i = 0; i < BUFFER_SIZE; i++) buffer[i] = -1;

    /* Initialize semaphores */
    sem_init(&mutex_sem, 0, 1);           /* Binary mutex        */
    sem_init(&empty_sem, 0, BUFFER_SIZE); /* All slots empty     */
    sem_init(&full_sem,  0, 0);           /* No slots full yet   */

    /* Create producer and consumer threads */
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    /* Wait for both threads to finish */
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    /* Destroy semaphores */
    sem_destroy(&mutex_sem);
    sem_destroy(&empty_sem);
    sem_destroy(&full_sem);

    printf("\nAll items produced and consumed. No deadlock, no overflow.\n");
    return 0;
}
