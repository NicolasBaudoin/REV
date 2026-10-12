# Exam Rank 04 — processus, pipes, signaux, parsing

| Niveau | Exos (1 tiré au sort par niveau) | Thème |
|--------|----------------------------------|-------|
| level1 | `ft_popen` · `picoshell` · `sandbox` | `pipe` / `fork` / `dup2` / `execvp` / `wait` / signaux |
| level2 | `vbc` · `argo` | parser récursif (descente récursive) |

- Le code de chaque exo est dans son dossier, les explications dans [`CHEATSHEET.md`](CHEATSHEET.md).
- La méthode (pourquoi « page blanche + espacement ») est dans [`../METHODE.md`](../METHODE.md).
- Tu t'entraînes dans `entrainement/<exo>/`, tu vérifies avec `./test.sh <exo>` (à la racine).

## Contenu des dossiers

| Fichier | Rôle |
|---------|------|
| `sub.txt` | le sujet tel qu'à l'exam |
| `<exo>.c` | **la solution de référence** (corrigée, condensée, testée) |
| `given.c` | le fichier fourni à l'exam (argo, vbc) — à compléter |
| `test_main.c` | le main de test du dépôt source (ft_popen, picoshell, sandbox) |
| `extra_test.c` | cas limites en plus, utilisés par `test.sh` |

## L'idée en une phrase (retiens ça d'abord, le code se re-dérive)

- **ft_popen** — « pipe, fork ; l'enfant branche le bon bout sur stdin/stdout et exec ; le parent ferme l'autre bout et renvoie le sien ».
- **picoshell** — « `in` = lecture du pipe précédent ; à chaque cmd : pipe si suivante, fork, l'enfant branche `in`→0 et `fd[1]`→1 ; le parent ferme et avance ; `wait` à la fin ».
- **sandbox** — « handler SIGALRM vide sans SA_RESTART, fork, alarm, waitpid : EINTR = timeout → SIGKILL + waitpid ».
- **vbc** — « `add` appelle `mul` qui appelle `factor` ; la parenthèse relance `add` ; s'il reste un caractère → erreur ».
- **argo** — « `parse_value` aiguille sur `peek` ; la map boucle `"clé" : valeur (, | })` ; `dst` toujours libérable ».

Formule-la **à voix haute** avant de coder : si tu bloques sur la phrase, tu bloqueras sur le code.

## Planning espacé (~2 semaines, 45–60 min/jour)

Chaque exo revient à intervalles croissants (1 → 2 → 4 → 7 jours) et on **mélange** les exos.

| Jour | Page blanche (dans le désordre) |
|------|---------------------------------|
| J1   | ft_popen · vbc |
| J2   | picoshell · **ft_popen** (rappel) · argo |
| J3   | sandbox · **vbc** · picoshell |
| J4   | argo · **ft_popen** · sandbox |
| J5   | picoshell · vbc · **argo** |
| J6   | léger : seulement les lignes ratées de la semaine |
| J7   | sandbox · argo · **ft_popen** · vbc |
| J8+  | 2 exos tirés au sort (1 level1 + 1 level2), chronométrés |
| J-2  | simulation : tirage au sort, conditions d'exam, `./test.sh` seulement à la fin |
| J-1  | uniquement tes points faibles + repos |

## Corrections par rapport aux réponses du dépôt source

Les réponses de `42_examshell` ont été recopiées puis **vérifiées** ; celles-ci ont été corrigées :

| Exo | Problème dans la réponse source | Correction |
|-----|---------------------------------|------------|
| picoshell | renvoyait 0 même si `execvp` échoue (le `test_main.c` le signale) ; si `pipe`/`fork` échoue en cours de route : fd `in` jamais fermé et enfants jamais attendus | statut des enfants vérifié ; `in` fermé et `wait` sur toute erreur |
| sandbox | pas d'`alarm(0)` après une fin normale | ajouté |
| vbc | « Unexpected end of **file** », pré-vérification qui ne détecte pas `1++2`, `''`… | descente récursive pure, messages exacts |
| argo | segfault sur `-42`, pas d'erreur sur `1x`, `char` comparé à `EOF`, buffer fixe de 4096 et fuite sur chaîne non terminée | réécrit, états toujours libérables, sans fuite (valgrind) |
