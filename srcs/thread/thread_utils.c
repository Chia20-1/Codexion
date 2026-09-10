/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 16:04:27 by chilim            #+#    #+#             */
/*   Updated: 2026/09/10 20:27:16 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>

int	join_coders(t_data *data)
{
	int	i;
	int	error;
	int	first_error;

	i = 0;
	first_error = 0;
	while (i < data->config.number_of_coders)
	{
		if (data->coders[i].thread_created)
		{
			error = pthread_join(data->coders[i].thread, NULL);
			if (error == 0)
				data->coders[i].thread_created = false;
			else if (first_error == 0)
				first_error = error;
		}
		i++;
	}
	return (first_error);
}

bool	create_monitor(t_data *data)
{
	if (pthread_create(&data->monitor.thread, NULL,
			monitor_routine, data) != 0)
		return (false);
	data->monitor.thread_created = true;
	return (true);
}

int	join_monitor(t_data *data)
{
	int	error;

	if (!data->monitor.thread_created)
		return (0);
	error = pthread_join(data->monitor.thread, NULL);
	if (error == 0)
		data->monitor.thread_created = false;
	return (error);
}