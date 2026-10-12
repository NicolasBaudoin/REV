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
