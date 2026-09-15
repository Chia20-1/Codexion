/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:47:36 by chilim            #+#    #+#             */
/*   Updated: 2026/09/15 16:42:00 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	dongle_is_available(t_dongle *dongle, long long now)
{
	return (dongle->current_owner == NULL
		&& now >= dongle->cooldown_deadline);
}

bool	dongle_pair_try_acquire(t_coder *coder, long long now)
{
	t_dongle	*first;
	t_dongle	*second;
	bool		acquired;

	first = coder->left;
	second = coder->right;
	if (first == second)
		return (false);
	if (first > second)
	{
		first = coder->right;
		second = coder->left;
	}
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	acquired = dongle_is_available(first, now)
		&& dongle_is_available(second, now);
	if (acquired)
	{
		first->current_owner = coder;
		second->current_owner = coder;
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (acquired);
}
