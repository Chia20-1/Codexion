/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 20:31:55 by chilim            #+#    #+#             */
/*   Updated: 2026/09/05 20:26:14 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	init_coders(t_data *data)
{
	int			i;
	t_coder		*coder;

	data->coders = ft_calloc((size_t)data->config.number_of_coders,
			sizeof(t_coder));
	if (!data->coders)
		return (false);
	i = 0;
	while (i < data->config.number_of_coders)
	{
		coder = &data->coders[i];
		coder->id = i + 1;
		coder->data = data;
		i++;
	}
	return (true);
}

static bool	init_dongles(t_data *data)
{
	int			i;
	t_coder		*coder;
	t_dongle	*dongle;

	data->dongles = ft_calloc((size_t)data->config.number_of_coders,
			sizeof(t_dongle));
	if (!data->dongles)
		return (false);
	i = 0;
	while (i < data->config.number_of_coders)
	{
		coder = &data->coders[i];
		dongle = &data->dongles[i];
		coder->left = dongle;
		coder->right = &data->dongles[
			(i + 1) % data->config.number_of_coders
		];
		if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
			return (false);
		dongle->has_mutex = true;
		i++;
	}
	return (true);
}

static bool	init_scheduler(t_data *data)
{
	t_scheduler	*scheduler;

	scheduler = &data->scheduler;
	scheduler->heap_size = 0;
	scheduler->heap_capacity = data->config.number_of_coders;
	scheduler->arrival_counter = 0;
	scheduler->request_heap = ft_calloc((size_t)scheduler->heap_capacity,
			sizeof(*scheduler->request_heap));
	if (!scheduler->request_heap)
		return (false);
	if (!init_scheduler_mutex(data))
		return (false);
	if (!init_scheduler_cond(data))
		return (false);
	return (true);
}

static bool	init_monitor(t_data *data)
{
	t_monitor	*monitor;

	monitor = &data->monitor;
	monitor->should_stop = false;
	if (pthread_mutex_init(&monitor->state_mutex, NULL) != 0)
		return (false);
	monitor->has_state_mutex = true;
	if (pthread_mutex_init(&monitor->log_output_mutex, NULL) != 0)
		return (false);
	monitor->has_log_mutex = true;
	if (pthread_cond_init(&monitor->wakeup_cond, NULL) != 0)
		return (false);
	monitor->has_wakeup_cond = true;
	return (true);
}

bool	init_data(t_data *data)
{
	if (!init_coders(data)
		|| !init_dongles(data)
		|| !init_scheduler(data)
		|| !init_monitor(data))
	{
		cleanup_data(data);
		return (false);
	}
	return (true);
}
