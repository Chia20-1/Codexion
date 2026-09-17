/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 20:30:52 by chilim            #+#    #+#             */
/*   Updated: 2026/09/17 16:55:50 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	build_waiting_list(t_data *data, long long now)
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

static bool	scheduler_grants_request(t_data *data)
{
	t_scheduler	*queue;
	long long	now;
	int			waiting_count;
	int			i;

	queue = &data->scheduler;
	now = get_time_ms();
	if (now == -1)
		return (false);
	waiting_count = build_waiting_list(data, now);
	i = 0;
	while (i < waiting_count)
	{
		if (!push_heap(data, queue->waiting_requests[i]))
			return (false);
		queue->waiting_requests[i] = NULL;
		i++;
	}
	return (pthread_cond_broadcast(&queue->request_queue_cond) == 0);
}

t_request_result	scheduler_process_request(t_coder *coder)
{
	t_data		*data;
	t_scheduler	*queue;
	long long	last_compile_start;

	data = coder->data;
	queue = &data->scheduler;
	pthread_mutex_lock(&queue->request_queue_mutex);
	pthread_mutex_lock(&data->monitor.sim_state_mutex);
	if (data->monitor.should_stop)
	{
		pthread_mutex_unlock(&data->monitor.sim_state_mutex);
		pthread_mutex_unlock(&queue->request_queue_mutex);
		return (REQUEST_STOPPED);
	}
	last_compile_start = coder->last_compile_start;
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
}
