# Day 2 — Mutexes, race conditions, and deadlocks

## Study order

1. Run `01_race.c`.
2. Explain why the final counter may be less than 5.
3. Run `02_mutex.c`.
4. Identify the critical section between `pthread_mutex_lock` and `pthread_mutex_unlock`.
5. Read the deadlock example, but run it only with a timeout:

```sh
timeout 3s ./03_deadlock
```

6. Run `04_deadlock_fixed.c` and compare the lock order.

## Mutex lifecycle

```text
pthread_mutex_init()
        ↓
pthread_mutex_lock()
        ↓
      shared data
        ↓
pthread_mutex_unlock()
        ↓
pthread_mutex_destroy()
```

## Race condition

A race occurs when multiple threads access shared data and at least one thread
modifies it without synchronization. Reading, incrementing, and writing a
counter must be treated as one protected operation.

## Deadlock

`03_deadlock.c` creates a circular wait:

```text
A owns lock1 → waits for lock2
B owns lock2 → waits for lock1
```

The four Coffman conditions are:

1. Mutual exclusion — a lock has one owner at a time.
2. Hold and wait — a thread holds one lock while requesting another.
3. No preemption — a lock cannot be forcibly taken away.
4. Circular wait — each thread waits for a lock held by another thread.

`04_deadlock_fixed.c` prevents circular wait by using one global lock order:
both workers acquire `lock1` before `lock2`.

## End-of-day goal

Be able to explain why unsynchronized shared state is unsafe, how a mutex
protects a critical section, and how consistent lock ordering prevents deadlock.
