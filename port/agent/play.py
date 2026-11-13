#!/usr/bin/env python3
"""Play and probe the running Boktai 3 source port from the command line.

    python3 port/agent/play.py start --save tests/saves/ShinBok2.sav
    python3 port/agent/play.py continue-save      # title -> file select -> Continue
    python3 port/agent/play.py press down 40 ; play.py shot
    python3 port/agent/play.py c 'print("%d\\n", gScriptVm.result);'

The game runs in a daemon (port/agent/daemon.mjs, started by `start`) that
stops at frame boundaries or breakpoints; every command here runs there and
returns. Names resolve to GBA addresses through the ELF and symbols/*.csv.
See port/agent/README.md for every command.
"""
import argparse
import base64
import bisect
import collections
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
import time
import urllib.request

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/agent'))
STATE = ROOT / 'build/port/agent'
INFO = STATE / 'daemon.json'
DEFAULT_PORT = 8766
SHOT = STATE / 'shot.png'
KEYS = ['a', 'b', 'select', 'start', 'right', 'left', 'up', 'down', 'r', 'l']


# -- connection ---------------------------------------------------------------
def daemon_port():
    if INFO.exists():
        return json.loads(INFO.read_text())['port']
    return DEFAULT_PORT


def request(cmd, timeout=600):
    data = json.dumps(cmd).encode()
    req = urllib.request.Request(f'http://127.0.0.1:{daemon_port()}/cmd', data=data, method='POST')
    try:
        with urllib.request.urlopen(req, timeout=timeout) as r:
            return json.loads(r.read() or b'{}')
    except urllib.error.URLError as e:
        sys.exit(f'no daemon ({e.reason}); run: python3 port/agent/play.py start')


def alive():
    try:
        with urllib.request.urlopen(f'http://127.0.0.1:{daemon_port()}/status.json', timeout=2):
            return True
    except Exception:
        return False


# -- symbols ------------------------------------------------------------------
_syms = None


def syms():
    global _syms
    if _syms is None:
        from snippet import symbols
        s = symbols()
        code = {k: v & ~1 for k, v in s['code'].items()}
        by_addr = {}
        for name, a in sorted(code.items()):
            if a not in by_addr or name.startswith('sub_') is False and by_addr[a].startswith('sub_'):
                by_addr[a] = name
        data_by = {}
        for name, a in s['data'].items():
            data_by.setdefault(a, name)
        keys = sorted(by_addr)
        _syms = {'code': code, 'data': s['data'], 'by_addr': by_addr, 'keys': keys, 'data_by': data_by}
    return _syms


def address(text):
    """A number (hex with 0x or 8 hex digits) or a function/data name."""
    t = text.strip()
    if re.fullmatch(r'0x[0-9a-fA-F]+|[0-9a-fA-F]{8}', t):
        return int(t, 16)
    if re.fullmatch(r'-?\d+', t):
        return int(t)
    s = syms()
    if t in s['code']:
        return s['code'][t]
    if t in s['data']:
        return s['data'][t]
    m = re.fullmatch(r'(?:sub|gUnk)_([0-9A-Fa-f]{8})', t)
    if m:
        return int(m[1], 16)
    sys.exit(f'unknown symbol {t}')


def value(text):
    t = text.strip()
    if re.fullmatch(r'-?\d+', t):
        return int(t) & 0xffffffff
    return address(t)


def name_of(a):
    s = syms()
    if a in s['by_addr']:
        return s['by_addr'][a]
    i = bisect.bisect_right(s['keys'], a) - 1
    if i >= 0 and a - s['keys'][i] < 0x2000:
        return f"{s['by_addr'][s['keys'][i]]}+0x{a - s['keys'][i]:x}"
    return f'0x{a:08x}'


def keymask(spec):
    mask = 0
    for k in re.split(r'[+&,]', spec.lower()):
        if k:
            if k not in KEYS:
                sys.exit(f'unknown key {k} (keys: {" ".join(KEYS)})')
            mask |= 1 << KEYS.index(k)
    return mask


# -- output -------------------------------------------------------------------
def show_status(r):
    if 'error' in r:
        sys.exit('error: ' + r['error'])
    stop = r.get('stop') or {}
    line = f"frame {r.get('frame')}"
    if stop.get('reason') == 'break':
        args = ' '.join(f'{a:#x}' for a in stop.get('args', []))
        line += f" | break {stop['kind']} {name_of(stop['address'])} r0-r3: {args}"
    elif stop.get('reason') not in (None, 'frame'):
        line += f" | {stop['reason']}"
    if 'scene' in r:
        p = r.get('player', {})
        line += f" | scene {r['scene']} | player x={p.get('x')} y={p.get('y')} z={p.get('z')} | life {r.get('life')}"
    print(line)
    if r.get('breaks'):
        print('breaks: ' + ', '.join(name_of(b['address']) + ('' if b['enter'] else ' (exit)') + (' +exit' if b['enter'] and b['exit'] else '') for b in r['breaks']))
    for l in r.get('log', []):
        print(l)


def shot(path=None):
    path = Path(path or SHOT).resolve()
    r = request({'op': 'shot', 'path': str(path)})
    if 'error' in r:
        sys.exit(r['error'])
    print(path)


# -- commands -----------------------------------------------------------------
def cmd_start(a):
    if alive():
        request({'op': 'quit'})
        time.sleep(0.3)
    wasm = ROOT / 'build/port/game/game.wasm'
    if not wasm.exists() or a.build:
        if subprocess.run([sys.executable, str(ROOT / 'port/build_game.py')]).returncode:
            sys.exit('build failed')
    STATE.mkdir(parents=True, exist_ok=True)
    if INFO.exists():
        INFO.unlink()
    args = ['node', str(ROOT / 'port/agent/daemon.mjs'), '--port', str(a.port), '--time', a.time,
            '--solar', str(a.solar)]
    if a.save:
        args += ['--save', str(Path(a.save).resolve())]
    log = open(STATE / 'daemon.log', 'w')
    subprocess.Popen(args, stdout=log, stderr=subprocess.STDOUT, start_new_session=True, cwd=ROOT)
    for _ in range(100):
        if INFO.exists() and alive():
            break
        time.sleep(0.1)
    else:
        sys.exit('daemon did not start; see build/port/agent/daemon.log')
    print(f'daemon on http://127.0.0.1:{a.port}/ (live view)')
    show_status(request({'op': 'status'}))


def cmd_stop(a):
    if alive():
        request({'op': 'quit'})
    print('stopped')


def cmd_status(a):
    show_status(request({'op': 'status'}))


def run_steps(steps, until_break=False, shot_after=False):
    r = request({'op': 'run', 'steps': steps, 'untilBreak': until_break})
    show_status(r)
    if shot_after:
        shot()


def cmd_run(a):
    run_steps([[keymask(a.keys) if a.keys else 0, a.frames]], a.until_break, a.shot)


def cmd_press(a):
    run_steps([[keymask(a.keys), a.frames], [0, a.after]], False, a.shot)


def cmd_seq(a):
    """Steps like 'a:6 _:20 down+left:30' (key:frames, _ = nothing held)."""
    steps = []
    for item in ' '.join(a.steps).split():
        k, _, n = item.partition(':')
        steps.append([0 if k in ('_', '') else keymask(k), int(n or 6)])
    run_steps(steps, a.until_break, a.shot)


def cmd_continue_save(a):
    """From power-on with a save: through the title to the file select, then Continue."""
    run_steps([[0, 950], [1, 6], [0, 144], [8, 6], [0, 144], [1, 6], [0, 144], [1, 6], [0, 300]], False, a.shot)


def cmd_shot(a):
    shot(a.path)


def fmt_read(data, kind, base):
    if kind == 'hex':
        for i in range(0, len(data), 16):
            row = data[i:i + 16]
            print(f'{base + i:08x}  ' + ' '.join(f'{b:02x}' for b in row).ljust(48) + '  ' +
                  ''.join(chr(b) if 32 <= b < 127 else '.' for b in row))
        return
    size, code = {'u8': (1, 'B'), 's8': (1, 'b'), 'u16': (2, 'H'), 's16': (2, 'h'),
                  'u32': (4, 'I'), 's32': (4, 'i')}[kind]
    vals = struct.unpack(f'<{len(data) // size}{code}', data[:len(data) // size * size])
    for i in range(0, len(vals), 8):
        print(f'{base + i * size:08x}  ' + ' '.join(
            (f'{v:#x}' if kind[0] == 'u' and kind != 'u8' else str(v)) for v in vals[i:i + 8]))


def cmd_read(a):
    base = address(a.where)
    if a.deref:
        r = request({'op': 'read', 'address': base, 'length': 4})
        base = struct.unpack('<I', base64.b64decode(r['data']))[0] + a.deref_offset
    r = request({'op': 'read', 'address': base, 'length': a.length})
    if 'error' in r:
        sys.exit(r['error'])
    fmt_read(base64.b64decode(r['data']), a.type, base)


def cmd_write(a):
    size = a.size
    v = value(a.value)
    data = (v & ((1 << (8 * size)) - 1)).to_bytes(size, 'little')
    r = request({'op': 'write', 'address': address(a.where), 'data': base64.b64encode(data).decode()})
    print(r.get('error') or 'ok')


def cmd_call(a):
    r = request({'op': 'call', 'address': address(a.function), 'args': [value(x) for x in a.args],
                 'trace': False})
    if 'error' in r:
        sys.exit(r['error'] + ''.join('\n' + l for l in r.get('log', [])))
    res = r['result']
    print(f'r0 = {res:#x} ({res if res < 0x80000000 else res - (1 << 32)})')
    for l in r.get('log', []):
        print(l)


def run_snippet(code, includes=(), trace=False):
    from snippet import compile_snippet, SnippetError
    try:
        wasm = compile_snippet(code, includes)
    except SnippetError as e:
        sys.exit(str(e))
    r = request({'op': 'snippet', 'wasm': base64.b64encode(wasm).decode(), 'trace': trace})
    return r


def cmd_c(a):
    code = Path(a.file).read_text() if a.file else ' '.join(a.code)
    if a.trace:
        request({'op': 'trace', 'on': True})
    r = run_snippet(code, a.include, a.trace)
    sys.stdout.write(r.get('output', ''))
    for l in r.get('log', []):
        print(l)
    if 'error' in r:
        sys.exit('error: ' + r['error'])
    if a.trace:
        finish_trace(argparse.Namespace(out=a.trace_out, depth=a.depth, summary=20))
    if r.get('result'):
        res = r['result']
        print(f'result = {res:#x} ({res if res < 0x80000000 else res - (1 << 32)})')


def cmd_p(a):
    expr = ' '.join(a.expr)
    fmt = a.format or '%d (0x%x)'
    n = fmt.count('%') - 2 * fmt.count('%%')
    r = run_snippet(f'print("{fmt}\\n"' + (', (' + expr + ')') * n + ');')
    sys.stdout.write(r.get('output', ''))
    if 'error' in r:
        sys.exit('error: ' + r['error'])


def cmd_break(a):
    for f in a.functions:
        r = request({'op': 'break', 'address': address(f), 'kind': 'exit' if a.exit else 'enter', 'on': not a.off})
    show_status(r)


def cmd_clear(a):
    show_status(request({'op': 'clear'}))


def cmd_continue(a):
    show_status(request({'op': 'continue', 'frames': a.frames}))
    if a.shot:
        shot()


def cmd_callproc(a):
    """Run a script procedure: Script_Call(id, args) at the next TaskSystem_RunFrame entry."""
    r = request({'op': 'callproc', 'proc': a.id, 'args': [value(x) for x in a.args]})
    if 'error' in r:
        sys.exit(r['error'])
    print(f'proc {a.id} queued; it runs during the next frame')
    if a.frames:
        run_steps([[0, a.frames]], False, a.shot)


def cmd_play(a):
    """Free play at 60 fps: keys from the viewer page (or the studio) until pause."""
    r = request({'op': 'play'})
    if 'error' in r:
        sys.exit(r['error'])
    print(f'playing from frame {r["frame"]}: http://127.0.0.1:{daemon_port()}/ takes the keyboard; '
          'play.py pause stops')


def cmd_pause(a):
    show_status(request({'op': 'pause'}))


def cmd_goto(a):
    """Put the player at world x, z (and y) through struct Player (include/player.h)."""
    y = '' if a.y is None else f'p->position.y = {a.y};'
    r = run_snippet(f'struct Player *p = gPlayers[0]; p->position.x = {a.x}; p->position.z = {a.z}; {y} '
                    'p->previousPosition = p->position; print("%d %d %d\\n", p->position.x, p->position.y, p->position.z);',
                    ['player.h'])
    if 'error' in r:
        sys.exit(r['error'])
    run_steps([[0, a.frames]], False, a.shot)


def cmd_warp(a):
    """Warp to a scene the way a door does: proc 945(scene, entrance, 0)."""
    entrance = a.entrance
    if entrance is None:
        entrance = default_entrance(a.scene)
        if entrance is None:
            sys.exit('no known entrance for that scene; pass one (build/assets/stages.json lists them)')
    cmd_callproc(argparse.Namespace(id=945, args=[str(a.scene), str(entrance), '0'], frames=a.frames, shot=a.shot))


def default_entrance(scene):
    p = ROOT / 'build/centaur/scenes.lua'
    if p.exists():
        for m in re.finditer(r'scene\s*=\s*(\d+)\s*,\s*entrance\s*=\s*(\d+)', p.read_text()):
            if int(m[1]) == scene:
                return int(m[2])
    p = ROOT / 'build/assets/stages.json'
    if p.exists():
        data = json.loads(p.read_text())
        for s in data.get('scenes', []) if isinstance(data, dict) else []:
            if s.get('scene') == scene and s.get('entrances'):
                e = s['entrances'][0]
                return e.get('id') if isinstance(e, dict) else e
    return None


def cmd_trace(a):
    if a.action == 'start':
        r = request({'op': 'trace', 'on': True, 'root': address(a.root) if a.root else 0})
        print('tracing' + (f' inside {a.root}' if a.root else ''))
    else:
        finish_trace(a)


def finish_trace(a):
    raw = STATE / 'trace.bin'
    r = request({'op': 'trace', 'on': False, 'path': str(raw)})
    if 'error' in r:
        sys.exit(r['error'])
    data = raw.read_bytes()
    out = Path(a.out or (STATE / 'trace.txt'))
    counts = collections.Counter()
    depth = 0
    lines = []
    for i in range(0, len(data), 24):
        w0, frame, a0, a1, a2, a3 = struct.unpack_from('<6I', data, i)
        if w0 == 0xfffffffe:
            lines.append(f'-- frame {a0}')
            continue
        if w0 & 1:
            depth = max(0, depth - 1)
            if depth < a.depth:
                lines.append('  ' * depth + f'<- {name_of(w0 & ~1)} = {a0:#x}')
            continue
        counts[w0] += 1
        if depth < a.depth:
            lines.append('  ' * depth + f'{name_of(w0)}({a0:#x}, {a1:#x}, {a2:#x}, {a3:#x})')
        depth += 1
    out.write_text('\n'.join(lines) + '\n')
    print(f"{r['events']} events" + (f", {r['overflow']} dropped (ring full)" if r['overflow'] else '') +
          f'; call tree: {out}')
    for addr, n in counts.most_common(a.summary):
        print(f'{n:8d}  {name_of(addr)}')


SCRIPT_POINTS = {  # function: (what, peek [arg, offset, length])
    'Script_Call': ('call', []),
    'Script_CallInlineProcedure': ('inline', [[0, 0, 2]]),
    'Script_ExecuteNativeCommand': ('cmd', [[0, 0, 2]]),
}


def gcl():
    """Procedure and command names (gcl/names) and the procedure layout (gcx image)."""
    sys.path.insert(0, str(ROOT / 'tools/assets'))
    sys.path.insert(0, str(ROOT / 'tools'))
    import gclnames
    import gcx
    rom = (ROOT / 'baserom.gba').read_bytes()
    img = gcx.Image(rom)
    starts = sorted((0x08000000 + img.body + (w & 0xFFFFFF), pid + 1) for pid, w in enumerate(img.procs) if w)
    procs = {int(r['proc']): r['name'] for r in gclnames.rows('proc') if r.get('proc', '').isdigit()}
    commands = {k: r['name'] for k, r in gclnames.best('command').items()}
    keys = gclnames.best('chara')

    def operands(pc, name, limit=4):
        """The first operands of the command at pc, registry keys named (gcx.py)."""
        b = rom
        q, alen = gcx.arglist(b, pc - 0x08000000 + 2)
        end, out = q + alen, []
        while q < end and len(out) < limit:
            if b[q] & 0xF0 == 0x80:
                out.append('{...}')
                break
            t, q = gcx.operand(b, q)
            if q is None:
                break
            if gcx.KEYED.get(name) == len(out) and t.startswith('0x') and int(t, 16) in keys:
                t = keys[int(t, 16)]['name']
            out.append(t)
        return ' '.join(out)
    return starts, procs, commands, operands


def proc_label(pid, procs):
    return f'proc {pid}' + (f' ({procs[pid]})' if pid in procs else '')


def cmd_script(a):
    points = [{'address': address(f), 'peek': peek} for f, (_, peek) in SCRIPT_POINTS.items()]
    if a.action == 'start':
        r = request({'op': 'observe', 'on': True, 'points': points})
        print(r.get('error') or 'watching script calls and commands')
        return
    r = request({'op': 'observe', 'on': False})
    if 'error' in r:
        sys.exit(r['error'])
    starts, procs, commands, operands = gcl()
    kinds = {address(f): what for f, (what, _) in SCRIPT_POINTS.items()}
    keys = [s[0] for s in starts]
    lines, frame = [], None
    counts = collections.Counter()
    for f, addr, args, peeks in r['records']:
        if f != frame:
            frame = f
            lines.append(f'-- frame {f}')
        what = kinds.get(addr)
        if what == 'call':
            line = 'call ' + proc_label(args[0], procs) + (f' args@{args[1]:#x}' if args[1] else '')
        elif what in ('inline', 'cmd'):
            key = int.from_bytes(bytes.fromhex(peeks[0]), 'little') if peeks and peeks[0] else None
            i = bisect.bisect_right(keys, args[0]) - 1
            where = proc_label(starts[i][1], procs) if i >= 0 else '?'
            if what == 'inline':
                key = key - 0x10000 if key is not None and key >= 0x8000 else key
                line = f'  inline call proc {key} (from {where})'
            else:
                name = commands.get(key, f'cmd_{key:04X}' if key is not None else '?')
                try:
                    ops = operands(args[0], name) if 0x08000000 <= args[0] < 0x0a000000 else ''
                except Exception:
                    ops = ''
                line = f'  {name} {ops}  [{where}, pc {args[0]:#x}]'
        else:
            line = f'{name_of(addr)}{tuple(hex(x) for x in args)}'
        counts[line.split('  [')[0].strip()] += 1
        lines.append(line)
    out = Path(a.out or STATE / 'script.txt')
    out.write_text('\n'.join(lines) + '\n')
    print(f"{len(r['records'])} script events{' (limit reached)' if r.get('full') else ''}: {out}")
    for text, n in counts.most_common(a.summary):
        print(f'{n:6d}  {text}')


TASKS_C = r'''
struct TaskList *list = gTaskLists; int i;
for (i = 0; i < 14; i++, list++) {
    struct Task *t;
    for (t = list->head; t; t = t->next)
        print("%d %08x %u %x %08x %08x\n", i, (unsigned)t, t->id, t->flags,
              (unsigned)t->update, (unsigned)t->destroy);
}
'''


def cmd_tasks(a):
    """Every task the scheduler runs (gTaskLists), with its update function."""
    r = run_snippet(TASKS_C, ['engine.h'])
    if 'error' in r:
        sys.exit(r['error'])
    rows = [l.split() for l in r.get('output', '').splitlines() if l.strip()]
    counts = collections.Counter()
    for prio, task, tid, flags, update, destroy in rows:
        name = name_of(int(update, 16) & ~1) if int(update, 16) else '-'
        counts[name] += 1
        if not a.summary:
            print(f'{prio:>2} {task} id {tid:>5} {"dead " if int(flags, 16) & 1 else ""}{name}')
    if a.summary:
        for name, n in counts.most_common():
            print(f'{n:4d}  {name}')
    print(f'{len(rows)} tasks')


def cmd_sprites(a):
    """The sprites on screen: OAM entries matched to sprite resources (studio analysis)."""
    r = request({'op': 'capture', 'name': a.name})
    if 'error' in r:
        sys.exit(r['error'])
    sys.path.insert(0, str(ROOT / 'tools'))
    sys.path.insert(0, str(ROOT / 'tools/studio'))
    import live
    d = live.analyse_capture(a.name)
    for sp in d.get('sprites', []):
        m = sp.get('match') or []
        what = ' '.join(str(x) for x in m) if m else 'unmatched'
        print(f"oam {sp['oam']:3d}  x {sp['x']:4d} y {sp['y']:4d}  {sp['w']}x{sp['h']}  {what}"
              + (f" frame {sp['frame']}" if sp.get('frame') is not None else ''))
    print(f"{len(d.get('sprites', []))} sprites; capture {r['dir']}")


def cmd_capture(a):
    """Palettes, VRAM, OAM, RAM and the screen in the studio's capture format."""
    r = request({'op': 'capture', 'name': a.name})
    if 'error' in r:
        sys.exit(r['error'])
    print(r['dir'])
    print('studio: python3 tools/studio/serve.py, then http://localhost:8642/captures (sprites and text on screen)')


def cmd_checkpoint(a):
    print(request({'op': 'checkpoint', 'name': a.name}))


def cmd_restore(a):
    show_status(request({'op': 'restore', 'name': a.name}))


def cmd_restart(a):
    show_status(request({'op': 'restart', **({'save': str(Path(a.save).resolve())} if a.save else {}),
                         **({'save': ''} if a.no_save else {})}))


def cmd_sym(a):
    for t in a.names:
        if re.fullmatch(r'0x[0-9a-fA-F]+|[0-9a-fA-F]{8}', t):
            v = int(t, 16)
            print(f'{v:08x} {name_of(v & ~1)}' + (f" / {syms()['data_by'][v]}" if v in syms()['data_by'] else ''))
        else:
            hits = list(dict.fromkeys(n for n in list(syms()['code']) + list(syms()['data']) if re.search(t, n)))
            for n in hits[:40]:
                v = syms()['code'].get(n, syms()['data'].get(n))
                print(f'{v:08x} {n}')


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='command', required=True)

    def add(name, fn, help):
        p = sub.add_parser(name, help=help, description=help)
        p.set_defaults(fn=fn)
        return p

    p = add('start', cmd_start, 'start (or restart) the daemon, stopped at power-on')
    p.add_argument('--save', help='EEPROM save to boot with (e.g. tests/saves/ShinBok2.sav)')
    p.add_argument('--time', default='2025-11-13T12:00:00Z', help='cartridge clock (fixed: runs are deterministic)')
    p.add_argument('--solar', type=int, default=232, help='solar sensor: 0 bright sun .. 255 dark')
    p.add_argument('--port', type=int, default=DEFAULT_PORT)
    p.add_argument('--build', action='store_true', help='rebuild game.wasm first')
    add('stop', cmd_stop, 'stop the daemon')
    add('status', cmd_status, 'frame, why it stopped, scene, player position, breakpoints')
    p = add('run', cmd_run, 'run N frames (holding KEYS)')
    p.add_argument('frames', type=int)
    p.add_argument('keys', nargs='?', help='e.g. a, down+left')
    p.add_argument('--until-break', action='store_true')
    p.add_argument('--shot', action='store_true', help='screenshot afterwards')
    p = add('press', cmd_press, 'hold KEYS for N frames (default 6), then release for --after frames')
    p.add_argument('keys')
    p.add_argument('frames', type=int, nargs='?', default=6)
    p.add_argument('--after', type=int, default=10)
    p.add_argument('--shot', action='store_true')
    p = add('seq', cmd_seq, "input sequence: 'a:6 _:20 down+left:30' (key:frames; _ = nothing)")
    p.add_argument('steps', nargs='+')
    p.add_argument('--until-break', action='store_true')
    p.add_argument('--shot', action='store_true')
    p = add('continue-save', cmd_continue_save, 'from power-on: title, file select, Continue (about 1700 frames)')
    p.add_argument('--shot', action='store_true')
    p = add('shot', cmd_shot, 'screenshot (PNG) of the last frame; prints its path')
    p.add_argument('path', nargs='?')
    p = add('read', cmd_read, 'read memory at an address or symbol')
    p.add_argument('where')
    p.add_argument('length', type=lambda s: int(s, 0), nargs='?', default=64)
    p.add_argument('-t', '--type', default='hex', choices=['hex', 'u8', 's8', 'u16', 's16', 'u32', 's32'])
    p.add_argument('--deref', action='store_true', help='WHERE holds a pointer: read what it points to')
    p.add_argument('--deref-offset', type=lambda s: int(s, 0), default=0)
    p = add('write', cmd_write, 'write a value (1, 2 or 4 bytes)')
    p.add_argument('where')
    p.add_argument('value')
    p.add_argument('-s', '--size', type=int, default=4, choices=[1, 2, 4])
    p = add('call', cmd_call, 'call a game function now: call FUNC [ARGS...] (numbers or symbols)')
    p.add_argument('function')
    p.add_argument('args', nargs='*')
    p = add('c', cmd_c, 'run a C snippet in the game (statements, or a file with snippet_main)')
    p.add_argument('code', nargs='*')
    p.add_argument('-f', '--file')
    p.add_argument('-i', '--include', action='append', default=[], help='extra header from include/')
    p.add_argument('--trace', action='store_true', help='trace the game calls the snippet makes')
    p.add_argument('--trace-out')
    p.add_argument('--depth', type=int, default=12)
    p = add('p', cmd_p, "print a C expression: p 'gMain.state' [--format '%%x']")
    p.add_argument('expr', nargs='+')
    p.add_argument('--format')
    p = add('break', cmd_break, 'stop at the entry (or --exit) of functions')
    p.add_argument('functions', nargs='+')
    p.add_argument('--exit', action='store_true')
    p.add_argument('--off', action='store_true')
    add('clear', cmd_clear, 'remove every breakpoint')
    p = add('continue', cmd_continue, 'run until a breakpoint (at most --frames)')
    p.add_argument('--frames', type=int, default=600)
    p.add_argument('--shot', action='store_true')
    p = add('trace', cmd_trace, 'trace start [--root FUNC] | trace stop [--out FILE] [--depth N]')
    p.add_argument('action', choices=['start', 'stop'])
    p.add_argument('--root')
    p.add_argument('--out')
    p.add_argument('--depth', type=int, default=12)
    p.add_argument('--summary', type=int, default=20)
    p = add('callproc', cmd_callproc, 'run script procedure ID with word arguments (during the next frame)')
    p.add_argument('id', type=int)
    p.add_argument('args', nargs='*')
    p.add_argument('--frames', type=int, default=1)
    p.add_argument('--shot', action='store_true')
    add('play', cmd_play, 'free play at 60 fps (keys from the viewer page or studio) until pause')
    add('pause', cmd_pause, 'stop free play')
    p = add('goto', cmd_goto, 'move the player to world x z [y] (GameState+0x30 shows where it is)')
    p.add_argument('x', type=int)
    p.add_argument('z', type=int)
    p.add_argument('y', type=int, nargs='?')
    p.add_argument('--frames', type=int, default=2)
    p.add_argument('--shot', action='store_true')
    p = add('warp', cmd_warp, 'go to a scene the way a door does (proc 945)')
    p.add_argument('scene', type=int)
    p.add_argument('entrance', type=int, nargs='?')
    p.add_argument('--frames', type=int, default=120)
    p.add_argument('--shot', action='store_true')
    p = add('script', cmd_script, 'script start | script stop: procedures called and commands run (GCL names)')
    p.add_argument('action', choices=['start', 'stop'])
    p.add_argument('--out')
    p.add_argument('--summary', type=int, default=15)
    p = add('tasks', cmd_tasks, 'every scheduler task with its update function')
    p.add_argument('--summary', action='store_true', help='count tasks per update function')
    p = add('sprites', cmd_sprites, 'sprites on screen matched to sprite resources')
    p.add_argument('--name', default='agent')
    p = add('capture', cmd_capture, 'save a studio capture (build/centaur/captures/NAME)')
    p.add_argument('name', nargs='?')
    p = add('checkpoint', cmd_checkpoint, 'remember this point (replayed deterministically by restore)')
    p.add_argument('name')
    p = add('restore', cmd_restore, 'go back to a checkpoint')
    p.add_argument('name')
    p = add('restart', cmd_restart, 'power-on again (same save unless --save / --no-save)')
    p.add_argument('--save')
    p.add_argument('--no-save', action='store_true')
    p = add('sym', cmd_sym, 'look up names (regex) or addresses')
    p.add_argument('names', nargs='+')
    a = ap.parse_args()
    a.fn(a)


if __name__ == '__main__':
    main()
