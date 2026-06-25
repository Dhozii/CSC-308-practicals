/*
 * CSC 308 - Operating Systems
 * Practical Session 1: Mutex Lock Demonstration
 * Objective: Demonstrate mutual exclusion using pthread mutex locks
 *
 * Compile: gcc -o session1_mutex session1_mutex.c -lpthread
 * Run:     ./session1_mutex
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 10
#define INCREMENT_COUNT 100000

/* Shared counter - accessed by all threads */
long counter_with_mutex = 0;
long counter_without_mutex = 0;

/* Mutex lock variable */
pthread_mutex_t mutex;

/* Thread function WITH mutex protection */
void *increment_with_mutex(void *arg) {
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        pthread_mutex_lock(&mutex);      /* Lock before accessing shared resource */
        counter_with_mutex++;
        pthread_mutex_unlock(&mutex);    /* Unlock after done */
    }
    return NULL;
}

/* Thread function WITHOUT mutex protection (unsafe) */
void *increment_without_mutex(void *arg) {
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        counter_without_mutex++;         /* Race condition happens here */
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    printf("=== CSC 308 | Practical Session 1: Mutex Lock Demo ===\n\n");
    printf("Number of threads : %d\n", NUM_THREADS);
    printf("Increments/thread : %d\n", INCREMENT_COUNT);
    printf("Expected total    : %d\n\n", NUM_THREADS * INCREMENT_COUNT);

    /* ---- TEST 1: WITH MUTEX ---- */
    pthread_mutex_init(&mutex, NULL);   /* Initialize the mutex */

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_with_mutex, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);      /* Destroy the mutex when done */

    printf("[WITH MUTEX]    Final counter = %ld (Correct: %s)\n",
           counter_with_mutex,
           counter_with_mutex == (long)(NUM_THREADS * INCREMENT_COUNT) ? "YES" : "NO");

    /* ---- TEST 2: WITHOUT MUTEX ---- */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_without_mutex, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[WITHOUT MUTEX] Final counter = %ld (Correct: %s)\n",
           counter_without_mutex,
           counter_without_mutex == (long)(NUM_THREADS * INCREMENT_COUNT) ? "YES" : "NO");

    printf("\nNote: Run the program multiple times — without mutex gives different results each time!\n");

    return 0;
}
