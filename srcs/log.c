/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 14:08:20 by chilim            #+#    #+#             */
/*   Updated: 2026/09/07 16:05:44 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>
#include <pthread.h>

void		log_status(t_coder *coder, const char *status)
{
	t_data		*data;
	long long	elapsed_ms;

	data = coder->data;
	pthread_mutex_lock(&data->monitor.log_output_mutex);
	elapsed_ms = get_elapsed_ms(data);
	if (elapsed_ms != -1)
		printf("%lld %d %s\n", elapsed_ms, coder->id, status);
	pthread_mutex_unlock(&data->monitor.log_output_mutex);
}