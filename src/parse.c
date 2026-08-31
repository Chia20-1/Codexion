/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:27:51 by chilim            #+#    #+#             */
/*   Updated: 2026/08/31 20:32:26 by chilim           ###   ########.fr       */
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

static bool	check_scheduler(char *str)
{
	return (!ft_strcmp(str, "fifo") || !ft_strcmp(str, "edf"));
}

bool	parse_input(int argc, char **argv)
{
	int		i;

	if (argc != 9)
		return (false);
	i = 0;
	while (i < argc)
	{
		if (i == (argc - 1))
			return (check_scheduler(argv[i]));
		i++;
	}
}
