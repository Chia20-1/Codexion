/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:47:36 by chilim            #+#    #+#             */
/*   Updated: 2026/09/15 18:11:15 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdbool.h>
#include <limits.h>

static bool	dongle_is_available(t_dongle *dongle, long long now)
{
	return (dongle->current_owner == NULL
		&& now >= dongle->cooldown_deadline);
}

static bool	lock_dongle_pair(t_coder *coder, t_dongle **first,
		t_dongle **second)
{
	if (coder->left == coder->right)
		return (false);
	*first = coder->left;
	*second = coder->right;
	if (*first > *second)
	{
		*first = coder->right;
		*second = coder->left;
	}
	pthread_mutex_lock(&(*first)->mutex);
	pthread_mutex_lock(&(*second)->mutex);
	return (true);
}

static bool	update_deadline(t_data *data, long long release_time
	, long long *deadline)
{
	if (release_time < 0)
		return (false);
	if (release_time > LLONG_MAX - data->config.dongle_cooldown)
		return (false);
	*deadline = release_time + data->config.dongle_cooldown;
	return (true);
}

bool	dongle_pair_try_acquire(t_coder *coder, long long now)
{
	t_dongle	*first;
	t_dongle	*second;
	bool		acquired;

	if (!lock_dongle_pair(coder, &first, &second))
		return (false);
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

bool	dongle_pair_release(t_coder *coder, long long release_time)
{
	t_dongle	*first;
	t_dongle	*second;
	bool		released;
	long long	deadline;

	if (!update_deadline(coder->data, release_time, &deadline))
		return (false);
	if (!lock_dongle_pair(coder, &first, &second))
		return (false);
	released = (first->current_owner == coder)
		&& (second->current_owner == coder);
	if (released)
	{
		first->current_owner = NULL;
		second->current_owner = NULL;
		first->cooldown_deadline = deadline;
		second->cooldown_deadline = first->cooldown_deadline;
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (released);
}
