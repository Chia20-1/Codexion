/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 16:11:44 by chilim            #+#    #+#             */
/*   Updated: 2026/09/12 16:28:50 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request	*peek_heap(t_data *data)
{
	if (!(data->scheduler.request_heap)
		|| data->scheduler.heap_capacity == 0)
		return (NULL);
	return (data->scheduler.request_heap[0]);
}

t_request	*pop_heap(t_data *data)
{
	
}