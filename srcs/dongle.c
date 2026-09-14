/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 17:47:36 by chilim            #+#    #+#             */
/*   Updated: 2026/09/14 17:56:41 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	dongle_is_available(t_dongle *dongle, long long now)
{
	return (dongle->current_owner == NULL
		&& now >= dongle->cooldown_deadline);
}
