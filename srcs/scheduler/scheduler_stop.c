/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_stop.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:56:44 by chilim            #+#    #+#             */
/*   Updated: 2026/09/20 18:56:44 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stddef.h>

void	scheduler_clear_queue(t_data *data)
{
	t_scheduler	*queue;
	int			i;

	queue = &data->scheduler;
	pthread_mutex_lock(&queue->request_queue_mutex);
	i = 0;
	while (i < queue->heap_capacity)
	{
		queue->request_heap[i] = NULL;
		queue->waiting_requests[i] = NULL;
		i++;
	}
	queue->heap_size = 0;
	pthread_cond_broadcast(&queue->request_queue_cond);
	pthread_mutex_unlock(&queue->request_queue_mutex);
}
