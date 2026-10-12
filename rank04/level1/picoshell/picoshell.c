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
