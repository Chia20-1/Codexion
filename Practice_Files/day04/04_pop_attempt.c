/*
** Day 4, exercise 4: implement heap-down yourself.
** Push is provided so that you can focus on pop.
** This file compiles, but later checks fail until heap_down() is completed.
** Build: cc -Wall -Wextra -Werror 04_pop_attempt.c -o 04_pop_attempt
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
	(void)heap;
	(void)index;
	/*
	** YOUR TURN:
	** 1. Calculate the left and right child indices.
	** 2. Stop when the left child does not exist.
	** 3. Choose the smaller existing child.
	** 4. Stop if the current value is <= that child.
	** 5. Otherwise swap and continue from the child's index.
	*/
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

static int	heap_peek(const t_heap *heap, int *value)
{
	if (heap->size == 0)
		return (0);
	*value = heap->items[0];
	return (1);
}

static int	heap_pop(t_heap *heap, int *value)
{
	if (!heap_peek(heap, value))
		return (0);
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		heap_down(heap, 0);
	}
	return (1);
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
	while (heap_pop(&heap, &removed))
	{
		printf("popped %d; remaining heap is %s\n", removed,
			is_valid_min_heap(&heap) ? "valid" : "INVALID");
	}
	printf("Expected pop order: 1, 3, 5, 6, 8\n");
	return (0);
}
