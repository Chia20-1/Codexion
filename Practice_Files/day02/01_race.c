/*
** Day 2, exercise 1: a shared counter without protection.
** Build: gcc -Wall -Wextra -pthread 01_race.c -o 01_race
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static int g_counter;

static void *worker(void *arg)
{
	int id;
	int local;

	id = *(int *)arg;
	local = g_counter;
	usleep(100000);
	local = local + 1;
	g_counter = local;
	printf("worker %d wrote %d\n", id, local);
	return (NULL);
}

int main(void)
{
	pthread_t threads[5];
	int ids[5];
	int i;

	g_counter = 0;
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
	printf("final counter = %d (expected 5)\n", g_counter);
	return (0);
}
