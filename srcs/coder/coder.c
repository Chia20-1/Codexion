/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:21:05 by chilim            #+#    #+#             */
/*   Updated: 2026/09/21 21:17:44 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdbool.h>
#include <unistd.h>

static bool	coder_compile(t_coder *coder)
{
	t_compile_start	result;
	bool			completed;

	result = coder_start_compile(coder);
	if (result != COMPILE_STARTED)
	{
		if (result == COMPILE_ERROR)
			request_stop(coder->data, SIM_ERROR);
		if (coder->request->dongles_granted)
		{
			if (!scheduler_release_dongles(coder))
				return (false);
		}
		if (result == COMPILE_EXPIRED)
		{
			if (!coder_wait_for_stop(coder->data))
				return (false);
		}
		return (false);
	}
	completed = coder_wait_compile_duration(coder);
	return (coder_finish_compile(coder, completed));
}

static bool	coder_debug(t_coder *coder)
{
	if (is_stop_requested(coder->data))
		return (false);
	log_status(coder, "is debugging");
	if (!sleep_ms(coder->data->config.time_to_debug))
		return (false);
	return (!is_stop_requested(coder->data));
}

static bool	coder_refactor(t_coder *coder)
{
	if (is_stop_requested(coder->data))
		return (false);
	log_status(coder, "is refactoring");
	if (!sleep_ms(coder->data->config.time_to_refactor))
		return (false);
	return (!is_stop_requested(coder->data));
}

void	*coder_routine(void *argument)
{
	t_coder	*coder;

	coder = (t_coder *)argument;
	if (!coder_wait_for_start(coder->data))
	{
		request_stop(coder->data, SIM_ERROR);
		return (NULL);
	}
	while (!is_stop_requested(coder->data))
	{
		if (!coder_compile(coder) || !coder_debug(coder)
			|| !coder_refactor(coder))
		{
			if (!is_stop_requested(coder->data))
				request_stop(coder->data, SIM_ERROR);
			break ;
		}
	}
	return (NULL);
}
