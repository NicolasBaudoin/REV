# Exam 06 — Cheat-sheet mémo (versions condensées, testées)

Un seul gros exo par sujet : **mini_serv** (C, le classique) ou **mini_db** (C++, selon les versions
de l'exam). Les deux reposent sur **la même boucle `select`** → apprends-la une fois, réutilise-la.

Compilation : `cc -Wall -Wextra -Werror mini_serv.c` / `c++ -Wall -Wextra -Werror -std=c++98 mini_db.cpp`.
Tester : `./test.sh mini_serv` ou `./test.sh mini_db` (lance le serveur et un scénario de clients en Python),
et à la main : `./mini_serv 8081` puis plusieurs terminaux avec `nc 127.0.0.1 8081`.

---

## Le patron commun : serveur `select` (à savoir écrire les yeux fermés)

```
socket(AF_INET, SOCK_STREAM, 0)
addr : sin_family = AF_INET, sin_addr.s_addr = htonl(2130706433) /* 127.0.0.1 */, sin_port = htons(port)
bind(...) ; listen(...)                         -> toute erreur ici = "Fatal error\n" sur stderr, exit(1)

FD_ZERO(&afds) ; FD_SET(sockfd, &afds) ; maxfd = sockfd
while (1)
    rfds = wfds = afds                          <- select ABÎME les sets : on repart d'une copie à chaque tour
    select(maxfd + 1, &rfds, &wfds, NULL, NULL)
    for fd in 0..maxfd, si FD_ISSET(fd, &rfds) :
        fd == sockfd  -> accept, FD_SET, maxfd = max, état du client (id, buffer)
        sinon         -> recv : <= 0 => départ (FD_CLR, close, free) ; > 0 => traiter les lignes complètes
```

**Pourquoi un buffer par client ?** `recv` peut renvoyer une demi-ligne, ou trois lignes d'un coup.
On accumule, et on ne traite que ce qui finit par `\n` ; le reste attend le prochain `recv`.

---

## mini_serv  (C) — chat entre clients

**Messages exacts** :

```
stderr  "Wrong number of arguments\n"   (pas d'argument)        -> exit(1)
stderr  "Fatal error\n"                  (socket/bind/listen/malloc) -> exit(1)
autres  "server: client %d just arrived\n"
autres  "server: client %d just left\n"
autres  "client %d: " devant CHAQUE ligne reçue
```

**Mnémo — 5 blocs** : *globals (`ids[]`, `msgs[]`, 3 `fd_set`, `maxfd`, `next_id`) → `fatal` + `broadcast`
(à tous sauf l'émetteur) → `extract_message` + `str_join` (**fournis** dans le `main.c` du sujet : on les
recopie) → `main` (socket, bind 127.0.0.1, listen) → boucle `select` (accept / recv).*

```c
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

/*
** Squelette a retenir (5 blocs) :
**   1. globals : ids[fd], msgs[fd] (buffer de lignes incompletes), 3 fd_set, maxfd, next_id
**   2. fatal() / broadcast(sauf fd, msg)
**   3. extract_message + str_join  (FOURNIS dans le main.c du sujet : a recopier)
**   4. main : socket -> bind 127.0.0.1 -> listen  (erreur = "Fatal error")
**   5. boucle select :
**        listen pret  -> accept, id = next_id++, "server: client %d just arrived\n"
**        client pret  -> recv <= 0 : "server: client %d just left\n", free, FD_CLR, close
**                        sinon     : join au buffer, pour chaque ligne complete
**                                    -> "client %d: " + ligne a tous les AUTRES
*/

int		ids[65536];
char	*msgs[65536];
fd_set	afds, rfds, wfds;
int		maxfd = 0, next_id = 0;
char	rbuf[1001], wbuf[64];

void	fatal(void)
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

void	broadcast(int except, char *s)
{
	for (int fd = 0; fd <= maxfd; fd++)
		if (FD_ISSET(fd, &wfds) && fd != except)
			send(fd, s, strlen(s), 0);
}

/* ---- fournies par le sujet (main.c) ---- */
int	extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int		i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char	*str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}
/* ---------------------------------------- */

int	main(int ac, char **av)
{
	struct sockaddr_in	addr;
	int					sockfd, fd, cfd, n, r;
	char				*msg;

	if (ac != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		fatal();
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(2130706433);	/* 127.0.0.1 */
	addr.sin_port = htons(atoi(av[1]));
	if (bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) != 0
		|| listen(sockfd, SOMAXCONN) != 0)
		fatal();
	FD_ZERO(&afds);
	FD_SET(sockfd, &afds);
	maxfd = sockfd;
	while (1)
	{
		rfds = wfds = afds;
		if (select(maxfd + 1, &rfds, &wfds, NULL, NULL) < 0)
			continue ;
		for (fd = 0; fd <= maxfd; fd++)
		{
			if (!FD_ISSET(fd, &rfds))
				continue ;
			if (fd == sockfd)
			{
				cfd = accept(sockfd, NULL, NULL);
				if (cfd < 0)
					continue ;
				if (cfd > maxfd)
					maxfd = cfd;
				ids[cfd] = next_id++;
				msgs[cfd] = NULL;
				FD_SET(cfd, &afds);
				sprintf(wbuf, "server: client %d just arrived\n", ids[cfd]);
				broadcast(cfd, wbuf);
				continue ;
			}
			n = recv(fd, rbuf, 1000, 0);
			if (n <= 0)
			{
				sprintf(wbuf, "server: client %d just left\n", ids[fd]);
				broadcast(fd, wbuf);
				free(msgs[fd]);
				msgs[fd] = NULL;
				FD_CLR(fd, &afds);
				close(fd);
				continue ;
			}
			rbuf[n] = '\0';
			msgs[fd] = str_join(msgs[fd], rbuf);
			if (!msgs[fd])
				fatal();
			while ((r = extract_message(&msgs[fd], &msg)) == 1)
			{
				sprintf(wbuf, "client %d: ", ids[fd]);
				broadcast(fd, wbuf);
				broadcast(fd, msg);
				free(msg);
			}
			if (r < 0)
				fatal();
		}
	}
	return (0);
}
```

Pièges :
- **pas de `#define`** dans le fichier (interdit par le sujet) — d'où `2130706433` en dur pour 127.0.0.1.
- ids : le premier client a 0, puis `next_id++` à chaque arrivée, **jamais réutilisés** (même si un fd l'est).
- `broadcast` à tous **sauf** l'émetteur (et sauf le nouveau client pour « just arrived »).
- préfixe `client %d: ` devant **chaque** ligne : boucle `while (extract_message(...) == 1)`.
- client parti : prévenir les autres, `free` son buffer, `FD_CLR`, `close` — sinon fuite (vérifiée).
- client « paresseux » qui ne lit pas : ne pas le déconnecter ; on n'envoie qu'aux fd présents dans `wfds`.
- `recv` sur 1000 octets max dans un buffer de 1001 → place pour le `\0`.
- `htons`/`htonl` ne sont pas dans la liste mais sont utilisés par le `main.c` fourni : OK.

---

## mini_db  (C++) — base clé/valeur persistante

**Protocole** (une commande par ligne, réponse + `\n`) :

```
POST <k> <v>   -> "0"
GET <k>        -> "0 <v>" ou "1"
DELETE <k>     -> "0" ou "1"
autre chose    -> "2"
```

Au démarrage : charger le fichier s'il existe, puis afficher `ready\n`. Sur **SIGINT** : sauvegarder
toute la base dans le fichier et quitter.

**Mnémo — 4 blocs** : *charger (`ifstream >> k >> v`) → socket/bind/listen + `ready` →
boucle `select` avec `std::map<int, std::string>` comme buffers, 1 commande par `\n`, `istringstream`
pour découper → SIGINT met un **flag**, `select` est interrompu (EINTR), on sort, on sauvegarde.*

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
** Plan (4 blocs) :
**   1. charger le fichier (s'il existe) : "cle valeur" par ligne -> std::map
**   2. socket -> bind 127.0.0.1:port -> listen -> afficher "ready\n"
**   3. boucle select : accept / recv -> buffer par client -> 1 commande par '\n'
**        POST k v -> "0" | GET k -> "0 v" ou "1" | DELETE k -> "0" ou "1" | autre -> "2"
**   4. SIGINT : le handler met juste un flag ; select est interrompu (EINTR),
**      la boucle s'arrete -> on sauvegarde la map dans le fichier et on quitte.
*/

typedef std::map<std::string, std::string>	t_db;

static volatile sig_atomic_t	g_stop = 0;

static void	on_sigint(int)
{
	g_stop = 1;
}

static std::string	handle(t_db &db, const std::string &line)
{
	std::istringstream	iss(line);
	std::string			cmd, key, val, extra;

	iss >> cmd >> key >> val >> extra;
	if (cmd == "POST" && !val.empty() && extra.empty())
	{
		db[key] = val;
		return ("0");
	}
	if (cmd == "GET" && !key.empty() && val.empty())
	{
		t_db::iterator	it = db.find(key);
		if (it == db.end())
			return ("1");
		return ("0 " + it->second);
	}
	if (cmd == "DELETE" && !key.empty() && val.empty())
		return (db.erase(key) ? "0" : "1");
	return ("2");
}

static void	load(t_db &db, const std::string &path)
{
	std::ifstream	in(path.c_str());
	std::string		k, v;

	while (in >> k >> v)
		db[k] = v;
}

static void	save(const t_db &db, const std::string &path)
{
	std::ofstream	out(path.c_str());

	for (t_db::const_iterator it = db.begin(); it != db.end(); ++it)
		out << it->first << " " << it->second << "\n";
}

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./mini_db <port> <path>" << std::endl;
		return (1);
	}
	std::string	path(av[2]);
	t_db		db;
	load(db, path);

	int	srv = socket(AF_INET, SOCK_STREAM, 0);
	if (srv < 0)
		return (1);
	int	opt = 1;
	setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	sockaddr_in	addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);		/* 127.0.0.1 */
	addr.sin_port = htons(std::atoi(av[1]));
	if (bind(srv, (sockaddr *)&addr, sizeof(addr)) < 0 || listen(srv, SOMAXCONN) < 0)
	{
		close(srv);
		return (1);
	}
	signal(SIGINT, on_sigint);
	std::cout << "ready" << std::endl;

	fd_set						all, rd;
	int							maxfd = srv;
	std::map<int, std::string>	buf;
	char						tmp[4096];

	FD_ZERO(&all);
	FD_SET(srv, &all);
	while (!g_stop)
	{
		rd = all;
		if (select(maxfd + 1, &rd, NULL, NULL, NULL) < 0)
			continue ;									/* EINTR (SIGINT) -> on reteste g_stop */
		for (int fd = 0; fd <= maxfd; ++fd)
		{
			if (!FD_ISSET(fd, &rd))
				continue ;
			if (fd == srv)
			{
				int	c = accept(srv, NULL, NULL);
				if (c >= 0)
				{
					FD_SET(c, &all);
					if (c > maxfd)
						maxfd = c;
					buf[c] = "";
				}
				continue ;
			}
			ssize_t	n = recv(fd, tmp, sizeof(tmp), 0);
			if (n <= 0)
			{
				close(fd);
				FD_CLR(fd, &all);
				buf.erase(fd);
				continue ;
			}
			buf[fd].append(tmp, n);
			size_t	pos;
			while ((pos = buf[fd].find('\n')) != std::string::npos)
			{
				std::string	res = handle(db, buf[fd].substr(0, pos)) + "\n";
				buf[fd].erase(0, pos + 1);
				send(fd, res.c_str(), res.size(), 0);
			}
		}
	}
	save(db, path);
	for (int fd = 0; fd <= maxfd; ++fd)
		if (FD_ISSET(fd, &all))
			close(fd);
	return (0);
}
```

Pièges :
- **ne rien faire de lourd dans le handler** : il met `g_stop = 1` ; `select` renvoie -1 (EINTR) et la boucle
  s'arrête proprement → sauvegarde dans `main` (pas d'`ofstream` dans un handler de signal).
- `std::cout << "ready" << std::endl` : le `endl` vide le buffer, sinon le testeur attend « ready » pour rien.
- `istringstream >> cmd >> key >> val >> extra` : vérifier qu'il n'y a **ni trop ni trop peu** de mots.
- une commande peut arriver en plusieurs `recv` (ou plusieurs commandes en un seul) → buffer par client.
- C++98 : pas d'`auto`, `ifstream(path.c_str())` (la réponse du dépôt source utilisait du C++11,
  et appelait `handleMessage` sur un fd **déjà fermé** ✗).
- `SO_REUSEADDR` : relancer tout de suite après un Ctrl-C sans « Address already in use ».

---

## Récap des pièges (2 min avant de rendre)

| Exo | Piège n°1 |
|-----|-----------|
| mini_serv | pas de `#define` ; `rfds = wfds = afds` à chaque tour ; buffer par client ; préfixe par ligne ; free + close au départ |
| mini_db | handler SIGINT = flag seulement ; `ready` + `endl` ; buffer par client ; C++98 |
