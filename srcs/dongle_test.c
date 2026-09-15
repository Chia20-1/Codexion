#include "codexion.h"
#include <assert.h>
#include <stdio.h>

int	main(void)
{
	t_data		data = {0};
	t_dongle	dongles[3] = {0};
	t_coder		a = {0};
	t_coder		b = {0};

	data.config.dongle_cooldown = 100;
	a.data = &data;
	a.left = &dongles[0];
	a.right = &dongles[1];
	b.data = &data;
	b.left = &dongles[1];
	b.right = &dongles[2];

	assert(pthread_mutex_init(&dongles[0].mutex, NULL) == 0);
	assert(pthread_mutex_init(&dongles[1].mutex, NULL) == 0);
	assert(pthread_mutex_init(&dongles[2].mutex, NULL) == 0);
	assert(pthread_mutex_init(
			&data.scheduler.request_queue_mutex, NULL) == 0);
	pthread_mutex_lock(&data.scheduler.request_queue_mutex);

	/* A acquires both dongles. */
	assert(dongle_pair_try_acquire(&a, 0));
	assert(dongles[0].current_owner == &a);
	assert(dongles[1].current_owner == &a);

	/* B shares dongle 1 with A: B must get neither. */
	assert(!dongle_pair_try_acquire(&b, 0));
	assert(dongles[1].current_owner == &a);
	assert(dongles[2].current_owner == NULL);

	/* B cannot release A's dongle. */
	assert(!dongle_pair_release(&b, 500));
	assert(dongles[1].current_owner == &a);

	/* A releases both; cooldown lasts until 600. */
	assert(dongle_pair_release(&a, 500));
	assert(dongles[0].current_owner == NULL);
	assert(dongles[1].current_owner == NULL);
	assert(dongles[0].cooldown_deadline == 600);
	assert(dongles[1].cooldown_deadline == 600);

	/* Check the exact cooldown boundary. */
	assert(!dongle_pair_try_acquire(&b, 599));
	assert(dongles[1].current_owner == NULL);
	assert(dongles[2].current_owner == NULL);
	assert(dongle_pair_try_acquire(&b, 600));
	assert(dongles[1].current_owner == &b);
	assert(dongles[2].current_owner == &b);
	assert(dongle_pair_release(&b, 700));

	/* One dongle cannot count as a pair. */
	a.right = a.left;
	assert(!dongle_pair_try_acquire(&a, 800));

	pthread_mutex_unlock(&data.scheduler.request_queue_mutex);
	assert(pthread_mutex_destroy(&dongles[0].mutex) == 0);
	assert(pthread_mutex_destroy(&dongles[1].mutex) == 0);
	assert(pthread_mutex_destroy(&dongles[2].mutex) == 0);
	assert(pthread_mutex_destroy(
			&data.scheduler.request_queue_mutex) == 0);
	puts("All dongle tests passed.");
	return (0);
}