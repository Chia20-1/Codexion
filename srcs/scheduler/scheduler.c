/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 20:30:52 by chilim            #+#    #+#             */
/*   Updated: 2026/09/21 16:17:30 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <errno.h>
#include <limits.h>
#include <stddef.h>

static bool	scheduler_grants_request(t_data *data)
{
	t_scheduler	*queue;
	long long	now;
	int			waiting_count;
	int			original_count;
	int			i;

	queue = &data->scheduler;
	now = get_time_ms();
	if (now == -1)
		return (false);
	original_count = queue->heap_size;
	waiting_count = build_waiting_list(data, now);
	i = 0;
	while (i < waiting_count)
	{
		if (!push_heap(data, queue->waiting_requests[i]))
			return (false);
		queue->waiting_requests[i] = NULL;
		i++;
	}
	if (waiting_count < original_count)
		return (pthread_cond_broadcast(&queue->request_queue_cond) == 0);
	return (true);
}

static bool	queue_request(t_coder *coder, long long last_start)
{
	t_scheduler	*queue;
	t_request	*request;

	queue = &coder->data->scheduler;
	request = coder->request;
	if (last_start < 0
		|| last_start > LLONG_MAX - coder->data->config.time_to_burnout
		|| queue->arrival_counter == ULONG_MAX)
		return (false);
	request->burnout_deadline = last_start
		+ coder->data->config.time_to_burnout;
	request->arrival_order = queue->arrival_counter++;
	request->dongles_granted = false;
	return (push_heap(coder->data, request));
}

static t_request_result	wait_for_grant(t_coder *coder)
{
	t_data		*data;
	int			error;

	data = coder->data;
	while (true)
	{
		if (is_stop_requested(data))
			return (REQUEST_STOPPED);
		if (!scheduler_grants_request(data))
			return (REQUEST_ERROR);
		if (is_stop_requested(data))
			return (REQUEST_STOPPED);
		if (coder->request->dongles_granted)
			return (REQUEST_GRANTED);
		error = scheduler_wait(data);
		if (error != 0 && error != ETIMEDOUT)
			return (REQUEST_ERROR);
	}
}

bool	scheduler_release_dongles(t_coder *coder)
{
	t_data		*data;
	long long	now;
	bool		success;

	data = coder->data;
	pthread_mutex_lock(&data->scheduler.request_queue_mutex);
	now = get_time_ms();
	success = dongle_pair_release(coder, now);
	if (success)
	{
		coder->request->dongles_granted = false;
		success = scheduler_grants_request(data);
	}
	if (success)
		success = (pthread_cond_broadcast(
					&data->scheduler.request_queue_cond) == 0);
	pthread_mutex_unlock(&data->scheduler.request_queue_mutex);
	return (success);
}

t_request_result	scheduler_process_request(t_coder *coder)
{
	t_data				*data;
	t_scheduler			*queue;
	t_request_result	result;
	long long			last_compile_start;

	data = coder->data;
	queue = &data->scheduler;
	pthread_mutex_lock(&queue->request_queue_mutex);
	pthread_mutex_lock(&data->monitor.sim_state_mutex);
	if (data->monitor.state != SIM_RUNNING)
	{
		pthread_mutex_unlock(&data->monitor.sim_state_mutex);
		pthread_mutex_unlock(&queue->request_queue_mutex);
		return (REQUEST_STOPPED);
	}
	last_compile_start = coder->last_compile_start;
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	if (!queue_request(coder, last_compile_start))
	{
		pthread_mutex_unlock(&queue->request_queue_mutex);
		return (REQUEST_ERROR);
	}
	result = wait_for_grant(coder);
	pthread_mutex_unlock(&queue->request_queue_mutex);
	return (result);
}
