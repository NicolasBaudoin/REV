#include <unistd.h>

int n;
char *s;
int used[256];
char buf[256];

void rec(int depth)
{
    int i;
    
    if (depth == n)
    {
        write(1, buf, n);
        write(1, "\n", 1);
        return ;
    }
    i = 0;
    while (i < n)
    {
        if (!used[i])
        {
            used[i] = 1;
            bug[depth] = s[i];
            rec(depth + 1);
            used[i] = 0;
        }
        i++
    }
}

int main(int ac, char **av)
{
    int i, j;
    char t;
    
    if (ac != 2)
        return 0
    s = av[1];
    n = 0;
    while (s[n])
        n++;
    i = 0;
    while (i < n)
    {
        j = i + 1;
        while (j < n)
        {
            if (s[i] > s[j])
            { t = s[i]; s[i] = s[j]; s[j] = t; }
            j++;
        }
        i++;
    }
    rec(0);
    return 0;
}
