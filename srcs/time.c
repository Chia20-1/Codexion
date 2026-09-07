/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 19:06:26 by chilim            #+#    #+#             */
/*   Updated: 2026/09/07 15:03:11 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>
#include <sys/time.h>
#include <unistd.h>

long long	get_time_ms(void)
{
	struct timeval	time;
	long long		usec_part;

	if (gettimeofday(&time, NULL) == -1)
		return (-1);
	usec_part = time.tv_usec / 1000;
	if (time.tv_sec > (LLONG_MAX - usec_part) / 1000)
		return (-1);
	return (time.tv_sec * 1000LL + usec_part);
}

long long	get_elapsed_ms(t_data *data)
{
	long long	current;

	current = get_time_ms();
	if (current == -1)
		return (-1);
	return(current - data->start_time);
}

bool	sleep_ms(long long duration_ms)
{
	long long	start;
	long long	current;

	if (duration_ms <= 0)
		return (true);
	start = get_time_ms();
	if (start == -1)
		return (false);
	while (true)
	{
		current = get_time_ms();
		if (current == -1)
			return (false);
		if (current - start >= duration_ms)
			return (true);
		if (usleep(1000) == -1)
			return (false);
	}
}
