#include <stdlib.h>
#include <unistd.h>

int	g_n;
int	*g_q;

void	putnbr(int x)
{
	char	c;

	if (x >= 10)
		putnbr(x / 10);
	c = x % 10 + '0';
	write(1, &c, 1);
}

int	ok(int col, int row)
{
	int	c;

	c = 0;
	while (c < col)
	{
		if (g_q[c] == row)
			return (0);
		if (g_q[c] - c == row - col)
			return (0);
		if (g_q[c] + c == row + col)
			return (0);
		c++;
	}
	return (1);
}

void	solve(int col)
{
	int	row;

	if (col == g_n)
	{
		row = 0;
		while (row < g_n)
		{
			putnbr(g_q[row]);
			if (row < g_n - 1)
				write(1, " ", 1);
			row++;
		}
		write(1, "\n", 1);
		return ;
	}
	row = 0;
	while (row < g_n)
	{
		if (ok(col, row))
		{
			g_q[col] = row;
			solve(col + 1);
		}
		row++;
	}
}

int	main(int ac, char **av)
{
	if (ac != 2)
		return (1);
	g_n = atoi(av[1]);
	g_q = malloc(sizeof(int) * (g_n + 1));
	if (!g_q)
		return (1);
	solve(0);
	free(g_q);
	return (0);
}
