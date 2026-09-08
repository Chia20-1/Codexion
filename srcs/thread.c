/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:28:06 by chilim            #+#    #+#             */
/*   Updated: 2026/09/08 18:07:51 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

bool	run_simulation(t_data *data)
{
	int	i;

	i = 0;
	data->start_time = get_time_ms();
	while (i < data->config.number_of_coders)
	{
		if (pthread_create(
				&data->coders[i].thread,
				NULL, coder_routine,
				&data->coders[i]) != 0)
		{
			pthread_mutex_lock(&data->monitor.state_mutex);
			data->monitor.should_stop = true;
			pthread_mutex_unlock(&data->monitor.state_mutex);
			while (i-- > 0)
			{
				pthread_join(data->coders[i].thread, NULL);
				data->coders[i].thread_created = false;
			}
			return (false);
		}
		data->coders[i].thread_created = true;
		i++;
	}
	return (true);
}
