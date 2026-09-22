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

// Wake up the monitor to inform the burnout
// Release the mutex so the monitor can record burnout
bool	coder_wait_for_stop(t_data *data)
{
	t_monitor	*monitor;
	int			error;

	monitor = &data->monitor;
	pthread_mutex_lock(&monitor->sim_state_mutex);
	while (monitor->state == SIM_RUNNING)
	{
		error = pthread_cond_wait(&monitor->wakeup_cond,
			&monitor->sim_state_mutex);
		if (error != 0)
		{
			monitor->wait_error = error;
			pthread_mutex_unlock(&monitor->sim_state_mutex);
			return (false);
		}
	}
	pthread_mutex_unlock(&monitor->sim_state_mutex);
	return (true);
}
