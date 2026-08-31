/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 14:11:41 by chilim            #+#    #+#             */
/*   Updated: 2026/08/31 19:48:36 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <stdbool.h>

typedef struct s_config
{
	int			number_of_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	int			number_of_compiles_required;
	long long	dongle_cooldown;
	char		*scheduler;
}	t_config;

typedef struct s_coder
{
	int			id;
	int			compile_count;
	long long	last_compile_start;
	t_dongle	*left;
	t_dongle	*right;
	t_data		*data;	
}	t_coder;

typedef struct s_dongle
{
	t_coder				*current_owner;
	long long			cooldown_deadline;
	pthread_mutex_t		mutex;
}	t_dongle;

typedef struct s_monitor
{
	bool				should_stop;
	pthread_mutex_t		stop_flag_mutex;
	pthread_mutex_t		log_output_mutex;
	pthread_mutex_t		monitor_event_mutex;
	pthread_cond_t		monitor_wakeup_cond;
}	t_monitor;

typedef struct s_request
{
	t_coder			*coder;
	long long		burnout_deadline;
	unsigned long	arrival_order;
	bool			dongles_granted;
}	t_request;

typedef struct s_scheduler
{
	t_request		**request_heap;
	int				heap_size;
	int				heap_capacity;
	unsigned long	arrival_counter;
	pthread_mutex_t	request_queue_mutex;
	pthread_cond_t	request_queue_changed;
}	t_scheduler;

typedef struct s_data
{
	long long			start_time;
	t_config			config;
	t_coder				*coders;
	t_dongle			*dongles;
	t_scheduler			scheduler;
	t_monitor			monitor;
}	t_data;

#endif