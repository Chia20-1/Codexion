/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 20:48:57 by chilim            #+#    #+#             */
/*   Updated: 2026/09/22 19:04:46 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>

bool	coder_wait_for_start(t_data *data)
{
	t_monitor	*monitor;
	int			error;
	bool		ready;

	monitor = &data->monitor;
	pthread_mutex_lock(&monitor->sim_state_mutex);
	while (!monitor->simulation_started
		&& monitor->state == SIM_RUNNING)
	{
		error = pthread_cond_wait(&monitor->wakeup_cond,
				&monitor->sim_state_mutex);
		if (error != 0)
		{
			monitor->wait_error = error;
			if (monitor->state == SIM_RUNNING)
				monitor->state = SIM_ERROR;
			pthread_cond_broadcast(&monitor->wakeup_cond);
			break ;
		}
	}
	ready = (monitor->state == SIM_RUNNING);
	pthread_mutex_unlock(&monitor->sim_state_mutex);
	return (ready);
}

bool	coder_start_compile(t_coder *coder)
{
	t_request_result	result;
	long long			now;
	bool				started;

	started = false;
	result = scheduler_process_request(coder);
	if (result == REQUEST_GRANTED)
	{
		pthread_mutex_lock(&coder->data->monitor.sim_state_mutex);
		if (coder->data->monitor.state == SIM_RUNNING)
		{
			now = get_time_ms();
			if (now != -1)
			{
				coder->last_compile_start = now;
				started = (pthread_cond_broadcast(
							&coder->data->monitor.wakeup_cond) == 0);
			}
		}
		pthread_mutex_unlock(&coder->data->monitor.sim_state_mutex);
	}
	return (started);
}

bool	coder_run_compile(t_coder *coder, bool started)
{
	bool	completed;

	completed = false;
	if (started)
	{
		log_status(coder, "has taken a dongle");
		log_status(coder, "has taken a dongle");
		log_status(coder, "is compiling");
		completed = sleep_ms(coder->data->config.time_to_compile);
	}
	return (completed);
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
