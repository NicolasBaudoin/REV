# Apprendre les exos de l'Exam 03 — vite et pour de vrai

Guide basé sur la science de l'apprentissage, appliqué directement à la mémorisation des 6 exos
(`filter`, `permutations`, `powerset`, `n_queens`, `rip`, `broken_gnl`).
Le code de référence est dans les dossiers ; les mnémos dans [`CHEATSHEET.md`](CHEATSHEET.md).

---

## TL;DR — la seule chose à retenir

Ton cerveau ne mémorise pas en **relisant**. Il mémorise en **récupérant de mémoire** (retrieval)
et en **espaçant** ces récupérations dans le temps. Concrètement :

> **Ferme le corrigé. Écris le code de mémoire. Compile. Compare. Recommence demain.**

Tout le reste de ce document explique pourquoi ça marche et comment l'organiser.

---

## Ce que disent les études (et ce que ça change pour toi)

### 1. Se tester > relire (l'effet de test / retrieval practice)

C'est le résultat le plus important pour toi. Roediger & Karpicke (2006) : des étudiants apprennent
un texte. Les « relecteurs » le relisent 4×, les « testeurs » le lisent 1× puis se testent.
5 minutes après, les relecteurs gagnent (83 % vs 71 %) — **mais une semaine plus tard, les testeurs
écrasent : 61 % vs 40 %.** Relire donne l'*illusion* de savoir ; se tester construit la vraie mémoire.

Dans la grande revue de Dunlosky et al. (2013), sur 10 techniques évaluées, **la pratique par test
(« practice testing ») et la pratique distribuée sont les deux seules classées « utilité élevée »**.
Le surlignage et la relecture, eux, sont classés « utilité faible ».

**→ Pour toi :** ta première action sur chaque exo, ce n'est pas de le lire, c'est d'essayer de
l'écrire sur une page blanche **sans regarder**. Même si tu bloques — l'effort de récupération est
précisément ce qui grave la mémoire.

### 2. Espacer les révisions (le spacing effect)

Réviser 3 fois en espaçant sur plusieurs jours bat 3 fois d'affilée le même soir, pour un temps
total identique. C'est un des effets les plus robustes de la psychologie cognitive : la méta-analyse
de Cepeda et al. (254 études, ~14 000 participants) le confirme sur tous les âges et matières.
Ordre de grandeur : l'intervalle optimal entre deux révisions vaut environ **10–20 % du délai avant
l'examen**. Exam dans ~2 semaines → réviser chaque exo tous les **1 à 3 jours**.

**→ Pour toi :** ne « bachote » pas un exo 2 h d'un coup. Fais-en 20–30 min, puis reviens dessus
demain, puis dans 2 jours, puis dans 4. (Planning concret plus bas.)

### 3. Mélanger les exos (l'interleaving)

Rohrer & Taylor : en maths, **mélanger les types de problèmes au lieu de les grouper par bloc a
doublé les scores** au test. Pourquoi ? En bloc, tu sais d'avance quelle méthode appliquer. Mélangés,
tu dois d'abord **reconnaître de quel problème il s'agit** — exactement ce qu'on te demande le jour J,
où les exos tombent sans étiquette.

**→ Pour toi :** ne fais pas « 5× filter à la suite ». Fais `filter`, puis `rip`, puis `powerset`,
puis reviens à `filter`. Tire un exo au hasard et code-le. Le but est de savoir *quel patron sortir*,
pas juste de réciter un patron connu.

### 4. La difficulté « désirable » et l'effet de génération

Bjork : les conditions qui rendent l'apprentissage plus **dur sur le moment** (espacer, se tester,
mélanger, produire la réponse avant de la voir) donnent une meilleure rétention à long terme.
« La baisse de la force de récupération est le moteur de la hausse de la force de stockage » :
quand tu as *à moitié oublié* et que tu reconstruis avec effort, c'est là que la mémoire se renforce.

**→ Pour toi :** si écrire l'exo de mémoire est pénible, **c'est bon signe**. La facilité de la
relecture est un piège. Cherche l'inconfort productif.

### 5. La pratique délibérée (deliberate practice)

Ericsson : répéter aveuglément ne suffit pas. Ce qui transforme la répétition en compétence, c'est
**le feedback immédiat** et le travail ciblé sur le point faible précis. Découper la compétence,
attention pleine, corriger l'erreur, recommencer.

**→ Pour toi :** ton feedback immédiat, c'est **le compilateur et le `diff`**. Tu n'as pas besoin de
deviner si c'est juste : tu compiles, tu compares à la sortie attendue. Et tu ne réécris pas l'exo
en entier à chaque fois — tu **cibles la ligne où tu as buggé** (le calcul `l/r` de rip, les
diagonales de n_queens, le décalage du buffer de gnl…).

---

## La méthode « page blanche » (ton protocole de base)

Pour un exo donné, une session = **5 étapes, ~15–25 min** :

1. **Rappel actif (5–10 min).** Fichier fermé, éditeur vide. Écris l'exo entièrement de mémoire.
   Bloqué ? Note un commentaire `// TODO trou ici` et continue. Ne regarde pas le corrigé.
2. **Compile & teste (2 min).** Lance la compilation et les exemples du sujet.
   Le compilateur/`diff` = ton correcteur objectif.
3. **Compare au corrigé (3 min).** Ouvre le fichier de référence. Repère **exactement** où ça diverge.
4. **Cible l'erreur (3 min).** Ré-écris *juste la partie ratée* 2–3 fois, en comprenant pourquoi.
   Formule-le en une phrase à voix haute (« next_permutation = pivot depuis la fin, swap, reverse »).
5. **Note & planifie.** Marque l'exo comme « à revoir demain » si tu as bloqué, « dans 2–4 jours »
   si c'était fluide.

> Règle d'or : **tu n'as pas le droit de relire un exo sans avoir d'abord essayé de l'écrire.**

### Un petit harnais de test qui fait le feedback pour toi

Mets ce script à la racine et lance-le après chaque tentative — il compile et compare aux exemples
du sujet, tu vois en un coup d'œil ce qui passe :

```bash
#!/bin/bash
# test.sh — feedback immédiat sur un exo. Usage: ./test.sh filter
set -e
case "$1" in
  filter)
    cc -Wall -Wextra -Werror level1/filter/filter.c -o /tmp/f
    echo 'abcdefaaaabcdeabcabcdabc' | /tmp/f abc   # attendu: ***defaaa***de******d***
    echo 'ababcabababc' | /tmp/f ababc             # attendu: *****ab*****
    ;;
  permutations)
    cc -Wall -Wextra -Werror level2/permutations/permutations.c -o /tmp/p
    /tmp/p abc                                      # attendu: abc acb bac bca cab cba
    ;;
  powerset)
    cc -Wall -Wextra -Werror level2/powerset/powerset.c -o /tmp/ps
    /tmp/ps 3 1 0 2 4 5 3                            # attendu: 3 / 0 3 / 1 2 / 1 0 2
    ;;
  n_queens)
    cc -Wall -Wextra -Werror level2/n_queens/n_queens.c -o /tmp/nq
    /tmp/nq 4                                        # attendu: 1 3 0 2 / 2 0 3 1
    ;;
  rip)
    cc -Wall -Wextra -Werror level2/rip/rip.c -o /tmp/rip
    /tmp/rip '(()' | cat -e                          # attendu:  ()$ / ( )$
    ;;
  broken_gnl)
    cc -Wall -Wextra -Werror -I level1/broken_gnl -D BUFFER_SIZE=5 \
       level1/broken_gnl/get_next_line.c level1/broken_gnl/main.c -o /tmp/gnl
    echo "compile OK — teste avec tes fichiers"
    ;;
  *) echo "exo inconnu: $1" ;;
esac
```

---

## Planning espacé (exemple sur ~2 semaines)

L'idée : chaque exo revient à intervalles **croissants** (1 → 2 → 4 → 7 jours), et chaque jour tu
**mélanges** 3–4 exos différents. ~45–60 min/jour suffisent.

| Jour | Exos à faire « page blanche » (dans le désordre) |
|------|--------------------------------------------------|
| J1   | filter · permutations · rip |
| J2   | powerset · n_queens · broken_gnl · **filter** (rappel) |
| J3   | rip · permutations · **powerset** |
| J4   | filter · n_queens · **broken_gnl** |
| J5   | permutations · rip · **powerset** |
| J6   | repos léger : 1 rappel de chacun, juste les parties ratées |
| J7   | n_queens · broken_gnl · **filter** · rip |
| J8+  | 1 passage complet des 6, en aléatoire, chronométré |
| J-2  | simulation : tirage au sort de 2–3 exos, conditions d'exam |
| J-1  | uniquement tes points faibles + repos |

Astuce : écris les 6 noms sur des bouts de papier, tire-en 3 au hasard chaque jour. Le hasard force
l'interleaving et évite de n'entraîner que ce que tu aimes déjà.

---

## Décomposer par « points de bascule », pas par lignes

On ne mémorise pas 40 lignes ; on mémorise **la structure + 2-3 endroits piégeux**. Pour chaque exo,
retiens d'abord *l'idée* en une phrase, puis les pièges. Le reste se re-dérive.

- **filter** — « lis TOUT stdin, puis à chaque i : `memmem` matche → étoiles + saute, sinon 1 char ».
  Piège : lit **stdin**, pas argv ; erreur → `perror("Error")` + `return 1`.
- **permutations** — « trier, puis `next_permutation` en boucle ».
  Piège : le swap récursif casse l'ordre alpha ; pivot **depuis la fin**, swap, reverse la queue.
- **powerset** — « DFS include/exclude, imprime à la feuille si `sum==target` ».
  Piège : `in[i]=0` **avant** `in[i]=1` ; ordre des éléments = ordre du set ; sous-ensemble vide = ligne vide.
- **n_queens** — « place colonne par colonne, teste ligne + 2 diagonales ».
  Piège : diagonales `q[c]-c==row-col` et `q[c]+c==row+col` ; `putnbr` récursif (n ≥ 10).
- **rip** — « compte `l`/`r` à enlever, DFS qui remplace par des espaces, imprime si valide ».
  Piège : remplacer par **espaces** (garde la longueur) ; **pas** de déduplication.
- **broken_gnl** — « répare les 5 bugs » : strchr sans `\0`, memcpy à l'envers, memmove récursif,
  `tmp` jamais recalculé + EOF, buffer statique pas décalé.

Formule chacune **à voix haute, dans tes mots** avant de coder : verbaliser (self-explanation) est
une des techniques à utilité moyenne-forte de Dunlosky, et ça révèle instantanément les trous.

---

## Les pièges qui gaspillent ton temps

- **Relire le corrigé en hochant la tête.** Sensation de maîtrise = fausse. Si tu ne l'as pas écrit
  de mémoire, tu ne le sais pas.
- **Surligner / recopier passivement.** Utilité faible (Dunlosky). Remplace par un test.
- **Bloquer un seul exo en boucle le même soir.** C'est du massed practice : impressionnant le soir,
  oublié en 2 jours. Espace et mélange.
- **Fuir l'exo qui fait mal.** C'est précisément celui à programmer en premier le lendemain.
- **Confondre « je comprends » et « je sais reproduire ».** Comprendre est nécessaire mais ne suffit
  pas : seule la production sous contrainte prépare au clavier de l'exam.

---

## Option flashcards (spaced repetition automatisé)

Si tu veux automatiser l'espacement, un outil type **Anki** applique l'algorithme pour toi.
Fais des cartes **recto = déclencheur / verso = à produire**, orientées *production*, pas
reconnaissance :

- Recto : « rip — comment compter le nombre de parenthèses à enlever ? »
  Verso : boucle `l/r` (`(` → `l++` ; `)` → si `l>0` `l--` sinon `r++`).
- Recto : « permutations — les 3 étapes de next_permutation »
  Verso : pivot `i` depuis la fin (`s[i]<s[i+1]`) → `j` tel que `s[j]>s[i]` → swap → reverse `i+1..fin`.
- Recto : « broken_gnl — les 5 bugs »
  Verso : strchr/memcpy/memmove/tmp+EOF/décalage buffer.

Mais la carte ne remplace pas le clavier : la flashcard entretient les **points de bascule**,
la page blanche entraîne la **production complète**. Utilise les deux.

---

## Routine minimale si tu es pressé (30 min/jour)

1. Tire 2 exos au hasard.
2. Page blanche + compile + diff sur chacun (≈10 min l'un).
3. 5 min : réécris uniquement les lignes ratées, verbalise le pourquoi.
4. Note lesquels revoir demain.

Fais ça tous les jours en espaçant, et le jour J tu écris les 6 les yeux fermés.

---

## Sources

- Dunlosky, J., Rawson, K. A., Marsh, E. J., Nathan, M. J., & Willingham, D. T. (2013). *Improving Students' Learning With Effective Learning Techniques.* Psychological Science in the Public Interest, 14(1), 4–58. — [PDF](https://www.whz.de/fileadmin/lehre/hochschuldidaktik/docs/dunloskiimprovingstudentlearning.pdf) · [SAGE](https://journals.sagepub.com/doi/abs/10.1177/1529100612453266)
- Roediger, H. L., & Karpicke, J. D. (2006). *The Power of Testing Memory.* Perspectives on Psychological Science, 1(3), 181–210. — [PDF](http://psychnet.wustl.edu/memory/wp-content/uploads/2018/04/Roediger-Karpicke-2006_PPS.pdf)
- Rohrer, D., Dedrick, R. F., & Stershic, S. (2015). *Interleaved Practice Improves Mathematics Learning.* Journal of Educational Psychology. — [PDF](http://uweb.cas.usf.edu/~drohrer/pdfs/Rohrer_et_al_2015JEdPsych.pdf)
- Cepeda, N. J., Vul, E., Rohrer, D., Wixted, J. T., & Pashler, H. (2008). *Spacing Effects in Learning: A Temporal Ridgeline of Optimal Retention.* Psychological Science, 19(11), 1095–1102. — [PDF](https://laplab.ucsd.edu/articles/Cepeda%20et%20al%202008_psychsci.pdf)
- Bjork, E. L., & Bjork, R. A. (2011). *Making Things Hard on Yourself, but in a Good Way: Creating Desirable Difficulties to Enhance Learning.* — [PDF](https://www.unh.edu/teaching-learning-resource-hub/sites/default/files/media/2023-06/itow-introducing-desirable-difficulties-into-practice-and-instruction-bjork-and-bjork.pdf)
- Ericsson, K. A. — Deliberate practice & expert performance (overview). — [Frontiers in Psychology (2019)](https://www.frontiersin.org/journals/psychology/articles/10.3389/fpsyg.2019.02396/full)
