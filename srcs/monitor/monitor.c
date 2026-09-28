/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 20:58:47 by chilim            #+#    #+#             */
/*   Updated: 2026/09/22 17:40:35 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

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

static void	monitor_check(t_data *data, t_monitor_scan *scan)
{
	long long	now;

	now = get_time_ms();
	if (now == -1 || !scan_coders(data, now, scan))
		data->monitor.state = SIM_ERROR;
	else if (scan->victim != NULL)
	{
		now = get_time_ms();
		if (now == -1)
			data->monitor.state = SIM_ERROR;
		else
		{
			data->monitor.state = SIM_BURNOUT;
			printf("%lld %d burned out\n", (now - data->start_time),
				scan->victim->id);
		}
	}
	else if (scan->all_completed)
		data->monitor.state = SIM_COMPLETED;
}

static void	monitor_loop(t_data *data)
{
	t_monitor_scan	scan;

	while (true)
	{
		pthread_mutex_lock(&data->monitor.log_output_mutex);
		pthread_mutex_lock(&data->monitor.sim_state_mutex);
		if (data->monitor.state == SIM_RUNNING)
			monitor_check(data, &scan);
		pthread_mutex_unlock(&data->monitor.log_output_mutex);
		if (data->monitor.state != SIM_RUNNING)
		{
			pthread_cond_broadcast(&data->monitor.wakeup_cond);
			pthread_mutex_unlock(&data->monitor.sim_state_mutex);
			break ;
		}
		monitor_wait_next_dl(data, scan.next_deadline);
		pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	}
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
		if (!monitor_wait_start_gate(data))
			break ;
	}
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	monitor_loop(data);
	scheduler_clear_queue(data);
	return (NULL);
}
