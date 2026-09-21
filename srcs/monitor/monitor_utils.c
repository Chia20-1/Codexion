/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 20:50:31 by chilim            #+#    #+#             */
/*   Updated: 2026/09/21 21:09:35 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>

bool	monitor_wait(t_data *data)
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
