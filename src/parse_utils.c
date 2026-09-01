/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:37:09 by chilim            #+#    #+#             */
/*   Updated: 2026/09/01 15:50:00 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/codexion.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t			i;
	unsigned char	*str1;
	unsigned char	*str2;

	i = 0;
	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	while (str1[i] != '\0' && str1[i] == str2[i])
		i++;
	return (str1[i] - str2[i]);
}

void set_integer_config_value(int value, int index, t_config *config)
{
	if (index == 1)
		config->number_of_coders = value;
	else if (index == 6)
		config->number_of_compiles_required = value;
	return ;	
}

void set_llong_config_value(long long value, int index, t_config *config)
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