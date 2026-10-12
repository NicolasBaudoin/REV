#include <stdio.h>
#include <stdlib.h>

int	*g_set;
int	*g_in;
int	g_n;
int	g_target;

void	print_set(void)
{
	int	first;
	int	k;

	first = 1;
	k = 0;
	while (k < g_n)
	{
		if (g_in[k])
		{
			if (!first)
				printf(" ");
			printf("%d", g_set[k]);
			first = 0;
		}
		k++;
	}
	printf("\n");
}

void	rec(int i, int sum)
{
	if (i == g_n)
	{
		if (sum == g_target)
			print_set();
		return ;
	}
	g_in[i] = 0;
	rec(i + 1, sum);
	g_in[i] = 1;
	rec(i + 1, sum + g_set[i]);
}

int	main(int ac, char **av)
{
	int	i;

	if (ac < 2)
		return (1);
	g_n = ac - 2;
	g_target = atoi(av[1]);
	g_set = malloc(sizeof(int) * (g_n + 1));
	g_in = calloc(g_n + 1, sizeof(int));
	if (!g_set || !g_in)
		return (1);
	i = 0;
	while (i < g_n)
	{
		g_set[i] = atoi(av[i + 2]);
		i++;
	}
	rec(0, 0);
	free(g_set);
	free(g_in);
	return (0);
}
