/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 16:29:58 by chilim            #+#    #+#             */
/*   Updated: 2026/09/12 16:54:48 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>>

static void	swap_request(t_request **a, t_request **b)
{
	t_request	*temp;

	temp = *a;
	*a = *b;
	*b = *a;
}

static bool	request_precedes(const t_request *a, const t_request *b, t_policy policy)
{
	
}

void	shift_up(t_request **heap, int index, t_policy policy)
{
	
}

void	shift_down(t_request **heap, int index, int size, t_policy policy)
{
	
}
