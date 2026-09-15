/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chilim <chilim@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 14:11:41 by chilim            #+#    #+#             */
/*   Updated: 2026/09/15 16:25:05 by chilim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stdlib.h>

/* ******************************************************** */
/*                    STRUCT CONTAINERS                     */
/* ******************************************************** */
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;
typedef struct s_monitor	t_monitor;
typedef struct s_request	t_request;
typedef struct s_scheduler	t_scheduler;
typedef struct s_data		t_data;
typedef struct s_config		t_config;

typedef enum e_policy
{
	POLICY_FIFO,
	POLICY_EDF
}	t_policy;

struct s_config
{
	int			number_of_coders;
	long long	time_to_burnout;
	long long	time_to_compile;
	long long	time_to_debug;
	long long	time_to_refactor;
	int			number_of_compiles_required;
	long long	dongle_cooldown;
	char		*scheduler;
};

struct s_coder
{
	int			id;
	int			compile_count;
	long long	last_compile_start;
	t_dongle	*left;
	t_dongle	*right;
	t_data		*data;
	t_request	*request;
	pthread_t	thread;
	bool		thread_created;	
};

struct s_dongle
{
	t_coder				*current_owner;
	long long			cooldown_deadline;
	pthread_mutex_t		mutex;
	bool				has_mutex;
};

struct s_monitor
{
	bool				should_stop;
	int					wait_error;
	pthread_t			thread;
	pthread_mutex_t		state_mutex;
	pthread_mutex_t		log_output_mutex;
	pthread_cond_t		wakeup_cond;
	bool				has_state_mutex;
	bool				has_log_mutex;
	bool				has_wakeup_cond;
	bool				thread_created;
	bool				simulation_started;
};

struct s_request
{
	t_coder			*coder;
	long long		burnout_deadline;
	unsigned long	arrival_order;
	bool			dongles_granted;
};

struct s_scheduler
{
	t_policy		policy;
	t_request		**request_heap;
	int				heap_size;
	int				heap_capacity;
	unsigned long	arrival_counter;
	pthread_mutex_t	request_queue_mutex;
	pthread_cond_t	request_queue_cond;
	bool			has_mutex;
	bool			has_cond;
};

struct s_data
{
	long long			start_time;
	t_config			config;
	t_coder				*coders;
	t_dongle			*dongles;
	t_scheduler			scheduler;
	t_monitor			monitor;
};

/* ******************************************************** */
/*                      PARSE INPUT                         */
/* ******************************************************** */
void		set_integer_config_value(int value, int index, t_config *config);
void		set_llong_config_value(long long value, int index,
				t_config *config);
bool		parse_input(int argc, char **argv, t_config *config);

/* ******************************************************** */
/*                      INITIALIZE                          */
/* ******************************************************** */
void		*ft_calloc(size_t nmemb, size_t size);
bool		init_scheduler_mutex(t_data *data);
bool		init_scheduler_cond(t_data *data);
bool		init_data(t_data *data);

/* ******************************************************** */
/*                        THREAD                            */
/* ******************************************************** */
int			join_coders(t_data *data);
bool		create_monitor(t_data *data);
int			join_monitor(t_data *data);
bool		run_simulation(t_data *data);

/* ******************************************************** */
/*                        CODER                             */
/* ******************************************************** */
void		*coder_routine(void *argument);

/* ******************************************************** */
/*                        DONGLE                            */
/* ******************************************************** */
bool		dongle_pair_try_acquire(t_coder *coder, long long now);

/* ******************************************************** */
/*                     TIME & LOG                           */
/* ******************************************************** */
long long	get_time_ms(void);
long long	get_elapsed_ms(t_data *data);
bool		sleep_ms(long long duration_ms);
void		log_status(t_coder *coder, const char *status);

/* ******************************************************** */
/*                       MONITOR                            */
/* ******************************************************** */
bool		simulation_should_stop(t_data *data);
void		request_stop(t_data *data);
void		*monitor_routine(void *argument);

/* ******************************************************** */
/*                          HEAP                            */
/* ******************************************************** */
void		shift_up(t_request **heap, int index, t_policy policy);
void		shift_down(t_request **heap, int index, int size, t_policy policy);
t_request	*peek_heap(t_data *data);
t_request	*pop_heap(t_data *data);
bool		push_heap(t_data *data, t_request *request);

/* ******************************************************** */
/*                        CLEAN UP                          */
/* ******************************************************** */
void		cleanup_data(t_data *data);

#endif
