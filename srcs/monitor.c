/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 20:58:47 by chilim            #+#    #+#             */
/*   Updated: 2026/09/21 16:34:23 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>

// Getter for run_simulation flag
bool	is_stop_requested(t_data *data)
{
	bool	stop;

	pthread_mutex_lock(&data->monitor.sim_state_mutex);
	stop = (data->monitor.state != SIM_RUNNING);
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	return (stop);
}

// Setter for run_simulation flag
void	request_stop(t_data *data, t_sim_state reason)
{
	if (reason != SIM_COMPLETED && reason != SIM_BURNOUT
		&& reason != SIM_ERROR)
		return ;
	pthread_mutex_lock(&data->monitor.sim_state_mutex);
	if (data->monitor.state == SIM_RUNNING)
		data->monitor.state = reason;
	pthread_cond_broadcast(&data->monitor.wakeup_cond);
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	scheduler_clear_queue(data);
}

static bool	monitor_wait(t_data *data)
{
	int	error;

	error = pthread_cond_wait(&data->monitor.wakeup_cond,
			&data->monitor.sim_state_mutex);
	if (error != 0)
	{
		data->monitor.wait_error = error;
		if (data->monitor.state == SIM_RUNNING)
			data->monitor.state = SIM_ERROR;
		pthread_cond_broadcast(&data->monitor.wakeup_cond);
		return (false);
	}
	return (true);
}

// 1st wait for creating threads
// 2nd wait for shutdown
void	*monitor_routine(void *argument)
{
	t_data	*data;

	data = (t_data *)argument;
	pthread_mutex_lock(&data->monitor.sim_state_mutex);
	while (!data->monitor.simulation_started
		&& (data->monitor.state == SIM_RUNNING))
	{
		if (!monitor_wait(data))
			break ;
	}
	while (data->monitor.state == SIM_RUNNING)
	{
		if (!monitor_wait(data))
			break ;
	}
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	scheduler_clear_queue(data);
	return (NULL);
}
