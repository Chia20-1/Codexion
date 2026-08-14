/*
** Day 2, exercise 2: protect the shared counter with a mutex.
** Build: gcc -Wall -Wextra -pthread 02_mutex.c -o 02_mutex
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef struct s_worker_data
{
	int				id;
	pthread_mutex_t	*counter_mutex;
}	t_worker_data;

static int g_counter;

static void *worker(void *arg)
{
	t_worker_data	*data;
	int 			local;

	data = (t_worker_data *)arg;
	pthread_mutex_lock(data->counter_mutex);
	local = g_counter;
	usleep(100000);
	local = local + 1;
	g_counter = local;
	printf("worker %d wrote %d\n", data->id, local);
	pthread_mutex_unlock(data->counter_mutex);
	return (NULL);
}

int main(void)
{
	pthread_t 		threads[5];
	pthread_mutex_t	counter_mutex;
	t_worker_data	data[5];
	int 			i;

	g_counter = 0;
	pthread_mutex_init(&counter_mutex, NULL);

	i = 0;
	while (i < 5)
	{
		data[i].id = i + 1;
		data[i].counter_mutex = &counter_mutex;
		pthread_create(&threads[i], NULL, worker, &data[i]);
		i++;
	}
	i = 0;
	while (i < 5)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_mutex_destroy(&counter_mutex);
	printf("final counter = %d (expected 5)\n", g_counter);
	return (0);
}
