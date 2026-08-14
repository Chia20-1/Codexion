/*
** Day 3, exercise 2: sleep until shared state becomes available.
** Build: cc -Wall -Wextra -Werror -pthread 02_condition_wait.c -o 02_condition_wait
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef struct s_shared
{
	pthread_mutex_t	mutex;
	pthread_cond_t	condition;
	int				available;
}t_shared;

static void	*worker(void *arg)
{
	t_shared	*shared;

	shared = (t_shared *)arg;
	pthread_mutex_lock(&shared->mutex);
	while (shared->available == 0)
	{
		printf("worker: unavailable, going to sleep\n");
		pthread_cond_wait(&shared->condition, &shared->mutex);
		printf("worker: woke up, checking again\n");
	}
	printf("worker: resource is available\n");
	pthread_mutex_unlock(&shared->mutex);
	return (NULL);
}

int	main(void)
{
	t_shared	shared;
	pthread_t	thread;

	shared.available = 0;
	pthread_mutex_init(&shared.mutex, NULL);
	pthread_cond_init(&shared.condition, NULL);
	pthread_create(&thread, NULL, worker, &shared);
	usleep(250000);
	pthread_mutex_lock(&shared.mutex);
	printf("main: making the resource available\n");
	shared.available = 1;
	pthread_cond_signal(&shared.condition);
	pthread_mutex_unlock(&shared.mutex);
	pthread_join(thread, NULL);
	pthread_cond_destroy(&shared.condition);
	pthread_mutex_destroy(&shared.mutex);
	return (0);
}

