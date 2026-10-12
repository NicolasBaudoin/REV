#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
int sandbox(void (*f)(void), unsigned int timeout, bool verbose);
void ab(void){abort();}
void ex(void){exit(42);}
void ign(void){signal(SIGALRM, SIG_IGN); signal(SIGTERM, SIG_IGN); while(1);}
void slp(void){sleep(1);}
void stop(void){raise(SIGSTOP);}  /* stoppe -> finit en timeout */
void kil(void){kill(getpid(), SIGKILL);}
int main(void){
	setvbuf(stdout, NULL, _IONBF, 0);	/* pas de doublons de printf apres fork */
	printf("%d\n", sandbox(ab, 2, true));
	printf("%d\n", sandbox(ex, 2, true));
	printf("%d\n", sandbox(ign, 1, true));
	printf("%d\n", sandbox(slp, 3, true));
	printf("%d\n", sandbox(stop, 1, true));
	printf("%d\n", sandbox(kil, 1, true));
	printf("zombies: %d\n", waitpid(-1, NULL, WNOHANG));
}
