/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:21:05 by chilim            #+#    #+#             */
/*   Updated: 2026/09/21 18:41:21 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdbool.h>
#include <unistd.h>

static bool	coder_compile(t_coder *coder)
{
	bool	started;
	bool	completed;

	started = coder_start_compile(coder);
	completed = coder_run_compile(coder, started);
	completed = coder_finish_compile(coder, completed);
	return (completed);
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

bool	all_coders_completed(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->config.number_of_coders)
	{
		if (data->coders[i].compile_count
			< data->config.number_of_compiles_required)
			return (false);
		i++;
	}
	return (true);
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
			request_stop(coder->data, SIM_ERROR);
			break ;
		}
	}
	return (NULL);
}
