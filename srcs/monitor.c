/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 20:58:47 by chilim            #+#    #+#             */
/*   Updated: 2026/09/13 15:38:39 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>

// Getter for run_simulation flag
bool	simulation_should_stop(t_data *data)
{
	bool	stop;

	pthread_mutex_lock(&data->monitor.state_mutex);
	stop = data->monitor.should_stop;
	pthread_mutex_unlock(&data->monitor.state_mutex);
	return (stop);
}

// Setter for run_simulation flag
void	request_stop(t_data *data)
{
	pthread_mutex_lock(&data->monitor.state_mutex);
	data->monitor.should_stop = true;
	pthread_cond_broadcast(&data->monitor.wakeup_cond);
	pthread_mutex_unlock(&data->monitor.state_mutex);
}

static bool	monitor_wait(t_data *data)
{
	int	error;

	error = pthread_cond_wait(&data->monitor.wakeup_cond,
				&data->monitor.state_mutex);
	if (error != 0)
	{
		data->monitor.wait_error = error;
		data->monitor.should_stop = true;
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
	pthread_mutex_lock(&data->monitor.state_mutex);
	while (!data->monitor.simulation_started
		&& !data->monitor.should_stop)
	{
		if (!monitor_wait(data))
			break;
	}
	while (!data->monitor.should_stop)
	{
		if (!monitor_wait(data))
			break;
	}
	pthread_mutex_unlock(&data->monitor.state_mutex);
	return (NULL);
}
