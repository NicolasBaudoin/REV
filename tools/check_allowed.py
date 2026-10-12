#!/usr/bin/env python3
"""Verifie qu'un fichier .c n'appelle que des fonctions autorisees par le sub.txt.
Usage: python3 tools/check_allowed.py <sub.txt> <fichier.c> [fonctions_en_plus...]
Sortie 0 si OK, 1 si appel suspect (liste affichee)."""
import re, sys

KEYWORDS = {"if", "while", "for", "switch", "return", "sizeof", "else", "do", "case", "main"}
MACROS = {"WIFEXITED", "WEXITSTATUS", "WIFSIGNALED", "WTERMSIG", "WIFSTOPPED", "WSTOPSIG",
          "FD_SET", "FD_CLR", "FD_ISSET", "FD_ZERO", "htons", "htonl", "ntohs", "ntohl",
          "va_start", "va_arg", "va_end", "va_copy", "assert"}


def allowed_from_subject(path):
    lines = open(path, encoding="utf-8", errors="replace").read().splitlines()
    out, grab = [], False
    for line in lines:
        if line.lower().startswith("allowed functions"):
            grab = True
            out.append(line.split(":", 1)[1] if ":" in line else "")
            continue
        if grab:
            if not line.strip() or line.lstrip()[:1] in "-_":
                break
            out.append(line)
    return {w for w in re.split(r"[\s,]+", " ".join(out)) if w}


TOKENS = re.compile(r"""/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*'""", re.S)


def strip_code(src):
    # commentaires, chaines et caracteres en UNE passe (un '"' dans '...' ne casse rien)
    src = TOKENS.sub(lambda m: " " if m.group(0)[0] == "/" else m.group(0)[0] * 2, src)
    return re.sub(r"^\s*#[^\n]*", " ", src, flags=re.M)


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    allowed = allowed_from_subject(sys.argv[1])
    if not allowed:
        print("(pas de liste 'Allowed functions' dans le sujet : verification sautee)")
        return 0
    allowed |= set(sys.argv[3:])
    src = strip_code(open(sys.argv[2], encoding="utf-8", errors="replace").read())
    # fonctions definies/declarees localement : "type nom(" en debut de ligne
    local = set(re.findall(r"^[A-Za-z_][\w \t\*]*?[\s\*]\**([A-Za-z_]\w*)\s*\(", src, flags=re.M))
    local |= set(re.findall(r"\(\s*\*\s*([A-Za-z_]\w*)\s*\)\s*\(", src))  # pointeurs de fonction
    calls = set(re.findall(r"(?<![\w.>])([A-Za-z_]\w*)\s*\(", src))
    bad = sorted(c for c in calls - local - KEYWORDS - MACROS - allowed
                 if not re.fullmatch(r"(int|char|void|size_t|long|unsigned|node|json|pair)", c))
    if bad:
        print("Appels hors 'Allowed functions' :", ", ".join(bad))
        return 1
    print("Fonctions autorisees : OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
