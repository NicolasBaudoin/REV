#include <unistd.h>

int	ft_len(char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	swap(char *a, char *b)
{
	char	t;

	t = *a;
	*a = *b;
	*b = t;
}

int	main(int ac, char **av)
{
	char	*s;
	int		n;
	int		i;
	int		j;

	if (ac != 2)
		return (0);
	s = av[1];
	n = ft_len(s);
	i = 0;
	while (i < n)
	{
		j = i + 1;
		while (j < n)
		{
			if (s[i] > s[j])
				swap(&s[i], &s[j]);
			j++;
		}
		i++;
	}
	while (1)
	{
		write(1, s, n);
		write(1, "\n", 1);
		i = n - 2;
		while (i >= 0 && s[i] >= s[i + 1])
			i--;
		if (i < 0)
			return (0);
		j = n - 1;
		while (s[j] <= s[i])
			j--;
		swap(&s[i], &s[j]);
		i++;
		j = n - 1;
		while (i < j)
			swap(&s[i++], &s[j--]);
	}
}
