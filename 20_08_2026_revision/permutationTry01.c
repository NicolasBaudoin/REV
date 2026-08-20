#include <unistd.h>

int	ft_len(char *s){int i=0;while(s[i])i++;return (i);}
void	swap(char *a,char *b){char t=*a;*a=*b;*b=t;}

int main(int ac, char **av)
{
    if (ac != 2)
        return 1;
    
}