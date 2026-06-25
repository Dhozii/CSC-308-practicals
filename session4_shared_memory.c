/*
 * CSC 308 - Operating Systems
 * Practical Session 4: Shared Memory Programming
 * Objective: Implement inter-process communication using shared memory
 *
 * Compile: gcc -o session4_shared_memory session4_shared_memory.c -lpthread
 * Run:     ./session4_shared_memory
 *
 * Applications: Database servers, Web servers, Scientific computing, Real-time systems
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define SHM_KEY      1234
#define SHM_SIZE     1024

/* Shared memory structure: holds data + a semaphore for synchronization */
typedef struct {
    char message[512];           /* Message written by one process, read by another */
    int  counter;                /* Shared integer counter                           */
    int  writer_done;            /* Flag: writer has finished                        */
} SharedData;

int main() {
    int shmid;
    SharedData *shm_ptr;
    pid_t pid;

    printf("=== CSC 308 | Practical Session 4: Shared Memory IPC ===\n\n");

    /* Step 1: Create a shared memory segment using shmget() */
    shmid = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    printf("[PARENT] Shared memory segment created (shmid = %d)\n", shmid);

    /* Step 2: Attach shared memory to parent process address space */
    shm_ptr = (SharedData *)shmat(shmid, NULL, 0);
    if (shm_ptr == (SharedData *)-1) {
        perror("shmat failed");
        exit(1);
    }

    /* Initialize shared memory */
    memset(shm_ptr, 0, sizeof(SharedData));
    shm_ptr->writer_done = 0;
    shm_ptr->counter     = 0;
    printf("[PARENT] Shared memory attached and initialized.\n\n");

    /* Step 3: Fork a child process */
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    /* ======== CHILD PROCESS: WRITER ======== */
    if (pid == 0) {
        /* Step 2 (child): Attach shared memory in child process too */
        shm_ptr = (SharedData *)shmat(shmid, NULL, 0);
        if (shm_ptr == (SharedData *)-1) {
            perror("child shmat failed");
            exit(1);
        }

        printf("[CHILD]  Writer process started (PID: %d)\n", getpid());

        /* Write data into shared memory */
        for (int i = 1; i <= 5; i++) {
            shm_ptr->counter = i;
            snprintf(shm_ptr->message, sizeof(shm_ptr->message),
                     "Hello from child process! Count = %d", i);
            printf("[CHILD]  Wrote: \"%s\"\n", shm_ptr->message);
            sleep(1);  /* Simulate writing at intervals */
        }

        /* Signal that writing is done */
        shm_ptr->writer_done = 1;
        printf("[CHILD]  Writing complete. Signaling parent...\n");

        /* Step 5: Detach shared memory in child */
        shmdt(shm_ptr);
        exit(0);
    }

    /* ======== PARENT PROCESS: READER ======== */
    else {
        printf("[PARENT] Reader process waiting for child to write...\n\n");
        sleep(1);  /* Give child a head start */

        /* Step 4: Read data from shared memory (synchronize using flag) */
        int last_count = 0;
        while (!shm_ptr->writer_done) {
            if (shm_ptr->counter > last_count) {
                printf("[PARENT] Read:  \"%s\"\n", shm_ptr->message);
                last_count = shm_ptr->counter;
            }
            usleep(200000);  /* Poll every 200ms */
        }

        /* Read the final value after writer signals done */
        printf("[PARENT] Read:  \"%s\"\n", shm_ptr->message);
        printf("\n[PARENT] Writer finished. Final counter = %d\n", shm_ptr->counter);

        /* Wait for child process to exit */
        wait(NULL);

        /* Step 5: Detach and remove shared memory */
        shmdt(shm_ptr);
        shmctl(shmid, IPC_RMID, NULL);  /* Remove shared memory segment */

        printf("[PARENT] Shared memory detached and removed. IPC complete.\n\n");
        printf("Applications of Shared Memory IPC:\n");
        printf("  - Database servers  (e.g., PostgreSQL shared buffer pool)\n");
        printf("  - Web servers       (e.g., Nginx worker processes)\n");
        printf("  - Scientific computing (parallel simulations)\n");
        printf("  - Real-time systems (low-latency data exchange)\n");
    }

    return 0;
}
