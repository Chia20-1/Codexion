/*
** Day 4, exercise 2: implement heap-up yourself.
** This file compiles, but its checks fail until heap_up() is completed.
** Build: cc -Wall -Wextra -Werror 02_push_attempt.c -o 02_push_attempt
*/

#include <stdio.h>

#define HEAP_CAPACITY 16

typedef struct s_heap
{
	int	items[HEAP_CAPACITY];
	int	size;
}t_heap;

static void	print_heap(const t_heap *heap)
{
	int	i;

	printf("[");
	i = 0;
	while (i < heap->size)
	{
		printf("%d", heap->items[i]);
		if (i + 1 < heap->size)
			printf(", ");
		i++;
	}
	printf("]\n");
}

static int	is_valid_min_heap(const t_heap *heap)
{
	int	i;

	i = 1;
	while (i < heap->size)
	{
		if (heap->items[(i - 1) / 2] > heap->items[i])
			return (0);
		i++;
	}
	return (1);
}

static void	heap_up(t_heap *heap, int index)
{
	(void)heap;
	(void)index;
	/*
	** YOUR TURN:
	** 1. While index is not 0, calculate the parent index.
	** 2. Stop if the parent value is <= the current value.
	** 3. Otherwise swap them and continue from the parent index.
	*/
}

static int	heap_push(t_heap *heap, int value)
{
	int	inserted_index;

	if (heap->size >= HEAP_CAPACITY)
		return (0);
	inserted_index = heap->size;
	heap->items[inserted_index] = value;
	heap->size++;
	heap_up(heap, inserted_index);
	return (1);
}

int	main(void)
{
	t_heap	heap;
	int		values[5] = {8, 3, 6, 1, 5};
	int		i;

	heap.size = 0;
	i = 0;
	while (i < 5)
	{
		printf("push %d -> ", values[i]);
		heap_push(&heap, values[i]);
		print_heap(&heap);
		if (is_valid_min_heap(&heap))
			printf("heap rule: PASS\n");
		else
			printf("heap rule: FAIL (implement heap_up)\n");
		i++;
	}
	printf("Expected final heap: [1, 3, 6, 8, 5]\n");
	return (0);
}
