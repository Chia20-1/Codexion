/*
** Day 3, exercise 1: measure elapsed time in milliseconds.
** Build: cc -Wall -Wextra -Werror 01_time_ms.c -o 01_time_ms
*/

#include<sys/time.h>
#include<unistd.h>
#include<stdio.h>

long long	now_ms()
{
	struct timeval		time;
	long long			result;

	result = 0;
	gettimeofday(&time, NULL);
	// Converting seconds: 1 sec = 1000 ms
	result = result + ((long long)time.tv_sec * (long long)1000);
	// Converting seconds: 1000 microseconds = 1ms
	result = result + (time.tv_usec / 1000);
	return (result);
}

int	main(void)
{
	long long	start;
	long long	end;
	long long	elapsed_time;

	start = now_ms();
	usleep(25000);
	end = now_ms();
	elapsed_time = end - start;
	printf("Elapsed time: %lldms", elapsed_time);
	return (0);
}

