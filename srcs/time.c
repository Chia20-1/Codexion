/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 19:06:26 by chilim            #+#    #+#             */
/*   Updated: 2026/09/29 15:39:19 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
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
	return (current - data->start_time);
}

t_sleep_result	sleep_ms(t_data *data, long long duration_ms)
{
	long long	start;
	long long	now;

	if (duration_ms <= 0)
		return (SLEEP_COMPLETED);
	start = get_time_ms();
	if (start == -1)
		return (SLEEP_ERROR);
	while (true)
	{
		if (is_stop_requested(data))
			return (SLEEP_STOPPED);
		now = get_time_ms();
		if (now == -1)
			return (SLEEP_ERROR);
		if (now - start >= duration_ms)
			return (SLEEP_COMPLETED);
		if (usleep(1000) == -1)
			return (SLEEP_ERROR);
	}
}
