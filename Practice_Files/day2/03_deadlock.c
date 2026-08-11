/*
** Day 2, exercise 3: deliberate deadlock.
** This program is expected to hang. Stop it with Ctrl-C.
** Build: gcc -Wall -Wextra -pthread 03_deadlock.c -o 03_deadlock
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static pthread_mutex_t g_lock1;
static pthread_mutex_t g_lock2;

static void *worker_a(void *arg)
{
	(void)arg;
	pthread_mutex_lock(&g_lock1);
	printf("A owns lock1, waiting for lock2\n");
	usleep(100000);
	pthread_mutex_lock(&g_lock2);
	return (NULL);
}

static void *worker_b(void *arg)
{
	(void)arg;
	pthread_mutex_lock(&g_lock2);
	printf("B owns lock2, waiting for lock1\n");
	usleep(100000);
	pthread_mutex_lock(&g_lock1);
	return (NULL);
}

int main(void)
{
	pthread_t a;
	pthread_t b;

	pthread_mutex_init(&g_lock1, NULL);
	pthread_mutex_init(&g_lock2, NULL);
	pthread_create(&a, NULL, worker_a, NULL);
	pthread_create(&b, NULL, worker_b, NULL);
	pthread_join(a, NULL);
	pthread_join(b, NULL);
	return (0);
}
