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
