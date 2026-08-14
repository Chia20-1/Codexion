/*
** Day 3, exercise 3: compare signal with broadcast.
** Build: cc -Wall -Wextra -Werror -pthread 03_signal_and_broadcast.c -o 03_signal_and_broadcast
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define WORKER_COUNT 4

typedef struct s_shared
{
	pthread_mutex_t	mutex;
	pthread_cond_t	condition;
	int				permits;
}t_shared;

typedef struct s_worker
{
	int			id;
	t_shared	*shared;
}t_worker;

static void	*worker(void *arg)
{
	t_worker	*data;

	data = (t_worker *)arg;
	pthread_mutex_lock(&data->shared->mutex);
	while (data->shared->permits == 0)
		pthread_cond_wait(&data->shared->condition, &data->shared->mutex);
	data->shared->permits--;
	printf("worker %d passed the gate\n", data->id);
	pthread_mutex_unlock(&data->shared->mutex);
	return (NULL);
}

static void	grant_one(t_shared *shared)
{
	pthread_mutex_lock(&shared->mutex);
	shared->permits++;
	printf("main: added one permit and called signal\n");
	pthread_cond_signal(&shared->condition);
	pthread_mutex_unlock(&shared->mutex);
}

static void	grant_rest(t_shared *shared)
{
	pthread_mutex_lock(&shared->mutex);
	shared->permits += WORKER_COUNT - 1;
	printf("main: added the remaining permits and called broadcast\n");
	pthread_cond_broadcast(&shared->condition);
	pthread_mutex_unlock(&shared->mutex);
}

int	main(void)
{
	t_shared	shared;
	t_worker	workers[WORKER_COUNT];
	pthread_t	threads[WORKER_COUNT];
	int			i;

	shared.permits = 0;
	pthread_mutex_init(&shared.mutex, NULL);
	pthread_cond_init(&shared.condition, NULL);
	i = 0;
	while (i < WORKER_COUNT)
	{
		workers[i].id = i + 1;
		workers[i].shared = &shared;
		pthread_create(&threads[i], NULL, worker, &workers[i]);
		i++;
	}
	usleep(200000);
	grant_one(&shared);
	usleep(250000);
	grant_rest(&shared);
	i = 0;
	while (i < WORKER_COUNT)
		pthread_join(threads[i++], NULL);
	pthread_cond_destroy(&shared.condition);
	pthread_mutex_destroy(&shared.mutex);
	return (0);
}

