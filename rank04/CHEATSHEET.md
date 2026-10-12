# Exam 04 — Cheat-sheet mémo (versions condensées, testées)

Ordre conseillé : **ft_popen → picoshell → sandbox → vbc → argo**
(les 3 premiers partagent le même patron `pipe / fork / dup2 / exec / wait`).

Compilation type : `cc -Wall -Wextra -Werror fichier.c`. Tester : `./test.sh <exo>` à la racine.

---

## Le patron commun du niveau 1 (à savoir par cœur)

```
pipe(fd)        fd[0] = bout LECTURE, fd[1] = bout ÉCRITURE      (0 = lire comme stdin, 1 = écrire comme stdout)
fork()          -1 erreur | 0 = je suis l'enfant | >0 = je suis le parent (pid de l'enfant)
dup2(old, new)  "new devient une copie de old"  -> dup2(fd[1], 1) : mon stdout va dans le pipe
execvp(f, av)   ne revient QUE si erreur -> toujours suivi de exit(1)
```

**Règle d'or des fd** : après un `dup2`, l'original ne sert plus → `close`. Chaque processus
ferme **tous** les bouts qu'il n'utilise pas. Un bout d'écriture oublié ouvert = le lecteur
n'aura jamais EOF = programme bloqué.

---

## ft_popen  (level1)

**But** : lancer `file argv` et renvoyer UN fd : sa sortie (`'r'`) ou son entrée (`'w'`). Erreur → `-1`.

**Mnémo** : *« r : je lis ce que l'enfant écrit » → l'enfant branche `fd[1]` sur stdout, je garde `fd[0]`.
« w : j'écris ce que l'enfant lit » → l'enfant branche `fd[0]` sur stdin, je garde `fd[1]`.*

```c
#include <unistd.h>
#include <stdlib.h>

/*
** 'r' : je LIS ce que l'enfant ECRIT  -> enfant: stdout = fd[1] ; je garde fd[0]
** 'w' : j'ECRIS ce que l'enfant LIT   -> enfant: stdin  = fd[0] ; je garde fd[1]
*/
int	ft_popen(const char *file, char *const argv[], char type)
{
	int		fd[2];
	pid_t	pid;

	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (close(fd[0]), close(fd[1]), -1);
	if (pid == 0)
	{
		if (type == 'r' && dup2(fd[1], STDOUT_FILENO) == -1)
			exit(1);
		if (type == 'w' && dup2(fd[0], STDIN_FILENO) == -1)
			exit(1);
		close(fd[0]);
		close(fd[1]);
		execvp(file, argv);
		exit(1);
	}
	if (type == 'r')
		return (close(fd[1]), fd[0]);
	return (close(fd[0]), fd[1]);
}
```

Pièges : vérifier les paramètres **avant** `pipe` (sinon fuite de fd) ; si `fork` échoue, fermer les 2 bouts ;
dans l'enfant, fermer les 2 bouts **après** le `dup2` ; `exit(1)` après `execvp`.

---

## picoshell  (level1)

**But** : exécuter `cmd1 | cmd2 | ... | cmdN`, attendre tous les enfants, renvoyer 0 (ou 1 si erreur).

**Mnémo** : *une variable `in` = bout lecture du pipe précédent (-1 au début).
Pour chaque cmd : `pipe` si il y a une suivante → `fork` →
enfant : `in`→stdin, `fd[1]`→stdout, ferme tout, exec →
parent : ferme `in` et `fd[1]`, `in = fd[0]`. À la fin seulement : `wait` en boucle.*

```c
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
** in = bout lecture du pipe PRECEDENT (-1 pour la 1re commande)
** Pour chaque cmd : pipe() si cmd suivante, fork,
**   enfant : stdin <- in ; stdout -> fd[1] (si suivante) ; ferme tout ; execvp
**   parent : ferme in et fd[1], garde fd[0] comme nouveau in
** A la fin : wait() TOUS les enfants ; 1 si erreur ou si un enfant exit != 0
** (ex: execvp rate -> exit(1)). Un enfant tue par SIGPIPE (yes | head) n'est pas une erreur.
*/
int	picoshell(char **cmds[])
{
	int		fd[2];
	int		in = -1;
	int		ret = 0;
	int		status;
	int		i = 0;
	pid_t	pid;

	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) == -1)
		{
			ret = 1;
			break ;
		}
		pid = fork();
		if (pid == -1)
		{
			if (cmds[i + 1])
			{
				close(fd[0]);
				close(fd[1]);
			}
			ret = 1;
			break ;
		}
		if (pid == 0)
		{
			if (in != -1 && (dup2(in, STDIN_FILENO) == -1 || close(in) == -1))
				exit(1);
			if (cmds[i + 1] && (close(fd[0]) == -1
					|| dup2(fd[1], STDOUT_FILENO) == -1 || close(fd[1]) == -1))
				exit(1);
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (in != -1)
			close(in);
		in = -1;
		if (cmds[i + 1])
		{
			close(fd[1]);
			in = fd[0];
		}
		i++;
	}
	if (in != -1)
		close(in);
	while (wait(&status) > 0)
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			ret = 1;
	return (ret);
}
```

Pièges :
- **ne pas `wait` dans la boucle** : si `cmd1` remplit le pipe (64 Ko) avant que `cmd2` existe → interblocage.
- oublier `close(fd[1])` dans le parent → le dernier lecteur attend un EOF qui ne vient jamais.
- en cas d'erreur, fermer `in` (et le pipe en cours) **puis quand même** attendre les enfants déjà lancés.
- statut : `execvp` raté → l'enfant fait `exit(1)` → on renvoie 1 (le `test_main.c` le vérifie).
  Un enfant tué par SIGPIPE (`yes | head`) n'est pas une erreur.

---

## sandbox  (level1)

**But** : lancer `f` dans un enfant ; renvoyer 1 si elle finit avec `exit(0)`, 0 si signal / code ≠ 0 /
timeout, -1 si erreur de **notre** fonction. Messages exacts (si `verbose`) :

```
Nice function!
Bad function: exited with code <code>
Bad function: <strsignal(sig)>
Bad function: timed out after <timeout> seconds
```

**Mnémo** : *« alarme qui interrompt l'attente » : handler SIGALRM vide **sans SA_RESTART** → fork →
`alarm(timeout)` → `waitpid` : s'il renvoie -1 avec `errno == EINTR`, c'est l'alarme → `kill(SIGKILL)` +
`waitpid` (pas de zombie). Sinon : `WIFEXITED` / `WIFSIGNALED`.*

```c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

/* handler vide : il sert juste a INTERROMPRE waitpid (EINTR) quand l'alarme sonne */
static void	on_alarm(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction	sa;
	pid_t				pid;
	int					st;

	sa.sa_handler = on_alarm;
	sa.sa_flags = 0;			/* PAS de SA_RESTART : sinon waitpid repart et attend a l'infini */
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		f();
		exit(0);
	}
	alarm(timeout);
	if (waitpid(pid, &st, 0) == -1)
	{
		if (errno != EINTR)
			return (-1);
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);	/* pas de zombie */
		if (verbose)
			printf("Bad function: timed out after %u seconds\n", timeout);
		return (0);
	}
	alarm(0);
	if (WIFEXITED(st) && WEXITSTATUS(st) == 0)
	{
		if (verbose)
			printf("Nice function!\n");
		return (1);
	}
	if (WIFEXITED(st))
	{
		if (verbose)
			printf("Bad function: exited with code %d\n", WEXITSTATUS(st));
		return (0);
	}
	if (WIFSIGNALED(st))
	{
		if (verbose)
			printf("Bad function: %s\n", strsignal(WTERMSIG(st)));
		return (0);
	}
	return (-1);
}
```

Pièges :
- `SA_RESTART` (ou `signal()`) → `waitpid` redémarre tout seul et le timeout ne marche jamais.
- `SIGKILL` et pas `SIGTERM` : une « very bad function » peut ignorer SIGTERM / SIGALRM.
- `alarm(0)` après un `waitpid` réussi, sinon l'alarme sonne plus tard chez l'appelant.
- `%u` pour `timeout` (unsigned). `strsignal(WTERMSIG(st))` donne « Segmentation fault », « Aborted »…
- Une fonction qui se **stoppe** (SIGSTOP) n'est pas vue par `waitpid` sans `WUNTRACED` → elle finit en timeout : c'est bien « bad ».

---

## vbc  (level2) — calculatrice `+ * ()` à 1 chiffre

**Mnémo — 1 fonction par niveau de priorité** (du plus faible au plus fort) :

```
add    = mul    { '+' mul }        <- boucle while (accept '+')
mul    = factor { '*' factor }     <- boucle while (accept '*')
factor = CHIFFRE | '(' add ')'     <- récursion : la parenthèse relance tout
```

À la fin : s'il reste un caractère (`'1+2)'`, `'12'`) → `unexpected(*s)`. Le reste du fichier
(`new_node`, `destroy_tree`, `unexpected`, `accept`, `expect`, `eval_tree`, `main`) est **fourni**.

```c
node    *parse_add(char **s);

/* cree un noeud ADD/MULTI ; si echec, libere les 2 fils */
node    *join(int type, node *l, node *r)
{
    node    *n;

    if (!l || !r)
        return (destroy_tree(l), destroy_tree(r), NULL);
    n = new_node((node){.type = type, .l = l, .r = r});
    if (!n)
        return (destroy_tree(l), destroy_tree(r), NULL);
    return (n);
}

node    *parse_factor(char **s)
{
    node    *e;

    if (isdigit(**s))
        return (new_node((node){.type = VAL, .val = *(*s)++ - '0'}));
    if (accept(s, '('))
    {
        e = parse_add(s);
        if (e && !expect(s, ')'))
            return (destroy_tree(e), NULL);
        return (e);
    }
    unexpected(**s);
    return (NULL);
}

node    *parse_mul(char **s)
{
    node    *l = parse_factor(s);

    while (l && accept(s, '*'))
        l = join(MULTI, l, parse_factor(s));
    return (l);
}

node    *parse_add(char **s)
{
    node    *l = parse_mul(s);

    while (l && accept(s, '+'))
        l = join(ADD, l, parse_mul(s));
    return (l);
}

node    *parse_expr(char *s)
{
    node    *ret = parse_add(&s);

    if (ret && *s)          /* il reste des chars : '1+2)' -> ')' */
    {
        unexpected(*s);
        destroy_tree(ret);
        return (NULL);
    }
    return (ret);
}
```

Pièges :
- le `given.c` du sujet **ne compile pas tel quel** : supprimer la ligne `node *n(**s);`,
  remplacer `<malloc.h>` par `<stdlib.h>`, ajouter `return (0);` à la fin de `eval_tree` (`-Werror`).
- le `parse_expr` fourni libère l'arbre s'il reste des caractères mais **n'affiche rien** : ajouter `unexpected(*s)`.
- message exact : `Unexpected end of input` (la réponse du dépôt source disait « end of file » ✗).
- `'((1+3)*12+(3*(2+6))'` → `Unexpected token '2'` : l'erreur vient du **premier** caractère inattendu,
  donc pas de « pré-vérification des parenthèses » qui afficherait autre chose.
- libérer l'arbre à chaque erreur (`join` libère les deux fils si l'un est NULL).

---

## argo  (level2) — parser JSON (int, string, map)

**Mnémo** : *`peek` regarde, `accept` consomme si c'est le bon char, `expect` = accept ou erreur.
`parse_value` aiguille sur `peek` : `"` → string, `{` → map, chiffre ou `-` → int, sinon `unexpected`.*
Fonctions fournies dans `argo.c` : `peek`, `unexpected`, `accept`, `expect`, `free_json`, `serialize`, `main`.

```c
int	parse_value(json *dst, FILE *s);

/* "abc" -> malloc'd "abc". Seuls \\ et \" sont des echappements : on garde le char suivant. */
int	parse_string(char **out, FILE *s)
{
	char	*str = calloc(1, 1);
	char	*tmp;
	size_t	len = 0;
	int		c;

	if (!str)
		return (-1);
	getc(s);							/* le " ouvrant */
	while ((c = getc(s)) != '"')
	{
		if (c == '\\')
			c = getc(s);
		if (c == EOF)
		{
			free(str);
			unexpected(s);				/* -> "unexpected end of input" */
			return (-1);
		}
		tmp = realloc(str, len + 2);
		if (!tmp)
			return (free(str), -1);
		str = tmp;
		str[len++] = c;
		str[len] = '\0';
	}
	*out = str;
	return (1);
}

int	parse_int(json *dst, FILE *s)
{
	int	n;

	if (fscanf(s, "%d", &n) != 1)
	{
		unexpected(s);
		return (-1);
	}
	dst->type = INTEGER;
	dst->integer = n;
	return (1);
}

/* { "k":v , "k":v } -- chaque pair ajoutee est TOUJOURS dans un etat liberable */
int	parse_map(json *dst, FILE *s)
{
	pair	*tmp;
	pair	*p;

	dst->type = MAP;
	dst->map.size = 0;
	dst->map.data = NULL;
	getc(s);							/* le { */
	if (accept(s, '}'))
		return (1);
	while (1)
	{
		if (peek(s) != '"')
			return (unexpected(s), -1);
		tmp = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
		if (!tmp)
			return (-1);
		dst->map.data = tmp;
		p = &dst->map.data[dst->map.size];
		if (parse_string(&p->key, s) == -1)
			return (-1);
		p->value.type = INTEGER;		/* valeur "vide" liberable */
		dst->map.size++;
		if (!expect(s, ':') || parse_value(&p->value, s) == -1)
			return (-1);
		if (accept(s, '}'))
			return (1);
		if (!expect(s, ','))
			return (-1);
	}
}

int	parse_value(json *dst, FILE *s)
{
	int	c = peek(s);

	if (c == '"')
	{
		if (parse_string(&dst->string, s) == -1)
			return (-1);
		dst->type = STRING;
		return (1);
	}
	if (c == '{')
		return (parse_map(dst, s));
	if (isdigit(c) || c == '-')
		return (parse_int(dst, s));
	unexpected(s);						/* espace, }, EOF... */
	return (-1);
}

int	argo(json *dst, FILE *stream)
{
	dst->type = INTEGER;				/* main appelle free_json meme en cas d'echec */
	if (parse_value(dst, stream) == -1)
		return (-1);
	if (peek(stream) != EOF)			/* dechet apres la valeur : '1x', '{}}' */
	{
		unexpected(stream);
		return (-1);
	}
	return (1);
}
```

Pièges :
- **état libérable** : le `main` appelle `free_json(file)` même en cas d'échec → `dst->type = INTEGER`
  au début ; dans une map, initialiser `value.type` **avant** `size++` ; ne poser `type = STRING`
  qu'après un parse réussi.
- les messages viennent de la fonction fournie : `unexpected token '%c'` / `unexpected end of input`
  (minuscules, comme dans les exemples du sujet — même si le texte dit « Unexpected »).
- **pas d'espaces** : `{"a": 1}` → `unexpected token ' '`. Rien après la valeur : `1x` → `unexpected token 'x'`.
- string : `\"` et `\\` → on garde le char suivant ; EOF dans la string → `unexpected end of input`.
- nombres négatifs : `fscanf("%d")` les gère, il faut juste accepter `-` dans `parse_value`
  (la réponse du dépôt source faisait un segfault sur `-42`).

---

## Récap des pièges (2 min avant de rendre)

| Exo | Piège n°1 |
|-----|-----------|
| ft_popen | `'r'` → je garde `fd[0]` ; fermer les 2 bouts dans l'enfant après `dup2` ; `exit(1)` après exec |
| picoshell | `wait` **après** la boucle ; parent ferme `in` + `fd[1]` ; 1 si un enfant `exit != 0` |
| sandbox | handler SIGALRM **sans** SA_RESTART ; EINTR = timeout → `SIGKILL` + `waitpid` ; `alarm(0)` |
| vbc | `add → mul → factor` ; caractère restant → `unexpected(*s)` ; corriger le `given.c` |
| argo | `dst` toujours libérable ; pas d'espaces ; rien après la valeur ; accepter `-` |
