#!/usr/bin/env python3
"""Run scope workers (Codex) over a queue of NONMATCHING drafts, a few at a time.

  campaign.py queue [--category Bosses] [--all]    write build/scope/campaign/queue.json from origin/main
  campaign.py queue --undrafted [--reverse] [--max-size B]   phase 1: functions with no C yet (scope todo),
                                                   ARM ones left out; --reverse starts from the ROM's end
  campaign.py tick [--workers 4] [--model M] [--floor 15]   keep WORKERS running; return when one finishes
  campaign.py integrate TREE [--limit N] [--no-pr]   land every finished phase-1 result into TREE on a fresh
                                                   branch from origin/main, one commit each; then one ROM
                                                   build, PROGRESS, and a PR merged when the build is OK
  campaign.py harvest [--all]                      land-ready without a worker: pending functions whose `new`
                                                   seed (a matched twin's C) matches as it is
  campaign.py submit FUNC FILE [--name N] [--note T]   a draft of the coordinator's own, landed by integrate
  campaign.py review [--model M] | review --say MSG   an independent reviewer of scope itself
                                                   (build/scope/campaign/review/REVIEW.md); --say continues it
  campaign.py status                               what ran, what matched, what is in flight
  campaign.py land FUNC WORKTREE [REV]                  verify FUNC's best_match.c, unwrap its NONMATCHING block,
                                                   three-way merge it into WORKTREE's file, check the unit
                                                   (a phase-1 draft: `scope.py land` in WORKTREE, exact if
                                                   best_match.c matches, else best.c --nonmatching)

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
- Commands (WORKER.md, `scope.py --help`): why UID / why all, probe, spill, try (score several variant
  files of one hypothesis in one call, ~12 ms each), life, vs, pins, insn, uses.
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

DRAFT_BRIEF = """You are a worker on a GBA decompilation project (Boktai 3). Your job: write C for one function that has
none yet, and drive it as close to a byte-for-byte match with the original ROM as you can, using the `scope`
tool, in C the original authors could have written. Several other workers run at the same time on other
functions; you never touch the source tree.

Read first (you are in the scope checkout; run every command from its root):
- tools/scope/WORKER.md: the matching loop (why, knobs, try, probe, life, vs, pins).
- tools/scope/README.md: grep the section "agbcc facts established with scope" when a difference looks
  familiar (allocation order, loop motion, slot order, small forms). CLAUDE.md "Compiler facts".

Your function: {func} at {addr} ({size} bytes{arm}). Your directory: {wd}.
- {wd}/start.c is scope's workspace for it, opened with `scope.py new` ({seed}): the whole unit (or a seed
  file) with an empty definition of {func} where you write the body. Keep the `// scope: write ... here`
  line: it tells scope which function the file is about. Work on copies in your directory only.
- Prefix every scope command with SCOPE_RUN={run}:
    SCOPE_RUN={run} python3 tools/scope/scope.py {wd}/start.c        compile, align with the ROM, list hunks
  Use a copy (cp {wd}/start.c {wd}/d1.c) for each line of attack; `try` scores several copies at once.

Phase 1, a first draft (spend about a third of your effort here):
1. Read the ROM: `scope.py rom {func}` (calls and literal symbols named). `scope.py like {func}` lists matched
   functions with the most similar code, with shared calls and constants; read the closest one's C (grep its
   name in src/) for types, struct fields and idioms. Neighbors in the same unit file are the best guide to
   the structs and helpers in use.
2. Structs: `scope.py fields OFF... [STRUCT]` names include/ structs with members at the offsets the ROM
   reads. Use fields where a struct fits; if none does, declare a local struct in the file (padding
   members named unk_XX) rather than pointer arithmetic. Prototypes for callees: grep include/ and src/
   for the name; declare missing ones in the file with the types the call sites need.
3. Ghidra pseudo-C (if `new` put one in the file) is a reading aid only; write C in the style of the unit.
4. Compile until it builds (agbcc is C89: declarations first in each block; `COMPILE FAILED` = fix that
   first) and the control flow and calls line up with the ROM: same calls in the same order, same loops.

Phase 2, toward the match: follow WORKER.md's loop (why on the first difference of each hunk, confirm with a
knob, then C). `frame` compares stack slots; `hoist` explains loop-invariant motion and scores the dead
re-set lever; `probe` flips each cse/gcse/loop decision alone; `spill` checks reload rotation.

Rules:
- No hand-written instructions, ever. Register pins (`register T x asm("rN")`) and empty `asm("" : "+r"(x))`
  barriers are a last resort, only where a knob proved an allocation decision you could not reproduce in C
  after real attempts; say so in the report.
- Keep the C readable: struct fields and names, no magic offsets where a struct fits. Comments in the C must
  stand alone: do not mention REPORT.md, your directory or your experiments.
- Edit nothing outside {wd} (not src/, symbols/, tools/, include/); do not commit, push or run make. Never use
  `rm -rf` (move files to ~/.Trash). Do not run whole-corpus checks.
- Budget: about 150 compiles. A good draft that does not match is a fine result: stop when `why`-backed ideas
  run out.

Deliverables in {wd}:
- best.c: your best file (the whole file, as start.c is), matching or not. best_match.c as well if it MATCHes.
- result.json: {{"match": true/false, "counts": "N ROM / M draft" (or "MATCH"), "pins": P, "barriers": B,
  "compiles": C, "name": "Prefix_Action or null", "note": "one line: what the function does"}}
  (name: a provisional name in the project's Prefix_Action style, one prefix per object, like its unit's
  neighbors; null if you cannot tell.)
- REPORT.md: what the function does; for each step that helped, what scope said and what you changed; what
  is left and its diagnosis; and, as much as the C itself, **where scope's output was confusing, wrong or
  missing, and what command or output would have saved you time**.
Your last message: a five-line summary of the same.
"""

REVIEW_BRIEF = """You are an independent reviewer of `scope`, the agent-facing decompilation IDE in tools/scope/ of
this GBA decompilation project (Boktai 3; agbcc, GCC 2.95 Thumb). Its purpose: let an agent take a function
from nothing to a byte-exact C match by asking the compiler why, instead of trying C forms blindly. Phase 1
is writing a first draft for functions with no C (`todo`, `new`, `rom`, `like`, `fields`); phase 2 drives a
draft to a match (`why`, knobs, `try`, `probe`, `spill`, `frame`, `hoist`, `life`, `vs`, `pins`); `land`
puts the result in the tree. Other agents are using it right now on other functions.

Read tools/scope/README.md, tools/scope/WORKER.md and `python3 tools/scope/scope.py --help`, then the code
(tools/scope/scope.py, land.py, structure.py, fast.py, campaign.py, agbcc.patch). Use it yourself on real
functions: pick two from `python3 tools/scope/scope.py todo 300` near the end of the list and one existing
draft (grep -l NONMATCHING src/*.c), work them in build/scope/campaign/review/ with SCOPE_RUN=review, and
note every point where the tool slowed you down, misled you, or left you guessing.

Write build/scope/campaign/review/REVIEW.md, kept current as you go (rewrite it, do not only append):
- Top findings, ranked by how much agent time each would save across the remaining ~250 undrafted and
  ~290 drafted functions. For each: the evidence (command, output, what was wrong or missing), and a
  concrete change (which function in which file, what it should print or do).
- Bugs (wrong output, crashes) with a reproducer command.
- Docs that are wrong, stale or missing.
- What works well and should not change.
You may make small, safe fixes yourself in tools/scope/ (bugs with a clear reproducer), each listed in
REVIEW.md with its diff summary; do not touch src/, symbols/ or include/; do not commit, push or run make;
never `rm -rf`. The coordinator (another agent) reads REVIEW.md periodically and may send you questions or
ask you to look at something specific; answer those at the top of REVIEW.md under "Replies".
Your last message: the five most valuable findings, one line each.
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


def undrafted(args):
    """Phase-1 queue rows from `scope.py todo`: functions with no C yet, ARM ones (no scope compile) left out."""
    out = sh("python3", "tools/scope/scope.py", "todo", "100000")
    rows = []
    for m in re.finditer(r"^  ([0-9A-F]{8})\s+(\d+) B .*?  (\w+)\s+like .*$", out, re.M):
        if "[ARM" in m.group(0):
            continue
        rows.append({"func": m.group(3), "addr": m.group(1), "size": int(m.group(2)), "kind": "draft"})
    if "--max-size" in args:
        rows = [r for r in rows if r["size"] <= int(args[args.index("--max-size") + 1])]
    if "--reverse" in args:
        rows.reverse()
    return rows


def cmd_queue(args):
    if "--undrafted" in args:
        rows = undrafted(args)
        os.makedirs(CAMP, exist_ok=True)
        old = json.load(open(QUEUE)) if os.path.exists(QUEUE) else []
        seen = {r["func"] for r in old}
        merged = old + [r for r in rows if r["func"] not in seen]
        json.dump(merged, open(QUEUE, "w"), indent=1)
        print(f"{len(rows)} undrafted functions; queue now {len(merged)}")
        return 0
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
    if os.path.exists(os.path.join(wd, "harvested")) or os.path.exists(os.path.join(wd, "submitted")):
        return "done"
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


def start_draft(r, model, harvest_only=False):
    """A phase-1 worker: scope's workspace for FUNC (`scope.py new`) copied into its directory."""
    func = r["func"]
    wd = os.path.join(CAMP, func)
    os.makedirs(wd, exist_ok=True)
    env = dict(os.environ, SCOPE_RUN=func)
    out = sh("python3", "tools/scope/scope.py", "new", func, env=env)
    m = re.search(r"(build/scope/(?:[^\s/]+/)?work/[^\s/:]+\.c)", out)  # SCOPE_RUN: under build/scope/RUN/
    work = os.path.join(ROOT, m.group(1)) if m else ""
    if not os.path.exists(work):
        r["skip"] = "scope new failed: " + out.strip().splitlines()[-1:][0] if out.strip() else "scope new failed"
        print(f"skip {func}: {r['skip']}", flush=True)
        return False
    import shutil
    shutil.copy(work, os.path.join(wd, "start.c"))
    if os.path.exists(work[:-2] + ".base.c"):
        shutil.copy(work[:-2] + ".base.c", os.path.join(wd, "base.c"))
    open(os.path.join(wd, ".main_rev"), "w").write(sh("git", "rev-parse", "HEAD").strip())
    m = re.search(r"draft \S+: (.*)", out)
    seed = m.group(1).strip() if m else ("an earlier workspace: read what is there first" if "exists; continuing" in out
                                         else "an empty definition")
    rel = os.path.relpath(wd, ROOT)
    r["start_counts"] = "undrafted"
    if re.search(r"^  now: MATCH", out, re.M):
        # the seed (a matched twin's C) is exact already: no worker needed
        shutil.copy(work, os.path.join(wd, "best.c"))
        shutil.copy(work, os.path.join(wd, "best_match.c"))
        json.dump({"match": True, "counts": "MATCH", "pins": len(re.findall(r'\basm\("r\d+"\)', open(work).read())),
                   "barriers": 0, "compiles": 1, "name": None, "note": "seeded: " + seed},
                  open(os.path.join(wd, "result.json"), "w"))
        open(os.path.join(wd, "harvested"), "w").write(seed)
        print(f"harvested {func}: {seed} matches as seeded", flush=True)
        return False
    if harvest_only:
        return False
    brief = DRAFT_BRIEF.format(func=func, addr=r["addr"], size=r["size"], wd=rel, run=func, seed=seed, arm="")
    return launch(wd, brief, model, r)


def launch(wd, brief, model, r):
    open(os.path.join(wd, "brief.txt"), "w").write(brief)
    log = open(os.path.join(wd, "codex.json"), "w")
    p = subprocess.Popen(["codex", "exec", "-m", model, "-s", "workspace-write", "-C", ROOT, "-c",
                          "model_reasoning_effort=high", "--json", "-o", os.path.join(wd, "last.txt"), brief],
                         cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                         start_new_session=True)
    open(os.path.join(wd, "pid"), "w").write(str(p.pid))
    open(os.path.join(wd, "started"), "w").write(time.strftime("%Y-%m-%d %H:%M:%S"))
    print(f"started {r['func']} ({r['size']} B, {r.get('start_counts')})", flush=True)
    return True


def start(r, model):
    func = r["func"]
    if r.get("kind") == "draft":
        return start_draft(r, model)
    wd = os.path.join(CAMP, func)
    os.makedirs(wd, exist_ok=True)
    text = sh("git", "show", f"origin/main:{r['src']}")
    if block_span(text, func, r["addr"]) is None:  # matched on main since the queue was built
        r["skip"] = "matched on main"
        open(os.path.join(wd, "landed"), "w").write("main")
        print(f"skip {func}: no NONMATCHING block for it on main (matched already)", flush=True)
        return False
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
    return launch(wd, brief, model, r)


def cmd_tick(args):
    workers = int(args[args.index("--workers") + 1]) if "--workers" in args else 4
    model = args[args.index("--model") + 1] if "--model" in args else "gpt-6.1-sol"
    floor = int(args[args.index("--floor") + 1]) if "--floor" in args else FLOOR
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
        again = os.path.exists(os.path.join(wd, "again"))  # a reviewer asks for one more round anyway
        if res.get("match") or not (again or (m and int(m.group(1)) + int(m.group(2)) <= 8)):
            continue
        open(os.path.join(wd, ".second"), "w").write(str(res.get("counts")))
        hint = os.path.join(wd, "hint.txt")  # a reviewer's note for this function, read once
        hint = (" A reviewer adds: " + open(hint).read().strip()) if os.path.exists(hint) else ""
        if resume(r, model, note=(f"You stopped at {res['counts']}" + (": a reviewer has a lead (below)." if again else ": very close.")
                                   + " Take one more round of up to 80"
                                   " compiles on the remaining hunks (why on each, confirm with a knob, then C;"
                                   " a pin is acceptable only as the last resort). scope has new commands since"
                                   " you started: `scope.py FILE.c probe` flips each cse/gcse/loop decision"
                                   " (canonical register, jump follow, PRE, loop strength reduction) alone and names the one behind the"
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
        if q is not None and q <= floor:
            print(f"quota {q}% <= floor {floor}%: not starting more workers")
            break
        if start(r, model):
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


def land_draft(func, tree, wd):
    """A phase-1 result through `scope.py land` in TREE: its unit merge against the workspace base the
    worker started from, the check of every function in the file, the provisional name."""
    import shutil
    exact = os.path.exists(os.path.join(wd, "best_match.c"))
    cand = os.path.join(wd, "best_match.c" if exact else "best.c")
    if not os.path.exists(cand):
        sys.exit(f"{func}: no best.c")
    work = os.path.join(tree, "build", "scope", "work")
    os.makedirs(work, exist_ok=True)
    if os.path.exists(os.path.join(wd, "base.c")):
        shutil.copy(os.path.join(wd, "base.c"), os.path.join(work, func + ".base.c"))
    try:
        res = json.load(open(os.path.join(wd, "result.json")))
    except (OSError, ValueError):
        res = {}
    cmd = ["python3", "tools/scope/scope.py", "land", func, cand]
    name = res.get("name")
    if name and re.fullmatch(r"[A-Z]\w*_[A-Z]\w*", str(name)):
        cmd += ["--name", name] + (["--note", res["note"]] if res.get("note") else [])
    if not exact:
        cmd.append("--nonmatching")
    env = {k: v for k, v in os.environ.items() if k != "SCOPE_RUN"}  # the base was copied to the unscoped workspace
    p = subprocess.run(cmd, cwd=tree, capture_output=True, text=True, env=env)
    if p.returncode and "is already used" in p.stdout + p.stderr and "--name" in cmd:
        # the worker's name is taken: land the code anyway, under its sub_ label
        print((p.stdout + p.stderr).strip() + "\n  landing without the name")
        i = cmd.index("--name")
        cmd = cmd[:i] + cmd[i + 2:]
        if "--note" in cmd:
            j = cmd.index("--note")
            cmd = cmd[:j] + cmd[j + 2:]
        p = subprocess.run(cmd, cwd=tree, capture_output=True, text=True, env=env)
    print((p.stdout + p.stderr).strip())
    if p.returncode == 0:
        open(os.path.join(wd, "landed"), "w").write(tree)
    return p.returncode


def cmd_harvest(args):
    """Open every pending phase-1 function with `scope.py new` and keep the ones whose seed matches
    as it is (a twin's C): integrate lands them like worker results, and no worker is spent on them."""
    if "--all" in args:  # also the undrafted functions the queue left out (large ones)
        cmd_queue(["--undrafted"])
    rows = json.load(open(QUEUE))
    n = 0
    for r in rows:
        if r.get("kind") == "draft" and state(r["func"]) == "pending" and not r.get("skip"):
            if not start_draft(r, None, harvest_only=True) and os.path.exists(os.path.join(CAMP, r["func"], "harvested")):
                n += 1
    json.dump(rows, open(QUEUE, "w"), indent=1)
    print(f"{n} functions match as seeded")
    return 0


def cmd_submit(args):
    """Hand a draft written outside the campaign (the coordinator's own) to integrate:
    submit FUNC FILE [--name NAME] [--note TEXT]. FILE is a workspace copy (`scope.py new`); its base
    is the workspace's, so landing merges it like a worker's result."""
    import shutil
    func, src = args[0], os.path.abspath(os.path.expanduser(args[1]))
    wd = os.path.join(CAMP, func)
    os.makedirs(wd, exist_ok=True)
    env = {k: v for k, v in os.environ.items() if k != "SCOPE_RUN"}
    out = subprocess.run(["python3", "tools/scope/scope.py", src, func], cwd=ROOT, capture_output=True, text=True,
                         env=env).stdout
    exact = "\nMATCH" in out
    m = re.search(r"(\d+ ROM / \d+ draft)", out)
    shutil.copy(src, os.path.join(wd, "best.c"))
    if exact:
        shutil.copy(src, os.path.join(wd, "best_match.c"))
    # the workspace's base sits next to the draft; a copy made elsewhere finds it in any scope run
    base = os.path.join(os.path.dirname(src), func + ".base.c")
    if not os.path.exists(base):
        import glob
        found = sorted(glob.glob(os.path.join(ROOT, "build", "scope", "*", "work", func + ".base.c"))
                       + glob.glob(os.path.join(ROOT, "build", "scope", "work", func + ".base.c")), key=os.path.getmtime)
        base = found[-1] if found else base
    if os.path.exists(base):
        shutil.copy(base, os.path.join(wd, "base.c"))
    elif os.path.exists(os.path.join(wd, "base.c")):
        # an earlier attempt's base describes the unit as it was then: merging against it could undo neighbors
        os.replace(os.path.join(wd, "base.c"), os.path.join(wd, "base.c.stale"))
        print(f"{func}: no workspace base for {args[1]}; dropped the earlier attempt's base.c")
    name = args[args.index("--name") + 1] if "--name" in args else None
    note = args[args.index("--note") + 1] if "--note" in args else None
    json.dump({"match": exact, "counts": "MATCH" if exact else (m.group(1) if m else "?"), "pins": 0, "barriers": 0,
               "compiles": 0, "name": name, "note": note}, open(os.path.join(wd, "result.json"), "w"))
    open(os.path.join(wd, "submitted"), "w").write(src)
    rows = json.load(open(QUEUE)) if os.path.exists(QUEUE) else []
    if not any(r["func"] == func for r in rows):
        rows.append({"func": func, "addr": func[-8:], "size": 0, "kind": "draft"})
    for r in rows:
        if r["func"] == func:
            r["kind"] = "draft"
    json.dump(rows, open(QUEUE, "w"), indent=1)
    print(f"{func}: submitted ({'MATCH' if exact else (m.group(1) if m else '?')}); the next integrate lands it")
    return 0


def mismatch_files(tree, out, touched):
    """A ROM MISMATCH names no file, only its first differing address: the touched src/ files that
    define the function containing that address (by functions.csv start), so its landing is dropped."""
    m = re.search(r"first difference at 0x([0-9a-fA-F]+)", out)
    if not m:
        return set()
    addr = int(m.group(1), 16)
    starts = []
    with open(os.path.join(tree, "symbols", "functions.csv")) as f:
        for row in f:
            c = row.split(",")
            if len(c) > 1 and re.fullmatch(r"[0-9A-Fa-f]{8}", c[0]):
                starts.append((int(c[0], 16), c[1]))
    before = [s for s in starts if s[0] <= addr]
    if not before:
        return set()
    a, name = max(before)
    pat = re.compile(r"\b(?:%s|sub_%08X)\s*\(" % (re.escape(name), a))
    found = set()
    for f in touched:
        path = os.path.join(tree, f)
        if f.startswith("src/") and f.endswith(".c") and os.path.exists(path) and pat.search(open(path).read()):
            found.add(f)
    return found


def cmd_integrate(args):
    tree = os.path.abspath(os.path.expanduser(args[0]))
    limit = int(args[args.index("--limit") + 1]) if "--limit" in args else 10 ** 6
    def git(*a):
        return subprocess.run(["git", *a], cwd=tree, capture_output=True, text=True)
    if git("status", "--porcelain", "--", "src", "include", "symbols").stdout.strip():
        sys.exit(f"{tree}: src/, include/ or symbols/ has uncommitted changes")
    rows = json.load(open(QUEUE))
    ready = [r for r in rows if r.get("kind") == "draft" and state(r["func"]) == "done"
             and not os.path.exists(os.path.join(CAMP, r["func"], "landed"))
             and not os.path.exists(os.path.join(CAMP, r["func"], "land_failed"))
             and os.path.exists(os.path.join(CAMP, r["func"], "best.c"))][:limit]
    if not ready:
        print("nothing to integrate")
        return 0
    # keep a failed attempt inspectable even when its retry starts in the same minute
    branch = "claude/codex-drafts-" + time.strftime("%m%d-%H%M%S")
    fetched = git("fetch", "-q", "origin")
    if fetched.returncode:
        sys.exit(f"{tree}: fetch failed; nothing integrated\n" + fetched.stderr)
    if git("checkout", "-q", "-B", branch, "origin/main").returncode:
        sys.exit(f"{tree}: could not check out {branch}")
    done, landed, commits = [], [], {}

    def release_landings(reason):
        for wd in landed:
            marker = os.path.join(wd, "landed")
            if os.path.exists(marker):
                os.replace(marker, marker + "." + reason)

    def built(b):
        return b.returncode == 0 and "build/boktai3.gba: OK" in (b.stdout + b.stderr).splitlines()

    for r in ready:
        func, wd = r["func"], os.path.join(CAMP, r["func"])
        rc = land_draft(func, tree, wd)
        if rc:
            open(os.path.join(wd, "land_failed"), "w").write(str(rc))
            for p in git("status", "--porcelain", "--", "src", "include", "symbols").stdout.splitlines():
                if p.startswith("??"):  # never leave a half-landed file behind
                    import shutil
                    shutil.move(os.path.join(tree, p[3:]), os.path.expanduser(f"~/.Trash/{os.path.basename(p[3:])}.{int(time.time())}"))
            git("checkout", "--", "src", "include", "symbols")
            continue
        try:
            res = json.load(open(os.path.join(wd, "result.json")))
        except (OSError, ValueError):
            res = {}
        name = res.get("name") if res.get("name") and re.fullmatch(r"[A-Z]\w*_[A-Z]\w*", str(res.get("name"))) else func
        exact = os.path.exists(os.path.join(wd, "best_match.c"))
        label = f"{name} ({func})" if name != func else func
        title = f"Match {label}" if exact else f"Draft {label} (NONMATCHING, {res.get('counts', '?')})"
        landed.append(wd)
        added = git("add", "-A", "src", "include", "symbols")
        committed = (git("commit", "-q", "-m", title + "\n\nPhase-1 scope campaign worker.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>")
                     if added.returncode == 0 else added)
        if committed.returncode:
            release_landings("commit_failed")
            sys.exit(f"{tree}: commit failed on {branch}; changes kept for review, retry markers released\n"
                     + committed.stdout + committed.stderr)
        commits[wd] = (git("rev-parse", "HEAD").stdout.strip(),
                       set(git("show", "--name-only", "--format=", "HEAD").stdout.split()))
        done.append(title)
        print(title, flush=True)
    if not done or "--no-pr" in args:
        return 0
    b = subprocess.run(["make", "-j8"], cwd=tree, capture_output=True, text=True)
    for _ in range(10):  # make stops at its first failing file, so each round finds one culprit
        if built(b):
            break
        # the build names the file it failed on: drop the commits that touched it and build the rest,
        # so one bad draft does not hold the whole batch back
        out = b.stdout + b.stderr
        # a diagnostic names its file first (`src/x.c: ...`, `build/src/x.s:5: Error ...`); other error lines
        # name it anywhere
        errors = "\n".join(ln for ln in out.splitlines()
                           if re.match(r"\s*(?:\S*/)?(?:build/)?src/[\w/]+\.[cso]:", ln)
                           or re.search(r"(?i)error|failed|must be|undefined|can't|cannot", ln))
        files = {f"src/{m}.c" for m in re.findall(r"(?:build/)?src/([\w/]+)\.[cso]\b", errors)}
        files |= mismatch_files(tree, out, [f for wd in landed for f in commits[wd][1]])
        bad = [wd for wd in landed if commits[wd][1] & files]
        if not bad or len(bad) == len(landed):
            break
        for wd in bad:
            os.replace(os.path.join(wd, "landed"), os.path.join(wd, "land_failed"))
            open(os.path.join(wd, "land_failed"), "a").write("\ncombined build failed on its file:\n" + out[-1500:])
            i = landed.index(wd)
            print(f"dropped {os.path.basename(wd)}: the combined build failed on {', '.join(sorted(commits[wd][1] & files))}",
                  flush=True)
            del landed[i], done[i]
        git("reset", "-q", "--hard", "origin/main")
        if git("cherry-pick", *[commits[wd][0] for wd in landed]).returncode:
            git("cherry-pick", "--abort")
            break
        b = subprocess.run(["make", "-j8"], cwd=tree, capture_output=True, text=True)
    if not built(b):
        # nothing reached main: the next integrate retries them (the culprit fails its own unit check
        # or the build again); the commits stay on the local branch for a look
        release_landings("build_failed")
        sys.exit(f"{tree}: combined build failed on {branch}; not opening a PR\n" + (b.stdout + b.stderr)[-2000:])
    subprocess.run(["python3", "tools/progress.py", "--markdown", "PROGRESS.md"], cwd=tree, capture_output=True)
    git("add", "PROGRESS.md", "PROGRESS.json")
    git("commit", "-q", "-m", "Regenerate PROGRESS.md and PROGRESS.json\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>")
    pii = subprocess.run(["python3", "tools/pii_check.py", "--range", "origin/main..HEAD"], cwd=tree,
                         capture_output=True, text=True)
    if pii.returncode:
        sys.exit("pii_check failed:\n" + pii.stdout + pii.stderr)
    git("push", "-q", "-u", "origin", branch)
    body = ("Batch integration of phase-1 scope campaign workers, one commit per function:\n\n"
            + "\n".join(f"- {t}" for t in done) + "\n\n`build/boktai3.gba: OK`; PROGRESS regenerated.\n\n"
            "🤖 Generated with [Claude Code](https://claude.com/claude-code)")
    pr = subprocess.run(["gh", "pr", "create", "--head", branch, "--title",
                         f"Phase-1 campaign batch: {len(done)} functions", "--body", body],
                        cwd=tree, capture_output=True, text=True).stdout.strip().splitlines()[-1:]
    if pr:
        subprocess.run(["gh", "pr", "merge", pr[0], "--merge"], cwd=tree, capture_output=True)
        print(f"merged {pr[0]}")
    return 0


def cmd_review(args):
    """Start the scope reviewer, or (--say MSG) continue its session with a message from the coordinator."""
    wd = os.path.join(CAMP, "review")
    os.makedirs(wd, exist_ok=True)
    model = args[args.index("--model") + 1] if "--model" in args else "gpt-6.1-sol"
    if "--say" in args:
        if state("review") == "running":
            sys.exit("the reviewer is mid-turn; say it once its turn ends (campaign.py status shows it)")
        head = open(os.path.join(wd, "codex.json")).read(4000)
        m = re.search(r'"thread_id":"([0-9a-f-]+)"', head)
        log = open(os.path.join(wd, "codex.json"), "a")
        p = subprocess.Popen(["codex", "exec", "resume", "-c", "model_reasoning_effort=high", "--json", "-o",
                              os.path.join(wd, "last.txt"), m.group(1), args[args.index("--say") + 1]],
                             cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                             start_new_session=True)
        open(os.path.join(wd, "pid"), "w").write(str(p.pid))
        print("sent; the reply lands in REVIEW.md (Replies) and last.txt")
        return 0
    if state("review") == "running":
        sys.exit("the reviewer is running")
    return 0 if launch(wd, REVIEW_BRIEF, model, {"func": "review", "size": 0, "start_counts": "review"}) else 1


def cmd_land(args):
    func, tree = args[0], os.path.abspath(os.path.expanduser(args[1]))
    rows = {r["func"]: r for r in json.load(open(QUEUE))}
    r = rows[func]
    wd = os.path.join(CAMP, func)
    if r.get("kind") == "draft":
        return land_draft(func, tree, wd)
    cand = os.path.join(wd, "best_match.c")
    rel = os.path.relpath(cand, ROOT)
    out = sh("python3", "tools/scope/scope.py", rel, func, env=dict(os.environ, SCOPE_RUN=func))
    if "\nMATCH" not in out:
        # main may have changed headers or other functions since the worker started, so the worker's
        # whole file can stop compiling; the unit check on the merged tree below decides
        print(f"{func}: best_match.c no longer checks out on its own (main moved on); merging anyway")
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
    if not ok:
        # never leave a mismatching unit in the tree for a later commit to pick up
        open(target, "w").write(cur)
        sys.exit(f"{func}: restored {r['src']}; land by hand")
    open(os.path.join(wd, "landed"), "w").write(tree)
    return 0


if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else "status"
    sys.exit({"queue": cmd_queue, "tick": cmd_tick, "status": cmd_status, "land": cmd_land,
              "review": cmd_review, "integrate": cmd_integrate, "harvest": cmd_harvest,
              "submit": cmd_submit}[cmd](sys.argv[2:]))
