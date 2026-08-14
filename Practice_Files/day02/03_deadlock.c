/*
** Day 2, exercise 3: deliberate deadlock.
** This program is expected to hang. Stop it with Ctrl-C.
** Build: gcc -Wall -Wextra -pthread 03_deadlock.c -o 03_deadlock
*/

#include<pthread.h>
#include<unistd.h>
#include<stdio.h>

static int	global_counter;
static pthread_mutex_t		mutex_a;
static pthread_mutex_t		mutex_b;

void *worker_a(void *arg)
{
	int		local;
	int		id;

	local = global_counter;
	id = *(int *)arg;
	pthread_mutex_lock(&mutex_a);
	local = local + 1;
	global_counter = local;
	usleep(100000);
	printf("Worker %d at count %d\n", id, local);
	printf("Current global counter: %d\n", global_counter);
	pthread_mutex_lock(&mutex_b);
	return (NULL);
}

void *worker_b(void *arg)
{
	int		local;
	int		id;

	local = global_counter;
	id = *(int *)arg;
	pthread_mutex_lock(&mutex_b);
	local = local + 1;
	global_counter = local;
	usleep(100000);
	printf("Worker %d at count %d\n", id, local);
	printf("Current global counter: %d\n", global_counter);
	pthread_mutex_lock(&mutex_a);
	return (NULL);
}

int main(void)
{
	pthread_t			thread_a;
	pthread_t			thread_b;
	int					id_a;
	int					id_b;

	id_a = 1;
	id_b = 2;
	pthread_mutex_init(&mutex_a, NULL);
	pthread_mutex_init(&mutex_b, NULL);
	pthread_create(&thread_a, NULL, worker_a, &id_a);
	pthread_create(&thread_b, NULL, worker_b, &id_b);
	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);
}
