#include <stdio.h>
int picoshell(char **cmds[]);
int main(void){
	setvbuf(stdout, NULL, _IONBF, 0);	/* pas de doublons de printf apres fork */
	char **c1[] = {(char*[]){"ls", "/", NULL}, (char*[]){"grep", "bin", NULL}, (char*[]){"wc", "-l", NULL}, NULL};
	printf("ret=%d\n", picoshell(c1)); fflush(stdout);
	char **c2[] = {(char*[]){"echo", "squalala", NULL}, (char*[]){"cat", NULL}, (char*[]){"sed", "s/a/b/g", NULL}, NULL};
	printf("ret=%d\n", picoshell(c2)); fflush(stdout);
	char **c3[] = {(char*[]){"yes", NULL}, (char*[]){"head", "-2", NULL}, NULL};
	printf("ret=%d\n", picoshell(c3));
}
