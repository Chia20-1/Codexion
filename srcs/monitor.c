/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 20:58:47 by chilim            #+#    #+#             */
/*   Updated: 2026/09/09 14:35:37 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>
#include <pthread.h>
#include <stdbool.h>

bool	simulation_should_stop(t_data *data)
{
	bool	stop;

	pthread_mutex_lock(&data->monitor.state_mutex);
	stop = data->monitor.should_stop;
	pthread_mutex_unlock(&data->monitor.state_mutex);
	return (stop);
}

void	request_stop(t_data *data)
{
	pthread_mutex_lock(&data->monitor.state_mutex);
	data->monitor.should_stop = true;
	pthread_mutex_unlock(&data->monitor.state_mutex);
}
