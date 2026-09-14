#include "codexion.h"
#include <stdio.h>

static void	print_request(t_request *request)
{
	if (!request)
	{
		printf("NULL\n");
		return ;
	}
	printf("coder=%d arrival=%lu deadline=%lld\n",
		request->coder->id,
		request->arrival_order,
		request->burnout_deadline);
}

static void print_heap(t_data *data)
{
	int	i;

	printf("Heap: size=%d capacity=%d\n",
		data->scheduler.heap_size,
		data->scheduler.heap_capacity);
	i = 0;
	while (i < data->scheduler.heap_size)
	{
		printf(" [%d] ", i);
		print_request(data->scheduler.request_heap[i]);
		i++;
	}
}

static void	test_push(t_data *data, t_request *request)
{
	printf("\nPush: ");
	print_request(request);
	if (push_heap(data, request))
		printf("Result: accpeted\n");
	else
		printf("Result: rejected\n");
	print_heap(data);
}

static void	test_pop(t_data *data)
{
	printf("\nPop: ");
	print_request(pop_heap(data));
	print_heap(data);
}

int	main(void)
{
	t_data		data = {0};
	t_request	*storage[4] = {0};
	t_coder		coders[4] = {{.id = 1}, {.id = 2}, {.id = 3}, {.id = 4}};
	t_request	request[4] = {
		{.coder = &coders[0], .burnout_deadline = 900, .arrival_order = 3},
		{.coder = &coders[1], .burnout_deadline = 700, .arrival_order = 1},
		{.coder = &coders[2], .burnout_deadline = 700, .arrival_order = 4},
		{.coder = &coders[3], .burnout_deadline = 800, .arrival_order = 2}};		
	int			i;

	data.scheduler.policy = POLICY_FIFO;
	data.scheduler.request_heap = storage;
	data.scheduler.heap_capacity = sizeof(storage) / sizeof(storage[0]);

	test_pop(&data);
	i = 0;
	while (i < 4)
	{
		test_push(&data, &request[i]);
		i++;
	}
	printf("\nPeek: ");
	print_request(peek_heap(&data));
	while (data.scheduler.heap_size > 0)
		test_pop(&data);
	test_pop(&data);
	return (0);
}