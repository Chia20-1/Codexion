/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:21:05 by chilim            #+#    #+#             */
/*   Updated: 2026/09/15 18:20:09 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdbool.h>
#include <unistd.h>

void	*coder_routine(void *argument)
{
	t_coder	*coder;
	t_data	*data;
	bool	stop;
	int		error;

	coder = (t_coder *)argument;
	data = coder->data;
	pthread_mutex_lock(&data->monitor.sim_state_mutex);
	while (!data->monitor.simulation_started
		&& !data->monitor.should_stop)
	{
		error = pthread_cond_wait(&data->monitor.wakeup_cond,
				&data->monitor.sim_state_mutex);
		if (error != 0)
		{
			data->monitor.wait_error = error;
			data->monitor.should_stop = true;
			pthread_cond_broadcast(&data->monitor.wakeup_cond);
			break ;
		}
	}
	stop = data->monitor.should_stop;
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
	if (stop)
		return (NULL);
	return (NULL);
}
