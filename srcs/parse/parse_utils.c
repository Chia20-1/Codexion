/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:37:09 by chilim            #+#    #+#             */
/*   Updated: 2026/09/05 20:26:14 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	set_integer_config_value(int value, int index, t_config *config)
{
	if (index == 1)
		config->number_of_coders = value;
	else if (index == 6)
		config->number_of_compiles_required = value;
	return ;
}

void	set_llong_config_value(long long value, int index, t_config *config)
{
	if (index == 2)
		config->time_to_burnout = value;
	else if (index == 3)
		config->time_to_compile = value;
	else if (index == 4)
		config->time_to_debug = value;
	else if (index == 5)
		config->time_to_refactor = value;
	else if (index == 7)
		config->dongle_cooldown = value;
	return ;
}
