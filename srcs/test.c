#include "codexion.h"
#include <assert.h>
#include <stdio.h>

typedef struct s_test
{
	t_coder				*coder;
	t_request_result	result;
}	t_test;

static void	*request_worker(void *argument)
{
	t_test	*test;

	test = argument;
	test->result = scheduler_process_request(test->coder);
	return (NULL);
}

static void	wait_until_queued(t_data *data)
{
	long long	deadline;
	int			size;

	deadline = get_time_ms() + 2000;
	while (true)
	{
		pthread_mutex_lock(&data->scheduler.request_queue_mutex);
		size = data->scheduler.heap_size;
		pthread_mutex_unlock(&data->scheduler.request_queue_mutex);
		if (size == 1)
			return ;
		assert(get_time_ms() < deadline);
		assert(sleep_ms(1));
	}
}

int	main(void)
{
	t_data		data;
	t_test		test;
	pthread_t	thread;

	data = (t_data){0};
	data.config.number_of_coders = 1;
	data.config.time_to_burnout = 5000;
	data.config.scheduler = "fifo";
	assert(init_data(&data));
	data.scheduler.policy = POLICY_FIFO;
	test.coder = &data.coders[0];
	test.result = REQUEST_ERROR;

	assert(pthread_create(&thread, NULL, request_worker, &test) == 0);
	wait_until_queued(&data);
	request_stop(&data);
	assert(pthread_join(thread, NULL) == 0);
	assert(test.result == REQUEST_STOPPED);
	assert(data.scheduler.heap_size == 0);
	assert(data.scheduler.request_heap[0] == NULL);
	assert(data.scheduler.waiting_requests[0] == NULL);
	assert(!data.coders[0].request->dongles_granted);
	assert(data.dongles[0].current_owner == NULL);
	cleanup_data(&data);
	puts("PASS: queued request cancelled and cleared");
	return (0);
}