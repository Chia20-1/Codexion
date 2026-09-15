/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:28:06 by chilim            #+#    #+#             */
/*   Updated: 2026/09/15 18:21:16 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

static bool	create_coders(t_data *data)
{
	int		i;

	i = 0;
	while (i < data->config.number_of_coders)
	{
		if (pthread_create(
				&data->coders[i].thread,
				NULL, coder_routine,
				&data->coders[i]) != 0)
		{
			pthread_mutex_lock(&data->monitor.sim_state_mutex);
			data->monitor.should_stop = true;
			pthread_cond_broadcast(&data->monitor.wakeup_cond);
			pthread_mutex_unlock(&data->monitor.sim_state_mutex);
			return (false);
		}
		data->coders[i].thread_created = true;
		i++;
	}
	return (true);
}

static bool	start_simulation(t_data *data)
{
	int	i;

	pthread_mutex_lock(&data->monitor.sim_state_mutex);
	data->start_time = get_time_ms();
	if (data->start_time == -1)
	{
		data->monitor.should_stop = true;
		pthread_cond_broadcast(&data->monitor.wakeup_cond);
		pthread_mutex_unlock(&data->monitor.sim_state_mutex);
		return (false);
	}
	i = 0;
	while (i < data->config.number_of_coders)
	{
		data->coders[i].last_compile_start = data->start_time;
		i++;
	}
	data->monitor.simulation_started = true;
	pthread_cond_broadcast(&data->monitor.wakeup_cond);
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	return (true);
}

bool	run_simulation(t_data *data)
{
	bool	startup_status;
	int		coder_error;
	int		monitor_error;

	if (!create_monitor(data))
		return (false);
	startup_status = create_coders(data) && start_simulation(data);
	if (!startup_status)
		request_stop(data);
	coder_error = join_coders(data);
	request_stop(data);
	monitor_error = join_monitor(data);
	if (coder_error != 0 || monitor_error != 0)
		printf("Join errors: coders=%d, monitor=%d\n",
			coder_error, monitor_error);
	return (startup_status && coder_error == 0 && monitor_error == 0
		&& data->monitor.wait_error == 0);
}
