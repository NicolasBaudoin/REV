# Exam Rank 05 — C++ (classes, opérateurs) et algo sur grille

| Niveau | Exos (1 tiré au sort par niveau) | Thème |
|--------|----------------------------------|-------|
| level1 | `vect2` · `bigint` · `polyset` | C++98 : forme canonique, surcharge d'opérateurs, héritage |
| level2 | `life` · `bsq` | C : lecture d'entrée, grille 2D, programmation dynamique |

- Le code de chaque exo est dans son dossier, les explications dans [`CHEATSHEET.md`](CHEATSHEET.md).
- La méthode est dans [`../METHODE.md`](../METHODE.md).
- Tu t'entraînes dans `entrainement/<exo>/`, tu vérifies avec `./test.sh <exo>` (à la racine).

## Contenu des dossiers

| Fichier | Rôle |
|---------|------|
| `sub.txt` | le sujet tel qu'à l'exam |
| `*.hpp` / `*.cpp` / `*.c` | **la solution de référence** (corrigée, condensée, testée) |
| `main.cpp`, `main_subject.cpp` | les mains du sujet / du dépôt source |
| `edge_test.cpp` | cas limites (bigint, vect2) que le main du sujet ne vérifie pas |
| `polyset/given/` | les classes fournies à l'exam (`bag`, `array_bag`, `tree_bag`…) — à ne pas modifier |
| `bsq/example_file` | la map d'exemple du sujet |

## L'idée en une phrase

- **vect2** — « écrire les `op=`, dériver `+ - *` ; deux `operator[]` ; `3 * v` hors classe ; `<<` dans `os` ».
- **bigint** — « string de chiffres ; addition avec retenue de droite à gauche ; shift = ajouter/enlever des zéros ; comparer la longueur puis la string ».
- **polyset** — « hérite du sac ET de `searchable_bag`, écris `has` ; `set` = référence sur un sac + `insert si !has` ».
- **life** — « dessiner avec le stylo, puis N générations dans un 2e tableau (2-3 survit, 3 naît) ».
- **bsq** — « valider la map, puis `dp = 1 + min(haut, gauche, diag)`, max strict, remplir le carré ».

## Planning espacé (~2 semaines, 45–60 min/jour)

| Jour | Page blanche (dans le désordre) |
|------|---------------------------------|
| J1   | vect2 · life |
| J2   | bigint · **vect2** (rappel) · bsq |
| J3   | polyset · **life** · bigint |
| J4   | bsq · **vect2** · polyset |
| J5   | bigint · life · **bsq** |
| J6   | léger : seulement les parties ratées |
| J7   | polyset · bsq · **vect2** · life |
| J8+  | 1 exo level1 + 1 exo level2 tirés au sort, chronométrés |
| J-2  | simulation en conditions d'exam |
| J-1  | points faibles + repos |

Astuce C++ : le compilateur est ton meilleur correcteur. Écris d'abord le `.hpp` de mémoire (la liste
des opérateurs **est** le sujet), compile avec le main fourni, et laisse les erreurs te dire ce qui manque.

## Corrections par rapport aux réponses du dépôt source

| Exo | Problème dans la réponse source | Correction |
|-----|---------------------------------|------------|
| bigint | `a > b` vrai quand `a == b` ; retenue jamais remise à 0 (`UINT_MAX + UINT_MAX` faux) ; `0 << 3` → `0000` | réécrit ; `edge_test.cpp` ajouté pour attraper ces 3 bugs |
| vect2 | `operator<<` écrivait dans `std::cout` au lieu de `os` | corrigé ; `edge_test.cpp` le vérifie |
| polyset | `set.hpp` sans include guard | `#pragma once` ; `get_bag` const + non-const |
| bsq | le `main` n'affichait pas la ligne vide au bon endroit entre plusieurs maps ; lignes en trop acceptées | réécrit en un seul fichier |
| life | — (correcte) | condensée en un seul fichier |
