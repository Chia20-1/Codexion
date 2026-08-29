#include <stdio.h>
#include <stdbool.h>

#define HEAP_CAPACITY 16

typedef struct s_heap
{
	int	items[HEAP_CAPACITY];
	int	size;
}	t_heap;

static void	swap_int(int *a, int *b)
{
	int	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

static void	heap_up(t_heap *heap, int index) 
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (heap->items[parent] <= heap->items[index])
			break;
		swap_int(&heap->items[parent], &heap->items[index]);
		index = parent;
	}
}

static void	heap_down(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	smaller;

	while(true)
	{
		left = (2 * index) + 1;
		right = (2 * index) + 2;
		if (left >= heap->size)
			break;
		smaller = left;
		if (right < heap->size
			&& heap->items[right] < heap->items[left])
			smaller = right;
		if (heap->items[index] <= heap->items[smaller])
			break;
		swap_int(&heap->items[index], &heap->items[smaller]);
		index = smaller;
	}
}

static bool	heap_push(t_heap *heap, int value)
{
	int	index;

	if (heap->size >= HEAP_CAPACITY)
		return (false) ;
	index = heap->size;
	heap->size++;
	heap->items[index] = value;
	heap_up(heap, index);
	return (true);	
}

static bool	heap_peek(t_heap *heap, int *value)
{
	if (heap->size == 0)
		return (false);
	*value = heap->items[0];
	return (true);
}

static bool	heap_pop(t_heap *heap, int *value)
{
	if (!heap_peek(heap, value))
		return (false);
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		heap_down(heap, 0);
	}
	return (true);
}

static void	print_heap(t_heap *heap)
{
	int	index;

	index = 0;
	printf("[");
	while (index < heap->size)
	{
		printf("%d", heap->items[index]);
		if (index < heap->size - 1)
			printf(", ");
		index++;
	}
	printf("]\n");
}

int	main(void)
{
	int	value[5] = {8, 2, 1, 3, 6};
	int 	i = 0;
	int	highest_priority = 0;
	t_heap	heap;

	heap.size = 0;
	while (i < 5)
		heap_push(&heap, value[i++]);
	print_heap(&heap);
	while (heap_pop(&heap, &highest_priority))
	{
		printf("Current highest priority: %d\n", highest_priority);
		print_heap(&heap);
	}
	return (0);
}

