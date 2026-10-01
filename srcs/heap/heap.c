/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 16:11:44 by chilim            #+#    #+#             */
/*   Updated: 2026/09/29 15:33:29 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdbool.h>
#include <stddef.h>

t_request	*pop_heap(t_data *data)
{
	t_scheduler	*queue;
	t_request	*root;

	queue = &data->scheduler;
	if (!queue->request_heap || queue->heap_size == 0)
		return (NULL);
	root = queue->request_heap[0];
	queue->heap_size--;
	if (queue->heap_size > 0)
		queue->request_heap[0] = queue->request_heap[queue->heap_size];
	queue->request_heap[queue->heap_size] = NULL;
	if (queue->heap_size > 0)
		shift_down(queue->request_heap, 0, queue->heap_size, queue->policy);
	return (root);
}

bool	push_heap(t_data *data, t_request *request)
{
	t_scheduler	*queue;

	queue = &data->scheduler;
	if (!request ||!queue->request_heap
		|| queue->heap_size >= queue->heap_capacity)
		return (false);
	queue->request_heap[queue->heap_size] = request;
	queue->heap_size++;
	shift_up(queue->request_heap, queue->heap_size - 1, queue->policy);
	return (true);
}
