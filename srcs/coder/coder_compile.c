/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_compile.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 23:39:32 by chilim            #+#    #+#             */
/*   Updated: 2026/09/22 23:39:32 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>
#include <pthread.h>
#include <stdio.h>

t_compile_start	coder_start_compile(t_coder *coder)
{
	t_request_result	request;
	t_compile_start		result;
	t_monitor			*monitor;

	request = scheduler_process_request(coder);
	if (request == REQUEST_STOPPED)
		return (COMPILE_STOPPED);
	if (request != REQUEST_GRANTED)
		return (COMPILE_ERROR);
	monitor = &coder->data->monitor;
	pthread_mutex_lock(&monitor->log_output_mutex);
	pthread_mutex_lock(&monitor->sim_state_mutex);
	result = coder_run_compile(coder);
	pthread_mutex_unlock(&monitor->sim_state_mutex);
	pthread_mutex_unlock(&monitor->log_output_mutex);
	return (result);
}

t_compile_start	validate_compile_status(t_coder *coder, long long now)
{
	t_data		*data;
	long long	deadline;

	data = coder->data;
	if (data->monitor.state != SIM_RUNNING)
		return (COMPILE_STOPPED);
	if (now == -1)
		return (COMPILE_ERROR);
	if (coder->last_compile_start
        > LLONG_MAX - data->config.time_to_burnout)
        return (COMPILE_ERROR);
	deadline = coder->last_compile_start + data->config.time_to_burnout;
	if (now >= deadline)
		return (COMPILE_EXPIRED);
	return (COMPILE_STARTED);
}

t_compile_start	coder_run_compile(t_coder *coder)
{
	t_compile_start	result;
	long long		now;
	long long		elapsed;

	now = get_time_ms();
	result = validate_compile_status(coder, now);
	if (result == COMPILE_STARTED)
	{
		coder->last_compile_start = now;
		elapsed = now - coder->data->start_time;
		printf("%lld %d has taken a dongle\n", elapsed, coder->id);
		printf("%lld %d has taken a dongle\n", elapsed, coder->id);
		printf("%lld %d is compiling\n", elapsed, coder->id);
	}
	if (result == COMPILE_STARTED || result == COMPILE_EXPIRED)
	{
		if (pthread_cond_broadcast(&coder->data->monitor.wakeup_cond) != 0)
			return (COMPILE_ERROR);
	}
	return (result);
}

bool	coder_finish_compile(t_coder *coder, bool completed)
{
	bool	notified;

	notified = true;
	pthread_mutex_lock(&coder->data->monitor.sim_state_mutex);
	if (completed && (coder->data->monitor.state == SIM_RUNNING))
	{
		coder->compile_count++;
		notified = (pthread_cond_broadcast(
					&coder->data->monitor.wakeup_cond) == 0);
	}
	pthread_mutex_unlock(&coder->data->monitor.sim_state_mutex);
	if (coder->request->dongles_granted)
	{
		if (!scheduler_release_dongles(coder))
			return (false);
	}
	return (completed && notified);
}

bool	coder_wait_compile_duration(t_coder *coder)
{
	if (is_stop_requested(coder->data))
		return (false);
	return (sleep_ms(coder->data->config.time_to_compile));
}
