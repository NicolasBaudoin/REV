#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/*
** 1) dessiner : stylo en (0,0), leve. w/a/s/d bouge (reste dans le plateau),
**    x leve/baisse. Si stylo baisse -> la case courante devient vivante.
** 2) N iterations : voisins vivants sur 8 cases (hors plateau = mort)
**    vivante : survit si 2 ou 3 voisins ; morte : nait si exactement 3.
**    -> TOUJOURS calculer dans un 2e tableau, puis echanger.
** 3) afficher : 'O' vivant, ' ' mort, '\n' par ligne.
*/

int	w, h;

int	alive(char *b, int y, int x)
{
	return (y >= 0 && y < h && x >= 0 && x < w && b[y * w + x]);
}

int	neighbours(char *b, int y, int x)
{
	int	n = 0;

	for (int dy = -1; dy <= 1; dy++)
		for (int dx = -1; dx <= 1; dx++)
			if ((dy || dx) && alive(b, y + dy, x + dx))
				n++;
	return (n);
}

int	main(int ac, char **av)
{
	char	*b, *nb, *tmp, c;
	int		it, x = 0, y = 0, pen = 0, n;

	if (ac != 4)
		return (1);
	w = atoi(av[1]);
	h = atoi(av[2]);
	it = atoi(av[3]);
	if (w <= 0 || h <= 0)
		return (1);
	b = calloc(w * h, 1);
	nb = calloc(w * h, 1);
	if (!b || !nb)
		return (free(b), free(nb), 1);
	while (read(0, &c, 1) == 1)
	{
		if (c == 'w' && y > 0)
			y--;
		else if (c == 's' && y < h - 1)
			y++;
		else if (c == 'a' && x > 0)
			x--;
		else if (c == 'd' && x < w - 1)
			x++;
		else if (c == 'x')
			pen = !pen;
		if (pen)
			b[y * w + x] = 1;
	}
	while (it-- > 0)
	{
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
			{
				n = neighbours(b, i, j);
				nb[i * w + j] = (n == 3 || (n == 2 && b[i * w + j]));
			}
		tmp = b;
		b = nb;
		nb = tmp;
	}
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
			putchar(b[i * w + j] ? 'O' : ' ');
		putchar('\n');
	}
	free(b);
	free(nb);
	return (0);
}
