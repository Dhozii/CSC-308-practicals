# CSC 308 – Operating Systems Practicals
**Nnamdi Azikiwe University (UNIZIK)**

This repository contains four practical C programs covering process synchronization and inter-process communication (IPC) concepts as required for of CSC 308 – Operating Systems.

---

## Practical Sessions

| # | Topic | File |
|---|-------|------|
| 1 | Mutex Lock Demonstration | `session1_mutex.c` |
| 2 | Producer-Consumer Simulation | `session2_producer_consumer.c` |
| 3 | Semaphore Implementation in C | `session3_semaphore.c` |
| 4 | Shared Memory Programming | `session4_shared_memory.c` |

---

## How to Compile and Run

> **Requirements:** Linux OS (or WSL on Windows), GCC compiler

### Session 1 – Mutex Lock
```bash
gcc -o session1_mutex session1_mutex.c -lpthread
./session1_mutex
```
**Expected Output:** With mutex, counter equals exactly `NUM_THREADS × INCREMENT_COUNT`. Without mutex, the result is incorrect and non-deterministic.

---

### Session 2 – Producer-Consumer
```bash
gcc -o session2_producer_consumer session2_producer_consumer.c -lpthread
./session2_producer_consumer
```
**Expected Output:** Producer adds items to the circular buffer; consumer removes them. Buffer never overflows or underflows. No deadlock occurs.

---

### Session 3 – Semaphore vs Mutex
```bash
gcc -o session3_semaphore session3_semaphore.c -lpthread
./session3_semaphore
```
**Expected Output:** Both mutex and binary semaphore produce correct counter values. Counting semaphore limits concurrent thread access to 3 at a time.

---

### Session 4 – Shared Memory
```bash
gcc -o session4_shared_memory session4_shared_memory.c
./session4_shared_memory
```
**Expected Output:** Child process writes messages to shared memory; parent process reads them. Shared memory is properly cleaned up with `shmdt()` and `shmctl(IPC_RMID)`.

---

## Key Concepts Covered

- **Mutex Locks** – Binary locking mechanism for thread synchronization (`pthread_mutex_t`)
- **POSIX Semaphores** – Generalized synchronization with `sem_wait()` / `sem_post()`
- **Producer-Consumer Problem** – Classic IPC pattern using circular buffer + semaphores
- **Shared Memory IPC** – Fast inter-process communication via `shmget()` / `shmat()` / `shmdt()`
- **Race Conditions** – Demonstrated by running counter increment without protection
- **Counting Semaphores** – Limiting concurrent access to a pool of N resources

---

## Functions Reference

| Function | Purpose |
|----------|---------|
| `pthread_mutex_init/lock/unlock/destroy` | Mutex lifecycle and usage |
| `sem_init/wait/post/destroy` | POSIX semaphore operations |
| `shmget()` | Create shared memory segment |
| `shmat()` | Attach shared memory to process |
| `shmdt()` | Detach shared memory |
| `shmctl(IPC_RMID)` | Delete shared memory segment |
| `fork()` | Create child process |

---

*CSC 308 – Operating Systems | UNIZIK*
