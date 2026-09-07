#include "codexion.h"
#include <stdio.h>

static int	check_sleep(long long duration_ms)
{
	t_data		data;
	long long	elapsed_ms;

	data.start_time = get_time_ms();
	if (data.start_time == -1)
	{
		printf("FAIL: get_time_ms() returned -1\n");
		return (1);
	}
	if (!sleep_ms(duration_ms))
	{
		printf("FAIL: sleep_ms(%lld) returned false\n", duration_ms);
		return (1);
	}
	elapsed_ms = get_elapsed_ms(&data);
	if (elapsed_ms < 0 || elapsed_ms < duration_ms)
	{
		printf("FAIL: requested %lld ms, elapsed %lld ms\n",
			duration_ms, elapsed_ms);
		return (1);
	}
	printf("PASS: requested %lld ms, elapsed %lld ms\n",
		duration_ms, elapsed_ms);
	return (0);
}

int	main(void)
{
	int	failures;

	failures = check_sleep(0);
	failures += check_sleep(-10);
	failures += check_sleep(10);
	failures += check_sleep(100);
	failures += check_sleep(500);
	return (failures != 0);
}
