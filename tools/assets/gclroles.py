#!/usr/bin/env python3
"""Working names for the scene scripts' unnamed procedures, from what they do.

  gclroles.py                 coverage, and how many procedures each role names
  gclroles.py PROC...         a procedure's name and the evidence for it
  gclroles.py --csv [OUT]     every generated name as proc.csv rows (to promote by hand)

A scene file (tools/assets/gclfiles.py) is its entry procedure and the
procedures only it reaches, and those follow a fixed shape: the entry calls a
stage setup, the doors and entrances, the enemies, the objects, the music, and
hands callbacks to cutscenes, message windows and entrances. This names them
after that shape:

* a scene entry: its area (tools/assets/areas.py labels) and id,
  `white_forest_3880`; `scene_N` where the area has no label;
* a procedure private to one scene: the scene's name and a role,
  `white_forest_3874_doors`, `sealed_prison_2744_after_cutscene_162`.

A role comes from how the procedure is referred to (a cutscene's `onfinish=`,
an entrance's arrival callback, `message_then(proc=...)`), else from its
body: the library files (gcl/names/files.csv) its calls belong to, and the
native routines it starts (SceneMessages_ScriptStart, the message window,
the camera). The first rule that applies wins (ROLES below lists them);
a procedure no rule fits keeps `proc_N`. Repeats in one scene take `_2`, `_3`
in id order.

These are names of the reading kind, at confidence `auto`: gcl/names/proc.csv
always wins, and promoting a name there (with a better one if you have it)
is how it becomes ours. They are computed from the ROM's scripts every time,
so nothing generated is stored.
"""
import collections
import csv
import functools
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.dirname(HERE))

# area label (data/labels/areas.csv, start of the text) -> name prefix
AREA_PREFIX = [
    ("San Miguel", "san_miguel"),
    ("Sealed Prison", "sealed_prison"),
    ("Lifeless Town route", "route_lifeless_town"),
    ("Lifeless Town", "lifeless_town"),
    ("Ancient Tree route", "route_ancient_tree"),
    ("Ancient Tree", "ancient_tree"),
    ("White Forest route", "route_white_forest"),
    ("White Forest", "white_forest"),
    ("Pirate Island route", "route_pirate_island"),
    ("Pirate Island", "pirate_island"),
    ("Dark Castle A", "dark_castle_a"),
    ("Dark Castle B", "dark_castle_b"),
    ("Dark Castle", "dark_castle"),
    ("Paradise", "paradise"),
    ("Arena", "arena"),
    ("Menu", "menu"),
    ("Mission", "mission"),
]

# library file (gcl/names/files.csv) -> role of a procedure whose calls are mostly there
FILE_ROLE = {
    "scene_setup": "stage", "doors": "doors", "player": "player", "events": "event",
    "objects": "objects", "items": "items", "actors": "characters", "enemy_rules": "enemies",
    "enemy_spawners": "enemies", "progress": "story", "managers": "managers",
}

# the rules, in order, for the summary (gclroles.py with no arguments)
ROLES = """\
after_cutscene_N   onfinish= of SceneMessages_ScriptStart(scene=N)
cutscene_N         starts cutscene N (SceneMessages_ScriptStart(scene=N))
cutscene_N_step    a step callback in a cutscene's scripts=[...]
on_arrival         the callback of an entrance (entrance_*): runs when the player arrives there
after_message      message_then(proc=...) / open_message_window(proc=...): runs when the message closes
on_KEY             any other on_ keyword callback (on_press=, on_release=; on_proc= takes the callee: on_toggle_switch)
stage              loads the stage (load_stage, load_stage_raw)
music              play_bgm only (with conditions)
exit_to_SCENE      one door_transition*: leaves for SCENE
doors / entrances  door and entrance procedures
talk               sets a message table (MessageWindow_SetMessageTable)
story_N            advance_story(value=N), with HUD and continue cleanup
camera / signal    Camera_* / mesg only
route              set_actor_route only
choice             a choice menu
exit               leaves through a door_transition among other things
set_state          assignments only, no calls
FILE ROLE          most calls in one library file (stage, doors, enemies, objects, items, ...)
CALLEE             else its first call that is not bookkeeping (begin_event, HUD, sound...), in snake_case:
                   game_state_set_area_flag, show_stage_icon
calc               no calls, returns a value"""


@functools.lru_cache(None)
def _area_prefixes():
    """{scene: prefix} from the area labels."""
    try:
        import areas
        a = areas.build()
    except (Exception, SystemExit):  # no stage survey yet: every scene is scene_N
        return {}
    out = {}
    for ar in a["areas"]:
        lab = ar.get("label") or ""
        pre = next((p for k, p in AREA_PREFIX if lab.startswith(k)), None)
        if pre:
            for s in ar["scenes"]:
                out[s] = pre
    return out


_PROC = re.compile(r"\bproc_(\d+)\b(?!\()")


def references(line):
    """[(pid, container, keyword)] for each procedure named as a value in a
    readable line: the call it is an argument of and its keyword (or '')."""
    line = line.split("  # ")[0]
    if line.lstrip().startswith("#"):
        return []
    out, stack, i = [], [], 0
    while i < len(line):
        ch = line[i]
        if ch == '"':
            j = line.find('"', i + 1)
            while j > 0 and line[j - 1] == "\\":
                j = line.find('"', j + 1)
            i = (j if j > 0 else len(line)) + 1
            continue
        m = re.match(r"(spawn )?([A-Za-z_]\w*)", line[i:])
        if m:
            word = m.group(2)
            j = i + m.end()
            if j < len(line) and line[j] == "(":
                stack.append([word, ""])
                i = j + 1
                continue
            if j < len(line) and line[j] == "=" and line[j:j + 2] != "==" and stack:
                stack[-1][1] = word
                i = j + 1
                continue
            p = _PROC.match(line[i:])
            if p and not m.group(1):
                out.append((int(p.group(1)), stack[-1][0] if stack else "", stack[-1][1] if stack else ""))
            i = j
            continue
        if ch == "(":
            stack.append(["", ""])
        elif ch == ")":
            if stack:
                stack.pop()
        elif ch == "," and stack:
            if not _in_list(line, i):
                stack[-1][1] = ""
        i += 1
    return out


def _in_list(line, i):
    """Whether position i is inside [...] (a list keeps its keyword)."""
    return line.count("[", 0, i) > line.count("]", 0, i)


def _heads(lines):
    """Callable names at the start of each statement, and proc ids called."""
    heads, calls = [], []
    for ln in lines:
        s = ln.strip()
        if not s or s.startswith("#"):
            continue
        m = re.match(r"(?:spawn )?([A-Za-z_]\w*)\(", s)
        if m:
            heads.append(m.group(1))
            c = re.match(r"proc_(\d+)\(", s)
            if c:
                calls.append(int(c.group(1)))
    return heads, calls


def _cutscene_role(lines):
    m = re.search(r"SceneMessages_ScriptStart\(scene=(\d+)", "\n".join(lines))
    return (f"cutscene_{m.group(1)}", f"starts cutscene {m.group(1)}") if m else None


def _body_role(lines, names, file_of):
    """(role, evidence) from a procedure's own statements, or None."""
    text = "\n".join(lines)
    heads, calls = _heads(lines)
    hs = set(heads)
    if hs & {"load_stage", "load_stage_raw"}:
        return "stage", "loads the stage"
    m = re.search(r"SceneMessages_ScriptStart\(scene=(\d+)", text)
    if m:
        return f"cutscene_{m.group(1)}", f"starts cutscene {m.group(1)}"
    if hs and hs <= {"play_bgm", "play_bgm_with_fade", "play_bgm_fade"}:
        return "music", "chooses the music"
    tr = [h for h in heads if h.startswith("door_transition")]
    if tr and len(set(heads)) == 1 and len(tr) == 1:
        t = re.search(r"scene=(proc_\d+)", text)
        if t:
            return f"exit_to_{t.group(1)}", "one door_transition"
        return "exit", "one door_transition"
    if heads and all(h.startswith(("door", "locked_door", "entrance")) for h in heads):
        if all(h.startswith("entrance") for h in heads):
            return "entrances", "entrances only"
        return "doors", "doors and entrances"
    if "MessageWindow_SetMessageTable" in hs:
        return "talk", "sets a message table"
    if "advance_story" in hs and hs <= {"advance_story", "restore_hud", "end_event_restore_hud", "run_continue_proc",
                                        "end_event", "end_event_and_close_message"}:
        v = re.search(r"advance_story\(value=(\d+)", text)
        return (f"story_{v.group(1)}" if v else "story"), "advance_story"
    if hs and all(h.startswith("Camera_") for h in hs):
        return "camera", "camera only"
    if hs == {"mesg"}:
        return "signal", "mesg only"
    if hs == {"set_actor_route"}:
        return "route", "set_actor_route only"
    if any(h.startswith(("open_choice_menu", "ChoiceMenu_")) for h in hs):
        return "choice", "a choice menu"
    if hs and any(h.startswith("Camera_") for h in hs) and \
            hs <= {h for h in hs if h.startswith("Camera_")} | {"mesg", "Script_ScheduleOnce", "end_event", "begin_event"}:
        return "camera", "moves the camera"
    if tr:
        return "exit", "leaves through door_transition"
    if not hs and re.search(r"^\s*[\w\[\]\.:]+ = ", text, re.M) and "return" not in text:
        return "set_state", "assignments only"
    # library files of the calls (named or not)
    votes = collections.Counter()
    for h in heads:
        pid = names.get(h) or (int(h[5:]) if h.startswith("proc_") and h[5:].isdigit() else None)
        f = file_of.get(pid) if pid else None
        if f in FILE_ROLE:
            votes[FILE_ROLE[f]] += 1
    if votes:
        role, n = votes.most_common(1)[0]
        if n * 2 > len(heads):
            return role, f"{n} of {len(heads)} calls are {role} procedures"
    # last: what it mainly does, after its first significant call
    for h in heads:
        if h in BOOKKEEPING or h.startswith(("Sound_", "Hud_", "proc_")):
            continue
        return snake(h), f"first call {h}"
    if not heads and "return " in text:
        return "calc", "computes a value, no calls"
    return None


# calls that set up or tidy rather than say what a procedure is for
BOOKKEEPING = {"begin_event", "end_event", "nop", "run_continue_proc", "restore_hud", "end_event_restore_hud",
               "Script_ScheduleOnce", "ensure_scene_actor_manager", "print"}


def snake(name):
    """GameState_SetAreaFlag -> game_state_set_area_flag; SceneObjects_ScriptActivate ->
    scene_objects_activate; snake_case names stay."""
    name = re.sub(r"_Script(?=[A-Z])", "_", name)
    return re.sub(r"(?<=[a-z0-9])(?=[A-Z])", "_", name).lower().replace("__", "_")


def _ref_role(refs):
    """(role, evidence) from how other procedures refer to this one, or None."""
    for container, kw, line in refs:
        if container == "SceneMessages_ScriptStart" and kw == "onfinish":
            m = re.search(r"SceneMessages_ScriptStart\(scene=(\d+)", line)
            return (f"after_cutscene_{m.group(1)}" if m else "after_cutscene"), "a cutscene's onfinish"
    return None


def _step_role(refs):
    for container, kw, line in refs:
        if container == "SceneMessages_ScriptStart" and kw == "scripts":
            m = re.search(r"SceneMessages_ScriptStart\(scene=(\d+)", line)
            return (f"cutscene_{m.group(1)}_step" if m else "cutscene_step"), "in a cutscene's scripts"
    return _late_ref_role(refs)


def _late_ref_role(refs):
    for container, kw, line in refs:
        if container.startswith("entrance"):
            return "on_arrival", f"the callback of {container}"
    for container, kw, line in refs:
        if container in ("message_then", "open_message_window"):
            return "after_message", container
    for container, kw, line in refs:
        if kw == "on_proc" and container:
            return f"on_{container}", f"{container}({kw}=...)"
        if kw.startswith("on_"):
            return kw, f"{container}({kw}=...)"
    return None


@functools.lru_cache(None)
def generate():
    """{proc: (name, role, evidence)} for the unnamed scene procedures."""
    import actors
    import gclfiles
    import gclview
    names = gclview.Names(auto=False)
    img = gclview._image()
    ps = actors.procs()
    lines = {}
    for c in ps:
        stmts = gclview.procedure(img, c)
        r = gclview.Readable(names, c)
        r.body(stmts or [], 0)
        lines[c] = r.lines
    refs = collections.defaultdict(list)
    for c, ls in lines.items():
        for ln in ls:
            for pid, container, kw in references(ln):
                refs[pid].append((container, kw, ln))
    file_of = gclfiles.file_of()
    files = gclfiles.files()
    named = {r["name"]: p for p, r in names.procs.items()}
    pre = _area_prefixes()
    out, taken = {}, set(named)

    scene_name = {}
    for f in files:
        s = f["entry"]
        if f["kind"] != "scene" or s is None:
            continue
        if s in names.procs:
            scene_name[s] = names.procs[s]["name"]
            continue
        n = f"{pre.get(s, 'scene')}_{s}"
        scene_name[s] = n
        out[s] = (n, "scene", "scene entry" + (f", area {pre[s]}" if s in pre else ""))
        taken.add(n)
    # exit_to_ uses the target's name, so scenes are named first
    for f in files:
        s = f["entry"]
        if f["kind"] != "scene" or s is None:
            continue
        used = collections.Counter()
        for c in f["procs"]:
            if c == s or c in names.procs:
                continue
            rf = refs.get(c, [])
            role = _ref_role(rf) or _cutscene_role(lines[c]) or _step_role(rf) or \
                _body_role(lines[c], named, file_of)
            if not role:
                continue
            r, ev = role
            r = re.sub(r"proc_(\d+)", lambda m: scene_name.get(int(m.group(1)), m.group(0)), r)
            if r.startswith("exit_to_") and r[8:] in scene_name.values():
                t = r[8:]
                r = "exit_to_" + (t.rsplit("_", 1)[1] if t.startswith(scene_name[s].rsplit("_", 1)[0] + "_") else t)
            used[r] += 1
            n = f"{scene_name[s]}_{r}" + (f"_{used[r]}" if used[r] > 1 else "")
            while n in taken:
                used[r] += 1
                n = f"{scene_name[s]}_{r}_{used[r]}"
            taken.add(n)
            out[c] = (n, r, ev)
    return out


def auto_rows():
    """proc.csv-shaped rows for gclview.Names."""
    return {c: {"proc": str(c), "name": n, "params": "", "confidence": "auto", "evidence": f"auto ({role}): {ev}"}
            for c, (n, role, ev) in generate().items()}


def main():
    args = sys.argv[1:]
    g = generate()
    if args and args[0] == "--csv":
        f = open(args[1], "w", newline="", encoding="utf-8") if len(args) > 1 else sys.stdout
        w = csv.writer(f)
        w.writerow(["proc", "name", "params", "confidence", "evidence"])
        for c, r in sorted(auto_rows().items()):
            w.writerow([r[k] for k in ("proc", "name", "params", "confidence", "evidence")])
        return
    if args:
        for a in args:
            c = int(a)
            print(f"proc {c}: " + (f"{g[c][0]}  ({g[c][1]}: {g[c][2]})" if c in g else "no generated name"))
        return
    import gclfiles
    import gclview
    names = gclview.Names(auto=False)
    files = gclfiles.files()
    scene_procs = [c for f in files if f["kind"] == "scene" for c in f["procs"]]
    unnamed = [c for c in scene_procs if c not in names.procs]
    got = [c for c in unnamed if c in g]
    print(f"scene procedures: {len(scene_procs)}, unnamed in proc.csv: {len(unnamed)}, named here: {len(got)}"
          f" ({100 * len(got) // max(1, len(unnamed))}%)")
    roles = collections.Counter("exit_to_SCENE" if g[c][1].startswith("exit_to_") else re.sub(r"\d+", "N", g[c][1])
                                for c in got)
    for r, n in roles.most_common():
        print(f"  {n:5}  {r}")
    print("\nrules, first match wins:\n" + ROLES)


if __name__ == "__main__":
    main()
