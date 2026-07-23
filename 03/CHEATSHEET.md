# Exam 03 — Cheat-sheet mémo (versions condensées, testées)

Ordre conseillé par difficulté : **filter → permutations → powerset → n_queens → rip → broken_gnl**.

Compilation type : `cc -Wall -Wextra -Werror *.c` (pour tsp : `-lm`).

---

## filter  (level1) — lit **stdin**, pas argv !

**But** : `./filter s` = `sed 's/s/***/g'`. Remplace chaque occurrence de `s` par autant de `*`. Buffers de taille aléatoire → **on lit TOUT stdin** puis on scanne (évite les matchs à cheval sur 2 reads).

**Erreurs** : 0 arg / arg vide / plusieurs args → `return 1`. Erreur read/malloc → `perror("Error")` + `return 1`.

**Mnémo** : *lis tout dans un buffer qui grandit (realloc), puis à chaque i : si `memmem` matche → sl étoiles + saute sl, sinon écris 1 char.*

```c
#define _GNU_SOURCE
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int	main(int ac, char **av)
{
	if (ac != 2 || !av[1][0])
		return (1);
	char	*s = av[1];
	size_t	sl = strlen(s), cap = 0, len = 0, i = 0, k;
	char	*buf = NULL, tmp[4096];
	ssize_t	n;

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
	return (free(buf), 0);
}
```

---

## permutations  (level2) — ordre **alphabétique**

**But** : imprimer toutes les permutations d'une string, **en ordre alpha**. Pas de doublons dans l'entrée.

**Piège #1** : le swap récursif classique NE donne PAS l'ordre alpha → utiliser **`next_permutation`** (trier d'abord).

**Mnémo `next_permutation`** : *depuis la fin, trouve le 1er `i` avec `s[i] < s[i+1]` ; trouve `j` (fin) avec `s[j] > s[i]` ; swap(i,j) ; reverse la queue `i+1..fin`.*

```c
#include <unistd.h>

int	ft_len(char *s){int i=0;while(s[i])i++;return (i);}
void	swap(char *a,char *b){char t=*a;*a=*b;*b=t;}

int	main(int ac, char **av)
{
	if (ac != 2)
		return (0);
	char	*s = av[1];
	int		n = ft_len(s), i, j;

	i = 0;                                   // tri à bulles
	while (i < n){ j = i + 1; while (j < n){ if (s[i] > s[j]) swap(&s[i], &s[j]); j++; } i++; }
	while (1)
	{
		write(1, s, n);
		write(1, "\n", 1);
		i = n - 2;
		while (i >= 0 && s[i] >= s[i + 1]) i--;
		if (i < 0) return (0);
		j = n - 1;
		while (s[j] <= s[i]) j--;
		swap(&s[i], &s[j]);
		i++; j = n - 1;
		while (i < j) swap(&s[i++], &s[j--]);
	}
}
```

---

## powerset  (level2) — sous-ensembles dont la somme = n

**But** : `./powerset n a b c ...` → tous les sous-ensembles de `{a,b,c...}` dont la somme vaut `n`. **Ordre des éléments = ordre du set** (pas de doublons '1 2'/'2 1'). Sous-ensemble vide = ligne vide.

**Mnémo** : *DFS include/exclude sur chaque index. `in[i]=0` d'abord puis `in[i]=1` (exclude-first) → reproduit l'ordre du sujet. À la feuille : si `sum==target`, imprime les `in[k]`.*

```c
#include <stdio.h>
#include <stdlib.h>

int	*g_set, *g_in, g_n, g_target;

void	print_set(void)
{
	int	first = 1, k = 0;
	while (k < g_n)
	{
		if (g_in[k]) { if (!first) printf(" "); printf("%d", g_set[k]); first = 0; }
		k++;
	}
	printf("\n");
}

void	rec(int i, int sum)
{
	if (i == g_n) { if (sum == g_target) print_set(); return ; }
	g_in[i] = 0; rec(i + 1, sum);
	g_in[i] = 1; rec(i + 1, sum + g_set[i]);
}

int	main(int ac, char **av)
{
	int	i = 0;
	if (ac < 2) return (1);
	g_n = ac - 2;
	g_target = atoi(av[1]);
	g_set = malloc(sizeof(int) * (g_n + 1));
	g_in = calloc(g_n + 1, sizeof(int));
	if (!g_set || !g_in) return (1);
	while (i < g_n) { g_set[i] = atoi(av[i + 2]); i++; }
	rec(0, 0);
	return (free(g_set), free(g_in), 0);
}
```

---

## n_queens  (level2) — une ligne par solution

**But** : imprimer toutes les solutions ; format `<row_col0> <row_col1> ... \n` (indice de ligne de la reine dans chaque colonne). Ordre libre.

**Mnémo conflit** : *même ligne `q[c]==row` ; même diagonale `q[c]-c == row-col` (↘) ou `q[c]+c == row+col` (↙).* On place colonne par colonne, ligne 0→n-1.

⚠️ `putnbr` récursif car pour n ≥ 10 les indices dépassent 9 (multi-chiffres).

```c
#include <stdlib.h>
#include <unistd.h>

int	g_n, *g_q;

void	putnbr(int x){ if (x >= 10) putnbr(x / 10); char c = x % 10 + '0'; write(1, &c, 1); }

int	ok(int col, int row)
{
	int	c = 0;
	while (c < col)
	{
		if (g_q[c] == row || g_q[c] - c == row - col || g_q[c] + c == row + col)
			return (0);
		c++;
	}
	return (1);
}

void	solve(int col)
{
	int	row = 0;
	if (col == g_n)
	{
		while (row < g_n) { putnbr(g_q[row]); if (row < g_n - 1) write(1, " ", 1); row++; }
		write(1, "\n", 1);
		return ;
	}
	while (row < g_n) { if (ok(col, row)) { g_q[col] = row; solve(col + 1); } row++; }
}

int	main(int ac, char **av)
{
	if (ac != 2) return (1);
	g_n = atoi(av[1]);
	g_q = malloc(sizeof(int) * (g_n + 1));
	if (!g_q) return (1);
	solve(0);
	return (free(g_q), 0);
}
```

---

## rip  (level2) — remplacer les parenthèses par des **espaces**

**But** : enlever le **minimum** de parenthèses pour équilibrer, en **remplaçant par des espaces** (la longueur est conservée). Imprimer **toutes** les solutions distinctes. Allowed : `puts, write` seulement (pas de malloc → on modifie `av[1]` en place + backtracking).

**⚠️ PAS de déduplication** : sur `(()` il faut les DEUX résultats `" ()"` et `"( )"` (issus des `((` consécutifs). Donc on garde toutes les positions distinctes.
Note : le sujet montre parfois des exemples **non-exhaustifs** (ex. `(()(()(` : le sujet liste 4 solutions mais il y en a 5 valides, dont `" () () "`). Ton code sort les 5 → c'est correct.

**Mnémo** :
1. Compter combien enlever : `l` = `(` non fermées, `r` = `)` en trop.
2. `dfs(s, start, l, r)` : quand `l==0 && r==0` → si `valid` alors `puts`. Sinon, à chaque i depuis `start` : remplace un `(` (si l>0) ou `)` (si r>0) par espace, recurse `i+1`, restaure. Le `start` croissant garantit l'unicité (aucun doublon).

```c
#include <stdio.h>

int	valid(char *s)
{
	int	c = 0, i = 0;
	while (s[i])
	{
		if (s[i] == '(') c++;
		else if (s[i] == ')') { c--; if (c < 0) return (0); }
		i++;
	}
	return (c == 0);
}

void	dfs(char *s, int start, int l, int r)
{
	int	i = start;
	if (l == 0 && r == 0) { if (valid(s)) puts(s); return ; }
	while (s[i])
	{
		if (s[i] == '(' && l > 0) { s[i] = ' '; dfs(s, i + 1, l - 1, r); s[i] = '('; }
		else if (s[i] == ')' && r > 0) { s[i] = ' '; dfs(s, i + 1, l, r - 1); s[i] = ')'; }
		i++;
	}
}

int	main(int ac, char **av)
{
	int	l = 0, r = 0, i = 0;
	if (ac != 2) return (1);
	while (av[1][i])
	{
		if (av[1][i] == '(') l++;
		else if (av[1][i] == ')') { if (l > 0) l--; else r++; }
		i++;
	}
	dfs(av[1], 0, l, r);
	return (0);
}
```

---

## broken_gnl  (level1) — RÉPARER les 5 bugs du fichier fourni

**Méthode mémoire** : on ne réécrit PAS. Le sujet donne un `get_next_line.c` cassé avec toujours **les mêmes 5 bugs**. On les repère et on les corrige. Beaucoup plus facile à retenir.

### Les 5 bugs à repérer

**Bug 1 — `ft_strchr` : boucle infinie si `c` absent** (pas de test `\0`).
```c
while(s[i] != c) i++;          // BUG : dépasse le '\0'
// CORRIGÉ :
while (s[i] && s[i] != (char)c) i++;
if (s[i] == (char)c) return (s + i);
return (NULL);
```

**Bug 2 — `ft_memcpy` : commence à `n-1` et copie à l'envers** (`while(--n > 0)`), fragile / off-by-one.
```c
// CORRIGÉ : boucle simple avant->arrière
size_t i = 0;
while (i < n) { ((char *)dest)[i] = ((char *)src)[i]; i++; }
return (dest);
```

**Bug 3 — `ft_memmove` : récursion infinie** (`if (dest > src) return ft_memmove(dest, src, n);`).
```c
// CORRIGÉ :
if (dest == src || n == 0) return (dest);
if (dest > src) { while (n--) ((char *)dest)[n] = ((char *)src)[n]; }  // arrière
else ft_memcpy(dest, src, n);                                          // avant
return (dest);
```

**Bug 4 — `get_next_line` : `tmp` jamais recalculé + EOF (`read==0`) non géré → boucle infinie.**
```c
// dans la boucle while(!tmp), après le read :
b[read_ret] = '\0';
if (read_ret == 0) { if (!ret[0]) { free(ret); return (NULL); } return (ret); }
tmp = ft_strchr(b, '\n');      // <- la mise à jour qui manquait
```

**Bug 5 — le buffer statique n'est pas décalé après extraction** (le reste après `\n` est perdu / relu).
```c
// juste avant le return final :
ft_memmove(b, tmp + 1, ft_strlen(tmp + 1) + 1);
return (ret);
```

Astuce anti-crash liée : au tout début, `ret = malloc(1); ret[0] = '\0';` pour que `ft_strlen(*s1)` dans `str_append_mem` ne parte pas sur un `NULL`.

### Fichier corrigé complet (à recopier)

```c
#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
    int i = 0;
    while (s[i] && s[i] != (char)c)
        i++;
    if (s[i] == (char)c)
        return s + i;
    return NULL;
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
    size_t i = 0;
    while (i < n)
    {
        ((char *)dest)[i] = ((char *)src)[i];
        i++;
    }
    return dest;
}

size_t ft_strlen(char *s)
{
    size_t res = 0;
    while (s[res])
        res++;
    return res;
}

int str_append_mem(char **s1, char *s2, size_t size2)
{
    size_t size1 = ft_strlen(*s1);
    char *tmp = malloc(size2 + size1 + 1);
    if (!tmp)
        return 0;
    ft_memcpy(tmp, *s1, size1);
    ft_memcpy(tmp + size1, s2, size2);
    tmp[size1 + size2] = '\0';
    free(*s1);
    *s1 = tmp;
    return 1;
}

int str_append_str(char **s1, char *s2)
{
    return str_append_mem(s1, s2, ft_strlen(s2));
}

void *ft_memmove(void *dest, const void *src, size_t n)
{
    if (dest == src || n == 0)
        return dest;
    if (dest > src)
    {
        while (n--)
            ((char *)dest)[n] = ((char *)src)[n];
    }
    else
        ft_memcpy(dest, src, n);
    return dest;
}

char *get_next_line(int fd)
{
    static char b[BUFFER_SIZE + 1] = "";
    char *ret = NULL;
    char *tmp = ft_strchr(b, '\n');

    ret = malloc(1);
    if (!ret)
        return NULL;
    ret[0] = '\0';

    while (!tmp)
    {
        if (!str_append_str(&ret, b))
            return NULL;
        b[0] = '\0';
        int read_ret = read(fd, b, BUFFER_SIZE);
        if (read_ret == -1)
        {
            free(ret);
            return NULL;
        }
        b[read_ret] = '\0';
        if (read_ret == 0)
        {
            if (!ret[0])
            {
                free(ret);
                return NULL;
            }
            return ret;
        }
        tmp = ft_strchr(b, '\n');
    }
    if (!str_append_mem(&ret, b, tmp - b + 1))
    {
        free(ret);
        return NULL;
    }
    ft_memmove(b, tmp + 1, ft_strlen(tmp + 1) + 1);
    return ret;
}
```

---

## Récap des pièges (à relire 2 min avant de rendre)

| Exo | Piège n°1 |
|-----|-----------|
| filter | lit **stdin** (pas argv) ; `perror("Error")` + `return 1` sur erreur |
| permutations | trier + `next_permutation` (le swap récursif casse l'ordre alpha) |
| powerset | ordre éléments = ordre du set ; `exclude-first` ; sous-ensemble vide = ligne vide |
| n_queens | `putnbr` récursif (n ≥ 10) ; diagonales `q[c]±c` |
| rip | remplacer par **espaces** (garde la longueur) ; **pas** de dédup ; sujet exemples non-exhaustifs |
| broken_gnl | 5 bugs : strchr sans `\0` / memcpy à l'envers / memmove récursif / `tmp` pas recalculé + EOF / buffer pas décalé |
