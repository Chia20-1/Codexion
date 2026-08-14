/*
** Day 3, exercise 4: wait for shared state, but only until a deadline.
** Build: cc -Wall -Wextra -Werror -pthread 04_timed_wait.c -o 04_timed_wait
*/

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

typedef struct s_shared
{
	pthread_mutex_t	mutex;
	pthread_cond_t	condition;
	int				ready;
}t_shared;

static long long	now_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((long long)time.tv_sec * 1000LL + time.tv_usec / 1000);
}

static struct timespec	deadline_after_ms(long milliseconds)
{
	struct timeval		now;
	struct timespec	deadline;

	gettimeofday(&now, NULL);
	deadline.tv_sec = now.tv_sec + milliseconds / 1000;
	deadline.tv_nsec = now.tv_usec * 1000L;
	deadline.tv_nsec += (milliseconds % 1000) * 1000000L;
	if (deadline.tv_nsec >= 1000000000L)
	{
		deadline.tv_sec++;
		deadline.tv_nsec -= 1000000000L;
	}
	return (deadline);
}

static void	*worker(void *arg)
{
	t_shared			*shared;
	struct timespec	deadline;
	long long		start;
	int				result;

	shared = (t_shared *)arg;
	start = now_ms();
	deadline = deadline_after_ms(500);
	result = 0;
	pthread_mutex_lock(&shared->mutex);
	while (shared->ready == 0 && result == 0)
		result = pthread_cond_timedwait(&shared->condition,
				&shared->mutex, &deadline);
	if (shared->ready != 0)
		printf("worker: state became ready\n");
	else if (result == ETIMEDOUT)
		printf("worker: timed out after %lld ms\n", now_ms() - start);
	else
		printf("worker: pthread_cond_timedwait returned %d\n", result);
	pthread_mutex_unlock(&shared->mutex);
	return (NULL);
}

int	main(void)
{
	t_shared	shared;
	pthread_t	thread;

	shared.ready = 0;
	pthread_mutex_init(&shared.mutex, NULL);
	pthread_cond_init(&shared.condition, NULL);
	pthread_create(&thread, NULL, worker, &shared);
	pthread_join(thread, NULL);
	pthread_cond_destroy(&shared.condition);
	pthread_mutex_destroy(&shared.mutex);
	return (0);
}

