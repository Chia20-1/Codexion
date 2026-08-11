#include <pthread.h>
#include <stdio.h>

void	*worker(void *arg)
{
	int		id;

	id = *(int *)arg;
	printf("Worker: %d\n", id);
	return (NULL);
}

int main(void)
{
	pthread_t	threads[5];
	int			ids[5];
	int			i;

	i = 0;
	while (i < 5)
	{
		ids[i] = i + 1;
		pthread_create(&threads[i], NULL, worker, &ids[i]);
		i++;
	}
	i = 0;
	while (i < 5)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	printf("Worker has all finished their work\n");
	return (0);
}
