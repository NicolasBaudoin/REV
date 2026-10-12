#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
** Grammaire (priorite : () > * > +) -- 1 fonction par niveau :
**   add    = mul    { '+' mul }
**   mul    = factor { '*' factor }
**   factor = DIGIT | '(' add ')'
*/

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int accept(char **s, char c)
{
    if (**s == c)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}

/* ======================= A ECRIRE (le reste est fourni) ======================= */

node    *parse_add(char **s);

/* cree un noeud ADD/MULTI ; si echec, libere les 2 fils */
node    *join(int type, node *l, node *r)
{
    node    *n;

    if (!l || !r)
        return (destroy_tree(l), destroy_tree(r), NULL);
    n = new_node((node){.type = type, .l = l, .r = r});
    if (!n)
        return (destroy_tree(l), destroy_tree(r), NULL);
    return (n);
}

node    *parse_factor(char **s)
{
    node    *e;

    if (isdigit(**s))
        return (new_node((node){.type = VAL, .val = *(*s)++ - '0'}));
    if (accept(s, '('))
    {
        e = parse_add(s);
        if (e && !expect(s, ')'))
            return (destroy_tree(e), NULL);
        return (e);
    }
    unexpected(**s);
    return (NULL);
}

node    *parse_mul(char **s)
{
    node    *l = parse_factor(s);

    while (l && accept(s, '*'))
        l = join(MULTI, l, parse_factor(s));
    return (l);
}

node    *parse_add(char **s)
{
    node    *l = parse_mul(s);

    while (l && accept(s, '+'))
        l = join(ADD, l, parse_mul(s));
    return (l);
}

node    *parse_expr(char *s)
{
    node    *ret = parse_add(&s);

    if (ret && *s)          /* il reste des chars : '1+2)' -> ')' */
    {
        unexpected(*s);
        destroy_tree(ret);
        return (NULL);
    }
    return (ret);
}

/* ============================================================================ */

int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
    return (0);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    node *tree = parse_expr(argv[1]);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
}
