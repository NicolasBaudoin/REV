#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int ac, char *av)
{
    if (av != 2)
        return 1;
    char *buf = NULL, *s = av[1], tmp[4096];
    int n, i, k, len = 0, cap = 0, sl = strlen(s);
    
    while ((n = read(0, tmp, sizeof(tmp))) > 0)
    {
        if (len + n + 1 > cap)
        {
            cap = (len + n + 1) * 2;
            char *nb = realloc(buf, cap);
            if (!nb)
                return (free(buf), perror("Error"), 1);
            buf = nb;
        }
        memmove(buf + len, tmp, n);
        len += n;
    }
    if (n < 0)
        return (free(buf), perror("Error"), 1);
    i = 0;
    while (i < len)
    {
        if (sl + i <= len && memmem(buf + i, sl, s, sl) == buf + i)
        {
            k = 0;
            while (k++ < sl)
                write(1, "*", 1);
            i += sl;
        }
        else
            write(1, buf + i++, 1);
    }
    free(buf);
    return 0;
}