/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:46:22 by chilim            #+#    #+#             */
/*   Updated: 2026/09/17 20:03:05 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	requests_share_dongle(t_request *a, t_request *b)
{
	t_coder	*first;
	t_coder	*second;

	first = a->coder;
	second = b->coder;
	return (first->left == second->left
		|| first->left == second->right
		|| first->right == second->left
		|| first->right == second->right);
}

bool	has_earlier_conflict(t_scheduler *queue, t_request *request,
	int waiting_count)
{
	int	i;

	i = 0;
	while (i < waiting_count)
	{
		if (requests_share_dongle(queue->waiting_requests[i], request))
			return (true);
		i++;
	}
	return (false);
}

int	build_waiting_list(t_data *data, long long now)
{
	t_scheduler	*queue;
	t_request	*request;
	int			waiting_count;

	queue = &data->scheduler;
	waiting_count = 0;
	while (queue->heap_size > 0)
	{
		request = pop_heap(data);
		if (has_earlier_conflict(queue, request, waiting_count)
			|| !dongle_pair_try_acquire(request->coder, now))
		{
			queue->waiting_requests[waiting_count] = request;
			waiting_count++;
		}
		else
			request->dongles_granted = true;
	}
	return (waiting_count);
}
