#include <stdio.h>

int	valid(char *s)
{
	int	c;
	int	i;

	c = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] == '(')
			c++;
		else if (s[i] == ')')
		{
			c--;
			if (c < 0)
				return (0);
		}
		i++;
	}
	return (c == 0);
}

void	dfs(char *s, int start, int l, int r)
{
	int	i;

	if (l == 0 && r == 0)
	{
		if (valid(s))
			puts(s);
		return ;
	}
	i = start;
	while (s[i])
	{
		if (s[i] == '(' && l > 0)
		{
			s[i] = ' ';
			dfs(s, i + 1, l - 1, r);
			s[i] = '(';
		}
		else if (s[i] == ')' && r > 0)
		{
			s[i] = ' ';
			dfs(s, i + 1, l, r - 1);
			s[i] = ')';
		}
		i++;
	}
}

int	main(int ac, char **av)
{
	int	l;
	int	r;
	int	i;

	if (ac != 2)
		return (1);
	l = 0;
	r = 0;
	i = 0;
	while (av[1][i])
	{
		if (av[1][i] == '(')
			l++;
		else if (av[1][i] == ')')
		{
			if (l > 0)
				l--;
			else
				r++;
		}
		i++;
	}
	dfs(av[1], 0, l, r);
	return (0);
}
