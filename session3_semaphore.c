/*
 * CSC 308 - Operating Systems
 * Practical Session 3: Semaphore Implementation in C
 * Objective: Compare mutex locks and semaphores for protecting shared resources
 *
 * Compile: gcc -o session3_semaphore session3_semaphore.c -lpthread
 * Run:     ./session3_semaphore
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS        10
#define INCREMENT_COUNT    50000

/* --- Shared resources --- */
long counter_mutex = 0;
long counter_sem   = 0;

/* --- Synchronization tools --- */
pthread_mutex_t mutex;
sem_t binary_sem;

/* Counting semaphore demo: only 3 threads can enter at once */
sem_t counting_sem;
int active_threads = 0;

/* --- MUTEX approach --- */
void *increment_mutex(void *arg) {
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        pthread_mutex_lock(&mutex);
        counter_mutex++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/* --- BINARY SEMAPHORE approach --- */
void *increment_semaphore(void *arg) {
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        sem_wait(&binary_sem);   /* P operation - decrement */
        counter_sem++;
        sem_post(&binary_sem);   /* V operation - increment */
    }
    return NULL;
}

/* --- COUNTING SEMAPHORE demo: max 3 concurrent threads --- */
void *counting_semaphore_demo(void *arg) {
    int id = *((int *)arg);

    sem_wait(&counting_sem);     /* Acquire slot — blocks if 3 already inside */
    active_threads++;
    printf("  Thread %2d ENTERED  | Active threads: %d\n", id, active_threads);

    sleep(1);  /* Simulate work inside critical section */

    active_threads--;
    printf("  Thread %2d EXITING  | Active threads: %d\n", id, active_threads);
    sem_post(&counting_sem);     /* Release slot */

    return NULL;
}

/* --- Timing helper --- */
double time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0
         + (end.tv_nsec - start.tv_nsec) / 1e6;
}

int main() {
    pthread_t threads[NUM_THREADS];
    struct timespec t_start, t_end;

    printf("=== CSC 308 | Practical Session 3: Semaphore vs Mutex ===\n\n");
    printf("Threads: %d | Increments per thread: %d | Expected: %d\n\n",
           NUM_THREADS, INCREMENT_COUNT, NUM_THREADS * INCREMENT_COUNT);

    /* === TEST 1: MUTEX === */
    pthread_mutex_init(&mutex, NULL);
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, increment_mutex, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    pthread_mutex_destroy(&mutex);

    printf("[MUTEX]           Counter = %ld | Correct: %s | Time: %.2f ms\n",
           counter_mutex,
           counter_mutex == (long)(NUM_THREADS * INCREMENT_COUNT) ? "YES" : "NO",
           time_diff(t_start, t_end));

    /* === TEST 2: BINARY SEMAPHORE === */
    sem_init(&binary_sem, 0, 1);
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, increment_semaphore, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    sem_destroy(&binary_sem);

    printf("[BINARY SEM]      Counter = %ld | Correct: %s | Time: %.2f ms\n",
           counter_sem,
           counter_sem == (long)(NUM_THREADS * INCREMENT_COUNT) ? "YES" : "NO",
           time_diff(t_start, t_end));

    /* === TEST 3: COUNTING SEMAPHORE (max 3 threads at a time) === */
    printf("\n--- Counting Semaphore: Only 3 threads allowed simultaneously ---\n");
    sem_init(&counting_sem, 0, 3);  /* Allow max 3 threads inside */

    int ids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, counting_semaphore_demo, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    sem_destroy(&counting_sem);

    printf("\nKey Takeaway:\n");
    printf("  - Mutex:           Best for binary (0/1) exclusive access\n");
    printf("  - Binary Semaphore: Similar to mutex, but more flexible signaling\n");
    printf("  - Counting Semaphore: Controls access to a POOL of N resources\n");

    return 0;
}
