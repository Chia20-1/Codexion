/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 20:30:52 by chilim            #+#    #+#             */
/*   Updated: 2026/09/16 14:18:52 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request_result	scheduler_process_request(t_coder *coder)
{
	t_data		*data;
	t_scheduler	*queue;
	long long	last_compile_start;

	data = coder->data;
	queue = &data->scheduler;
	pthread_mutex_lock(&queue->request_queue_mutex);
	pthread_mutex_lock(&data->monitor.sim_state_mutex);
	if (data->monitor.should_stop)
	{
		pthread_mutex_unlock(&data->monitor.sim_state_mutex);
		pthread_mutex_unlock(&queue->request_queue_mutex);
		return (REQUEST_STOPPED);
	}
	last_compile_start = coder->last_compile_start;
	pthread_mutex_unlock(&data->monitor.sim_state_mutex);
}

bool	scheduler_grants_request(t_data *data)
{
	
}
