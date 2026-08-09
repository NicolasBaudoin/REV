#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int ac, char **av)
{
	if (ac != 2 || !av[1][0])
		return 1;

	char *buf, *s, tmp[4096];

	int sl, n, i, k, cap, len;

	buf = NULL;
	s = av[1];
	cap = 0;
	sl = strlen(s);
	len = 0; // oublie de len

	while ((n = read(0, tmp, sizeof(tmp))) > 0)
	{
		if ( len + n + 1 > cap)
		{
			cap = (len + n + 1) * 2;
			char *nb = realloc(buf, cap);
			if (!nb)
				return (free(buf), perror("Error"), 1);
			buf = nb; //forgot from this
		}
		memmove(buf + len, tmp, n);
		len += n;
	} // to this
	if (n < 0)
		return (free(buf), perror("Error"), 1);
	i = 0;
	while (i < len)
	{
		//petit doute ici
		if (sl + i <= len && memmem(buf + i, sl, s, sl) == buf + i)
		{
			k = 0;
			while (k++ < sl)
				write(1, "*", 1);
			i += sl; // petit doute ici
		}
		else
			write(1, buf + i++, 1);
	}
	free(buf);
	return 0;
}
