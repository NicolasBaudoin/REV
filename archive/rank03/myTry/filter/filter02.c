/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filter02.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbaudoin <nbaudoin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/11 11:19:40 by nbaudoin          #+#    #+#             */
/*   Updated: 2026/08/11 11:19:41 by nbaudoin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int ac, char **av)
{
	if (ac != 2 || !av[1][0])
		return 1;

	char *buf, *s, tmp[4096];
	int n, i, k, len, sl, cap;

	cap = 0, len = 0;
	buf = NULL;
	s = av[1];
	sl = strlen(s);

	while ((n = read(0, tmp, sizeof(tmp))) > 0)
	{
		if (len + n + 1 > cap)
		{
			cap = (len + n + 1) * 2;
			char *nb = realloc(buf, cap);
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
	return 0;
}
