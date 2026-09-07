/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:27:51 by chilim            #+#    #+#             */
/*   Updated: 2026/09/07 14:35:58 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static bool	parse_scheduler(char *str, t_config *config)
{
	if (strcmp(str, "fifo") != 0
		&& strcmp(str, "edf") != 0)
		return (false);
	config->scheduler = str;
	return (true);
}

static bool	parse_integer(int index, char *str, t_config *config)
{
	int	value;
	int	digit;

	if (*str == '\0')
		return (false);
	value = 0;
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (false);
		digit = *str - '0';
		if (value > (INT_MAX - digit) / 10)
			return (false);
		value = value * 10 + digit;
		str++;
	}
	if (index == 1 && value < 1)
		return (false);
	set_integer_config_value(value, index, config);
	return (true);
}

static bool	parse_long_long(int index, char *str, t_config *config)
{
	long long	value;
	long long	digit;

	if (*str == '\0')
		return (false);
	value = 0;
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (false);
		digit = *str - '0';
		if (value > (LLONG_MAX - digit) / 10)
			return (false);
		value = value * 10 + digit;
		str++;
	}
	set_llong_config_value(value, index, config);
	return (true);
}

bool	parse_input(int argc, char **argv, t_config *config)
{
	int		i;
	bool	is_valid_input;

	if (argc != 9)
		return (false);
	i = 1;
	while (i < argc)
	{
		if (i == (argc - 1))
			is_valid_input = parse_scheduler(argv[i], config);
		else if (i == 1 || i == 6)
			is_valid_input = parse_integer(i, argv[i], config);
		else
			is_valid_input = parse_long_long(i, argv[i], config);
		if (!is_valid_input)
			return (false);
		i++;
	}
	return (true);
}
