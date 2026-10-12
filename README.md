# REV — révisions des exams 42 (ranks 04 → 06)

Réponses des exams (copiées depuis [terminal-42s/42_examshell](https://github.com/terminal-42s/42_examshell/tree/main/.resources),
**vérifiées, corrigées et condensées**), cheat-sheets, et un harnais de test pour s'entraîner
« page blanche ». Le rank 03 et avant sont rangés dans [`archive/`](archive/).

## Par où commencer

1. Lis [`METHODE.md`](METHODE.md) (5 min) : pourquoi on écrit de mémoire au lieu de relire.
2. Ouvre le `README.md` du rank que tu prépares : les exos, l'idée de chacun en une phrase, un planning.
3. Écris un exo **de mémoire** dans `entrainement/<exo>/`, puis lance `./test.sh <exo>`.
4. Compare avec le `CHEATSHEET.md` du rank, cible ce qui a raté, recommence demain.

## Le parcours

| Rank | Exos | Cheat-sheet | Thème |
|------|------|-------------|-------|
| [04](rank04/) | level1 : ft_popen · picoshell · sandbox — level2 : vbc · argo | [CHEATSHEET](rank04/CHEATSHEET.md) | processus, pipes, signaux, parser récursif |
| [05](rank05/) | level1 : vect2 · bigint · polyset — level2 : life · bsq | [CHEATSHEET](rank05/CHEATSHEET.md) | C++98 (opérateurs, héritage), grilles, DP |
| [06](rank06/) | mini_serv · mini_db | [CHEATSHEET](rank06/CHEATSHEET.md) | serveur réseau avec `select` |

## Organisation du dépôt

```
REV/
├── README.md            <- tu es ici
├── METHODE.md           <- la méthode d'apprentissage (tester, espacer, mélanger)
├── test.sh              <- ./test.sh <exo> : compile, vérifie, compare à la référence
├── rank04/ rank05/ rank06/
│   ├── README.md        <- exos, idée en une phrase, planning, corrections apportées
│   ├── CHEATSHEET.md    <- versions condensées + mnémos + pièges (à relire après chaque essai)
│   └── levelN/<exo>/    <- sub.txt (sujet) + solution de référence + fichiers fournis/tests
├── entrainement/        <- TES essais de mémoire : entrainement/<exo>/<fichiers de l'exam>
├── tools/               <- vérificateur des fonctions autorisées (utilisé par test.sh)
├── archive/rank03/      <- tout le travail du rank 03 (exos, cheat-sheet, anciens essais)
└── .github/             <- CI : à chaque push sur main, test.sh tourne sur tes essais
```

## Le harnais de test

```bash
./test.sh list            # exos disponibles et fichiers à rendre
./test.sh vbc             # teste entrainement/vbc/vbc.c
./test.sh bigint --ref    # teste la solution de référence
./test.sh all             # tous tes essais d'un coup
```

Pour chaque exo : compilation avec les flags de l'exam (`-Wall -Wextra -Werror`, `-std=c++98` en C++),
vérification des appels contre `Allowed functions` du sujet, puis **comparaison de ta sortie
(stdout + stderr + code de retour) avec celle de la référence** sur les exemples du sujet et des cas
limites. Si `valgrind` est installé, les fuites sont signalées. Les exos réseau lancent ton serveur et
un scénario de clients (Python 3).

Besoin : `cc`, `c++`, `python3` ; `valgrind` conseillé.
