/*
** day02.c
**
** Learn mutexes, race conditions, and deadlocks.
** This file shows:
**   - a shared counter race condition
**   - how to protect shared state with pthread mutexes
**   - a simple deadlock example with two mutexes
**
** Build:
**   gcc -pthread Practice_Files/day02.c -o Practice_Files/day02
**
** Run:
**   ./Practice_Files/day02
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/* Shared state for the counter exercises. */
static int g_counter = 0;
static pthread_mutex_t g_counter_mutex;

/* A small helper to show a thread ID in the output. */
static void print_thread(const char *name, int id)
{
    printf("[%s] thread %d\n", name, id);
}

/*
** Race condition worker:
** Each thread reads the counter, waits, then writes it back.
** Without a lock, two threads can read the same value and both store the same incremented result.
*/
static void *race_worker(void *arg)
{
    int id = *(int *)arg;
    int local;

    print_thread("race", id);
    local = g_counter;
    usleep(100000); /* 100 ms: force the race by delaying */
    local = local + 1;
    g_counter = local;
    printf("[race] thread %d wrote %d\n", id, local);
    return NULL;
}

/*
** Mutex-protected worker:
** The same counter update, but the mutex makes it atomic from the point of view of other threads.
*/
static void *mutex_worker(void *arg)
{
    int id = *(int *)arg;

    print_thread("mutex", id);
    pthread_mutex_lock(&g_counter_mutex);
    g_counter = g_counter + 1;
    printf("[mutex] thread %d incremented counter to %d\n", id, g_counter);
    pthread_mutex_unlock(&g_counter_mutex);
    return NULL;
}

/*
** These two mutexes are used for the deadlock demonstration.
** Thread A locks first m1 then m2. Thread B locks first m2 then m1.
*/
static pthread_mutex_t g_lock1;
static pthread_mutex_t g_lock2;

static void *deadlock_a(void *arg)
{
    (void)arg;
    printf("[deadlock] A trying lock1\n");
    pthread_mutex_lock(&g_lock1);
    printf("[deadlock] A got lock1\n");
    usleep(100000); /* let B start and grab lock2 */
    printf("[deadlock] A trying lock2\n");
    pthread_mutex_lock(&g_lock2);
    printf("[deadlock] A got lock2 (no deadlock!)\n");
    pthread_mutex_unlock(&g_lock2);
    pthread_mutex_unlock(&g_lock1);
    return NULL;
}

static void *deadlock_b(void *arg)
{
    (void)arg;
    printf("[deadlock] B trying lock2\n");
    pthread_mutex_lock(&g_lock2);
    printf("[deadlock] B got lock2\n");
    usleep(100000); /* let A hold lock1 first */
    printf("[deadlock] B trying lock1\n");
    pthread_mutex_lock(&g_lock1);
    printf("[deadlock] B got lock1 (no deadlock!)\n");
    pthread_mutex_unlock(&g_lock1);
    pthread_mutex_unlock(&g_lock2);
    return NULL;
}

static void run_race_example(void)
{
    pthread_t threads[5];
    int ids[5];
    int i;

    g_counter = 0;
    printf("\n=== Race condition example ===\n");
    i = 0;
    while (i < 5)
    {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, race_worker, &ids[i]);
        i++;
    }
    i = 0;
    while (i < 5)
    {
        pthread_join(threads[i], NULL);
        i++;
    }
    printf("[race] final counter = %d (expected 5)\n", g_counter);
}

static void run_mutex_example(void)
{
    pthread_t threads[5];
    int ids[5];
    int i;

    g_counter = 0;
    printf("\n=== Mutex-protected example ===\n");
    pthread_mutex_init(&g_counter_mutex, NULL);
    i = 0;
    while (i < 5)
    {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, mutex_worker, &ids[i]);
        i++;
    }
    i = 0;
    while (i < 5)
    {
        pthread_join(threads[i], NULL);
        i++;
    }
    pthread_mutex_destroy(&g_counter_mutex);
    printf("[mutex] final counter = %d (expected 5)\n", g_counter);
}

static void run_deadlock_example(void)
{
    pthread_t thread_a;
    pthread_t thread_b;

    printf("\n=== Deadlock example ===\n");
    pthread_mutex_init(&g_lock1, NULL);
    pthread_mutex_init(&g_lock2, NULL);

    pthread_create(&thread_a, NULL, deadlock_a, NULL);
    pthread_create(&thread_b, NULL, deadlock_b, NULL);

    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);

    pthread_mutex_destroy(&g_lock1);
    pthread_mutex_destroy(&g_lock2);
}

int main(void)
{
    printf("Day 2: Mutexes, race conditions, deadlocks\n");
    run_race_example();
    run_mutex_example();
    run_deadlock_example();
    printf("\nSummary:\n");
    printf(" - A race happens when two threads read/modify/write the same data without coordination.\n");
    printf(" - A mutex is a lock that makes one thread own the shared state while it updates it.\n");
    printf(" - Deadlock can happen when each thread waits for a lock held by the other.\n");
    printf(" - To avoid deadlock, keep a consistent lock order or avoid holding multiple locks at once.\n");
    return 0;
}
