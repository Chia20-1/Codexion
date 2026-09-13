/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 16:29:58 by chilim            #+#    #+#             */
/*   Updated: 2026/09/13 21:44:52 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_request(t_request **a, t_request **b)
{
	t_request	*temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

static bool	is_before(const t_request *a, const t_request *b, t_policy policy)
{
	if (policy == POLICY_EDF
		&& a->burnout_deadline != b->burnout_deadline)
		return (a->burnout_deadline < b->burnout_deadline);
	if (a->arrival_order != b->arrival_order)
		return (a->arrival_order < b->arrival_order);
	return (a->coder->id < b->coder->id);
}

void	shift_up(t_request **heap, int index, t_policy policy)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!is_before(heap[index], heap[parent], policy))
			break ;
		swap_request(&heap[index], &heap[parent]);
		index = parent;
	}
}

void	shift_down(t_request **heap, int index, int size, t_policy policy)
{
	int	left;
	int	right;
	int	best;

	while (index < size / 2)
	{
		left = index * 2 + 1;
		right = index * 2 + 2;
		best = index;
		if (is_before(heap[left], heap[best], policy))
			best = left;
		if (right < size
			&& is_before(heap[right], heap[best], policy))
			best = right;
		if (best == index)
			break ;
		swap_request(&heap[index], &heap[best]);
		index = best;
	}
}
