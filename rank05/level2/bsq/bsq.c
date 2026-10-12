#include <stdio.h>
#include <stdlib.h>

/*
** 1) 1re ligne : fscanf "%d %c %c %c" puis la fin de ligne doit etre "\n"
** 2) n lignes via getline : finit par '\n', meme longueur > 0, que empty/obstacle
** 3) DP : dp[i][j] = cote du plus grand carre dont le coin BAS-DROIT est (i,j)
**        = 0 si obstacle, sinon 1 + min(haut, gauche, diag haut-gauche)
**    garder le max STRICT (>) en parcourant ligne par ligne -> le plus haut puis le plus a gauche
*/

typedef struct s_map
{
	int		n;
	char	empty;
	char	obst;
	char	full;
	int		w;
	char	**g;
}	t_map;

static void	free_map(t_map *m)
{
	for (int i = 0; i < m->n; i++)
		free(m->g[i]);
	free(m->g);
}

static int	is_print(char c)
{
	return (c >= 32 && c <= 126);
}

static int	load(FILE *f, t_map *m)
{
	char	*line = NULL;
	size_t	cap = 0;
	ssize_t	len;

	m->g = NULL;
	if (fscanf(f, "%d %c %c %c", &m->n, &m->empty, &m->obst, &m->full) != 4
		|| m->n <= 0 || m->empty == m->obst || m->empty == m->full
		|| m->obst == m->full || !is_print(m->empty) || !is_print(m->obst)
		|| !is_print(m->full))
		return (-1);
	len = getline(&line, &cap, f);					/* reste de la 1re ligne */
	if (len != 1 || line[0] != '\n')
		return (free(line), -1);
	m->g = calloc(m->n, sizeof(char *));
	if (!m->g)
		return (free(line), -1);
	for (int i = 0; i < m->n; i++)
	{
		len = getline(&line, &cap, f);
		if (len < 2 || line[len - 1] != '\n' || (i > 0 && len - 1 != m->w))
			return (free(line), free_map(m), -1);
		m->w = len - 1;
		line[len - 1] = '\0';
		for (int j = 0; j < m->w; j++)
			if (line[j] != m->empty && line[j] != m->obst)
				return (free(line), free_map(m), -1);
		m->g[i] = line;								/* on garde le buffer */
		line = NULL;
		cap = 0;
	}
	if (getline(&line, &cap, f) != -1)				/* lignes en trop */
		return (free(line), free_map(m), -1);
	free(line);
	return (0);
}

static void	solve(t_map *m)
{
	int	*dp = calloc((size_t)m->n * m->w, sizeof(int));
	int	best = 0, bi = 0, bj = 0, v;

	if (!dp)
		return ;
	for (int i = 0; i < m->n; i++)
		for (int j = 0; j < m->w; j++)
		{
			if (m->g[i][j] == m->obst)
				v = 0;
			else if (i == 0 || j == 0)
				v = 1;
			else
			{
				v = dp[(i - 1) * m->w + j];
				if (dp[i * m->w + j - 1] < v)
					v = dp[i * m->w + j - 1];
				if (dp[(i - 1) * m->w + j - 1] < v)
					v = dp[(i - 1) * m->w + j - 1];
				v++;
			}
			dp[i * m->w + j] = v;
			if (v > best)
			{
				best = v;
				bi = i - v + 1;
				bj = j - v + 1;
			}
		}
	free(dp);
	for (int i = bi; i < bi + best; i++)
		for (int j = bj; j < bj + best; j++)
			m->g[i][j] = m->full;
}

static void	bsq(FILE *f)
{
	t_map	m;

	if (!f || load(f, &m) == -1)
	{
		fputs("map error\n", stderr);
		return ;
	}
	solve(&m);
	for (int i = 0; i < m.n; i++)
	{
		fputs(m.g[i], stdout);
		fputs("\n", stdout);
	}
	free_map(&m);
}

int	main(int ac, char **av)
{
	FILE	*f;

	if (ac == 1)
		bsq(stdin);
	for (int i = 1; i < ac; i++)
	{
		f = fopen(av[i], "r");
		bsq(f);
		if (f)
			fclose(f);
		if (i < ac - 1)
			fputs("\n", stdout);					/* ligne vide entre 2 maps */
	}
	return (0);
}
