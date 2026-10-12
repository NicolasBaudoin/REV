#!/usr/bin/env bash
# Revue automatique des tentatives "de memoire" poussees dans entrainement/<exo>/.
# Usage: review_revision_c.sh [<commit_avant> [<commit_apres>]]
#  - exos modifies dans entrainement/ -> ./test.sh <exo> (compare a la reference)
#  - sinon (references / harnais modifies, ou lancement manuel) -> ./test.sh all --ref
set -uo pipefail

before=${1:-}
after=${2:-HEAD}
changed=()
if [[ -n "$before" && "$before" != "0000000000000000000000000000000000000000" ]] \
	&& git cat-file -e "$before^{commit}" 2>/dev/null; then
	mapfile -t changed < <(git diff --name-only "$before" "$after" -- 'entrainement/*/*' \
		| cut -d/ -f2 | sort -u)
fi

printf '# Revue automatique des exercices\n\n'
failed=0
if ((${#changed[@]} == 0)); then
	printf 'Aucune tentative modifiee dans `entrainement/` : verification de toutes les references.\n\n'
	printf '```text\n'
	./test.sh all --ref || failed=1
	printf '```\n\n'
else
	for exo in "${changed[@]}"; do
		[[ -d "entrainement/$exo" ]] || continue
		printf '## `entrainement/%s`\n\n```text\n' "$exo"
		./test.sh "$exo" || failed=1
		printf '```\n\n'
	done
fi

if ((failed)); then
	printf '> Des controles ont echoue : relis les lignes `KO` ci-dessus, corrige, re-pousse.\n'
	printf '> Pour une revue pedagogique detaillee, lance l agent **Revision C Reviewer**.\n'
	exit 1
fi
printf '> Controles automatiques reussis ; cela ne remplace pas la revue pedagogique.\n'
