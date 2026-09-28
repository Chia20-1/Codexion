/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 20:50:31 by chilim            #+#    #+#             */
/*   Updated: 2026/09/22 16:42:29 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <time.h>

bool	monitor_wait_start_gate(t_data *data)
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

void	init_scan(t_monitor_scan *scan)
{
	scan->victim = NULL;
	scan->next_deadline = LLONG_MAX;
	scan->all_completed = true;
}

void	update_scan(t_coder *coder, long long now, t_monitor_scan *scan)
{
	t_config	*config;
	long long	deadline;

	config = &coder->data->config;
	deadline = coder->last_compile_start + config->time_to_burnout;
	if (now >= deadline)
	{
		if (scan->victim == NULL)
			scan->victim = coder;
	}
	else if (deadline < scan->next_deadline)
		scan->next_deadline = deadline;
}

bool	scan_coders(t_data *data, long long now, t_monitor_scan *scan)
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

bool	monitor_wait_next_dl(t_data *data, long long deadline)
{
	struct timespec	timeout;
	int				error;

	timeout.tv_sec = deadline / 1000;
	timeout.tv_nsec = (deadline % 1000) * 1000000;
	error = pthread_cond_timedwait(&data->monitor.wakeup_cond,
			&data->monitor.sim_state_mutex, &timeout);
	if (error != 0 && error != ETIMEDOUT)
	{
		data->monitor.wait_error = error;
		if (data->monitor.state == SIM_RUNNING)
			data->monitor.state = SIM_ERROR;
		pthread_cond_broadcast(&data->monitor.wakeup_cond);
		return (false);
	}
	return (true);
}
