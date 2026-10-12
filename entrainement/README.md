# entrainement/ — tes essais de mémoire

Un dossier par exo, avec **les noms de fichiers de l'exam** (comme le dossier `rendu/` à l'exam) :

```
entrainement/
├── vbc/vbc.c
├── bigint/bigint.hpp
├── bigint/bigint.cpp
└── polyset/searchable_array_bag.hpp ...
```

`./test.sh list` (à la racine) donne les fichiers attendus pour chaque exo.

## Le cycle

```bash
mkdir -p entrainement/vbc && vim entrainement/vbc/vbc.c   # écrire SANS regarder le corrigé
./test.sh vbc                                              # feedback immédiat
# lire les KO, comparer avec rank04/CHEATSHEET.md, réécrire la partie ratée
git add entrainement/vbc && git commit -m "vbc: essai J3" && git push
```

- Les fichiers fournis à l'exam (`given.c` d'argo/vbc, classes de `polyset/given/`) : pars d'une copie
  (`cp rank04/level2/vbc/given.c entrainement/vbc/vbc.c`) et complète-la de mémoire.
- Pour un nouvel essai le lendemain, efface et recommence : l'historique git garde tes anciennes versions,
  ce qui permet de voir tes progrès (`git log -p entrainement/vbc`).
- À chaque push sur `main`, la CI lance `./test.sh` sur les exos modifiés et affiche le résultat
  dans l'onglet *Actions*. L'agent **Revision C Reviewer** peut ensuite faire une revue détaillée.
