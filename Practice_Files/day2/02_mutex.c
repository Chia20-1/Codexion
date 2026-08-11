/*
** Day 2, exercise 2: protect the shared counter with a mutex.
** Build: gcc -Wall -Wextra -pthread 02_mutex.c -o 02_mutex
*/

#include <pthread.h>
#include <stdio.h>

static int g_counter;
static pthread_mutex_t g_counter_mutex;

static void *worker(void *arg)
{
	int id;

	id = *(int *)arg;
	pthread_mutex_lock(&g_counter_mutex);
	g_counter = g_counter + 1;
	printf("worker %d incremented counter to %d\n", id, g_counter);
	pthread_mutex_unlock(&g_counter_mutex);
	return (NULL);
}

int main(void)
{
	pthread_t threads[5];
	int ids[5];
	int i;

	g_counter = 0;
	pthread_mutex_init(&g_counter_mutex, NULL);
	i = 0;
	while (i < 5)
	{
		ids[i] = i + 1;
		pthread_create(&threads[i], NULL, worker, &ids[i]);
		i++;
	}
	i = 0;
	while (i < 5)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_mutex_destroy(&g_counter_mutex);
	printf("final counter = %d (expected 5)\n", g_counter);
	return (0);
}
