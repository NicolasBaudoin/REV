#define _GNU_SOURCE
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int	main(int ac, char **av)
{
	char	*s;
	size_t	sl;
	char	*buf;
	size_t	cap;
	size_t	len;
	char	tmp[4096];
	ssize_t	n;
	size_t	i;
	size_t	k;

	if (ac != 2 || !av[1][0])
		return (1);
	s = av[1];
	sl = strlen(s);
	buf = NULL;
	cap = 0;
	len = 0;
	while ((n = read(0, tmp, sizeof(tmp))) > 0)
	{
		if (len + n + 1 > cap)
		{
			cap = (len + n + 1) * 2;
			char *nb = realloc(buf, cap);
			if (!nb)
				return (free(buf), perror("Error"), 1);
			buf = nb;
		}
		memmove(buf + len, tmp, n);
		len += n;
	}
	if (n < 0)
		return (free(buf), perror("Error"), 1);
	i = 0;
	while (i < len)
	{
		if (i + sl <= len && memmem(buf + i, sl, s, sl) == buf + i)
		{
			k = 0;
			while (k++ < sl)
				write(1, "*", 1);
			i += sl;
		}
		else
			write(1, buf + i++, 1);
	}
	free(buf);
	return (0);
}
