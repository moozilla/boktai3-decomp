#!/usr/bin/env python3
"""Run scope workers (Codex) over a queue of NONMATCHING drafts, a few at a time.

  campaign.py queue [--category Bosses] [--all]    write build/scope/campaign/queue.json from origin/main
  campaign.py tick [--workers 4] [--model M]       keep WORKERS running; return when one finishes
  campaign.py status                               what ran, what matched, what is in flight
  campaign.py land FUNC WORKTREE [REV]                  verify FUNC's best_match.c, unwrap its NONMATCHING block,
                                                   three-way merge it into WORKTREE's file, check the unit

Each worker gets its own directory build/scope/campaign/FUNC/ (start.c = the draft as on origin/main,
base.c = the same, kept for a three-way merge at landing) and SCOPE_RUN=FUNC. Workers are detached:
`tick` returns as soon as any worker finishes (so whoever drives the campaign can land results) and
the others keep running; the next `tick` counts them. No worker starts below the quota floor.
"""
import glob
import json
import os
import re
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
CAMP = os.path.join(ROOT, "build", "scope", "campaign")
QUEUE = os.path.join(CAMP, "queue.json")
FLOOR = 15  # percent of the weekly Codex quota that must stay unused

BRIEF = """You are a worker on a GBA decompilation project (Boktai 3). Your job: make one C function compile
byte-for-byte identical to the original ROM, using the `scope` tool, in C the original authors could have
written.

First read the worker brief: tools/scope/WORKER.md (you are in the scope checkout; run every command from
its root). Compiler facts: tools/scope/README.md, section "agbcc facts established with scope" (grep it).

Task:
- Function: {func} ({size} bytes, currently {counts}). Your directory: {wd}. The draft is {wd}/start.c (a
  copy of {src}); work on copies in your directory only.
- Prefix every scope command with SCOPE_RUN={run}, e.g. `SCOPE_RUN={run} python3 tools/scope/scope.py
  {wd}/start.c`. Other workers run at the same time on other functions.
- Goal 1: MATCH in plain C. Use `why` on the first difference of each hunk, confirm the diagnosis with a
  what-if knob (`AGBCC_PRI=name:+N`, `AGBCC_FORCE=name:REG`, `AGBCC_SPILL=K:REG`), then find C with the same
  effect: declaration or statement order, a temporary, a type, operand order, an inline helper, a variable
  reused for a later value, `x * 64` vs `x << 6`, splitting or joining an expression. Several changes often
  pay off only together (WORKER.md step 5).
- Register pins (`register T x asm("rN")`) and empty `asm("" : "+r"(x))` barriers are a last resort: use
  one only where a knob proved an allocation or reload decision that you could not reproduce in C after
  real attempts, and say so in the report. Never hand-written instructions. If the draft already has
  pins or barriers, try to remove them (`pins` shows the cost of each).
- Goal 2: if you matched with concessions, remove as many as you can while keeping MATCH.
- Keep the C readable: struct fields and names, no magic offsets where the draft has fields. Comments in
  the C must stand alone: do not mention REPORT.md, your directory or your experiments.
- Do not edit anything outside {wd} (not src/, symbols/, tools/, include/); do not commit, push or run
  make. Never use `rm -rf` (move files to ~/.Trash). agbcc is C89 (declarations first in each block).
- Budget: about 120 compiles; stop earlier when you run out of ideas backed by `why`.

Deliverables in {wd}:
- best_match.c: your best MATCHing file, if any (the whole file, NONMATCHING block kept as it is in
  start.c: the #ifdef/#else/#endif stay; the landing step removes them).
- best.c: your best non-matching file (fewest differing instructions), if you did not match.
- result.json: {{"match": true/false, "counts": "N ROM / M draft" (best non-match, or "MATCH"),
  "pins": P, "barriers": B, "compiles": C}}
- REPORT.md: what `why` said and what C change it led to, for each step that helped; what you could not
  fix and the diagnosis; where scope's output was confusing, wrong or missing.
Your last message: a five-line summary of the same.
"""


def sh(*a, **k):
    return subprocess.run(a, cwd=ROOT, capture_output=True, text=True, **k).stdout


def drafts_on_main():
    """function name -> src path, for every INCLUDE_ASM in the #else branch of a NONMATCHING block."""
    out = {}
    for f in sh("git", "ls-tree", "-r", "--name-only", "origin/main", "src").split():
        if not f.endswith(".c"):
            continue
        t = sh("git", "show", f"origin/main:{f}")
        for m in re.finditer(r"#ifdef NONMATCHING(.*?)#else(.*?)#endif", t, re.S):
            for n in re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', m.group(2)):
                out[n] = f
    return out


def cmd_queue(args):
    prog = json.loads(sh("git", "show", "origin/main:PROGRESS.json"))
    drafts = drafts_on_main()
    want = None if "--all" in args else (args[args.index("--category") + 1] if "--category" in args else "Bosses")
    rows = []
    for c in prog["categories"]:
        if want and c["name"] != want:
            continue
        for u in c["units"]:
            for name, addr, size, fuzzy, *_ in u["functions"]:
                src = drafts.get(name) or drafts.get("sub_" + addr)
                if fuzzy < 100 and src:
                    rows.append({"func": name, "addr": addr, "size": size, "fuzzy": fuzzy, "src": src,
                                 "category": c["name"], "unit": u["name"]})
    # likely wins first: high fuzzy per byte left, small before large
    rows.sort(key=lambda r: (100 - r["fuzzy"]) * r["size"])
    os.makedirs(CAMP, exist_ok=True)
    old = json.load(open(QUEUE)) if os.path.exists(QUEUE) else []
    seen = {r["func"] for r in old}
    merged = old + [r for r in rows if r["func"] not in seen]
    json.dump(merged, open(QUEUE, "w"), indent=1)
    print(f"{len(rows)} drafts ({want or 'all categories'}); queue now {len(merged)}")


def quota():
    out = sh("python3", "tools/codex_quota.py")
    m = re.search(r"(\d+)% left", out)
    return int(m.group(1)) if m else None


def state(func):
    wd = os.path.join(CAMP, func)
    if not os.path.exists(os.path.join(wd, "pid")):
        return "pending"
    pid = int(open(os.path.join(wd, "pid")).read())
    try:
        os.kill(pid, 0)
        alive = subprocess.run(["ps", "-o", "stat=,command=", "-p", str(pid)], capture_output=True,
                               text=True).stdout.strip()
        if alive and not alive.startswith("Z") and "codex" in alive:  # not a reused pid
            return "running"
    except OSError:
        pass
    if not os.path.exists(os.path.join(wd, "result.json")) and failed_turn(wd):
        return "failed"
    return "done"


def failed_turn(wd):
    """The worker's last event was a failed turn (e.g. 'Selected model is at capacity')."""
    try:
        tail = open(os.path.join(wd, "codex.json")).read()[-3000:]
    except OSError:
        return None
    m = re.findall(r'"type":"turn.failed","error":\{"message":"([^"]*)"', tail)
    return m[-1] if m else None


def resume(r, model, note=None):
    func = r["func"]
    wd = os.path.join(CAMP, func)
    head = open(os.path.join(wd, "codex.json")).read(4000)
    m = re.search(r'"thread_id":"([0-9a-f-]+)"', head)
    n = int(open(os.path.join(wd, "retries")).read()) if os.path.exists(os.path.join(wd, "retries")) else 0
    if not m or n >= 3:
        open(os.path.join(wd, "gave_up"), "w").write(failed_turn(wd) or "?")
        print(f"gave up on {func} after {n} resumes")
        return False
    open(os.path.join(wd, "retries"), "w").write(str(n + 1))
    note = note or (f"You were interrupted by an error ({failed_turn(wd)}). Continue the same task from where"
                    f" you stopped; your files are in {os.path.relpath(wd, ROOT)}. Finish with the deliverables.")
    if os.path.exists(os.path.join(wd, ".reported")):
        os.remove(os.path.join(wd, ".reported"))
    log = open(os.path.join(wd, "codex.json"), "a")
    p = subprocess.Popen(["codex", "exec", "resume", "-c", "model_reasoning_effort=high", "--json", "-o",
                          os.path.join(wd, "last.txt"), m.group(1), note],
                         cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                         start_new_session=True)
    open(os.path.join(wd, "pid"), "w").write(str(p.pid))
    print(f"resumed {func} (attempt {n + 1})", flush=True)
    return True


def start(r, model):
    func = r["func"]
    wd = os.path.join(CAMP, func)
    os.makedirs(wd, exist_ok=True)
    text = sh("git", "show", f"origin/main:{r['src']}")
    open(os.path.join(wd, "base.c"), "w").write(text)
    open(os.path.join(wd, ".main_rev"), "w").write(sh("git", "rev-parse", "origin/main").strip())
    open(os.path.join(wd, "start.c"), "w").write(text)
    rel = os.path.relpath(wd, ROOT)
    env = dict(os.environ, SCOPE_RUN=func)
    counts = sh("python3", "tools/scope/scope.py", f"{rel}/start.c", func, env=env)
    m = re.search(r"(\d+ ROM / \d+ draft[^;]*)", counts)
    counts = m.group(1) if m else ("MATCH already" if "MATCH" in counts else "see scope")
    r["start_counts"] = counts
    brief = BRIEF.format(func=func, size=r["size"], counts=counts, wd=rel, src=r["src"], run=func)
    open(os.path.join(wd, "brief.txt"), "w").write(brief)
    log = open(os.path.join(wd, "codex.json"), "w")
    p = subprocess.Popen(["codex", "exec", "-m", model, "-s", "workspace-write", "-C", ROOT, "-c",
                          "model_reasoning_effort=high", "--json", "-o", os.path.join(wd, "last.txt"), brief],
                         cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                         start_new_session=True)
    open(os.path.join(wd, "pid"), "w").write(str(p.pid))
    open(os.path.join(wd, "started"), "w").write(time.strftime("%Y-%m-%d %H:%M:%S"))
    print(f"started {func} ({r['size']} B, {counts})", flush=True)


def cmd_tick(args):
    workers = int(args[args.index("--workers") + 1]) if "--workers" in args else 4
    model = args[args.index("--model") + 1] if "--model" in args else "gpt-6.1-sol"
    rows = json.load(open(QUEUE))
    running = {r["func"] for r in rows if state(r["func"]) == "running"}
    for r in rows:  # one more round for near-misses (a few insns left), in the same session
        wd = os.path.join(CAMP, r["func"])
        if len(running) >= workers or state(r["func"]) != "done" or os.path.exists(os.path.join(wd, ".second")):
            continue
        try:
            res = json.load(open(os.path.join(wd, "result.json")))
        except (OSError, ValueError):
            continue
        m = re.match(r"(\d+) ROM / (\d+) draft", str(res.get("counts", "")))
        if res.get("match") or not m or int(m.group(1)) + int(m.group(2)) > 8:
            continue
        open(os.path.join(wd, ".second"), "w").write(res["counts"])
        hint = os.path.join(wd, "hint.txt")  # a reviewer's note for this function, read once
        hint = (" A reviewer adds: " + open(hint).read().strip()) if os.path.exists(hint) else ""
        if resume(r, model, note=(f"You stopped at {res['counts']}: very close. Take one more round of up to 80"
                                   " compiles on the remaining hunks (why on each, confirm with a knob, then C;"
                                   " a pin is acceptable only as the last resort). scope has new commands since"
                                   " you started: `scope.py FILE.c probe` flips each cse/gcse decision"
                                   " (canonical register, jump follow, PRE) alone and names the one behind the"
                                   " difference, with the C lines involved; and a difference that is only a"
                                   " moved branch target is now explained (loop head or jump threading)."
                                   " Rerun scope on your best file first. Update the deliverables." + hint)):
            running.add(r["func"])
    for r in rows:  # resume workers a model error stopped, before starting new ones
        if len(running) < workers and state(r["func"]) == "failed" and \
                not os.path.exists(os.path.join(CAMP, r["func"], "gave_up")) and resume(r, model):
            running.add(r["func"])
    for r in rows:
        if len(running) >= workers:
            break
        if state(r["func"]) != "pending" or r.get("skip"):
            continue
        q = quota()
        if q is not None and q <= FLOOR:
            print(f"quota {q}% <= floor {FLOOR}%: not starting more workers")
            break
        start(r, model)
        running.add(r["func"])
    json.dump(rows, open(QUEUE, "w"), indent=1)
    if not running:
        print("nothing running; queue " + ("empty" if all(state(r["func"]) != "pending" for r in rows)
                                           else "has pending items (quota?)"))
        return 0
    for r in rows:  # finished while no tick was watching: report once
        wd = os.path.join(CAMP, r["func"])
        if state(r["func"]) == "done" and not os.path.exists(os.path.join(wd, ".reported")):
            open(os.path.join(wd, ".reported"), "w").write("")
            print(f"finished earlier {r['func']}: {summary(r['func'])}")
    print(f"{len(running)} running: {', '.join(sorted(running))}", flush=True)
    while True:
        done = [f for f in running if state(f) in ("done", "failed")]
        if done:
            for f in done:
                if state(f) == "done":
                    open(os.path.join(CAMP, f, ".reported"), "w").write("")
                print(f"finished {f}: {summary(f)}" if state(f) == "done" else
                      f"stopped {f}: {failed_turn(os.path.join(CAMP, f))} (the next tick resumes it)")
            return 0
        time.sleep(30)


def summary(func):
    wd = os.path.join(CAMP, func)
    try:
        r = json.load(open(os.path.join(wd, "result.json")))
        return (f"MATCH ({r.get('pins', '?')} pins, {r.get('barriers', '?')} barriers)" if r.get("match")
                else f"best {r.get('counts')}") + f", {r.get('compiles', '?')} compiles"
    except (OSError, ValueError):
        return "no result.json"


def cmd_status(args):
    rows = json.load(open(QUEUE))
    for r in rows:
        s = state(r["func"])
        if s == "pending":
            continue
        landed = os.path.exists(os.path.join(CAMP, r["func"], "landed"))
        print(f"{s:8s} {r['func']:45s} {r['size']:5d} B  {r.get('start_counts', ''):28s} "
              + (summary(r["func"]) if s == "done" else "") + ("  [landed]" if landed else ""))
    print(f"pending: {sum(1 for r in rows if state(r['func']) == 'pending')}; quota {quota()}%")


def block_span(text, func, addr):
    """(start, end) of FUNC's #ifdef NONMATCHING ... #endif block in TEXT."""
    for m in re.finditer(r"#ifdef NONMATCHING\n(.*?)#else\n(.*?)#endif\n?", text, re.S):
        names = re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', m.group(2))
        if func in names or "sub_" + addr in names:
            return m.start(), m.end()
    return None


def unwrap(text, func, addr):
    """The matching C without its #ifdef NONMATCHING / #else INCLUDE_ASM / #endif wrapper and fuzzy notes."""
    for m in re.finditer(r"#ifdef NONMATCHING\n(.*?)#else\n(.*?)#endif\n?", text, re.S):
        names = re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', m.group(2))
        if func in names or "sub_" + addr in names:
            body = "".join(ln for ln in m.group(1).splitlines(True)
                           if not re.match(r"\s*// (fuzzy:|Remaining differences)", ln))
            return text[:m.start()] + body + text[m.end():]
    return None


def cmd_land(args):
    func, tree = args[0], os.path.abspath(os.path.expanduser(args[1]))
    rows = {r["func"]: r for r in json.load(open(QUEUE))}
    r = rows[func]
    wd = os.path.join(CAMP, func)
    cand = os.path.join(wd, "best_match.c")
    rel = os.path.relpath(cand, ROOT)
    out = sh("python3", "tools/scope/scope.py", rel, func, env=dict(os.environ, SCOPE_RUN=func))
    if "\nMATCH" not in out:
        sys.exit(f"{func}: best_match.c does not match:\n{out[:600]}")
    new = unwrap(open(cand).read(), func, r["addr"])
    if new is None:
        sys.exit(f"{func}: no NONMATCHING block for it in best_match.c")
    # the merge base is the file as it was on main when the worker started (workers may edit base.c)
    revf = os.path.join(wd, ".main_rev")
    rev = open(revf).read().strip() if os.path.exists(revf) else (args[2] if len(args) > 2 else None)
    btext = sh("git", "show", f"{rev}:{r['src']}") if rev else open(os.path.join(wd, "base.c")).read()
    base = unwrap(btext, func, r["addr"]) or btext
    target = os.path.join(tree, r["src"])
    cur = open(target).read()
    curu = unwrap(cur, func, r["addr"])
    if curu is None:
        sys.exit(f"{func}: {r['src']} in {tree} has no NONMATCHING block for it (landed already?)")
    import tempfile
    with tempfile.TemporaryDirectory() as tmp:
        a, b, c = (os.path.join(tmp, n) for n in ("cur", "base", "new"))
        open(a, "w").write(curu)
        open(b, "w").write(base)
        open(c, "w").write(new)
        rc = subprocess.run(["git", "merge-file", a, b, c]).returncode
        if rc:
            # main changed the draft itself meanwhile (e.g. a fix.py sweep): a match supersedes any draft,
            # so if everything outside this function's NONMATCHING block is as the worker saw it, swap
            # the block for the worker's matching C
            span = block_span(cur, func, r["addr"])
            bspan = block_span(btext, func, r["addr"])
            nspan = block_span(open(cand).read(), func, r["addr"])
            if not (span and bspan and nspan) or cur[:span[0]] + cur[span[1]:] != btext[:bspan[0]] + btext[bspan[1]:]:
                sys.exit(f"{func}: merge conflict against the current {r['src']}; land by hand")
            wtext = open(cand).read()
            wun = unwrap(wtext[nspan[0]:nspan[1]], func, r["addr"])
            merged = cur[:span[0]] + wun + cur[span[1]:]
            print(f"{func}: main changed the draft since the worker started; replaced the draft with the match")
        else:
            merged = open(a).read()
    # worker notes pointing at their own REPORT.md / build paths do not belong in src
    merged = re.sub(r"^\s*//[^\n]*(REPORT\.md|build/scope)[^\n]*\n", "", merged, flags=re.M)
    pins = len(re.findall(r'\basm\("r\d+"\)', merged))
    bars = len(re.findall(r'\basm(?:\s+(?:volatile|__volatile__))?\s*\(\s*""', merged))
    marker = "// MATCHING_CONCESSION: register-pinning"
    if (pins or bars) and marker not in merged:
        merged = marker + "\n" + merged
    if not (pins or bars) and marker in merged:
        merged = re.sub(r"^// MATCHING_CONCESSION: register-pinning\n", "", merged, flags=re.M)
    open(target, "w").write(merged)
    subprocess.run(["make", "format"], cwd=tree, capture_output=True)
    chk = subprocess.run(["python3", "tools/check.py", r["src"]], cwd=tree, capture_output=True, text=True)
    last = (chk.stdout + chk.stderr).strip().splitlines()[-1:]
    ok = bool(last) and ": MATCH" in last[0]
    print(f"{func}: {'unit MATCH' if ok else 'UNIT MISMATCH'} in {tree} ({pins} pins, {bars} barriers in the file)")
    if ok:
        open(os.path.join(wd, "landed"), "w").write(tree)
    return 0 if ok else 1


if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else "status"
    sys.exit({"queue": cmd_queue, "tick": cmd_tick, "status": cmd_status, "land": cmd_land}[cmd](sys.argv[2:]))
