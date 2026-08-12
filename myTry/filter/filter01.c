#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int ac, char **av)
{
    if (ac != 2 || !av[1][0])
        return 1;
    
    
    char *b, *s, tmp[4096];
    int n, i, k, len, cap, sl;
    
    b = NULL;
    s = av[1];
    sl = strlen(s);
    cap = 0;
    len = 0;
    
    while ((n = read(0, tmp, sizeof(tmp))) > 0)
    {
        if (len + n + 1 > cap)
        {
            cap = (len + n + 1);
            char *nb = realloc(b, cap);
            // oublie securité malloc
            b = nb;
        }
        memmove(b + len, tmp, n);
        len += n;
    }
    if (n < 0)
        return (free(b), perror("Error"), 1);
    i = 0;
    while (i < len)
    {
        if ( i + sl <= len /*+ i en trop*/ && memmem(b + i, sl, s, sl) == b + i)
        {
            k = 0;
            while (k++ < sl)
                write(1, "*", 1);
            i += sl;
        }
        else
            write(1, b + i++, 1);
    }
    free(b);
    return 0;
}
