/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 17:52:37 by chilim            #+#    #+#             */
/*   Updated: 2026/09/22 14:31:08 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void	*ft_calloc(size_t nmemb, size_t size)
{
	unsigned char	*ptr;
	size_t			total;

	if (size != 0 && nmemb > SIZE_MAX / size)
		return (NULL);
	total = nmemb * size;
	ptr = malloc(total);
	if (!ptr)
		return (NULL);
	memset(ptr, 0, total);
	return (ptr);
}

bool	init_scheduler_mutex(t_data *data)
{
	t_scheduler	*scheduler;

	scheduler = &data->scheduler;
	if (pthread_mutex_init(&scheduler->request_queue_mutex, NULL) != 0)
		return (false);
	scheduler->has_mutex = true;
	return (true);
}

bool	init_scheduler_cond(t_data *data)
{
	t_scheduler	*scheduler;

	scheduler = &data->scheduler;
	if (pthread_cond_init(&scheduler->request_queue_cond, NULL) != 0)
		return (false);
	scheduler->has_cond = true;
	return (true);
}

void	init_scheduler_policy(t_data *data)
{
	if (strcmp(data->config.scheduler, "fifo") == 0)
		data->scheduler.policy = POLICY_FIFO;
	else if (strcmp(data->config.scheduler, "edf") == 0)
		data->scheduler.policy = POLICY_EDF;
}
