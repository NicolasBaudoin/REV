# Exam Rank 06 — serveur réseau avec `select`

| Exo | Langage | Thème |
|-----|---------|-------|
| `mini_serv` | C | serveur de chat : plusieurs clients, diffusion des messages |
| `mini_db` | C++ | serveur clé/valeur persistant (sauvegarde sur SIGINT) |

`mini_serv` est l'exo historique du rank 06 ; `mini_db` apparaît selon les versions de l'exam.
Les deux reposent sur **la même boucle `select`** : apprends le patron une fois (voir
[`CHEATSHEET.md`](CHEATSHEET.md)), puis seulement ce qui change.

## Contenu des dossiers

| Fichier | Rôle |
|---------|------|
| `sub.txt` | le sujet |
| `mini_serv.c` / `mini_db.cpp` | **la solution de référence** (testée) |
| `mini_serv/given_main.c` | ce que le dépôt source fournissait (un squelette socket/bind/listen) |
| `test_client.py` | scénario de clients utilisé par `./test.sh` (arrivées, messages multi-lignes, départs, client paresseux, SIGINT…) |

> La « réponse » mini_serv du dépôt source n'était qu'un squelette (socket/bind/listen puis `close`).
> La solution ici est la version complète classique, testée avec 4 clients simultanés et 200 Ko de messages.
> À l'exam, le `main.c` fourni contient `extract_message` et `str_join` : on les recopie tels quels.

## L'idée en une phrase

- **mini_serv** — « boucle `select` ; nouvelle connexion → id + "just arrived" aux autres ; données → buffer du client, chaque ligne complète part chez les autres avec `client %d: ` ; `recv <= 0` → "just left", free, close ».
- **mini_db** — « charger le fichier ; boucle `select` ; chaque ligne = une commande POST/GET/DELETE → 0/1/2 ; SIGINT lève un flag → on sort et on sauvegarde ».

## Planning (rank à 1 exo : profondeur plutôt que variété)

| Jour | Travail |
|------|---------|
| J1   | le **patron `select`** seul (socket → bind → listen → boucle accept/recv qui affiche ce qu'elle reçoit) |
| J2   | mini_serv complet page blanche · rappel du patron |
| J3   | mini_db page blanche |
| J4   | mini_serv page blanche (sans regarder `extract_message` : réécris-le si tu peux) |
| J6   | mini_db · mini_serv |
| J9   | mini_serv chronométré + test à la main avec 3 `nc` |
| J13  | mini_serv · mini_db |
| J-1  | relis le récap des pièges, une dernière page blanche du patron, repos |

Test à la main (indispensable, c'est ce que conseille le sujet) :

```bash
cc -Wall -Wextra -Werror mini_serv.c -o mini_serv && ./mini_serv 8081
# puis dans 2-3 autres terminaux :
nc 127.0.0.1 8081
```

## Corrections par rapport aux réponses du dépôt source

| Exo | Problème dans la réponse source | Correction |
|-----|---------------------------------|------------|
| mini_serv | squelette seulement (aucune gestion des clients) ; utilisait `printf`, `fprintf`, `inet_addr` (non autorisés) | solution complète, uniquement des fonctions autorisées |
| mini_db | C++11 (`auto`) ; écrivait le fichier **dans** le handler de signal ; appelait `handleMessage` sur un fd qu'il venait de fermer ; une commande coupée entre deux `recv` était perdue | C++98, flag + sauvegarde dans `main`, buffer par client |
