#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
int ft_popen(const char *file, char *const argv[], char type);
int main(void){
	setvbuf(stdout, NULL, _IONBF, 0);	/* pas de doublons de printf apres fork */
	char buf[4096]; int n;
	int fd = ft_popen("ls", (char *const []){"ls", "/", NULL}, 'r');
	int save = dup(0); dup2(fd, 0); close(fd);
	fd = ft_popen("grep", (char *const []){"grep", "u", NULL}, 'r');
	dup2(save, 0); close(save);
	while ((n = read(fd, buf, sizeof buf)) > 0) write(1, buf, n);
	close(fd);
	fd = ft_popen("tr", (char *const []){"tr", "a-z", "A-Z", NULL}, 'w');
	write(fd, "hello w mode\n", 13); close(fd);
	while (wait(NULL) > 0);
	printf("bad type -> %d\n", ft_popen("ls", (char *const []){"ls", NULL}, 'x'));
}
