#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


typedef struct	json {
	enum {
		MAP,
		INTEGER,
		STRING
	} type;
	union {
		struct {
			struct pair	*data;
			size_t		size;
		} map;
		int	integer;
		char	*string;
	};
}	json;

typedef struct	pair {
	char	*key;
	json	value;
}	pair;

void	free_json(json j);
int	argo(json *dst, FILE *stream);

int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("unexpected token '%c'\n", peek(stream));
	else
		printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

void	free_json(json j)
{
	switch (j.type)
	{
		case MAP:
			for (size_t i = 0; i < j.map.size; i++)
			{
				free(j.map.data[i].key);
				free_json(j.map.data[i].value);
			}
			free(j.map.data);
			break ;
		case STRING:
			free(j.string);
			break ;
		default:
			break ;
	}
}

void	serialize(json j)
{
	switch (j.type)
	{
		case INTEGER:
			printf("%d", j.integer);
			break ;
		case STRING:
			putchar('"');
			for (int i = 0; j.string[i]; i++)
			{
				if (j.string[i] == '\\' || j.string[i] == '"')
					putchar('\\');
				putchar(j.string[i]);
			}
			putchar('"');
			break ;
		case MAP:
			putchar('{');
			for (size_t i = 0; i < j.map.size; i++)
			{
				if (i != 0)
					putchar(',');
				serialize((json){.type = STRING, .string = j.map.data[i].key});
				putchar(':');
				serialize(j.map.data[i].value);
			}
			putchar('}');
			break ;
	}
}

/* ======================= A ECRIRE (le reste est fourni) ======================= */

int	parse_value(json *dst, FILE *s);

/* "abc" -> malloc'd "abc". Seuls \\ et \" sont des echappements : on garde le char suivant. */
int	parse_string(char **out, FILE *s)
{
	char	*str = calloc(1, 1);
	char	*tmp;
	size_t	len = 0;
	int		c;

	if (!str)
		return (-1);
	getc(s);							/* le " ouvrant */
	while ((c = getc(s)) != '"')
	{
		if (c == '\\')
			c = getc(s);
		if (c == EOF)
		{
			free(str);
			unexpected(s);				/* -> "unexpected end of input" */
			return (-1);
		}
		tmp = realloc(str, len + 2);
		if (!tmp)
			return (free(str), -1);
		str = tmp;
		str[len++] = c;
		str[len] = '\0';
	}
	*out = str;
	return (1);
}

int	parse_int(json *dst, FILE *s)
{
	int	n;

	if (fscanf(s, "%d", &n) != 1)
	{
		unexpected(s);
		return (-1);
	}
	dst->type = INTEGER;
	dst->integer = n;
	return (1);
}

/* { "k":v , "k":v } -- chaque pair ajoutee est TOUJOURS dans un etat liberable */
int	parse_map(json *dst, FILE *s)
{
	pair	*tmp;
	pair	*p;

	dst->type = MAP;
	dst->map.size = 0;
	dst->map.data = NULL;
	getc(s);							/* le { */
	if (accept(s, '}'))
		return (1);
	while (1)
	{
		if (peek(s) != '"')
			return (unexpected(s), -1);
		tmp = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
		if (!tmp)
			return (-1);
		dst->map.data = tmp;
		p = &dst->map.data[dst->map.size];
		if (parse_string(&p->key, s) == -1)
			return (-1);
		p->value.type = INTEGER;		/* valeur "vide" liberable */
		dst->map.size++;
		if (!expect(s, ':') || parse_value(&p->value, s) == -1)
			return (-1);
		if (accept(s, '}'))
			return (1);
		if (!expect(s, ','))
			return (-1);
	}
}

int	parse_value(json *dst, FILE *s)
{
	int	c = peek(s);

	if (c == '"')
	{
		if (parse_string(&dst->string, s) == -1)
			return (-1);
		dst->type = STRING;
		return (1);
	}
	if (c == '{')
		return (parse_map(dst, s));
	if (isdigit(c) || c == '-')
		return (parse_int(dst, s));
	unexpected(s);						/* espace, }, EOF... */
	return (-1);
}

int	argo(json *dst, FILE *stream)
{
	dst->type = INTEGER;				/* main appelle free_json meme en cas d'echec */
	if (parse_value(dst, stream) == -1)
		return (-1);
	if (peek(stream) != EOF)			/* dechet apres la valeur : '1x', '{}}' */
	{
		unexpected(stream);
		return (-1);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	char *filename = argv[1];
	FILE *stream = fopen(filename, "r");
	if (!stream)
		return 1;
	json	file;
	if (argo (&file, stream) != 1)
	{
		free_json(file);
		return 1;
	}
	serialize(file);
	printf("\n");
	free_json(file);	/* ajout : le main fourni ne libere pas */
	fclose(stream);
}
