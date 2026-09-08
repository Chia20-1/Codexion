/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 14:08:20 by chilim            #+#    #+#             */
/*   Updated: 2026/09/08 14:54:24 by chilim           ###   ########.fr       */
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

#include <stdbool.h>
#include <unistd.h>

int	main(void)
{
	t_data	data;
	t_coder	coder;

	data = (t_data){0};
	coder = (t_coder){0};
	coder.id = 1;
	coder.data = &data;
	if (pthread_mutex_init(&data.monitor.log_output_mutex, NULL) != 0)
		return (1);
	data.monitor.has_log_mutex = true;
	data.start_time = get_time_ms();
	if (data.start_time == -1)
	{
		pthread_mutex_destroy(&data.monitor.log_output_mutex);
		return (1);
	}
	log_status(&coder, "is debugging");
	if (!sleep_ms(100))
	{
		pthread_mutex_destroy(&data.monitor.log_output_mutex);
		return (1);	
	}
	log_status(&coder, "is refractoring");
	pthread_mutex_destroy(&data.monitor.log_output_mutex);
	return (0);
}