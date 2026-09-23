#include "codexion.h"
#include <assert.h>
#include <stdio.h>

static void	test_policy(t_policy policy, long long deadline_b,
		int expected_first)
{
	t_data		data;
	t_coder		coders[2];
	t_request	requests[2];
	t_request	*heap[2];
	t_request	*first;
	t_request	*second;

	data = (t_data){0};
	coders[0] = (t_coder){0};
	coders[1] = (t_coder){0};
	requests[0] = (t_request){0};
	requests[1] = (t_request){0};
	heap[0] = NULL;
	heap[1] = NULL;
	coders[0].id = 1;
	coders[1].id = 2;
	requests[0].coder = &coders[0];
	requests[0].arrival_order = 1;
	requests[0].burnout_deadline = 900;
	requests[1].coder = &coders[1];
	requests[1].arrival_order = 2;
	requests[1].burnout_deadline = deadline_b;
	data.scheduler.request_heap = heap;
	data.scheduler.heap_capacity = 2;
	data.scheduler.policy = policy;

	/* Insert B first: insertion order must not decide priority. */
	assert(push_heap(&data, &requests[1]));
	assert(push_heap(&data, &requests[0]));
	assert(data.scheduler.heap_size == 2);

	first = pop_heap(&data);
	second = pop_heap(&data);
	assert(first != NULL && second != NULL);
	assert(first->coder->id == expected_first);
	assert(second->coder->id == 3 - expected_first);
	assert(data.scheduler.heap_size == 0);
	assert(pop_heap(&data) == NULL);

	printf("PASS: policy=%s, A deadline=900, B deadline=%lld"
		" -> first coder=%d\n",
		policy == POLICY_FIFO ? "FIFO" : "EDF",
		deadline_b, first->coder->id);
}

int	main(void)
{
	test_policy(POLICY_FIFO, 700, 1);
	test_policy(POLICY_EDF, 700, 2);
	test_policy(POLICY_EDF, 900, 1);
	return (0);
}
