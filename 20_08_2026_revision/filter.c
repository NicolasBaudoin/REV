#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int ac, char **av)
{
	if (ac != 2 || !av[1][0])
		return 1;

		char *buf = NULL, *s = av[1], tmp[4096];
		int n, i, k, sl = strlen(s), len = 0, cap = 0;

		while ((n = read(0, tmp, sizeof(tmp))))
		{
			if (n + len + 1 > cap)
			{
				cap = (len + n + 1) * 2
			}
		}

}
