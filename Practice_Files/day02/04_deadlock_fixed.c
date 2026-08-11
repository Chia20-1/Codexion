/*
** Day 2, exercise 4: avoid deadlock with one lock order.
** Both workers lock lock1 before lock2.
** Build: gcc -Wall -Wextra -pthread 04_deadlock_fixed.c -o 04_deadlock_fixed
*/

#include <pthread.h>
#include <stdio.h>

static pthread_mutex_t g_lock1;
static pthread_mutex_t g_lock2;

static void *worker(void *arg)
{
	const char *name;

	name = (const char *)arg;
	pthread_mutex_lock(&g_lock1);
	pthread_mutex_lock(&g_lock2);
	printf("%s acquired both locks\n", name);
	pthread_mutex_unlock(&g_lock2);
	pthread_mutex_unlock(&g_lock1);
	return (NULL);
}

int main(void)
{
	pthread_t a;
	pthread_t b;

	pthread_mutex_init(&g_lock1, NULL);
	pthread_mutex_init(&g_lock2, NULL);
	pthread_create(&a, NULL, worker, "A");
	pthread_create(&b, NULL, worker, "B");
	pthread_join(a, NULL);
	pthread_join(b, NULL);
	pthread_mutex_destroy(&g_lock1);
	pthread_mutex_destroy(&g_lock2);
	return (0);
}
