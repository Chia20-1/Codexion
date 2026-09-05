/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 19:06:26 by chilim            #+#    #+#             */
/*   Updated: 2026/09/05 20:27:48 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	time;
	long long		result;

	if (gettimeofday(&time, NULL) == -1)
		return (-1);
	result = time.tv_sec * 1000LL;
	result = result + (time.tv_usec / 1000); 
	return (result);
}

long long	get_elapsed_ms(t_data *data)
{
	long long	current;

	current = get_time_ms();
	if (current == -1)
		return (-1);
	return(current - data->start_time);
}