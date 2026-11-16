#!/usr/bin/env python3
"""Rewrite member accesses after a struct's members move or get new names.

  retype_fields.py --map MAP [--embed "struct RoomActor" base] FILES...
  retype_fields.py --map MAP --dependents include/x.h [-j N]

When a struct gains an embedded base (`struct T { struct RoomActor base; ... }`) every
`p->health` on a T must become `p->base.health`; when a member is renamed, every access
must follow. Which accesses those are depends on the type of `p`, so this asks the
compiler: it compiles each unit with agbcc, and every "structure has no member named `old'"
error whose old name is in the map is rewritten on the reported line (in the unit or
in the header the error points at). It repeats until a unit compiles or nothing more can
be rewritten.

The map has one `old new` pair per line (`#` comments allowed): `field9F base.defense`
moves and renames, `word104 savedControl` renames. `--embed TYPE MEMBER` adds `f MEMBER.f`
for every top-level member f of TYPE (from layoutmerge's compiler probe), after the map's
own pairs, so a map pair for a base member wins.

Run it after editing the headers. It rewrites only accesses the compiler rejects, so it is
safe to re-run on a branch that already has some of the changes. Lines it cannot settle (a
member used inside a macro, an access split across lines, or more candidates on a line than
errors) are listed for a hand edit. It changes no code generation by itself: confirm with
`tools/header_dependents.py --check`.
"""
import argparse
import collections
import concurrent.futures
import itertools
import os
import re
import subprocess
import sys
import tempfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)
sys.path.insert(0, TOOLS)

NO_MEMBER = re.compile(r"^(?P<path>[^:\n]+):(?P<line>\d+): (?:structure|union) has no member named `(?P<member>\w+)'", re.M)
ERROR = re.compile(r"^[^:\n]+:\d+: (?!warning)", re.M)


def read_map(path):
    pairs = {}
    for ln in open(path):
        ln = ln.split("#", 1)[0].split()
        if not ln:
            continue
        if len(ln) != 2:
            sys.exit(f"{path}: expected `old new`, got {' '.join(ln)}")
        pairs.setdefault(ln[0], ln[1])
    return pairs


def top_members(spell):
    """Top-level member names of a struct, in order (from struct_views' compiler probe)."""
    import struct_views
    views = struct_views.probe([spell], [struct_views.defining_header(spell)])
    names = []
    for leaf in views[spell]["leaves"]:
        top = re.split(r"[.\[]", leaf["name"])[0]
        if top not in names:
            names.append(top)
    return names


def candidates(text, old, new, redo=False):
    """Spans (start, end, separator) of `->old` / `.old` in text. Unless redo, skip the
    ones already in the rewritten form (`base.old` when new is `base.old`)."""
    prefix = new.rsplit(".", 1)[0] if "." in new else None
    out = []
    for m in re.finditer(r"(->|\.)(\s*)" + re.escape(old) + r"\b", text):
        if not redo and prefix and m.group(1) == "." and text[:m.start()].endswith(prefix):
            continue
        out.append((m.start(), m.end(), m.group(1) + m.group(2)))
    return out


def plan_line(text, old, count, new):
    """("ok" | "ambiguous" | "none", spans) for `count` rejected accesses of old on a line.

    A line that already reads `x->base.old` and is still rejected has a second level to
    add (`x->base.base.old`, a class whose own `base` embeds the struct), so when the
    fresh candidates do not account for the errors, every occurrence is tried."""
    spans = candidates(text, old, new)
    if len(spans) == count:
        return "ok", spans
    every = candidates(text, old, new, redo=True)
    if len(every) == count:
        return "ok", every
    if spans and count % len(spans) == 0 and MACRO_USE.search(text):
        return "ok", spans  # a macro argument the macro expands more than once
    return ("ambiguous", spans) if len(spans) > count else ("none", spans)


def apply_spans(text, spans, new):
    for a, b, sep in sorted(spans, reverse=True):
        text = text[:a] + sep + new + text[b:]
    return text


def rewrite_line(text, wanted, pairs):
    """Rewrite one source line. wanted: {old: number of errors}. Returns (text, unsettled)."""
    unsettled = []
    for old, count in wanted.items():
        kind, spans = plan_line(text, old, count, pairs[old])
        if kind != "ok":
            unsettled.append(f"{old}: {count} error(s), {len(spans)} candidate(s)")
            continue
        text = apply_spans(text, spans, pairs[old])
    return text, unsettled


MACRO_USE = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
MACRO_DEF = re.compile(r"^[ \t]*#[ \t]*define[ \t]+(\w+)\(", re.M)


def macro_definitions(paths):
    """{name: [(path, line)]} of function-like macros in the given files."""
    defs = collections.defaultdict(list)
    for path in paths:
        try:
            with open(os.path.join(ROOT, path)) as f:
                text = f.read()
        except OSError:
            continue
        for m in MACRO_DEF.finditer(text):
            defs[m.group(1)].append((path, text.count("\n", 0, m.start()) + 1))
    return defs


def macro_body(lines, start):
    """Line numbers (1-based) of a #define starting at start, with its continuations."""
    out = [start]
    while lines[out[-1] - 1].rstrip().endswith("\\") and out[-1] < len(lines):
        out.append(out[-1] + 1)
    return out


def compile_errors(src):
    import build
    cpp = build.CPP + ["-nostdinc", "-undef"] + build.DEFINES + ["-I", "include", "-iquote", ".", src]
    pre = subprocess.run(cpp, cwd=ROOT, capture_output=True, text=True)
    if pre.returncode:
        return pre.stderr
    with tempfile.TemporaryFile(mode="w+") as f:
        f.write(pre.stdout)
        f.seek(0)
        r = subprocess.run([build.compiler_for(src)] + build.cflags_for(src) + ["-o", os.devnull],
                           stdin=f, cwd=ROOT, capture_output=True, text=True)
    return r.stderr


def errors_of(stderr, pairs):
    """({(path, line): Counter(old)}, other error lines)."""
    found = collections.defaultdict(collections.Counter)
    other = []
    for m in NO_MEMBER.finditer(stderr):
        if m["member"] in pairs:
            found[(os.path.normpath(m["path"]), int(m["line"]))][m["member"]] += 1
    for ln in stderr.splitlines():
        m = NO_MEMBER.match(ln)
        if ERROR.match(ln) and not (m and m["member"] in pairs) and "In function" not in ln:
            other.append(ln)
    return found, other


def run(files, pairs, jobs, dry_run=False, rounds=8, trials=16):
    """Rewrite until every unit compiles or nothing changes. Returns (touched paths,
    {(path, line): message} left for a hand edit, {unit: other errors})."""
    header_defs = macro_definitions(sorted(
        os.path.join("include", n) for n in os.listdir(os.path.join(ROOT, "include")) if n.endswith(".h")))
    pending = list(files)
    unsettled, failed = {}, {}
    touched = set()
    files_cache = {}

    def load(path):
        if path not in files_cache:
            with open(os.path.join(ROOT, path)) as f:
                files_cache[path] = f.read().split("\n")
        return files_cache[path]

    def save(path):
        if not dry_run:
            with open(os.path.join(ROOT, path), "w") as f:
                f.write("\n".join(files_cache[path]))

    for _ in range(rounds):
        if not pending:
            break
        unsettled = {}  # only what the latest compile still rejects
        with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
            results = dict(zip(pending, ex.map(compile_errors, pending)))
        # one count per (path, line): a header line is reported by every unit that includes it
        todo, reporter, stuck = {}, {}, []
        for src in pending:
            found, other = errors_of(results[src], pairs)
            for key, counts in found.items():
                if key not in todo:
                    todo[key], reporter[key] = counts, src
            if found:
                stuck.append(src)
            elif other:
                failed[src] = other[:3]
        changed, ambiguous, macros_done = set(), [], set()
        for (path, line), counts in sorted(todo.items()):
            text = load(path)
            for old, count in counts.items():
                kind, spans = plan_line(text[line - 1], old, count, pairs[old])
                if kind == "ok":
                    text[line - 1] = apply_spans(text[line - 1], spans, pairs[old])
                    changed.add(path)
                    unsettled.pop((path, line, old), None)
                elif kind == "ambiguous":
                    ambiguous.append((path, line, old, count, spans, reporter[(path, line)]))
                elif not spans and rewrite_argument(text, line, old, pairs[old], path, header_defs, load):
                    changed.add(path)
                elif not spans:
                    # the access is in a macro used on this line: rewrite the macro body
                    site = macro_site(text[line - 1], old, pairs[old], path, header_defs, load)
                    if site and (site, old) not in macros_done:
                        macros_done.add((site, old))
                        mpath, mlines = site
                        mtext = load(mpath)
                        for n in mlines:
                            mtext[n - 1] = apply_spans(mtext[n - 1], candidates(mtext[n - 1], old, pairs[old]), pairs[old])
                        changed.add(mpath)
                    elif not site and (path, line, old) not in unsettled:
                        unsettled[(path, line, old)] = f"{path}:{line}: {old}: {count} error(s), no access on the line or in a macro it uses: {text[line - 1].strip()}"
                else:
                    unsettled[(path, line, old)] = f"{path}:{line}: {old}: {count} error(s), {len(spans)} candidate(s): {text[line - 1].strip()}"
        for path in changed:
            save(path)
        # more candidates than errors: try each choice and keep the one the compiler accepts
        for path, line, old, count, spans, src in ambiguous:
            text = load(path)
            before = text[line - 1]
            base_errs = sum(1 for m in NO_MEMBER.finditer(results[src])
                            if os.path.normpath(m["path"]) == path and int(m["line"]) == line)
            chosen = None
            for combo in list(itertools.combinations(spans, count))[:trials]:
                text[line - 1] = apply_spans(before, combo, pairs[old])
                save(path)
                after = compile_errors(src)
                left = sum(1 for m in NO_MEMBER.finditer(after)
                           if os.path.normpath(m["path"]) == path and int(m["line"]) == line)
                if left <= base_errs - count:
                    chosen = text[line - 1]
                    break
            text[line - 1] = chosen if chosen is not None else before
            save(path)
            if chosen is not None:
                changed.add(path)
                unsettled.pop((path, line, old), None)
            else:
                unsettled[(path, line, old)] = f"{path}:{line}: {old}: {count} error(s), {len(spans)} candidate(s), no choice compiles: {before.strip()}"
        touched |= changed
        if not changed or dry_run:
            break
        pending = stuck
    return sorted(touched), unsettled, failed


def call_arguments(text, open_paren):
    """[(start, end)] of the top-level arguments of the call whose `(` is at open_paren."""
    depth, start, out = 0, open_paren + 1, []
    for i in range(open_paren, len(text)):
        c = text[i]
        if c in "([{":
            depth += 1
        elif c in ")]}":
            depth -= 1
            if depth == 0:
                out.append((start, i))
                return out
        elif c == "," and depth == 1:
            out.append((start, i))
            start = i + 1
    return None


def rewrite_argument(text, line, old, new, path, header_defs, load):
    """A macro that takes the member name as an argument and applies `->` to it
    (`ACTOR_OFFSET(saved.state)` for `&((T *)0)->member`): rewrite the argument."""
    local = macro_definitions([path])
    src = text[line - 1]
    for m in MACRO_USE.finditer(src):
        defs = local.get(m.group(1)) or header_defs.get(m.group(1)) or []
        if len(defs) != 1:
            continue
        mpath, start = defs[0]
        lines = load(mpath)
        body = " ".join(lines[n - 1].rstrip("\\") for n in macro_body(lines, start))
        head = re.match(r"\s*#\s*define\s+\w+\(([^)]*)\)", body)
        args = call_arguments(src, m.end() - 1)
        if not head or not args:
            continue
        params = [p.strip() for p in head.group(1).split(",")]
        for param, (a, b) in zip(params, args):
            arg = src[a:b]
            if re.search(r"(->|\.)\s*" + re.escape(param) + r"\b", body[head.end():]) \
                    and re.match(r"\s*" + re.escape(old) + r"\b", arg):
                lead = len(arg) - len(arg.lstrip())
                text[line - 1] = src[:a + lead] + new + src[a + lead + len(old):]
                return True
    return False


def macro_site(line_text, old, new, path, header_defs, load, depth=3):
    """(file, body line numbers) of the macro, used on the line or by a macro used there,
    whose body accesses old. Definitions of one name under #if branches of one file
    count as one site."""
    local = macro_definitions([path])
    for name in MACRO_USE.findall(line_text):
        defs = local.get(name) or header_defs.get(name) or []
        if not defs or len({d[0] for d in defs}) != 1:
            continue
        mpath = defs[0][0]
        lines = load(mpath)
        bodies = [macro_body(lines, start) for _, start in defs]
        hits = [n for body in bodies for n in body if candidates(lines[n - 1], old, new)]
        if hits:
            return mpath, tuple(hits)
        if depth > 1:
            for body in bodies:
                inner = " ".join(lines[n - 1] for n in body).split(")", 1)[-1]
                site = macro_site(inner, old, new, mpath, header_defs, load, depth - 1)
                if site:
                    return site
    return None


INCOMPATIBLE = re.compile(r"^(?P<path>[^:\n]+):(?P<line>\d+): warning: passing arg (?P<arg>\d+) of "
                          r"`(?P<func>\w+)' from incompatible pointer type", re.M)


def upcast(files, member, jobs):
    """Where a subclass pointer is passed for its base (`passing arg N of f from
    incompatible pointer type`), pass `&arg->member` instead, if that silences the
    warning. Returns the number of arguments rewritten."""
    done = 0
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        results = dict(zip(files, ex.map(compile_errors, files)))
    for src in files:
        stderr = results[src]
        for m in reversed(list(INCOMPATIBLE.finditer(stderr))):
            path, line = os.path.normpath(m["path"]), int(m["line"])
            if path != src:
                continue
            with open(os.path.join(ROOT, path)) as f:
                before = f.read()
            # the call ends on the reported line; it may start a few lines up
            lines = before.split("\n")
            end = sum(len(x) + 1 for x in lines[:line])
            start = sum(len(x) + 1 for x in lines[:max(0, line - 12)])
            calls = list(re.finditer(r"\b" + re.escape(m["func"]) + r"\s*\(", before[start:end]))
            args = calls and call_arguments(before, start + calls[-1].end() - 1)
            if not args or len(args) < int(m["arg"]):
                continue
            a, b = args[int(m["arg"]) - 1]
            arg = before[a:b].strip()
            if not re.fullmatch(r"\w+(->\w+|\.\w+)*", arg):
                continue
            lead = before[a:b].index(arg)
            new = before[:a + lead] + f"&{arg}->{member}" + before[a + lead + len(arg):]
            with open(os.path.join(ROOT, path), "w") as f:
                f.write(new)
            after = compile_errors(src)
            same = [w for w in INCOMPATIBLE.finditer(after)
                    if os.path.normpath(w["path"]) == path and int(w["line"]) == line and w["arg"] == m["arg"] and w["func"] == m["func"]]
            if same or ERROR.search("\n".join(ln for ln in after.splitlines() if "warning" not in ln and "In function" not in ln)):
                with open(os.path.join(ROOT, path), "w") as f:
                    f.write(before)
            else:
                done += 1
                stderr = after
    return done


def read_steps(path):
    """[(kind, args, pairs)] from a steps file: blocks headed `[embed TYPE MEMBER]`,
    `[map]`, `[types]` (old and new struct tags) or `[text]` (literal old and new text),
    each followed by `old new` lines, or `[upcast MEMBER]` (see upcast) alone."""
    steps = []
    for raw in open(path):
        ln = raw.split("#", 1)[0].strip()
        if not ln:
            continue
        if ln.startswith("["):
            head = ln.strip("[]").split()
            steps.append((head[0], head[1:], {}))
            continue
        old, new = ln.split()
        steps[-1][2].setdefault(old, new)
    return steps


def replace_text(files, pairs, regex=False):
    """Literal (or `struct OLD` -> `struct NEW` with regex) replacement in the files."""
    changed = []
    for path in files:
        full = os.path.join(ROOT, path)
        with open(full) as f:
            text = f.read()
        new = text
        for old, rep in pairs.items():
            if regex:
                new = re.sub(r"\bstruct " + re.escape(old) + r"\b", "struct " + rep, new)
            else:
                new = new.replace(old, rep)
        if new != text:
            with open(full, "w") as f:
                f.write(new)
            changed.append(path)
    return changed


def embed_pairs(spell, member):
    spell = spell if spell.startswith(("struct ", "union ")) else "struct " + spell
    return {name: f"{member}.{name}" for name in top_members(spell)}


def report(changed, unsettled, failed, dry_run):
    for path in changed:
        print(("would rewrite " if dry_run else "rewrote ") + path)
    for msg in unsettled.values():
        print(f"UNSETTLED {msg}")
    for src, lines in failed.items():
        print(f"ERROR {src}: " + "\n    ".join(lines))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*")
    ap.add_argument("--map", help="file of `old new` pairs")
    ap.add_argument("--embed", nargs=2, metavar=("TYPE", "MEMBER"),
                    help='add `f MEMBER.f` for each top-level member of TYPE')
    ap.add_argument("--steps", help="replay a steps file (see read_steps), e.g. tools/retype/room_actor.txt")
    ap.add_argument("--upcast", metavar="MEMBER",
                    help="pass `&arg->MEMBER` where a pointer argument draws an incompatible-pointer warning")
    ap.add_argument("--dependents", nargs="+", metavar="HEADER", help="units that include these headers")
    ap.add_argument("-j", type=int, default=os.cpu_count() or 4)
    ap.add_argument("--dry-run", action="store_true", help="report the first round's rewrites, change nothing")
    a = ap.parse_args()
    files = list(a.files)
    if a.dependents:
        import header_dependents
        files += header_dependents.dependents(a.dependents)
    files = sorted(set(files))
    if not files:
        sys.exit("no files")
    if a.upcast:
        print(f"{upcast(files, a.upcast, a.j)} arguments now pass &arg->{a.upcast}")
        return
    if a.steps:
        steps = read_steps(a.steps)
    else:
        pairs = read_map(a.map) if a.map else {}
        if a.embed:
            for k, v in embed_pairs(*a.embed).items():
                pairs.setdefault(k, v)
        if not pairs:
            sys.exit("nothing to rewrite: give --map, --embed or --steps")
        steps = [("map", [], pairs)]
    unsettled, failed = {}, {}
    for kind, args, pairs in steps:
        if kind == "upcast":
            if not a.dry_run:
                print(f"{upcast(files, args[0], a.j)} arguments now pass &arg->{args[0]}")
            continue
        if kind in ("types", "text"):
            if not a.dry_run:
                for path in replace_text(files, pairs, regex=kind == "types"):
                    print(f"rewrote {path} ({kind})")
            continue
        if kind == "embed":
            pairs = {**embed_pairs(*args), **pairs}
        changed, unsettled, failed = run(files, pairs, a.j, a.dry_run)
        report(changed, unsettled, {} if a.steps else failed, a.dry_run)
    if a.steps and failed:
        report([], {}, failed, a.dry_run)
    print(f"{len(files)} units; {len(unsettled)} lines to edit by hand; {len(failed)} with other errors")
    sys.exit(1 if unsettled or failed else 0)


if __name__ == "__main__":
    main()
