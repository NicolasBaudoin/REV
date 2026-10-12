#!/usr/bin/env bash
# test.sh — feedback immédiat sur un exo des ranks 04 à 06.
#
#   ./test.sh <exo>           teste TA version, écrite de mémoire dans entrainement/<exo>/
#   ./test.sh <exo> --ref     teste la solution de référence (rank0X/.../<exo>/)
#   ./test.sh all [--ref]     enchaîne tous les exos
#   ./test.sh list            liste les exos et les fichiers à rendre
#
# Ta version est compilée avec les flags de l'exam, lancée sur les exemples du sujet
# + des cas limites, et sa sortie (stdout, stderr, code de retour) est comparée à celle
# de la référence. Les appels de fonctions sont vérifiés contre "Allowed functions".

set -u
ROOT=$(cd "$(dirname "$0")" && pwd)
EXOS="ft_popen picoshell sandbox argo vbc bigint vect2 polyset bsq life mini_serv mini_db"
CC="cc -Wall -Wextra -Werror"
CXX="c++ -Wall -Wextra -Werror -std=c++98"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

if [ -t 1 ]; then G=$'\033[32m'; R=$'\033[31m'; Y=$'\033[33m'; B=$'\033[1m'; N=$'\033[0m'
else G=""; R=""; Y=""; B=""; N=""; fi

PASS=0; FAIL=0
ok()   { PASS=$((PASS + 1)); echo "  ${G}OK${N}  $1"; }
ko()   { FAIL=$((FAIL + 1)); echo "  ${R}KO${N}  $1"; }
warn() { echo "  ${Y}!!${N}  $1"; }

exo_dir() { find "$ROOT"/rank0[4-6] -type f -path "*/$1/sub.txt" -printf '%h\n' 2>/dev/null | head -n 1; }

files_of() {
	case "$1" in
		ft_popen|picoshell|sandbox|argo|mini_serv) echo "$1.c" ;;
		vbc|bsq|life) echo "$1.c (+ .h si besoin)" ;;
		bigint|vect2) echo "$1.hpp $1.cpp" ;;
		polyset) echo "searchable_array_bag.hpp/.cpp searchable_tree_bag.hpp/.cpp set.hpp/.cpp" ;;
		mini_db) echo "mini_db.cpp (+ .hpp si besoin)" ;;
	esac
}

# Les .c de l'élève (on ignore les fichiers de test et les fichiers fournis)
user_c()   { find "$1" -maxdepth 1 -name '*.c' ! -name 'test_main.c' ! -name 'extra_test.c' ! -name 'given*' | sort; }
user_cpp() { find "$1" -maxdepth 1 -name '*.cpp' ! -name 'main*.cpp' | sort; }

build() { # build <sortie> <compilateur...> -- fichiers
	local out=$1; shift
	if "$@" -o "$out" 2>"$TMP/build.log"; then return 0; fi
	ko "compilation : $*"
	sed 's/^/        /' "$TMP/build.log" | head -n 25
	return 1
}

allowed() { # allowed <fichier.c> [fonctions données par le sujet...]
	local f=$1; shift
	if ! command -v python3 >/dev/null; then return; fi
	local msg
	if msg=$(python3 "$ROOT/tools/check_allowed.py" "$RDIR/sub.txt" "$f" "$@"); then ok "$(basename "$f") : $msg"
	else ko "$(basename "$f") : $msg"; fi
}

run() { # run <binaire> <stdin> <args...>  -> stdout, stderr et code dans un seul texte
	local bin=$1 in=$2; shift 2
	printf '%s' "$in" | timeout 10 "$bin" "$@" >"$TMP/o" 2>"$TMP/e"
	local rc=$?
	printf '%s\n--- stderr ---\n%s\n--- exit %s ---\n' "$(cat "$TMP/o")" "$(cat "$TMP/e")" "$rc"
}

same() { # same <description> <attendu> <obtenu>
	if [ "$2" == "$3" ]; then ok "$1"; return; fi
	ko "$1"
	diff <(echo "$2") <(echo "$3") | sed 's/^</        attendu :/; s/^>/        obtenu  :/' | grep -v '^---\|^[0-9]' | head -n 12
}

cmp_args()  { same "$1" "$(run "$REF" "" "${@:2}")" "$(run "$USR" "" "${@:2}")"; }
cmp_stdin() { local d=$1 in=$2; shift 2; same "$d" "$(run "$REF" "$in" "$@")" "$(run "$USR" "$in" "$@")"; }

leaks() { # leaks <stdin> <binaire> <args...> : avertissement seulement
	command -v valgrind >/dev/null || return
	local in=$1; shift
	if printf '%s' "$in" | valgrind -q --leak-check=full --errors-for-leak-kinds=definite \
			--error-exitcode=42 "$@" >/dev/null 2>"$TMP/vg"; [ $? -eq 42 ]; then
		warn "valgrind signale des fuites/erreurs sur : ${*:2}"
		grep -m3 'definitely\|Invalid' "$TMP/vg" | sed 's/^/        /'
	fi
}

# ---------------------------------------------------------------- rank04

t_ft_popen() {
	allowed "$UDIR/ft_popen.c"
	build "$USR" $CC "$UDIR/ft_popen.c" "$RDIR/test_main.c" || return
	local out; out=$(timeout 20 "$USR" 2>&1)
	if echo "$out" | grep -q '❌'; then ko "test_main.c :"; echo "$out" | grep '❌' | sed 's/^/        /'
	else ok "test_main.c (fuites de fd, erreurs, stress)"; fi
	build "$REF" $CC "$RDIR/ft_popen.c" "$RDIR/extra_test.c" && build "$USR" $CC "$UDIR/ft_popen.c" "$RDIR/extra_test.c" \
		&& cmp_args "ls | grep (mode 'r'), tr (mode 'w'), type invalide"
}

t_picoshell() {
	allowed "$UDIR/picoshell.c"
	build "$USR" $CC "$UDIR/picoshell.c" "$RDIR/test_main.c" || return
	local out; out=$(timeout 20 "$USR" 2>&1)
	if echo "$out" | grep -q '❌'; then ko "test_main.c :"; echo "$out" | grep -B1 '❌' | sed 's/^/        /'
	else ok "test_main.c (8 pipelines + fuites de fd)"; fi
	build "$REF" $CC "$RDIR/picoshell.c" "$RDIR/extra_test.c" && build "$USR" $CC "$UDIR/picoshell.c" "$RDIR/extra_test.c" \
		&& cmp_args "ls|grep|wc, echo|cat|sed, yes|head"
}

t_sandbox() {
	allowed "$UDIR/sandbox.c"
	build "$REF" $CC "$RDIR/sandbox.c" "$RDIR/test_main.c" && build "$USR" $CC "$UDIR/sandbox.c" "$RDIR/test_main.c" \
		&& cmp_args "test_main.c : nice / segfault / timeout"
	build "$REF" $CC "$RDIR/sandbox.c" "$RDIR/extra_test.c" && build "$USR" $CC "$UDIR/sandbox.c" "$RDIR/extra_test.c" \
		&& cmp_args "abort, exit 42, ignore SIGALRM, sleep, SIGSTOP, SIGKILL, zombies"
}

t_argo() {
	local f; for f in $(user_c "$UDIR"); do allowed "$f" putchar fopen fclose; done
	build "$REF" $CC "$RDIR/argo.c" && build "$USR" $CC $(user_c "$UDIR") || return
	local i
	for i in '1' '"bonjour"' '"escape! \" "' '{"tomatoes":42,"potatoes":234}' \
		'{"recursion":{"recursion":{"recursion":{"recursion":"recursion"}}}}' \
		'"unfinished string' '"unfinished string 2\"' '{"no value?":}' \
		'-42' '{}' '""' '{"a":"b\\c"}' '{"a" :1}' '{"a":1,}' '{"a":1' '1x' 'x' '' \
		'{"a":{"b":1},"c":"d"}' ' 1' '{1:2}'; do
		cmp_stdin "argo '$i'" "$i" /dev/stdin
	done
	leaks '{"a":{"b":"c"},"d":1}' "$USR" /dev/stdin
	leaks '{"a":{"b":}' "$USR" /dev/stdin
}

t_vbc() {
	local f; for f in $(user_c "$UDIR"); do allowed "$f"; done
	build "$REF" $CC "$RDIR/vbc.c" && build "$USR" $CC -I"$UDIR" $(user_c "$UDIR") || return
	local long; long=$(sed -n "s/^\$> .\/vbc '\(2\*4+9[^']*\)'.*/\1/p" "$RDIR/sub.txt")
	local i
	for i in '1' '2+3' '3*4+5' '3+4*5' '(3+4)*5' '(((((2+2)*2+2)*2+2)*2+2)*2+2)*2' '1+' '1+2)' \
		'1+2+3+4+5' '(1)' '(((((((3)))))))' '(1+2)*3' '((6*6+7+5+8)*(1+0+4*8+7)+2)+4*(1+2)' \
		'((1+3)*12+(3*(2+6))' '' 'abc' '1++2' '1**2' '1+2*' '(*1+2)' '((1+2)*3))' '((1+2)*3' '()' '('; do
		cmp_args "vbc '$i'" "$i"
	done
	cmp_args "vbc (la longue expression du sujet)" "$long"
	leaks "" "$USR" '((6*6+7+5+8)*(1+0+4*8+7)+2)+4*(1+2)'
	leaks "" "$USR" '(1+2*'
}

# ---------------------------------------------------------------- rank05

t_bigint() {
	local m
	for m in main_subject.cpp main.cpp edge_test.cpp; do
		build "$REF" $CXX -I"$RDIR" "$RDIR/bigint.cpp" "$RDIR/$m" && build "$USR" $CXX -I"$UDIR" "$UDIR/bigint.cpp" "$RDIR/$m" \
			&& cmp_args "$m"
	done
}

t_vect2() {
	local m
	for m in main.cpp edge_test.cpp; do
		build "$REF" $CXX -I"$RDIR" "$RDIR/vect2.cpp" "$RDIR/$m" && build "$USR" $CXX -I"$UDIR" "$UDIR/vect2.cpp" "$RDIR/$m" \
			&& cmp_args "$m"
	done
}

t_polyset() {
	local g="$RDIR/given"
	build "$REF" $CXX -I"$RDIR" -I"$g" "$g/array_bag.cpp" "$g/tree_bag.cpp" "$RDIR/searchable_array_bag.cpp" \
		"$RDIR/searchable_tree_bag.cpp" "$RDIR/set.cpp" "$RDIR/main.cpp" || return
	build "$USR" $CXX -I"$UDIR" -I"$g" "$g/array_bag.cpp" "$g/tree_bag.cpp" "$UDIR/searchable_array_bag.cpp" \
		"$UDIR/searchable_tree_bag.cpp" "$UDIR/set.cpp" "$RDIR/main.cpp" || return
	cmp_args "main 5 3 8 3 0 1" 5 3 8 3 0 1
	cmp_args "main 1 2 3 4" 1 2 3 4
	cmp_args "main -7 42" -7 42
	leaks "" "$USR" 5 3 8 3 0 1
}

t_bsq() {
	local f; for f in $(user_c "$UDIR"); do allowed "$f"; done
	build "$REF" $CC "$RDIR/bsq.c" && build "$USR" $CC -I"$UDIR" $(user_c "$UDIR") || return
	local m="$TMP/maps"; mkdir -p "$m"
	cp "$RDIR/example_file" "$m/example"
	printf '3 . o x\n...\n.o.\n...\n'               > "$m/petite"
	printf '4 . o x\n....\n....\n..o.\n....\n'       > "$m/egalite"
	printf '1 . o x\nooo\n'                          > "$m/que_obstacles"
	printf '1 1 2 3\n1121\n'                         > "$m/chiffres"
	printf '2 . o x\n...\n..\n'                      > "$m/err_longueur"
	printf '2 . . x\n..\n..\n'                       > "$m/err_doublon"
	printf '1 . o x\n.a.\n'                          > "$m/err_char"
	printf '2 . o x\n...\n'                          > "$m/err_trop_peu"
	printf '2 . o x\n...\n...\n...\n'                > "$m/err_trop"
	printf '1 . o x\n...'                            > "$m/err_sans_newline"
	printf '0 . o x\n'                               > "$m/err_zero"
	printf ''                                        > "$m/err_vide"
	for f in "$m"/*; do cmp_args "bsq $(basename "$f")" "$f"; done
	cmp_args "bsq fichier_inexistant" "$m/nope"
	cmp_args "bsq plusieurs maps" "$m/petite" "$m/err_char" "$m/egalite"
	cmp_stdin "bsq sur l'entrée standard" "$(cat "$m/petite")
"
	leaks "" "$USR" "$m/example" "$m/err_longueur" "$m/err_trop"
}

t_life() {
	local f; for f in $(user_c "$UDIR"); do allowed "$f"; done
	build "$REF" $CC "$RDIR/life.c" && build "$USR" $CC -I"$UDIR" $(user_c "$UDIR") || return
	cmp_stdin "sdxddssaaww 5 5 0" 'sdxddssaaww' 5 5 0
	cmp_stdin "sdxssdswdxdddx... 10 6 0" 'sdxssdswdxdddxsaddawxwdxwaa' 10 6 0
	cmp_stdin "dxss 3 3 0" 'dxss' 3 3 0
	cmp_stdin "dxss 3 3 1" 'dxss' 3 3 1
	cmp_stdin "dxss 3 3 2" 'dxss' 3 3 2
	cmp_stdin "carré 5 5 3" 'sdxddssaaww' 5 5 3
	cmp_stdin "planeur 8 8 4" 'dxasxddxsxaaxdxd' 8 8 4
	cmp_stdin "sortir du plateau (aaaa wwww)" 'aaaawwwwxdddsss' 4 4 1
	cmp_stdin "entrée vide" '' 3 2 1
	leaks 'sdxddssaaww' "$USR" 5 5 3
}

# ---------------------------------------------------------------- rank06

t_mini_serv() {
	local f; for f in $(user_c "$UDIR"); do allowed "$f"; done
	if grep -q '^[[:space:]]*#[[:space:]]*define' $(user_c "$UDIR"); then ko "#define interdit"; fi
	build "$REF" $CC "$RDIR/mini_serv.c" && build "$USR" $CC $(user_c "$UDIR") || return
	local p=$((20000 + RANDOM % 20000))
	same "scénario 4 clients (arrivées, multi-lignes, départ, client paresseux)" \
		"$(python3 "$RDIR/test_client.py" "$REF" "$p" 2>&1)" "$(python3 "$RDIR/test_client.py" "$USR" $((p + 1)) 2>&1)"
}

t_mini_db() {
	build "$REF" $CXX "$RDIR/mini_db.cpp" && build "$USR" $CXX -I"$UDIR" $(user_cpp "$UDIR") || return
	local p=$((20000 + RANDOM % 20000))
	mkdir -p "$TMP/r" "$TMP/u"
	same "scénario POST/GET/DELETE, 2 clients, SIGINT + rechargement" \
		"$(python3 "$RDIR/test_client.py" "$REF" "$p" "$TMP/r" 2>&1)" "$(python3 "$RDIR/test_client.py" "$USR" $((p + 1)) "$TMP/u" 2>&1)"
}

# ---------------------------------------------------------------- main

test_exo() {
	local exo=$1 mode=$2
	RDIR=$(exo_dir "$exo")
	if [ -z "$RDIR" ]; then echo "${R}exo inconnu : $exo${N} (./test.sh list)"; FAIL=$((FAIL + 1)); return; fi
	UDIR="$ROOT/entrainement/$exo"
	[ "$mode" == "--ref" ] && UDIR=$RDIR
	echo "${B}== $exo${N}  (${UDIR#"$ROOT"/})"
	if [ ! -d "$UDIR" ]; then
		ko "dossier absent : crée entrainement/$exo/ avec : $(files_of "$exo")"
		return
	fi
	REF="$TMP/ref_$exo"; USR="$TMP/usr_$exo"
	"t_$exo"
}

case "${1:-}" in
	""|-h|--help) sed -n '2,12p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
	list) for e in $EXOS; do printf '  %-10s %-28s %s\n' "$e" "$(exo_dir "$e" | sed "s|$ROOT/||")" "$(files_of "$e")"; done; exit 0 ;;
	all) for e in $EXOS; do test_exo "$e" "${2:-}"; done ;;
	*) test_exo "$1" "${2:-}" ;;
esac

echo
if [ "$FAIL" -eq 0 ]; then echo "${G}${B}$PASS OK, 0 KO${N}"; else echo "${R}${B}$PASS OK, $FAIL KO${N}"; fi
[ "$FAIL" -eq 0 ]
