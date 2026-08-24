/*
** Day 4, exercise 5: observe a completed heap-down operation.
** Attempt 04_pop_attempt.c before reading this implementation.
** Build: cc -Wall -Wextra -Werror 05_pop_walkthrough.c -o 05_pop_walkthrough
*/

#include <stdio.h>

#define HEAP_CAPACITY 16

typedef struct s_heap
{
	int	items[HEAP_CAPACITY];
	int	size;
}t_heap;

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
		swap_int(&heap->items[parent], &heap->items[index]);
		index = parent;
	}
}

static void	heap_down(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	smaller;

	while (1)
	{
		left = 2 * index + 1;
		right = 2 * index + 2;
		if (left >= heap->size)
			break ;
		smaller = left;
		if (right < heap->size
			&& heap->items[right] < heap->items[left])
			smaller = right;
		if (heap->items[index] <= heap->items[smaller])
			break ;
		printf("  swap value %d at index %d with smaller child %d at index %d\n",
			heap->items[index], index, heap->items[smaller], smaller);
		swap_int(&heap->items[index], &heap->items[smaller]);
		index = smaller;
	}
}

static int	heap_push(t_heap *heap, int value)
{
	int	index;

	if (heap->size >= HEAP_CAPACITY)
		return (0);
	index = heap->size;
	heap->items[index] = value;
	heap->size++;
	heap_up(heap, index);
	return (1);
}

static int	heap_pop(t_heap *heap, int *value)
{
	if (heap->size == 0)
		return (0);
	*value = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		printf("  moved last value %d to index 0\n", heap->items[0]);
		heap_down(heap, 0);
	}
	return (1);
}

int	main(void)
{
	t_heap	heap;
	int		values[5] = {8, 3, 6, 1, 5};
	int		removed;
	int		i;

	heap.size = 0;
	i = 0;
	while (i < 5)
		heap_push(&heap, values[i++]);
	printf("Starting heap: [1, 3, 6, 8, 5]\n");
	while (heap_pop(&heap, &removed))
		printf("popped %d; %d item(s) remain\n", removed, heap.size);
	return (0);
}
