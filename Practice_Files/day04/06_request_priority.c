/*
** Day 4, exercise 6: compare Codexion requests for FIFO and EDF.
** This isolates comparison rules; later the same comparator can guide a heap.
** Build: cc -Wall -Wextra -Werror 06_request_priority.c -o 06_request_priority
*/

#include <stdio.h>

typedef struct s_request
{
	int			coder_id;
	long long	arrival_order;
	long long	deadline_ms;
}t_request;

static int	comes_before_fifo(const t_request *a, const t_request *b)
{
	if (a->arrival_order != b->arrival_order)
		return (a->arrival_order < b->arrival_order);
	return (a->coder_id < b->coder_id);
}

static int	comes_before_edf(const t_request *a, const t_request *b)
{
	if (a->deadline_ms != b->deadline_ms)
		return (a->deadline_ms < b->deadline_ms);
	if (a->arrival_order != b->arrival_order)
		return (a->arrival_order < b->arrival_order);
	return (a->coder_id < b->coder_id);
}

static void	print_winner(const char *policy, const t_request *a,
		const t_request *b, int a_wins)
{
	const t_request	*winner;

	if (a_wins)
		winner = a;
	else
		winner = b;
	printf("%s winner: coder %d (arrival %lld, deadline %lld)\n", policy,
		winner->coder_id, winner->arrival_order, winner->deadline_ms);
}

int	main(void)
{
	t_request	a;
	t_request	b;
	t_request	c;

	a = (t_request){.coder_id = 1, .arrival_order = 10, .deadline_ms = 900};
	b = (t_request){.coder_id = 2, .arrival_order = 11, .deadline_ms = 700};
	c = (t_request){.coder_id = 3, .arrival_order = 12, .deadline_ms = 700};
	printf("Predict these results before reading the next lines.\n\n");
	print_winner("FIFO, A versus B", &a, &b, comes_before_fifo(&a, &b));
	print_winner("EDF,  A versus B", &a, &b, comes_before_edf(&a, &b));
	print_winner("EDF,  B versus C", &b, &c, comes_before_edf(&b, &c));
	printf("\nWhy does B beat C when their deadlines are equal?\n");
	return (0);
}
