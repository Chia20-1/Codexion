/*
** Day 4, exercise 1: connect heap array indices to a binary tree.
** Build: cc -Wall -Wextra -Werror 01_heap_indices.c -o 01_heap_indices
*/

#include <stdio.h>

#define ITEM_COUNT 7

static void	print_relationships(const int *items, int size)
{
	int	i;
	int	left;
	int	right;

	i = 0;
	while (i < size)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		printf("index %d: value %d", i, items[i]);
		if (i > 0)
			printf(", parent index %d", (i - 1) / 2);
		if (left < size)
			printf(", left index %d", left);
		if (right < size)
			printf(", right index %d", right);
		printf("\n");
		i++;
	}
}

int	main(void)
{
	int	items[ITEM_COUNT] = {2, 5, 3, 9, 7, 8, 4};

	printf("Heap array: [2, 5, 3, 9, 7, 8, 4]\n\n");
	print_relationships(items, ITEM_COUNT);
	printf("\nBefore moving on, draw this tree on paper.\n");
	return (0);
}
