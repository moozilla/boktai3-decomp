#!/usr/bin/env python3
"""Cutscenes: the event tables that SceneMessages plays.

  cutscenes.py                    list every cutscene (index, steps, who starts it)
  cutscenes.py N [--json]         one cutscene as a timeline
  cutscenes.py --commands         the command set of each receiver kind
  cutscenes.py --targets          decomp status of each kind's registration,
                                  dispatcher and command handlers

A script starts a cutscene with `SceneMessages_ScriptStart(scene=N, step=S,
ids=[...], scripts=[...], onfinish=P)` (command 7348, `08020DF0`). N indexes
`gEventBytecodePtrs` (`08E8755C`): a cutscene is a list of steps, a step a
null-terminated list of messages. Each frame `SceneMessages_Update`
(`08020B8C`) delivers the current step's messages (`sub_08224EBC(N, step, i)`)
to their receivers and moves to the next step once every blocking message has
been consumed (`Trigger_ConsumePending`). When a step has no messages the
cutscene ends and `onfinish` runs.

A message is `u16 receiver, u8 kind, u8 blocking, u16 0, u8 command,
u8 nargs, s16 args[nargs]`. The receiver is an instance name (the NAME of
`chara TYPE NAME`, or a manager's fixed name) and kind its class: every
object that takes messages registers with `SceneMessages_RegisterReceiver(
id, kind)` (`08020CD4`) and switches on the command byte in its own dispatcher.

Kind 1 is the director itself (`0x4E69`, `SceneMessages_Dispatch`):
command 0 runs a script, 1 waits N frames, 2 waits for a signal
(`SceneMessages_ConsumeSignal`, command 2502, from a script). "Run" names an
entry of `ids`; the script at the same index of `scripts` is called with the
remaining arguments. The first script is usually the starter's own procedure,
a `match` on a counter (r700[0x119]): each run from the table shows the next
line and ends with `message_then` on itself, so dismissing the window runs the
following case, which closes it and sends the signal the step waits for. Each
run of it is one line.
"""
import json
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import gcx  # noqa: E402

TABLE = 0x08E8755C
DIRECTOR = 0x4E69

# receiver kinds: the class each registration site gives (08020CD4 callers)
KINDS = {
    1: ("director", "SceneMessages_Initialize", "SceneMessages_Dispatch"),
    2: ("player", "Player_RegisterSceneMessageReceiver", "PlayerCue_Dispatch"),
    3: ("registry actor", "SceneActorRegistry_RegisterCategoryReceiver", None),
    4: ("scene actor", "SceneActors_Initialize", "SceneActors_DispatchCommands"),
    5: ("npc", "SceneNpc_CreateFromScript", "SceneNpc_DispatchCommands"),
    6: ("camera", "Camera_Initialize", "Camera_DispatchCommands"),
    7: ("screen", "sub_08021394", "SceneCommands_Dispatch"),
    8: ("audio", "SceneAudioEvent_Register", "SceneAudioEvent_DispatchPending"),
    9: ("panel", "ScenePanel_RegisterReceiver", "ScenePanel_DispatchCommands"),
    10: ("object", "GroupObjects_Initialize", None),
    11: ("kind 11", "sub_08118890", None),
    12: ("kind 12", "sub_081202CC", None),
}

# command names we have read (low confidence unless the handler is named:
# from the handler's code and the arguments the tables pass); the rest show
# the handler's function name
COMMANDS = {
    1: {0: "run", 1: "wait", 2: "wait_signal"},
    # player (PlayerCue_Dispatch): directions are 0-7. Handlers are PlayerCue_*;
    # "motionN" names say only which motion (sub_0813AFEC) the command plays
    2: {0: "hide", 1: "place", 2: "stand", 3: "face", 4: "idle", 5: "stand_v2", 6: "stand_v3",
        7: "walk_to", 8: "motion7_front", 9: "motion7", 10: "motion7_end", 11: "kneel",
        12: "kneel_down", 13: "kneel_gasp", 14: "get_up", 15: "move_to", 16: "motion29",
        17: "motion30", 18: "motion30_end", 19: "motion29b", 20: "motion7_turn",
        21: "motion7_v7", 22: "motion7_v3", 23: "aim", 24: "shoot", 25: "aim_hold",
        26: "aim_end", 27: "face_side", 28: "motion11", 29: "motion10", 30: "form_motion",
        31: "state_motion", 32: "pose", 33: "run_to", 34: "run", 35: "windblown",
        36: "windblown_v2", 37: "windblown_v3", 38: "windblown_v4", 39: "stand_diagonal",
        40: "stand_diagonal_b", 41: "motion17", 42: "motion17_v2", 43: "shake_effect",
        44: "refresh", 45: "flag_5a1_on", 46: "flag_5a1_off", 47: "overlay_on",
        48: "overlay_set", 49: "overlay_off", 50: "overlay_set", 51: "motion38_to"},
    # scene actors (SceneActors_DispatchCommands): most commands only set a sub-state
    4: {2: "place", 9: "effect", 25: "flash"},
    # npc (SceneNpc_DispatchCommands), the same shapes as the player's
    5: {1: "stand", 3: "face", 4: "walk_to", 5: "run_to", 25: "pose", 26: "pose_end",
        64: "place"},
    # camera (Camera_DispatchCommands): 3 sub_08226474 (mode 5 over N frames)
    6: {0: "reset", 1: "snap_to", 2: "pan_to", 4: "sway", 5: "follow_player"},
    # screen (SceneCommands_Dispatch)
    7: {0: "particles_at", 1: "particles_stop", 2: "particles_on", 3: "fade", 4: "fade_palette",
        5: "blend_palette", 6: "quake", 7: "quake_stop", 8: "flash", 9: "done"},
    8: {0: "play_sound", 1: "start_song", 2: "start_bgm", 3: "fade_in_bgm", 4: "fade_out_bgm",
        5: "stop_bgm", 6: "start_ch12", 7: "fade_in_ch12", 8: "pause_ch12", 9: "fade_out_ch12"},
}

# calls a case makes that only acknowledge the message
TRIVIAL = {"sub_08020D54", "Trigger_ConsumePending", "sub_08226918", "sub_08226920"}

_rom = None


def rom():
    global _rom
    if _rom is None:
        _rom = gcx.load_rom()
    return _rom


def u32(a):
    return struct.unpack_from("<I", rom(), a - 0x08000000)[0]


def is_ptr(v):
    return 0x08000000 <= v < 0x08000000 + len(rom())


def message(a):
    b = rom()
    o = a - 0x08000000
    to, kind, block, _, cmd, n = struct.unpack_from("<HBBHBB", b, o)
    args = list(struct.unpack_from("<%dh" % n, b, o + 8)) if n else []
    return {"addr": a, "to": to, "kind": kind, "block": block, "cmd": cmd, "args": args}


_table = None


def table():
    """[[step messages, ...] or None per cutscene index]."""
    global _table
    if _table is None:
        _table = _read_table()
    return _table


def _read_table():
    out = []
    i = 0
    while True:
        t = u32(TABLE + 4 * i)
        if t and not is_ptr(t):
            break
        if i > 1024:
            break
        if not t:
            out.append(None)
        else:
            steps = []
            for s in range(4096):
                sp = u32(t + 4 * s)
                if not is_ptr(sp):
                    break
                msgs = []
                for k in range(64):
                    m = u32(sp + 4 * k)
                    if not is_ptr(m):
                        break
                    msgs.append(message(m))
                steps.append(msgs)
            out.append(steps)
        i += 1
    while out and out[-1] is None:
        out.pop()
    return out


def handlers():
    """{dispatcher function: {command: handler function}} from gen/code.s."""
    names = {}
    for ln in open(os.path.join(ROOT, "symbols", "functions.csv")):
        p = ln.split(",")
        if len(p) > 1 and re.match(r"^[0-9A-F]{8}$", p[0]):
            names["sub_" + p[0]] = p[1]
    funcs, cur = {}, None
    for ln in open(os.path.join(ROOT, "gen", "code.s")):
        m = re.match(r"\s*(?:thumb|arm)_func_start (\w+)", ln)
        if m:
            cur = m.group(1)
            funcs[cur] = []
        elif cur:
            funcs[cur].append(ln.rstrip())
    # the dispatchers call this by its current name in gen/code.s
    call = re.compile(r"\s*bl (sub_08020D54|%s)$" % re.escape(names.get("sub_08020D54", "sub_08020D54")))
    out = {}
    for f, L in funcs.items():
        if not any(call.match(x) for x in L):
            continue
        lab = {m.group(1): i for i, x in enumerate(L) for m in [re.match(r"(_[0-9A-F]+):", x)] if m}

        def first_bl(i):
            # the calls in the case body, minus the receiver bookkeeping
            calls = []
            for x in L[i + 1:i + 60]:
                m = re.match(r"\s*bl (\w+)", x)
                if m:
                    n = names.get(m.group(1), m.group(1))
                    if n not in TRIVIAL and n not in calls:
                        calls.append(n)
                if re.match(r"\s*b _", x) or "pop" in x or re.match(r"_[0-9A-F]+:", x):
                    break
            return "+".join(calls) or None
        cases = {}
        for x in L:
            m = re.match(r"\s*\.4byte (_\w+) @ case (\d+)", x)
            if m:
                cases.setdefault(int(m.group(2)), first_bl(lab[m.group(1)]))
        if not cases:
            k = None
            for i, x in enumerate(L):
                m = re.match(r"\s*cmp r\d+, #(0x[0-9a-f]+|\d+)$", x)
                if m:
                    k = int(m.group(1), 0)
                    continue
                m = re.match(r"\s*beq (_\w+)", x)
                if m and k is not None and m.group(1) in lab:
                    cases.setdefault(k, first_bl(lab[m.group(1)]))
                if not x.strip().startswith(("cmp", "b")):
                    k = None
        out[names.get(f, f)] = cases
    return out


def command_name(kind, cmd, _h=[]):
    n = COMMANDS.get(kind, {}).get(cmd)
    if n:
        return n
    if not _h:
        _h.append(handlers())
    d = KINDS.get(kind, (None, None, None))[2]
    h = _h[0].get(d, {}).get(cmd) if d else None
    return f"cmd{cmd}" + (f"<{h}>" if h else "")


# --- scripts: who starts each cutscene, who the receivers are -------------

_scan = None


def scan():
    """Starters, instance names and dialogue from the script image."""
    global _scan
    if _scan is not None:
        return _scan
    img = gcx.Image(rom())
    starts, instances, procs = [], {}, {}
    for pid in range(1, 16993):
        try:
            lines = gcx.disassemble(img, pid - 1, names=False)
        except Exception:
            continue
        if not lines:
            continue
        procs[pid] = lines
        for _, t in lines:
            m = re.search(r"command 0x7348 d: (-?\d+) c: (-?\d+)(?: p: ([0-9a-fx ]+?))?(?: P: ([0-9a-fx ]+?))?(?: e: (0x[0-9a-f]+|\d+))?$", t)
            if m:
                starts.append({"proc": pid, "event": int(m.group(1)), "step": int(m.group(2)),
                               "ids": [int(x, 0) for x in (m.group(3) or "").split()],
                               "scripts": [int(x, 0) for x in (m.group(4) or "").split()],
                               "onfinish": int(m.group(5), 0) if m.group(5) else 0})
            for m in re.finditer(r"chara (0x[0-9a-f]+|\d+) (0x[0-9a-f]+|\d+)", t):
                instances.setdefault(int(m.group(2), 0), set()).add(int(m.group(1), 0))
    # actor_name_of (proc 1154): character id -> instance name
    ids, cur = {}, None
    for _, t in procs.get(1154, []):
        m = re.match(r"case (\d+) \{", t)
        if m:
            cur = int(m.group(1))
        m = re.match(r"local1 = (0x[0-9a-f]+|\d+)$", t)
        if m and cur is not None:
            ids.setdefault(int(m.group(1), 0), cur)
    _scan = {"starts": starts, "instances": instances, "procs": procs, "charids": ids}
    return _scan


def chara_names():
    out = {}
    p = os.path.join(ROOT, "gcl", "names", "chara.csv")
    import csv
    for r in csv.DictReader(open(p, encoding="utf-8")):
        try:
            k = int(r["key"], 16)
        except (ValueError, KeyError):
            continue
        if r.get("confidence") in ("high", "medium") and k not in out:
            out[k] = r.get("english") or r.get("name")
    return out


def speakers():
    """character id -> speaker name (the speaker tokens, sub_0803232C)."""
    try:
        sys.path.insert(0, os.path.join(ROOT, "tools", "studio"))
        import atlas  # noqa: F401
        return {v: k for k, v in getattr(atlas, "SPEAKER_IDS", {}).items()}
    except Exception:
        return {}


FIXED = {0x4E69: "director", 0x730A: "screen", 0xD3A9: "audio", 0xFF40: "camera"}
# actor_name_of (proc 1154) ids by name, once one is confirmed on screen.
# 1: in the Sealed Prison only Django (the Player) and Otenko are on screen,
# and the NPC that proc_1676 places as character 1 moves beside the Player in
# every Otenko conversation there (cutscenes 5, 153-174; medium confidence)
CHARACTERS = {1: "Otenko"}


def receiver_name(to, kind, cn=None):
    if to in FIXED:
        return FIXED[to]
    s = scan()
    cn = cn if cn is not None else chara_names()
    types = s["instances"].get(to)
    if types:
        named = sorted(t for t in types if t in cn)
        t = (named or sorted(types))[0]
        # a second instance of a type (another Player) keeps its own name
        return (cn.get(t) or f"chara {t:04X}") + ("" if to == t or len(types) > 1 and to in types else f" {to:04X}")
    if to in s["charids"]:
        i = s["charids"][to]
        return CHARACTERS.get(i, f"character {i}")
    return f"{KINDS.get(kind, ('kind %d' % kind,))[0]} {to:04X}"


def dialogue(pid):
    """What a director script does each time it runs, by counter value:
    {case: (text id, "close" (proc_1025) or None, continues)}, where continues
    says the case ends with message_then (proc_1022) on the script itself: the
    window then runs the next case (the close) when the player dismisses it."""
    out, cur = {}, None
    for _, t in scan()["procs"].get(pid, []):
        m = re.match(r"case (\d+) \{", t)
        if m:
            cur = int(m.group(1))
            out.setdefault(cur, [None, False])
        if cur is None:
            continue
        m = re.search(r"text#(\d+)", t)
        if m and "0x1c64" in t and out[cur][0] is None:
            out[cur][0] = int(m.group(1))
        elif re.match(r"proc_102[457]\b", t) and out[cur][0] is None:
            out[cur][0] = "close"
        if re.match(r"proc_1022 ", t):
            out[cur][1] = True
    return out


def texts():
    return gcx.text_excerpts() if hasattr(gcx, "text_excerpts") else {}


_full = {}


def full_text(i):
    """(English, Japanese) of the message that starts at string i: its labeled
    first page and the pages after it, up to the one with <END>."""
    if not _full:
        for lang, f in (("jp", "bank.tsv"), ("en", "english.tsv")):
            d = _full.setdefault(lang, {})
            try:
                for ln in open(os.path.join(ROOT, "build", "text", f), encoding="utf-8"):
                    p = ln.rstrip("\n").split("\t", 2)
                    if len(p) == 3 and p[1] == "T" and p[0].isdigit():
                        d[int(p[0])] = p[2]
            except OSError:
                pass
    out = []
    for lang in ("en", "jp"):
        d = _full.get(lang, {})
        pages = []
        for k in range(i, i + 60):
            t = d.get(k)
            if t is None or (k > i and t.startswith("<LABEL=")):
                break
            body = re.sub(r"<[^>]*>", "", t.replace("<NAME>", "Django")).replace("\\n", " ").strip()
            if body:
                pages.append(re.sub(r"\s+", " ", body))
            if "<END>" in t:
                break
        out.append("\n".join(pages))
    return tuple(out)


def proc_name(pid, _n={}):
    if not _n:
        p = os.path.join(ROOT, "gcl", "names", "proc.csv")
        import csv
        for r in csv.DictReader(open(p, encoding="utf-8")):
            try:
                _n[int(r["proc"] if "proc" in r else r["id"])] = r["name"]
            except (ValueError, KeyError):
                pass
    return _n.get(pid, f"proc_{pid}")


def timeline(n, start=None):
    """Cutscene n as [(step, [rendered message, ...])]."""
    t = table()
    steps = t[n] if n < len(t) else None
    if not steps:
        return None, []
    st = start or next((s for s in scan()["starts"] if s["event"] == n), None)
    cn = chara_names()
    tx = texts()
    lines = dialogue(st["scripts"][0]) if st and st["scripts"] else {}
    counter = 1  # the start itself ran case 0
    out = []
    first = st["step"] if st else 0
    for i, msgs in enumerate(steps):
        if i < first:
            continue
        row = []
        for m in msgs:
            item = {"to": receiver_name(m["to"], m["kind"], cn), "kind": m["kind"], "block": m["block"],
                    "cmd": command_name(m["kind"], m["cmd"]), "args": m["args"]}
            if m["kind"] == 1 and m["cmd"] == 0 and m["args"] and st:
                who = m["args"][0] & 0xFFFF
                k = st["ids"].index(who) if who in st["ids"] else -1
                script = st["scripts"][k] if 0 <= k < len(st["scripts"]) else None
                item["script"] = script
                item["call"] = proc_name(script) if script else f"{who:04X}?"
                if k == 0:
                    tid, continues = lines.get(counter, (None, False))
                    item["line"] = counter
                    if tid == "close":
                        item["close"] = True
                    elif tid is not None:
                        item["text"] = tid
                        item["say"] = tx.get(tid, f"text#{tid}")
                        item["en"], item["jp"] = full_text(tid)
                    # a line that ends with message_then runs its own close case
                    # when dismissed, so the table's next run shows the next line
                    counter += 2 if continues else 1
            row.append(item)
        out.append((i, row))
    return st, out


def render(n):
    st, rows = timeline(n)
    if st is None and not rows:
        return [f"cutscene {n}: none"]
    out = []
    if st:
        out.append(f"cutscene {n}: started by proc_{st['proc']} at step {st['step']}; "
                   f"cast {', '.join('%04X=%s' % (i, proc_name(s)) for i, s in zip(st['ids'], st['scripts']))}; "
                   f"then proc_{st['onfinish']}")
    for i, row in rows:
        parts = []
        # the flagged message ends the step when its receiver finishes it;
        # wait_signal ends it when a script sends the signal
        for m in sorted(row, key=lambda m: bool(m["block"]) or m["cmd"] == "wait_signal"):
            ends = m["block"] and not (m["kind"] == 1 and m["cmd"] == "wait")
            if m["kind"] == 1 and m["cmd"] == "wait":
                t = f"wait {m['args'][0] if m['args'] else 0}f" if m["block"] else "(wait)"
            elif m["kind"] == 1 and m["cmd"] == "wait_signal":
                if any(x.get("close") or ("line" in x and "say" not in x) for x in row):
                    continue  # its own script sends the signal at once
                t = "until the player presses A" if any("say" in x for x in row) else "wait for signal"
            elif m["kind"] == 1 and m["cmd"] == "run":
                if "say" in m:
                    t = f"say \"{m['say']}\""
                elif m.get("close"):
                    t = "close window"
                elif "line" in m:
                    t = f"{m['call']}()"
                else:
                    t = f"{m['call']}({', '.join(map(str, m['args'][1:]))})"
            else:
                t = f"{m['to']}.{m['cmd']}({', '.join(map(str, m['args']))})"
            parts.append(t + (" and wait" if ends else ""))
        out.append(f"  {i:3d}  " + "; ".join(parts))
    return out


def main():
    a = sys.argv[1:]
    if not a:
        t = table()
        by = {}
        for s in scan()["starts"]:
            by.setdefault(s["event"], []).append(s["proc"])
        used = sum(1 for x in t if x)
        print(f"{len(t)} cutscene slots, {used} used, {sum(len(x) for x in t if x)} steps")
        for i, steps in enumerate(t):
            if steps:
                print(f"{i:4d}  {len(steps):3d} steps  {sum(len(s) for s in steps):4d} messages  "
                      f"started by {', '.join('proc_%d' % p for p in by.get(i, [])) or '-'}")
        return
    if a[0] == "--commands":
        h = handlers()
        for k, (name, reg, disp) in sorted(KINDS.items()):
            cs = h.get(disp, {}) if disp else {}
            print(f"kind {k} {name} (registered by {reg}, dispatcher {disp or '?'}): {len(cs)} commands")
            for c in sorted(cs):
                print(f"    {c:3d} {command_name(k, c)}")
        return
    if a[0] == "--targets":
        sys.path.insert(0, os.path.join(ROOT, "tools"))
        import paltrace
        import script_targets
        prog = script_targets.progress()
        byname = {v: k for k, v in paltrace.addr_names().items()}
        for ln in open(os.path.join(ROOT, "symbols", "functions.csv")):
            p = ln.split(",")
            if len(p) > 1 and re.match(r"^[0-9A-F]{8}$", p[0]):
                byname.setdefault(p[1], int(p[0], 16))
        h = handlers()
        total = done = 0
        for k, (name, reg, disp) in sorted(KINDS.items()):
            fs = {x for x in (reg, disp) if x}
            for v in h.get(disp, {}).values():
                fs |= {x for x in (v or "").split("+") if x}
            rows = []
            for f in sorted(fs):
                ad = byname.get(f) or (int(f[4:], 16) if f.startswith("sub_") else None)
                if ad in prog:
                    rows.append((f, ad, prog[ad][0], prog[ad][1]))
            size = sum(r[2] for r in rows)
            m = sum(r[2] for r in rows if r[3])
            total, done = total + size, done + m
            todo = ", ".join(f"{f} {sz}" for f, _, sz, ok in sorted(rows, key=lambda r: -r[2]) if not ok)
            print(f"kind {k:2d} {name:14s} {len(rows):3d} functions {size:6d} bytes, {m:6d} matched"
                  + (f"; to do: {todo}" if todo else ""))
        print(f"total {total} bytes, {done} matched ({100 * done // max(total, 1)}%)")
        return
    n = int(a[0], 0)
    if "--json" in a:
        st, rows = timeline(n)
        print(json.dumps({"start": st, "steps": rows}, ensure_ascii=False, indent=1))
    else:
        print("\n".join(render(n)))


if __name__ == "__main__":
    main()
