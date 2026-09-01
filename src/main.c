/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 16:16:18 by chilim            #+#    #+#             */
/*   Updated: 2026/09/01 16:25:01 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/codexion.h"
#include <stdio.h>

static void	print_config(t_config config)
{
	printf("===========================");
	printf("Number of coders: %d\n", config.number_of_coders);
	printf("Time to burnout: %lld\n", config.time_to_burnout);
	printf("Time to compile: %lld\n", config.time_to_compile);
	printf("Time to debug: %lld\n", config.time_to_debug);
	printf("Time to refractor: %lld\n", config.time_to_refactor);
	printf("Number of compiles required: %d\n", config.number_of_compiles_required);
	printf("Dongle cooldown: %lld\n", config.dongle_cooldown);
	printf("Scheduler: %s\n", config.scheduler);
}

int	main(int argc, char **argv)
{
	t_config	config;

	if (!parse_input(argc, argv, &config))
	{
		printf("Error");
		return (1);
	}
	print_config(config);
}
