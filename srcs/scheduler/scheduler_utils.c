/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:46:22 by chilim            #+#    #+#             */
/*   Updated: 2026/09/17 16:51:00 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	requests_share_dongle(t_request *a, t_request *b)
{
	t_coder	*first;
	t_coder	*second;

	first = a->coder;
	second = b->coder;
	return (first->left == second->left
		|| first->left == second->right
		|| first->right == second->left
		|| first->right == second->right);
}

bool	has_earlier_conflict(t_scheduler *queue, t_request *request,
	int waiting_count)
{
	int	i;

	i = 0;
	while (i < waiting_count)
	{
		if (requests_share_dongle(queue->waiting_requests[i], request))
			return (true);
		i++;
	}
	return (false);
}
