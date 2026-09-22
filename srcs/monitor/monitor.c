/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 20:58:47 by chilim            #+#    #+#             */
/*   Updated: 2026/09/22 15:42:13 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

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

static bool	scan_coders(t_data *data, long long now, t_monitor_scan *scan)
{
	t_coder		*coder;
	t_config	*config;
	int			i;

	config = &data->config;
	init_scan(scan);
	i = 0;
	while (i < config->number_of_coders)
	{
		coder = &data->coders[i];
		if (coder->compile_count < config->number_of_compiles_required)
			scan->all_completed = false;
		if (coder->last_compile_start > LLONG_MAX - config->time_to_burnout)
			return (false);
		update_scan(coder, now, scan);
		i++;
	}
	return (true);
}

static void	monitor_loop(t_data *data)
{
	t_monitor_scan	scan;
	long long		now;

	while (data->monitor.state == SIM_RUNNING)
	{
		now = get_time_ms();
		if (now == -1 || !scan_coders(data, now, &scan))
			data->monitor.state = SIM_ERROR;
		else if (scan.victim != NULL)
			data->monitor.state = SIM_BURNOUT;
		else if (scan.all_completed)
			data->monitor.state = SIM_COMPLETED;
		else if (!monitor_wait_next_dl(data, scan.next_deadline))
			break ;
	}
	pthread_cond_broadcast(&data->monitor.wakeup_cond);
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
	monitor_loop(data);
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	scheduler_clear_queue(data);
	return (NULL);
}
