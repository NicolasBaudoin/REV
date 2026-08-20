#!/usr/bin/env bash
set -uo pipefail

before=${1:-}
after=${2:-HEAD}
tmp_dir=$(mktemp -d)
trap 'rm -rf "$tmp_dir"' EXIT

if [[ -n "$before" && "$before" != "0000000000000000000000000000000000000000" ]] && git cat-file -e "$before^{commit}" 2>/dev/null; then
    mapfile -t revision_files < <(git diff --name-only "$before" "$after" -- '20_08_2026_revision/**/*.c')
else
    mapfile -t revision_files < <(find 20_08_2026_revision -type f -name '*.c' | sort)
fi

printf '# Revue automatique des exercices C\n\n'
if ((${#revision_files[@]} == 0)); then
    printf 'Aucun fichier C de révision modifié dans ce push.\n'
    exit 0
fi

failed=0
for file in "${revision_files[@]}"; do
    exercise=$(basename "$file" .c)
    subject=$(find 03 -type f -path "*/$exercise/sub.txt" | head -n 1)
    printf '## `%s`\n\n' "$file"

    if [[ -z "$subject" ]]; then
        printf ':warning: Sujet correspondant introuvable sous `03/`.\n\n'
        continue
    fi
    printf '**Sujet:** `%s`\n\n' "$subject"

    binary="$tmp_dir/$exercise"
    if cc -D_GNU_SOURCE -std=c99 -Wall -Wextra -Werror "$file" -o "$binary" 2>"$tmp_dir/$exercise.compile.log"; then
        printf ':white_check_mark: Compilation stricte réussie (`-std=c99 -Wall -Wextra -Werror`).\n\n'
    else
        printf ':x: **Compilation échouée.**\n\n```text\n%s```\n\n' "$(cat "$tmp_dir/$exercise.compile.log")"
        failed=1
        continue
    fi

    allowed=$(sed -n '/^Allowed functions:/,/^---/p' "$subject" | sed '1d;/^---/d' | tr ',[:space:]' '\n' | sed '/^$/d' | sort -u)
    calls=$(grep -hoE '[[:alnum:]_]+[[:space:]]*\(' "$file" | sed 's/[[:space:]]*($//' | sort -u)
    forbidden=()
    while read -r call; do
        [[ -z "$call" ]] && continue
        case "$call" in
            if|else|while|for|switch|return|sizeof|main) continue ;;
        esac
        if ! grep -Fxq "$call" <<< "$allowed"; then
            forbidden+=("$call")
        fi
    done <<< "$calls"

    if ((${#forbidden[@]} > 0)); then
        printf ':warning: Appels absents de la liste `Allowed functions`: `%s`.\n\n' "${forbidden[*]}"
        failed=1
    else
        printf ':white_check_mark: Les appels détectés correspondent à la liste `Allowed functions`.\n\n'
    fi

    printf 'Le sujet associe doit encore etre relu par l agent **Revision C Reviewer** pour les cas limites et la conformite comportementale.\n\n'
done

if ((failed)); then
    printf '> Des contrôles automatiques ont échoué.\n'
    exit 1
fi
printf '> Contrôles automatiques réussis; cela ne remplace pas la revue pédagogique.\n'
