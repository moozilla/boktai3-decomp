"""File operations behind `scope.py new`, `land` and `rename`: where a function's C lives, how a draft
goes into the source tree, and how a name change reaches every place that spells it.

A function with no C is in one of two states (scope.py todo counts them):
  in a unit   an INCLUDE_ASM line inside a src/*.c unit: its C replaces that line in that file
  plain       an inventory function with no C anywhere: a new src/fn/sub_ADDR.c
"""
import glob
import os
import re
import subprocess
import tempfile

IDENT = re.compile(r"[A-Za-z_]\w*\Z")
INCLUDE_ASM = re.compile(r'^[ \t]*INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\);[ \t]*\n?', re.M)
MARKER = "// MATCHING_CONCESSION: register-pinning"


def sources(root):
    pats = ("src/**/*.c", "src/**/*.h", "include/**/*.h")
    return sorted({p for pat in pats for p in glob.glob(os.path.join(root, pat), recursive=True)})


def include_asm_index(root, drafts=False):
    """{asm name: unit path} for every INCLUDE_ASM outside a NONMATCHING #else, in one pass over src/.
    DRAFTS: the ones inside a NONMATCHING #else too (a drafted function's unit)."""
    out = {}
    for path in sorted(glob.glob(os.path.join(root, "src", "**", "*.c"), recursive=True)):
        text = open(path).read()
        if "INCLUDE_ASM" not in text:
            continue
        plain = text if drafts else re.sub(r"#ifdef NONMATCHING\n.*?#endif", "", text, flags=re.S)
        for m in INCLUDE_ASM.finditer(plain):
            out.setdefault(m.group(1), os.path.relpath(path, root))
    return out


def include_asm_file(root, names, index=None):
    """(path, name) of the unit whose INCLUDE_ASM names one of NAMES, or None."""
    index = include_asm_index(root) if index is None else index
    return next(((index[n], n) for n in names if n in index), None)


def where(root, name, addr, index=None):
    """('unit', path, asm_name) | ('plain', path, None): where FUNC's C goes."""
    names = [name, f"sub_{addr:08X}"]
    # a drafted function's INCLUDE_ASM sits in its NONMATCHING #else: that unit is still its home
    hit = include_asm_file(root, names, index) or include_asm_file(root, names, include_asm_index(root, True))
    if hit:
        return "unit", hit[0], hit[1]
    path = f"src/fn/sub_{addr:08X}.c"
    return "plain", path, None


def csv_rows(root):
    path = os.path.join(root, "symbols", "functions.csv")
    lines = open(path).read().splitlines()
    return path, lines


def csv_name(root, addr):
    _, lines = csv_rows(root)
    for ln in lines[1:]:
        if ln.split(",")[0].upper() == f"{addr:08X}":
            return ln.split(",")[1]
    return None


def name_in_use(root, name):
    """Files that spell NAME (as a whole word), and whether functions.csv has it."""
    _, lines = csv_rows(root)
    in_csv = any(ln.split(",")[1:2] == [name] for ln in lines[1:])
    pat = re.compile(r"\b%s\b" % re.escape(name))
    code = lambda t: re.sub(r"/\*.*?\*/|//[^\n]*", "", t, flags=re.S)  # a name in a comment is not a use
    return in_csv, [os.path.relpath(p, root) for p in sources(root) if pat.search(code(open(p).read()))]


def rename(root, addr, old, new, mode="thumb", note=None, confidence="low", dry=False):
    """Name the function at ADDR NEW: its functions.csv row (added if it has none), and every spelling
    of OLD or sub_ADDR in src/ and include/ (definitions, calls, declarations, INCLUDE_ASM lines).
    `#define X sub_ADDR` aliases are dropped (the csv name replaces them) and X renamed too.
    Returns [(file, replacements)]."""
    if not IDENT.match(new):
        raise ValueError(f"{new}: not a C identifier")
    label = f"sub_{addr:08X}"
    in_csv, users = name_in_use(root, new)
    # a file that already aliases NEW to this function (`#define NEW sub_ADDR`) is not a collision
    alias = re.compile(r"^#define[ \t]+%s[ \t]+%s\b" % (re.escape(new), label), re.M)
    users = [u for u in users if not alias.search(open(os.path.join(root, u)).read())]
    if in_csv and csv_name(root, addr) == new:  # already this function's name: only the spellings change
        in_csv = False
    if in_csv or (users and new != old):
        raise ValueError(f"{new} is already used: " + ", ".join((["functions.csv"] if in_csv else []) + users[:5]))
    olds = {old, label}
    changed = []
    for p in sources(root):
        text = open(p).read()
        if not any(o in text for o in olds):
            continue
        for alias in re.findall(r"^#define[ \t]+(\w+)[ \t]+%s[ \t]*(?://[^\n]*)?$" % label, text, re.M):
            olds.add(alias)
    pat = re.compile(r"\b(%s)\b" % "|".join(re.escape(o) for o in sorted(olds, key=len, reverse=True)))
    for p in sources(root):
        text = open(p).read()
        if not pat.search(text):
            continue
        out = re.sub(r"^#define[ \t]+\w+[ \t]+%s[ \t]*(?://[^\n]*)?\n" % label, "", text, flags=re.M)
        out, n = pat.subn(new, out)
        if out != text:
            changed.append((os.path.relpath(p, root), n))
            if not dry:
                open(p, "w").write(out)
    path, lines = csv_rows(root)
    key = f"{addr:08X}"
    i = next((i for i, ln in enumerate(lines[1:], 1) if ln.split(",")[0].upper() == key), None)
    if i is not None:
        cols = lines[i].split(",", 5)
        cols[1] = new
        if note is not None:
            cols[5:] = [note.replace(",", ";")]
        lines[i] = ",".join(cols)
    else:
        row = f"{key},{new},{mode},{confidence},src-alias,{(note or '').replace(',', ';')}"
        j = next((j for j, ln in enumerate(lines[1:], 1) if ln.split(",")[0].upper() > key), len(lines))
        lines.insert(j, row)
    if not dry:
        open(path, "w").write("\n".join(lines) + "\n")
    changed.append(("symbols/functions.csv", 1))
    return changed


def concessions(text):
    pins = len(re.findall(r'\basm\("r\d+"\)|\basm\("(?:sb|sl|fp|ip)"\)', text))
    bars = len(re.findall(r'\basm(?:\s+(?:volatile|__volatile__))?\s*\(\s*""', text))
    # an empty `do { } while (0)` acts as a barrier: its loop notes end cse blocks and weight refs
    bars += len(re.findall(r'\bdo\s*\{\s*\}\s*while\s*\(\s*0\s*\)', text))
    # as does an if with nothing in its body (one whose body is DEBUG_PRINT(), include/debug.h, is plain C)
    bars += len(re.findall(r'\bif\s*\((?:[^()]|\((?:[^()]|\([^()]*\))*\))*\)\s*(?:\{\s*\}|;)',
                           re.sub(r"//[^\n]*|/\*.*?\*/", "", text, flags=re.S)))
    if (pins or bars) and MARKER not in text:
        text = MARKER + "\n" + text
    if not (pins or bars) and MARKER in text:
        text = re.sub(r"^// MATCHING_CONCESSION: register-pinning\n", "", text, flags=re.M)
    return text, pins, bars


def without_drafts(text):
    """TEXT without the #ifdef NONMATCHING branches, which the default build never compiles."""
    out, depth = [], 0  # depth > 0: inside a NONMATCHING branch, counting nested #if lines
    for line in text.splitlines(keepends=True):
        word = line.strip().split(None, 1)[0] if line.strip() else ""
        if depth:
            if word in ("#if", "#ifdef", "#ifndef"):
                depth += 1
            elif word == "#endif" or word == "#else" and depth == 1:
                depth -= 1
                if word == "#endif" and depth == 0:
                    out.append(line)
            continue
        if re.match(r"\s*#\s*ifdef\s+NONMATCHING\b", line):
            depth = 1
            continue
        out.append(line)
    return "".join(out)


def added_concessions(before, after):
    """(pins, barriers) that the compiled code of AFTER has beyond BEFORE (drafts don't count)."""
    _, p0, b0 = concessions(without_drafts(before))
    _, p1, b1 = concessions(without_drafts(after))
    return max(p1 - p0, 0), max(b1 - b0, 0)


def pin_refusal(func, pins, bars, unit):
    what = " and ".join(x for x in (f"{pins} register pin{'s' * (pins != 1)}" if pins else "",
                                    f"{bars} asm barrier{'s' * (bars != 1)}" if bars else "") if x)
    return (f"{func}: not landed: this match adds {what}.\n"
            "Pins are a last resort: the project is removing them (phase 3), so each one is work to undo later,\n"
            "and the C that gives the same allocation has usually turned out easier to find than it looked.\n"
            f"  scope.py {unit} pins       what removing each one costs, and the free ones\n"
            "  docs/WORKER.md \"Stay on a function until it matches\"   the approaches to try first\n"
            "If you have tried those and the function still needs them, land it anyway with --allow-pins:\n"
            "the unit gets the MATCHING_CONCESSION marker and counts as Register-pinned. Say why in its note.")


def context_key(text):
    """Whitespace-insensitive unit context, preserving string/character literal contents."""
    text = re.sub(r"^// scope: (?:write|put) .*\n", "", text, flags=re.M)
    tokens = re.findall(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|\s+|.', text, re.S)
    return "".join(t for t in tokens if not t.isspace())


def merge(current, base, draft):
    """Three-way merge of a unit: the draft was made from BASE; CURRENT is the file now. None on conflict."""
    if current == base:
        return draft
    with tempfile.TemporaryDirectory() as tmp:
        a, b, c = (os.path.join(tmp, n) for n in ("cur", "base", "new"))
        open(a, "w").write(current)
        open(b, "w").write(base)
        open(c, "w").write(draft)
        if subprocess.run(["git", "merge-file", a, b, c]).returncode:
            return None
        return open(a).read()


TYPE_WORDS = {"void", "char", "short", "int", "long", "unsigned", "signed", "float", "double", "const",
              "volatile", "u8", "u16", "u32", "u64", "s8", "s16", "s32", "s64", "vu8", "vu16", "vu32", "bool8",
              "bool16", "bool32", "uintptr_t", "size_t"}


def prototype(texts, name):
    """The definition head `RET NAME(PARAMS)` from the first declaration of NAME in TEXTS, with names
    given to unnamed parameters (arg0, arg1, ...); None when nothing declares it."""
    pat = re.compile(r"^[ \t]*((?:extern\s+)?[A-Za-z_][\w \t*]*?)\b%s\s*\(([^;{}]*)\)\s*;" % re.escape(name), re.M)
    for text in texts:
        m = pat.search(re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S))
        if not m:
            continue
        ret = re.sub(r"\bextern\s+", "", m.group(1)).strip()
        params = [p.strip() for p in re.split(r",(?![^()]*\))", m.group(2))] if m.group(2).strip() else ["void"]
        out = []
        for i, p in enumerate(params):
            words = re.findall(r"[A-Za-z_]\w*", p)
            named = (p != "void" and "(" not in p and words and words[-1] not in TYPE_WORDS
                     and not p.rstrip().endswith("*") and not re.search(r"\b(?:struct|union|enum)\s+\w+\s*$", p))
            out.append(p if p == "void" or named or "(" in p else f"{p} arg{i}".replace("* arg", "*arg"))
        return f"{ret}{'' if ret.endswith('*') else ' '}{name}({', '.join(out)})"
    return None


def stub_unit(text, asm_name, func, headers=()):
    """TEXT (a unit) with FUNC's INCLUDE_ASM line replaced by an empty definition to write the C in
    (with the signature a declaration in the unit or its headers gives, else void(void))."""
    head = prototype([text, *headers], func) or prototype([text, *headers], asm_name) or f"void {func}(void)"
    head = head.replace(asm_name, func) if asm_name != func else head
    stub = (f"// scope: write {func} here (`scope.py rom {func}` for the ROM listing)\n"
            f"{head}\n{{\n}}\n")
    for m in INCLUDE_ASM.finditer(text):
        if m.group(1) == asm_name:
            return text[:m.start()] + stub + text[m.end():]
    return None


def definition_span(text, name):
    """(start, end) of NAME's definition in TEXT, comment lines directly above included, or None."""
    m = re.search(r"^[A-Za-z_][^\n;=()]*\b%s\s*\([^;{]*\)\s*\{" % re.escape(name), text, re.M)
    if not m:
        return None
    depth, i = 0, text.index("{", m.start())
    while i < len(text):
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
        if depth == 0:
            break
    end = i + 1 if text[i:i + 1] == "\n" else i
    start = m.start()
    while start:
        prev = text.rfind("\n", 0, start - 1) + 1
        if not text[prev:start].lstrip().startswith("//"):
            break
        start = prev
    return start, end


def seed_from_twin(text, twin, func, others):
    """A one-function draft from the twin's file: TWIN renamed to FUNC, OTHERS (the file's other
    non-static functions and INCLUDE_ASM lines) removed; the structs, declarations and helpers stay."""
    for o in others:
        # a function the file defines under a `#define ALIAS sub_ADDR` alias is spelled ALIAS there
        aliases = re.findall(r"^#define[ \t]+(\w+)[ \t]+%s\b.*$" % re.escape(o), text, re.M)
        for name in [o] + aliases:
            sp = definition_span(text, name)
            if sp:
                text = text[:sp[0]] + text[sp[1]:]
        text = re.sub(r"^#define[ \t]+\w+[ \t]+%s\b.*\n" % re.escape(o), "", text, flags=re.M)
    text = INCLUDE_ASM.sub("", text)
    text = re.sub(r"#ifdef NONMATCHING\n.*?#endif\n?", "", text, flags=re.S)
    return re.sub(r"\b%s\b" % re.escape(twin), func, text)


def unwrap(text, func, addr):
    """The matching C without its #ifdef NONMATCHING / #else INCLUDE_ASM / #endif wrapper and fuzzy notes."""
    for m in re.finditer(r"#ifdef NONMATCHING\n(.*?)#else\n(.*?)#endif\n?", text, re.S):
        names = re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', m.group(2))
        if func in names or "sub_" + addr in names:
            body = "".join(ln for ln in m.group(1).splitlines(True)
                           if not re.match(r"\s*// (fuzzy:|Remaining differences)", ln))
            # a block can hold several functions (one file-wide #ifdef): the others stay nonmatching,
            # each in its own #ifdef NONMATCHING / #else INCLUDE_ASM / #endif
            for other in names:
                if other in (func, "sub_" + addr):
                    continue
                d = re.search(r"^[A-Za-z_][^\n;=]*\b" + other + r"\s*\([^;{]*\)\s*\{.*?^\}\n", body, re.S | re.M)
                if not d:
                    return None
                start = d.start()
                while True:  # take the comment lines directly above the definition along
                    prev = body.rfind("\n", 0, start - 1) + 1
                    if start == 0 or not body[prev:start].lstrip().startswith("//"):
                        break
                    start = prev
                inc = re.search(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*' + other + r"\s*\);", m.group(2)).group(0)
                body = (body[:start] + "#ifdef NONMATCHING\n" + body[start:d.end()] + "#else\n" + inc
                        + "\n#endif\n" + body[d.end():])
            return text[:m.start()] + body + text[m.end():]
    return None


def wrap_nonmatching(text, func, asm_name, fuzzy, shape=None):
    """TEXT with FUNC's definition inside `#ifdef NONMATCHING` (its fuzzy score first, as check.py
    --record writes it) and `INCLUDE_ASM` in the `#else`, so the default build keeps the ROM's bytes."""
    sp = definition_span(text, func)
    if not sp:
        return None
    # A non-inline static helper only FUNC uses would still be emitted by the default build and
    # shift every later address, so it moves into the block with FUNC.
    spans = {func: sp}
    for name in re.findall(r"^static\s+(?![^\n(]*\binline\b)[^\n;=(]*?\b(\w+)\s*\([^;{]*\)\s*\{", text, re.M):
        if name != func and definition_span(text, name):
            spans[name] = definition_span(text, name)
    private = set(spans) - {func}
    while True:
        inside = [spans[n] for n in private | {func}]
        leaked = {n for n in private
                  if any(not any(a <= m.start() < b for a, b in inside)
                         for m in re.finditer(r"\b%s\b" % re.escape(n), text))}
        if not leaked:
            break
        private -= leaked
    moved = sorted([spans[n] for n in private], key=lambda s: s[0])
    body = "".join(text[a:b].rstrip("\n") + "\n\n" for a, b in moved) + text[sp[0]:sp[1]].rstrip("\n") + "\n"
    note = f"// fuzzy: {fuzzy:.1f}%\n" + (f"// scope shape {shape} (differences once low registers are set aside)\n"
                                           if shape else "")
    block = (f"#ifdef NONMATCHING\n{note}{body}#else\n"
             f'INCLUDE_ASM("asm/nonmatching", {asm_name});\n#endif\n')
    out, at = "", 0
    for a, b in sorted(moved + [sp]):
        out += text[at:a] + (block if (a, b) == sp else "")
        at = b
    return out + text[at:]
