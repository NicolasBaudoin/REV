# Exam 05 — Cheat-sheet mémo (versions condensées, testées)

Ordre conseillé : **vect2 → bigint → polyset → life → bsq**
(level1 = C++98 / classes et opérateurs ; level2 = C / algo sur grille).

Compilation type : `c++ -Wall -Wextra -Werror -std=c++98 *.cpp` et `cc -Wall -Wextra -Werror *.c`.
Tester : `./test.sh <exo>` à la racine.

---

## Le patron commun du level1 : forme canonique + opérateurs

```
Forme canonique (OCF) : constructeur par défaut, constructeur de copie, operator=, destructeur.

Écrire D'ABORD les op= (+=, -=, *=) qui modifient *this et renvoient *this (une référence),
PUIS dériver les autres :  T operator+(const T &o) const { T r(*this); return (r += o); }

++x  : T &operator++()      { ...; return (*this); }            -> renvoie une RÉFÉRENCE
x++  : T  operator++(int)   { T old(*this); ++(*this); return (old); }   -> renvoie une COPIE

Comparaisons : écrire < et ==, dériver le reste :
  a > b  = b < a      a <= b = !(b < a)      a >= b = !(a < b)      a != b = !(a == b)

operator<< est HORS de la classe et écrit dans os (jamais std::cout) :
  std::ostream &operator<<(std::ostream &os, const T &x) { return (os << ...); }

`const` partout où on ne modifie pas : méthodes de lecture, paramètres par référence.
```

---

## vect2  (level1) — vecteur 2D d'int

**Mnémo** : *2 `operator[]` (un `const` qui renvoie `int`, un normal qui renvoie `int &`) ;
`3 * v` = fonction **libre** `operator*(int, const vect2 &)` ; `-v` = `operator-()` sans paramètre.*

```cpp
#ifndef VECT2_HPP
#define VECT2_HPP

#include <iostream>

class vect2
{
	private:
		int	_v[2];

	public:
		vect2();
		vect2(int x, int y);
		vect2(const vect2 &o);
		vect2 &operator=(const vect2 &o);
		~vect2();

		int		operator[](int i) const;	// lecture sur un const vect2
		int		&operator[](int i);			// ecriture : v[1] = 12

		vect2	&operator+=(const vect2 &o);
		vect2	&operator-=(const vect2 &o);
		vect2	&operator*=(int k);
		vect2	operator+(const vect2 &o) const;
		vect2	operator-(const vect2 &o) const;
		vect2	operator*(int k) const;		// v * 2
		vect2	operator-() const;			// -v

		vect2	&operator++();				// ++v
		vect2	operator++(int);			// v++
		vect2	&operator--();
		vect2	operator--(int);

		bool	operator==(const vect2 &o) const;
		bool	operator!=(const vect2 &o) const;
};

vect2			operator*(int k, const vect2 &v);	// 3 * v (hors classe !)
std::ostream	&operator<<(std::ostream &os, const vect2 &v);

#endif
```

```cpp
#include "vect2.hpp"

vect2::vect2() { _v[0] = 0; _v[1] = 0; }
vect2::vect2(int x, int y) { _v[0] = x; _v[1] = y; }
vect2::vect2(const vect2 &o) { *this = o; }
vect2::~vect2() {}

vect2	&vect2::operator=(const vect2 &o)
{
	_v[0] = o._v[0];
	_v[1] = o._v[1];
	return (*this);
}

int		vect2::operator[](int i) const { return (_v[i]); }
int		&vect2::operator[](int i) { return (_v[i]); }

/* on ecrit les "op=" et on derive les autres a partir d'eux */
vect2	&vect2::operator+=(const vect2 &o) { _v[0] += o._v[0]; _v[1] += o._v[1]; return (*this); }
vect2	&vect2::operator-=(const vect2 &o) { _v[0] -= o._v[0]; _v[1] -= o._v[1]; return (*this); }
vect2	&vect2::operator*=(int k) { _v[0] *= k; _v[1] *= k; return (*this); }

vect2	vect2::operator+(const vect2 &o) const { vect2 r(*this); return (r += o); }
vect2	vect2::operator-(const vect2 &o) const { vect2 r(*this); return (r -= o); }
vect2	vect2::operator*(int k) const { vect2 r(*this); return (r *= k); }
vect2	vect2::operator-() const { return (vect2(-_v[0], -_v[1])); }

vect2	&vect2::operator++() { return (*this += vect2(1, 1)); }
vect2	&vect2::operator--() { return (*this -= vect2(1, 1)); }
vect2	vect2::operator++(int) { vect2 old(*this); ++(*this); return (old); }
vect2	vect2::operator--(int) { vect2 old(*this); --(*this); return (old); }

bool	vect2::operator==(const vect2 &o) const { return (_v[0] == o._v[0] && _v[1] == o._v[1]); }
bool	vect2::operator!=(const vect2 &o) const { return (!(*this == o)); }

vect2	operator*(int k, const vect2 &v) { return (v * k); }

std::ostream	&operator<<(std::ostream &os, const vect2 &v)
{
	return (os << "{" << v[0] << ", " << v[1] << "}");	/* os, PAS std::cout */
}
```

Pièges : `v2 += v2 += v3` marche seulement si `+=` renvoie une **référence** ; `operator<<` doit écrire
dans `os` (la réponse du dépôt source écrivait dans `std::cout` ✗ — invisible avec le main du sujet,
visible avec un `ostringstream`, voir `edge_test.cpp`).

---

## bigint  (level1) — entier positif arbitrairement grand

**Mnémo** : *une `std::string` de chiffres, poids fort en premier ("1337").
`+` : de droite à gauche avec retenue, `insert` au début. `<< n` : ajouter n `'0'` (sauf si on vaut 0).
`>> n` : `erase` les n derniers (tout → "0"). `<` : plus court = plus petit, sinon ordre de la string.*

```cpp
#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <string>
#include <iostream>

/*
** On stocke le nombre en string de chiffres, poids fort en premier : 1337 -> "1337".
** Invariant : jamais de zero de tete (sauf "0" lui-meme).
*/
class bigint
{
	private:
		std::string	_d;
		unsigned int	to_uint() const;

	public:
		bigint();
		bigint(unsigned int n);
		bigint(const bigint &o);
		bigint &operator=(const bigint &o);
		~bigint();

		const std::string	&str() const;

		bigint	operator+(const bigint &o) const;
		bigint	&operator+=(const bigint &o);
		bigint	&operator++();			// ++b
		bigint	operator++(int);		// b++

		bigint	operator<<(unsigned int n) const;
		bigint	operator>>(unsigned int n) const;
		bigint	&operator<<=(unsigned int n);
		bigint	&operator>>=(unsigned int n);
		bigint	operator<<(const bigint &n) const;
		bigint	operator>>(const bigint &n) const;
		bigint	&operator<<=(const bigint &n);
		bigint	&operator>>=(const bigint &n);

		bool	operator==(const bigint &o) const;
		bool	operator!=(const bigint &o) const;
		bool	operator<(const bigint &o) const;
		bool	operator>(const bigint &o) const;
		bool	operator<=(const bigint &o) const;
		bool	operator>=(const bigint &o) const;
};

std::ostream	&operator<<(std::ostream &os, const bigint &b);

#endif
```

```cpp
#include "bigint.hpp"

bigint::bigint() : _d("0") {}

bigint::bigint(unsigned int n)
{
	if (n == 0)
		_d = "0";
	while (n > 0)
	{
		_d.insert(_d.begin(), char('0' + n % 10));
		n /= 10;
	}
}

bigint::bigint(const bigint &o) : _d(o._d) {}

bigint	&bigint::operator=(const bigint &o)
{
	if (this != &o)
		_d = o._d;
	return (*this);
}

bigint::~bigint() {}

const std::string	&bigint::str() const { return (_d); }

unsigned int	bigint::to_uint() const
{
	unsigned int	n = 0;

	for (size_t i = 0; i < _d.size(); i++)
		n = n * 10 + (_d[i] - '0');
	return (n);
}

/* addition "a la main" : de droite a gauche, avec retenue */
bigint	bigint::operator+(const bigint &o) const
{
	bigint	res;
	int		i = _d.size() - 1;
	int		j = o._d.size() - 1;
	int		carry = 0;
	int		sum;

	res._d = "";
	while (i >= 0 || j >= 0 || carry)
	{
		sum = carry;
		if (i >= 0)
			sum += _d[i--] - '0';
		if (j >= 0)
			sum += o._d[j--] - '0';
		res._d.insert(res._d.begin(), char('0' + sum % 10));
		carry = sum / 10;
	}
	return (res);
}

bigint	&bigint::operator+=(const bigint &o) { return (*this = *this + o); }
bigint	&bigint::operator++() { return (*this += bigint(1)); }

bigint	bigint::operator++(int)
{
	bigint	old(*this);

	++(*this);
	return (old);
}

/* << n : ajoute n zeros a droite (sauf si on vaut 0 -> pas de zeros de tete !) */
bigint	bigint::operator<<(unsigned int n) const
{
	bigint	res(*this);

	if (_d != "0")
		res._d.append(n, '0');
	return (res);
}

/* >> n : enleve les n derniers chiffres (tout -> "0") */
bigint	bigint::operator>>(unsigned int n) const
{
	bigint	res(*this);

	if (n >= _d.size())
		res._d = "0";
	else
		res._d.erase(_d.size() - n);
	return (res);
}

bigint	&bigint::operator<<=(unsigned int n) { return (*this = *this << n); }
bigint	&bigint::operator>>=(unsigned int n) { return (*this = *this >> n); }
bigint	bigint::operator<<(const bigint &n) const { return (*this << n.to_uint()); }
bigint	bigint::operator>>(const bigint &n) const { return (*this >> n.to_uint()); }
bigint	&bigint::operator<<=(const bigint &n) { return (*this = *this << n.to_uint()); }
bigint	&bigint::operator>>=(const bigint &n) { return (*this = *this >> n.to_uint()); }

/* comparaison : d'abord la longueur, puis ordre alphabetique (meme longueur) */
bool	bigint::operator<(const bigint &o) const
{
	if (_d.size() != o._d.size())
		return (_d.size() < o._d.size());
	return (_d < o._d);
}

bool	bigint::operator==(const bigint &o) const { return (_d == o._d); }
bool	bigint::operator!=(const bigint &o) const { return (!(*this == o)); }
bool	bigint::operator>(const bigint &o) const { return (o < *this); }
bool	bigint::operator<=(const bigint &o) const { return (!(o < *this)); }
bool	bigint::operator>=(const bigint &o) const { return (!(*this < o)); }

std::ostream	&operator<<(std::ostream &os, const bigint &b)
{
	return (os << b.str());
}
```

Pièges (les 3 étaient dans la réponse du dépôt source, invisibles avec le main du sujet → `edge_test.cpp`) :
- `a > b` codé `!(a < b)` → **vrai quand a == b** ✗. Écrire `b < a`.
- retenue pas remise à 0 quand la somme ≤ 9 → `UINT_MAX + UINT_MAX` faux ✗. Avec `carry = sum / 10` à chaque tour, impossible.
- `0 << 3` → `"0000"` ✗ (zéros de tête interdits).
- `(b << 10) + 42` : `42` devient un `bigint` grâce au constructeur `bigint(unsigned int)` **non explicit**.
- `d >>= (const bigint)2` → il faut aussi les versions des shifts qui prennent un `bigint`.

---

## polyset  (level1) — sac cherchable + ensemble

**But** : `searchable_array_bag` et `searchable_tree_bag` = un sac existant (`array_bag`, `tree_bag`)
**+** l'interface `searchable_bag` (méthode `bool has(int) const`). Puis `set` qui enveloppe un
`searchable_bag` et refuse les doublons.

**Mnémo** : *« hérite des deux, implémente `has` » :
`class searchable_array_bag : public array_bag, public searchable_bag`.
Le losange (`bag` en haut) est déjà réglé : les fichiers fournis héritent de `bag` en **virtual**.
`has` : boucle sur `data[0..size-1]` (array) / descente gauche-droite dans l'ABR (tree).
`set` garde une **référence** sur le sac ; `insert` = `if (!has(x)) bag.insert(x)`.*

```cpp
#pragma once

#include "array_bag.hpp"
#include "searchable_bag.hpp"

class searchable_array_bag : public array_bag, public searchable_bag
{
	public:
		searchable_array_bag();
		searchable_array_bag(const searchable_array_bag &o);
		searchable_array_bag &operator=(const searchable_array_bag &o);
		~searchable_array_bag();

		bool	has(int value) const;
};
```

```cpp
#include "searchable_array_bag.hpp"

searchable_array_bag::searchable_array_bag() : array_bag() {}
searchable_array_bag::searchable_array_bag(const searchable_array_bag &o) : array_bag(o) {}
searchable_array_bag::~searchable_array_bag() {}

searchable_array_bag	&searchable_array_bag::operator=(const searchable_array_bag &o)
{
	array_bag::operator=(o);
	return (*this);
}

/* recherche lineaire dans data[0..size-1] */
bool	searchable_array_bag::has(int value) const
{
	for (int i = 0; i < size; i++)
		if (data[i] == value)
			return (true);
	return (false);
}
```

```cpp
#pragma once

#include "tree_bag.hpp"
#include "searchable_bag.hpp"

class searchable_tree_bag : public tree_bag, public searchable_bag
{
	public:
		searchable_tree_bag();
		searchable_tree_bag(const searchable_tree_bag &o);
		searchable_tree_bag &operator=(const searchable_tree_bag &o);
		~searchable_tree_bag();

		bool	has(int value) const;
};
```

```cpp
#include "searchable_tree_bag.hpp"

searchable_tree_bag::searchable_tree_bag() : tree_bag() {}
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag &o) : tree_bag(o) {}
searchable_tree_bag::~searchable_tree_bag() {}

searchable_tree_bag	&searchable_tree_bag::operator=(const searchable_tree_bag &o)
{
	tree_bag::operator=(o);
	return (*this);
}

/* recherche dans un ABR : plus petit -> gauche, plus grand -> droite */
bool	searchable_tree_bag::has(int value) const
{
	node	*cur = tree;

	while (cur)
	{
		if (value == cur->value)
			return (true);
		cur = (value < cur->value) ? cur->l : cur->r;
	}
	return (false);
}
```

```cpp
#pragma once

#include "searchable_bag.hpp"

/*
** set = un "wrapper" autour d'un searchable_bag existant (on garde une REFERENCE,
** on ne copie pas le sac) : insert n'ajoute que si !has().
*/
class set
{
	private:
		searchable_bag	&_bag;
		set();								// pas de set sans sac

	public:
		set(searchable_bag &bag);
		set(const set &o);					// partage le meme sac
		set &operator=(const set &o);
		~set();

		void	insert(int value);
		void	insert(int *values, int count);
		bool	has(int value) const;
		void	print() const;
		void	clear();

		searchable_bag			&get_bag();
		const searchable_bag	&get_bag() const;
};
```

```cpp
#include "set.hpp"

set::set(searchable_bag &bag) : _bag(bag) {}
set::set(const set &o) : _bag(o._bag) {}
set::~set() {}

/* une reference ne peut pas etre "re-pointee" : on garde notre sac */
set	&set::operator=(const set &o)
{
	(void)o;
	return (*this);
}

void	set::insert(int value)
{
	if (!_bag.has(value))
		_bag.insert(value);
}

void	set::insert(int *values, int count)
{
	for (int i = 0; i < count; i++)
		insert(values[i]);
}

bool	set::has(int value) const { return (_bag.has(value)); }
void	set::print() const { _bag.print(); }
void	set::clear() { _bag.clear(); }

searchable_bag			&set::get_bag() { return (_bag); }
const searchable_bag	&set::get_bag() const { return (_bag); }
```

Pièges :
- `has` doit être `const` (le main l'appelle sur un `const searchable_array_bag`).
- `get_bag()` : le main fait `sa.get_bag().print()` → renvoyer une référence (version const + non-const).
- le main du sujet n'inclut que `searchable_bag.hpp` ; s'il ne compile pas, ajouter en tête
  les 3 includes (`searchable_array_bag.hpp`, `searchable_tree_bag.hpp`, `set.hpp`) — c'est ce que fait `polyset/main.cpp`.
- `tree_bag::print` n'affiche pas les `0` : c'est le code **fourni**, pas ton bug.

---

## life  (level2) — jeu de la vie

**Mnémo** : *1) dessiner : stylo en (0,0) levé, `wasd` bouge sans sortir du plateau, `x` lève/baisse,
stylo baissé → case vivante (y compris là où on vient de le baisser). 2) N fois : compter les 8 voisins
(hors plateau = mort), vivante survit à 2 ou 3, morte naît à 3 — dans un **2e tableau**, puis échanger.
3) afficher `O` / espace.*

```c
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
```

Pièges : calculer la génération suivante **dans un autre tableau** (sinon les cases déjà mises à jour
faussent les voisins) ; `read` caractère par caractère sur stdin jusqu'à EOF ; le `\n` final de `echo`
ne doit rien casser.

---

## bsq  (level2) — plus grand carré

**Mnémo** : *`dp[i][j]` = côté du plus grand carré dont le coin **bas-droit** est (i, j) :
0 si obstacle, 1 sur la 1re ligne/colonne, sinon `1 + min(haut, gauche, diagonale)`.
Parcours ligne par ligne avec un max **strict** (`>`) → en cas d'égalité, le plus haut puis le plus à gauche.*

```c
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
```

Validation de la map (sinon `map error\n` sur **stderr**, puis map suivante) :
- 1re ligne : `fscanf("%d %c %c %c")` → 4 valeurs, n > 0, 3 caractères distincts et imprimables, puis exactement `\n`.
- n lignes, chacune finie par `\n`, toutes de même longueur ≥ 1, uniquement « vide » / « obstacle ».
- rien après la n-ième ligne.

Pièges : sans argument → lire **stdin** ; plusieurs fichiers → une ligne vide entre deux résultats ;
`getline` avec `line = NULL` à chaque ligne pour garder chaque buffer.

---

## Récap des pièges (2 min avant de rendre)

| Exo | Piège n°1 |
|-----|-----------|
| vect2 | 2 `operator[]` ; `3 * v` hors classe ; `<<` écrit dans `os` ; `op=` renvoient une référence |
| bigint | `>` = `b < a` ; retenue recalculée à chaque tour ; `0 << n` reste `0` ; shifts aussi par `bigint` |
| polyset | `public array_bag, public searchable_bag` ; `has` const ; `set` = référence + `if (!has)` |
| life | 2e tableau pour la génération suivante ; stylo baissé → case vivante tout de suite |
| bsq | `1 + min(3 voisins)` ; max strict ; `map error` sur stderr ; stdin si pas d'argument |
