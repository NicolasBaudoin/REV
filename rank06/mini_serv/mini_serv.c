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
