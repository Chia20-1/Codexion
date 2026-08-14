/*
** Day 3, exercise 5: reset a monitor deadline when compiling starts.
** Build: cc -Wall -Wextra -Werror -pthread 05_burnout_monitor.c -o 05_burnout_monitor
*/

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct s_shared
{
	pthread_mutex_t	mutex;
	pthread_cond_t	changed;
	long long		started_at_ms;
	long long		last_compile_start_ms;
	long long		time_to_burnout_ms;
}t_shared;

static long long	now_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((long long)time.tv_sec * 1000LL + time.tv_usec / 1000);
}

static struct timespec	absolute_ms(long long milliseconds)
{
	struct timespec	time;

	time.tv_sec = milliseconds / 1000;
	time.tv_nsec = (milliseconds % 1000) * 1000000L;
	return (time);
}

static long long	since_start(t_shared *shared)
{
	return (now_ms() - shared->started_at_ms);
}

static void	*monitor(void *arg)
{
	t_shared			*shared;
	struct timespec	deadline;
	long long		deadline_ms;
	int				result;

	shared = (t_shared *)arg;
	pthread_mutex_lock(&shared->mutex);
	while (1)
	{
		deadline_ms = shared->last_compile_start_ms
			+ shared->time_to_burnout_ms;
		if (now_ms() >= deadline_ms)
		{
			printf("[%4lld ms] monitor: burnout deadline reached\n",
				since_start(shared));
			break ;
		}
		deadline = absolute_ms(deadline_ms);
		printf("[%4lld ms] monitor: waiting until +%lld ms\n",
			since_start(shared), deadline_ms - shared->started_at_ms);
		result = pthread_cond_timedwait(&shared->changed,
				&shared->mutex, &deadline);
		if (result == 0)
			printf("[%4lld ms] monitor: state changed, recalculating\n",
				since_start(shared));
		else if (result != ETIMEDOUT)
		{
			printf("monitor: pthread_cond_timedwait returned %d\n", result);
			break ;
		}
	}
	pthread_mutex_unlock(&shared->mutex);
	return (NULL);
}

static void	*coder(void *arg)
{
	t_shared	*shared;

	shared = (t_shared *)arg;
	usleep(150000);
	pthread_mutex_lock(&shared->mutex);
	shared->last_compile_start_ms = now_ms();
	printf("[%4lld ms] coder: compiling started; deadline reset\n",
		since_start(shared));
	pthread_cond_signal(&shared->changed);
	pthread_mutex_unlock(&shared->mutex);
	usleep(200000);
	pthread_mutex_lock(&shared->mutex);
	printf("[%4lld ms] coder: compiling finished; deadline unchanged\n",
		since_start(shared));
	pthread_mutex_unlock(&shared->mutex);
	return (NULL);
}

int	main(void)
{
	t_shared	shared;
	pthread_t	monitor_thread;
	pthread_t	coder_thread;

	shared.started_at_ms = now_ms();
	shared.last_compile_start_ms = shared.started_at_ms;
	shared.time_to_burnout_ms = 400;
	pthread_mutex_init(&shared.mutex, NULL);
	pthread_cond_init(&shared.changed, NULL);
	pthread_create(&monitor_thread, NULL, monitor, &shared);
	pthread_create(&coder_thread, NULL, coder, &shared);
	pthread_join(coder_thread, NULL);
	pthread_join(monitor_thread, NULL);
	pthread_cond_destroy(&shared.changed);
	pthread_mutex_destroy(&shared.mutex);
	return (0);
}
