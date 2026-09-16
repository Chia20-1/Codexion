/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 20:35:22 by chilim            #+#    #+#             */
/*   Updated: 2026/09/16 21:43:25 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

static void	cleanup_monitor(t_data *data)
{
	t_monitor	*monitor;

	monitor = &data->monitor;
	if (monitor->has_wakeup_cond)
		pthread_cond_destroy(&monitor->wakeup_cond);
	monitor->has_wakeup_cond = false;
	if (monitor->has_log_mutex)
		pthread_mutex_destroy(&monitor->log_output_mutex);
	monitor->has_log_mutex = false;
	if (monitor->has_state_mutex)
		pthread_mutex_destroy(&monitor->sim_state_mutex);
	monitor->has_state_mutex = false;
	return ;
}

static void	cleanup_scheduler(t_data *data)
{
	t_scheduler	*scheduler;

	scheduler = &data->scheduler;
	if (scheduler->has_cond)
		pthread_cond_destroy(&scheduler->request_queue_cond);
	scheduler->has_cond = false;
	if (scheduler->has_mutex)
		pthread_mutex_destroy(&scheduler->request_queue_mutex);
	scheduler->has_mutex = false;
	free(scheduler->request_heap);
	scheduler->request_heap = NULL;
	free(scheduler->waiting_requests);
	scheduler->waiting_requests = NULL;
}

static void	cleanup_dongles(t_data *data)
{
	int			i;
	t_dongle	*dongle;

	if (!data->dongles)
		return ;
	i = 0;
	while (i < data->config.number_of_coders)
	{
		dongle = &data->dongles[i];
		if (dongle->has_mutex)
		{
			pthread_mutex_destroy(&dongle->mutex);
			dongle->has_mutex = false;
		}
		i++;
	}
	free(data->dongles);
	data->dongles = NULL;
}

static void	cleanup_coders(t_data *data)
{
	free(data->coders);
	data->coders = NULL;
}

void	cleanup_data(t_data *data)
{
	cleanup_monitor(data);
	cleanup_scheduler(data);
	cleanup_dongles(data);
	cleanup_coders(data);
}
