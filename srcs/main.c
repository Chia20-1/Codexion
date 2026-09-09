/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 16:16:18 by chilim            #+#    #+#             */
/*   Updated: 2026/09/09 14:36:53 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>
#include <unistd.h>

// static void	print_config(t_config config)
// {
// 	printf("===========================");
// 	printf("Number of coders: %d\n", config.number_of_coders);
// 	printf("Time to burnout: %lld\n", config.time_to_burnout);
// 	printf("Time to compile: %lld\n", config.time_to_compile);
// 	printf("Time to debug: %lld\n", config.time_to_debug);
// 	printf("Time to refractor: %lld\n", config.time_to_refactor);
// 	printf("Number of compiles required: %d\n",
//		config.number_of_compiles_required);
// 	printf("Dongle cooldown: %lld\n", config.dongle_cooldown);
// 	printf("Scheduler: %s\n", config.scheduler);
// }

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
		cleanup_data(&data);
		return (1);
	}
	usleep(1000);
	request_stop(&data);
	i = 0;
	while (i < data.config.number_of_coders)
	{
		pthread_join(data.coders[i].thread, NULL);
		i++;
	}
	cleanup_data(&data);
	return (0);
}
