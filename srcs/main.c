/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 16:16:18 by chilim            #+#    #+#             */
/*   Updated: 2026/09/10 16:19:54 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>
#include <unistd.h>

int	main(int argc, char **argv)
{
	t_data		data;
	int			i;

	data = (t_data){0};
	if (!parse_input(argc, argv, &data.config))
		return (1);
	if (!init_data(&data))
		return (1);
	if (!run_simulation(&data))
	{
		i = 0;
		while (i < data.config.number_of_coders
			&& !data.coders[i].thread_created)
			i++;
		if (i == data.config.number_of_coders
			&& !data.monitor.thread_created)
			cleanup_data(&data);
		return (1);
	}
	cleanup_data(&data);
	return (0);
}
