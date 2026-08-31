/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:27:51 by chilim            #+#    #+#             */
/*   Updated: 2026/08/31 21:51:16 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/codexion.h"

static int	ft_strcmp(const char *s1, const char *s2)
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

static bool	parse_scheduler(char *str, t_config *config)
{
	return (!ft_strcmp(str, "fifo") || !ft_strcmp(str, "edf"));
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
