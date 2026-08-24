/*
** Day 4, exercise 3: observe a completed heap-up operation.
** Attempt 02_push_attempt.c before reading this implementation.
** Build: cc -Wall -Wextra -Werror 03_push_walkthrough.c -o 03_push_walkthrough
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

static void	swap_int(int *a, int *b)
{
	int	temporary;

	temporary = *a;
	*a = *b;
	*b = temporary;
}

static void	heap_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (heap->items[parent] <= heap->items[index])
			break ;
		printf("  swap value %d at index %d with parent %d at index %d\n",
			heap->items[index], index, heap->items[parent], parent);
		swap_int(&heap->items[parent], &heap->items[index]);
		index = parent;
	}
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
		printf("\nAppending %d at index %d\n", values[i], heap.size);
		heap_push(&heap, values[i]);
		printf("heap after push: ");
		print_heap(&heap);
		i++;
	}
	return (0);
}
