#include <unistd.h>
#include <stdio.h>

int ft_len(char *s){int i = 0;while(s[i]){i++;}return i;}
void ftswap(char *a, char *b){char t = *a; *a = *b; *b = t;}

int main(int ac, char **av)
{
	if (ac != 2)
		return 1;
	int sl = ft_len(av[1]),i = 0, j;
	char *s = av[1];
	while (i < sl)
	{
		j = i + 1;
		while (j < sl)
		{
			if (s[i] > s[j])
				ftswap(&s[i], &s[j]);
			j++;
		}
		i++;
	}
	while (1)
	{
		write(1, s, sl);
		write(1, "\n", 1);
		i = sl - 2;
		while (i >= 0 && s[i] > s[i + 1]){i--;}
		if (i < 0)
			return 0;
		j = sl - 1;
		while (s[j] <= s[i]){j--;}
		ftswap(&s[i], &s[j]);
		i++;
		j = sl - 1;
		while (i < j){ftswap(&s[i++], &s[j++]);}
	}
}