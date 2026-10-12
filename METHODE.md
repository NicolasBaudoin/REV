# La méthode — apprendre les exos d'exam vite et pour de vrai

Version générale (ranks 04 à 06) du guide écrit pour le rank 03
([`archive/rank03/README.md`](archive/rank03/README.md), qui détaille les études).

## TL;DR

Ton cerveau ne mémorise pas en **relisant**. Il mémorise en **récupérant de mémoire** et en
**espaçant** ces récupérations. Concrètement :

> **Ferme le corrigé. Écris l'exo de mémoire dans `entrainement/<exo>/`. Lance `./test.sh <exo>`.
> Compare. Recommence dans 1, 2, 4 puis 7 jours.**

## Les 5 résultats qui comptent (et ce qu'ils changent pour toi)

| Résultat | Ce que disent les études | → Pour toi |
|----------|--------------------------|-----------|
| **Se tester > relire** | Roediger & Karpicke (2006) : 1 semaine après, les « testeurs » retiennent 61 % contre 40 % pour les « relecteurs ». Dunlosky et al. (2013) : test et pratique espacée sont les 2 seules techniques « utilité élevée » ; relire et surligner = « faible ». | Ta 1re action sur un exo, c'est l'écrire **sans regarder**, même mal. |
| **Espacer** | Cepeda et al. (2008) : l'intervalle optimal ≈ 10–20 % du délai avant l'examen. | Exam dans 2 semaines → revoir chaque exo tous les 1–3 jours, pas 2 h d'affilée. |
| **Mélanger** | Rohrer et al. (2015) : mélanger les types de problèmes au lieu de les grouper a fortement amélioré les scores au test. | Ne fais pas 5× le même exo. Tire au sort : le jour J les exos tombent sans étiquette. |
| **Difficulté désirable** | Bjork & Bjork (2011) : ce qui est plus dur sur le moment (se tester, espacer) retient mieux. | Si écrire de mémoire est pénible, c'est que ça marche. |
| **Pratique délibérée** | Ericsson : la répétition ne paie qu'avec un **feedback immédiat** et un travail ciblé sur le point faible. | Ton feedback = `./test.sh`. Ne réécris pas tout : **cible la ligne ratée**. |

## Le protocole « page blanche » (une session = 15–30 min par exo)

1. **Rappel actif (10–15 min).** Corrigé fermé. Écris l'exo dans `entrainement/<exo>/` avec les noms de
   fichiers de l'exam. Bloqué ? `// TODO trou ici` et continue.
2. **Feedback (1 min).** `./test.sh <exo>` : compilation `-Wall -Wextra -Werror`, fonctions autorisées,
   sortie comparée à la référence sur les exemples du sujet et des cas limites.
3. **Compare (3 min).** Ouvre le `CHEATSHEET.md` du rank, repère **exactement** où ça diverge.
4. **Cible (5 min).** Réécris *juste la partie ratée* 2–3 fois. Dis à voix haute pourquoi
   (« sans SA_RESTART, sinon waitpid repart et le timeout ne marche jamais »).
5. **Planifie.** Bloqué → à revoir demain. Fluide → dans 3–4 jours.

> Règle d'or : **pas le droit de relire un exo sans avoir d'abord essayé de l'écrire.**

Pour recommencer à zéro le lendemain : `rm -r entrainement/<exo>` (ou garde l'historique avec git).

## Apprendre la structure, pas les lignes

On ne mémorise pas 80 lignes : on mémorise **un patron + 2-3 points de bascule**. Les ranks 04-06 en
ont très peu, réutilisés partout :

| Patron | Sert à |
|--------|--------|
| `pipe / fork / dup2 / close / execvp / wait` | ft_popen, picoshell, sandbox (rank04) |
| descente récursive `add → mul → factor` + `peek / accept / expect` | vbc, argo (rank04) |
| forme canonique + « écrire `op=` puis dériver » + « écrire `<` et `==` puis dériver » | vect2, bigint, polyset (rank05) |
| grille 2D + double tableau / DP | life, bsq (rank05) |
| boucle `select` + buffer de lignes par client | mini_serv, mini_db (rank06) |

Chaque `rank0X/README.md` donne l'**idée en une phrase** de chaque exo : formule-la avant de coder.
Verbaliser (self-explanation) révèle les trous instantanément.

## Les pièges qui gaspillent ton temps

- **Relire le corrigé en hochant la tête.** Sensation de maîtrise = fausse.
- **Bachoter un exo le même soir.** Impressionnant le soir, oublié en 2 jours.
- **Fuir l'exo qui fait mal.** C'est celui à refaire en premier demain.
- **Confondre « je comprends » et « je sais l'écrire ».** Seule la production au clavier prépare l'exam.
- **Tester seulement l'exemple du sujet.** Les correcteurs testent les cas limites : `test.sh` aussi.

## Option flashcards

Pour automatiser l'espacement des **points de bascule** (pas du code entier), Anki avec des cartes
orientées production :

- « sandbox : comment détecter le timeout ? » → handler SIGALRM vide sans SA_RESTART, `waitpid` → -1 + EINTR.
- « picoshell : pourquoi `wait` après la boucle ? » → sinon interblocage si le pipe se remplit.
- « bigint : `a > b` en fonction de `<` ? » → `b < a` (pas `!(a < b)`).
- « mini_serv : les 4 messages exacts ? » → just arrived / just left / `client %d: ` / Fatal error.

La carte entretient les points de bascule, la page blanche entraîne la production complète.

## Sources

- Dunlosky, J., Rawson, K. A., Marsh, E. J., Nathan, M. J., & Willingham, D. T. (2013). *Improving Students' Learning With Effective Learning Techniques.* Psychological Science in the Public Interest, 14(1), 4–58.
- Roediger, H. L., & Karpicke, J. D. (2006). *The Power of Testing Memory.* Perspectives on Psychological Science, 1(3), 181–210.
- Rohrer, D., Dedrick, R. F., & Stershic, S. (2015). *Interleaved Practice Improves Mathematics Learning.* Journal of Educational Psychology.
- Cepeda, N. J., Vul, E., Rohrer, D., Wixted, J. T., & Pashler, H. (2008). *Spacing Effects in Learning: A Temporal Ridgeline of Optimal Retention.* Psychological Science, 19(11), 1095–1102.
- Bjork, E. L., & Bjork, R. A. (2011). *Making Things Hard on Yourself, but in a Good Way: Creating Desirable Difficulties to Enhance Learning.*
- Ericsson, K. A. — Deliberate practice & expert performance.

Liens vers les PDF : voir [`archive/rank03/README.md`](archive/rank03/README.md#sources).
