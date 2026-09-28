/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:46:22 by chilim            #+#    #+#             */
/*   Updated: 2026/09/18 20:29:47 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stddef.h>
#include <time.h>

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

static long long	next_cooldown_deadline(t_data *data, long long now)
{
	t_scheduler	*queue;
	t_coder		*coder;
	long long	deadline;
	long long	next;
	int			i;

	queue = &data->scheduler;
	next = -1;
	i = -1;
	while (++i < queue->heap_size)
	{
		coder = queue->request_heap[i]->coder;
		if (coder->left == coder->right
			|| coder->left->current_owner != NULL
			|| coder->right->current_owner != NULL)
			continue ;
		deadline = coder->left->cooldown_deadline;
		if (coder->right->cooldown_deadline > deadline)
			deadline = coder->right->cooldown_deadline;
		if (deadline > now && (next == -1 || deadline < next))
			next = deadline;
	}
	return (next);
}

int	scheduler_wait(t_data *data)
{
	struct timespec	timeout;
	long long		now;
	long long		deadline;
	t_scheduler		*queue;

	queue = &data->scheduler;
	now = get_time_ms();
	if (now == -1)
		return (-1);
	deadline = next_cooldown_deadline(data, now);
	if (deadline == -1)
		return (pthread_cond_wait(&queue->request_queue_cond,
				&queue->request_queue_mutex));
	timeout.tv_sec = deadline / 1000;
	timeout.tv_nsec = (deadline % 1000) * 1000000;
	return (pthread_cond_timedwait(&queue->request_queue_cond,
			&queue->request_queue_mutex, &timeout));
}
