/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:21:05 by chilim            #+#    #+#             */
/*   Updated: 2026/09/09 14:29:56 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>

void	*coder_routine(void *argument)
{
	t_coder	*coder;

	coder = (t_coder *)argument;
	while (!simulation_should_stop(coder->data))
	{
		log_status(coder, "is refractoring");
		usleep(1000);
	}
	log_status(coder, "should stop");
	return (NULL);
}
