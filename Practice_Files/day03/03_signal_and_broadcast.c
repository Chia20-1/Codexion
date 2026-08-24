/*
** Day 3, exercise 3: compare signal with broadcast.
** Build: cc -Wall -Wextra -Werror -pthread 03_signal_and_broadcast.c -o 03_signal_and_broadcast
*/

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define WORKER_COUNT 4

typedef struct s_shared
{
	pthread_mutex_t	*lock;
	pthread_cond_t	*cond;
	unsigned int	permits;
}	t_shared;

typedef struct s_worker
{
	int			id;
	t_shared	*shared;
}	t_worker;

void	*worker(void *arg)
{
	t_worker *data = (t_worker *)arg;
	pthread_mutex_lock(data->shared->lock);
	while (data->shared->permits == 0)
		pthread_cond_wait(data->shared->cond, data->shared->lock);
	data->shared->permits--;
	printf("Worker %d passed the gate\n", data->id);
	pthread_mutex_unlock(data->shared->lock);
	return (NULL);
}

void	get_one(t_shared *shared)
{
	pthread_mutex_lock(shared->lock);
	shared->permits++;
	printf("Main: Added one permit and called signal\n");
	pthread_cond_signal(shared->cond);
	pthread_mutex_unlock(shared->lock);
}

void	get_rest(t_shared *shared)
{
	pthread_mutex_lock(shared->lock);
	shared->permits += WORKER_COUNT - 1;
	printf("Main: Added three permits and broadcasted signals\n");
	pthread_cond_broadcast(shared->cond);
	pthread_mutex_unlock(shared->lock);
}

int	main(void)
{
	pthread_mutex_t		lock;
	pthread_cond_t		cond;
	pthread_t			threads[WORKER_COUNT];
	t_worker			workers[WORKER_COUNT];
	t_shared			shared;
	int					i;

	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond, NULL);
	shared.lock = &lock;
	shared.cond = &cond;
	shared.permits = 0;
	i = 0;
	while (i < WORKER_COUNT)
	{
		workers[i].id = i + 1;
		workers[i].shared = &shared;
		pthread_create(&threads[i], NULL, worker, &workers[i]);
		i++;
	}
	usleep(200000);
	get_one(&shared);
	usleep(200000);
	get_rest(&shared);
	i = 0;
	while (i < WORKER_COUNT)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
}
