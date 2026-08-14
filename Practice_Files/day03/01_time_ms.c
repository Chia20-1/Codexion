/*
** Day 3, exercise 1: measure elapsed time in milliseconds.
** Build: cc -Wall -Wextra -Werror 01_time_ms.c -o 01_time_ms
*/

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

static long long	now_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((long long)time.tv_sec * 1000LL + time.tv_usec / 1000);
}

int	main(void)
{
	long long	start;
	long long	end;

	start = now_ms();
	usleep(250000);
	end = now_ms();
	printf("start   = %lld ms\n", start);
	printf("end     = %lld ms\n", end);
	printf("elapsed = %lld ms (requested sleep: 250 ms)\n", end - start);
	return (0);
}

