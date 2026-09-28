/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:28:06 by chilim            #+#    #+#             */
/*   Updated: 2026/09/21 18:40:25 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
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
			if (data->monitor.state == SIM_RUNNING)
				data->monitor.state = SIM_ERROR;
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
		if (data->monitor.state == SIM_RUNNING)
			data->monitor.state = SIM_ERROR;
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

	if (data->config.number_of_compiles_required == 0)
	{
		request_stop(data, SIM_COMPLETED);
		return (true);
	}
	if (!create_monitor(data))
		return (false);
	startup_status = create_coders(data) && start_simulation(data);
	if (!startup_status)
		request_stop(data, SIM_ERROR);
	coder_error = join_coders(data);
	if (coder_error != 0)
		request_stop(data, SIM_ERROR);
	monitor_error = join_monitor(data);
	if (coder_error != 0 || monitor_error != 0)
		printf("Join errors: coders=%d, monitor=%d\n",
			coder_error, monitor_error);
	return (startup_status && coder_error == 0 && monitor_error == 0
		&& data->monitor.wait_error == 0
		&& (data->monitor.state == SIM_COMPLETED
			|| data->monitor.state == SIM_BURNOUT));
}
