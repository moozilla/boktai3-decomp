"""Forge: isolated, asynchronous C experiments and fixed-layout playable mods.

No tracked source or normal build outputs are written. Production acceptance uses
build.py's stock compiler and check.py's linker; scope is separate diagnostic
evidence. Job IDs name immutable snapshots under build/studio/forge.
"""
import concurrent.futures
import difflib
import functools
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
import threading
import time
import urllib.parse
import uuid

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
JOBS = ROOT / 'build/studio/forge'
PYTHON = ROOT / 'build/venv/bin/python'
POOL = concurrent.futures.ThreadPoolExecutor(max_workers=2, thread_name_prefix='forge')
SLOTS = threading.BoundedSemaphore(4)
ID = re.compile(r'[0-9a-f]{32}')
MAX_SOURCE = 512 * 1024


def read_json(path):
    return json.loads(path.read_text())


def write_json(path, data):
    tmp = path.with_suffix('.tmp')
    tmp.write_text(json.dumps(data, ensure_ascii=False))
    tmp.replace(path)


def clean_env():
    # Inherited matching experiments must never affect a production verdict.
    return {k: v for k, v in os.environ.items()
            if not k.startswith(('AGBCC', 'CSE_', 'SCOPE_', 'BOKTAI3_'))
            and k not in ('CPP', 'BUILD_JOBS')}


def run(args, *, env=None, timeout=90):
    p = subprocess.run([str(x) for x in args], cwd=ROOT, env=env or clean_env(),
                       capture_output=True, text=True, timeout=timeout)
    return p.returncode, p.stdout + p.stderr


def public_log(text):
    return text.replace(str(ROOT), '.').replace(str(Path.home()), '~')[-40000:]


@functools.lru_cache(maxsize=2)
def _index(stamp):
    tree = read_json(ROOT / 'PROGRESS.json')
    result = {}
    for category in tree['categories']:
        for unit in category['units']:
            for row in unit['functions']:
                name, addr, size, _, _, file = row[:6]
                file = f'src/fn/sub_{addr}.c' if file == -1 else tree['files'][file] if file >= 0 else None
                if file and (ROOT / file).is_file():
                    result[addr] = dict(name=name, addr=addr, size=size, file=file,
                                        defined=row[6] if len(row) > 6 else name,
                                        category=category['name'], unit=unit['name'])
    return result


def index():
    return _index((ROOT / 'PROGRESS.json').stat().st_mtime_ns)


def function(query):
    query = urllib.parse.unquote(query)
    f = index().get(query.upper().removeprefix('0X'))
    if not f:
        f = next((x for x in index().values() if query in (x['name'], x['defined'], 'sub_' + x['addr'])), None)
    if not f:
        raise ValueError('No editable C function found. Open a function from Code.')
    return dict(f)


def source(query):
    f = function(query)
    text = (ROOT / f['file']).read_text()
    names = (f['defined'], f['name'], 'sub_' + f['addr'])
    pos = next((m.start() for name in names if (m := re.search(r'\b' + re.escape(name) + r'\s*\([^;{}]*\)\s*\{', text))), 0)
    f.update(source=text, line=text.count('\n', 0, pos) + 1,
             sha256=hashlib.sha256(text.encode()).hexdigest())
    return f


def job_dir(job_id):
    if not isinstance(job_id, str) or not ID.fullmatch(job_id):
        raise ValueError('Invalid Forge job ID')
    d = JOBS / job_id
    if not (d / 'request.json').is_file():
        raise FileNotFoundError('Forge job not found')
    return d


def validate_source(text, baseline):
    if not isinstance(text, str) or not text.strip() or len(text.encode()) > MAX_SOURCE or '\0' in text:
        raise ValueError('C source must be nonempty UTF-8 text under 512 KiB')
    # Include paths, compiler selection and flags belong to the source unit.
    # Keep them fixed in this workbench; ordinary C below them is editable.
    directives = lambda s: re.findall(r'^\s*#.*(?:\\\n.*)*|^// (?:COMPILER|CFLAGS):.*$', s, re.M)
    if directives(text) != directives(baseline):
        raise ValueError('Keep preprocessor directives and compiler settings unchanged; edit the C code.')


def submit(req, trans=None):
    if not isinstance(req, dict):
        raise ValueError('Expected a JSON object')
    f = source(req.get('function', ''))
    validate_source(req.get('source'), f['source'])
    if req.get('sha256') != f['sha256']:
        raise ValueError('Source changed on disk. Reopen the function before compiling.')
    action = req.get('action', 'compile')
    if action not in ('compile', 'play'):
        raise ValueError('Action must be compile or play')
    checkpoint = str(req.get('checkpoint', 'boot'))
    cp = next((c for c in checkpoints(trans) if c['id'] == checkpoint), None)
    if action == 'play' and not cp:
        raise ValueError('Choose an available story checkpoint')
    script = validate_script(req.get('script', 'wait 600\nshot result'))
    if not SLOTS.acquire(blocking=False):
        raise ValueError('Forge is busy (four jobs pending). Try again when a job finishes.')
    job_id = uuid.uuid4().hex
    d = JOBS / job_id
    try:
        d.mkdir(parents=True)
        (d / 'baseline.c').write_text(f.pop('source'))
        (d / Path(f['file']).name).write_text(req['source'])
        spec = dict(function=f, action=action, checkpoint=cp, script=script,
                    created=time.time(), id=job_id, text_edits=validate_text_edits(req.get('text_edits', [])))
        write_json(d / 'request.json', spec)
        write_json(d / 'result.json', dict(id=job_id, state='queued', stage='Waiting for compiler'))
        POOL.submit(execute, job_id)
    except BaseException:
        SLOTS.release()
        raise
    return dict(id=job_id, state='queued')


def execute(job_id):
    d = JOBS / job_id
    try:
        code, log = run([PYTHON, HERE / 'forge.py', '--worker', job_id], timeout=240)
        (d / 'worker.log').write_text(public_log(log))
        result = read_json(d / 'result.json')
        if code or result['state'] not in ('done', 'error'):
            write_json(d / 'result.json', dict(id=job_id, state='error', stage='Worker failed', error=public_log(log)))
        elif result['state'] == 'error' and log.strip():
            result['error'] += '\n' + public_log(log)
            write_json(d / 'result.json', result)
    except Exception as e:
        write_json(d / 'result.json', dict(id=job_id, state='error', stage='Worker failed', error=public_log(str(e))))
    finally:
        SLOTS.release()


def validate_script(script):
    """Only bounded controller input and named screenshots; no harness file IO/pokes."""
    if not isinstance(script, str) or len(script) > 10000:
        raise ValueError('Preview script must be text under 10,000 characters')
    lines, frames, shots = [], 0, 0
    keys = r'(?:A|B|L|R|START|SELECT|UP|DOWN|LEFT|RIGHT)(?:\+(?:A|B|L|R|START|SELECT|UP|DOWN|LEFT|RIGHT))*'
    for raw in script.splitlines():
        line = raw.strip()
        if not line or line.startswith('#'):
            continue
        if re.fullmatch(r'wait \d+', line):
            frames += int(line.split()[1])
        elif re.fullmatch(r'hold ' + keys + r' \d+', line):
            frames += int(line.split()[2])
        elif re.fullmatch(r'tap ' + keys, line):
            frames += 10
        elif re.fullmatch(r'shot [A-Za-z0-9_-]{1,48}', line):
            shots += 1
        else:
            raise ValueError('Preview accepts wait N, hold KEYS N, tap KEYS and shot NAME only')
        lines.append(line)
    if frames > 7200 or shots > 12:
        raise ValueError('Preview is limited to 7,200 frames and 12 screenshots')
    if not shots:
        lines.append('shot result')
    return '\n'.join(lines) + '\n'


def checkpoints(trans=None):
    out = [dict(id='boot', label='New game · boot', kind='boot')]
    if (ROOT / 'tests/saves/ShinBok2.sav').is_file():
        out.append(dict(id='test-save', label='Reference save · continue from title', kind='save',
                        path='tests/saves/ShinBok2.sav'))
    bank = Path(trans) / 'saves' if trans else ROOT.parent / 'boktai3trans/saves'
    if bank.is_dir():
        sys.path.insert(0, str(ROOT / 'tools/assets'))
        import saves
        for row in saves.bank(str(bank)):
            if 'error' in row:
                continue
            out.append(dict(id='bank-' + row['file'].split(' ', 1)[0], label=row['label'],
                            story=row['story'], scene=row['scene'], kind='bank', path=str(bank / row['file'])))
    return out


def validate_text_edits(edits):
    if not isinstance(edits, list) or len(edits) > 100:
        raise ValueError('Use at most 100 text edits')
    out, seen = [], set()
    for edit in edits:
        if not isinstance(edit, dict) or type(edit.get('id')) is not int or not isinstance(edit.get('text'), str):
            raise ValueError('Text edits need an integer id and text')
        if edit['id'] in seen or len(edit['text']) > 10000:
            raise ValueError('Duplicate text ID or oversized string')
        seen.add(edit['id'])
        out.append(dict(id=edit['id'], text=edit['text']))
    return out


def apply_text_edits(rom, edits):
    from datafmt import text_bank
    inverse = {c: g for g, c in text_bank.charmap().items()}
    count = (text_bank.BANK - text_bank.TABLE) // 4
    for edit in edits:
        i = edit['id']
        if not 0 <= i < count:
            raise ValueError(f'Text ID {i} is outside the bank')
        word, = struct.unpack_from('<I', rom, text_bank.TABLE - 0x08000000 + i * 4)
        if not word & 0x80000000:
            raise ValueError(f'Text ID {i} is binary data, not a string')
        start = text_bank.BANK - 0x08000000 + (word & 0x7FFFFFFF)
        end = rom.index(0, start)
        raw = text_bank.encode_string(edit['text'], inverse)
        if b'\0' in raw or len(raw) > end - start:
            raise ValueError(f'Text ID {i} needs {len(raw)} bytes; its fixed-layout budget is {end - start}')
        rom[start:end + 1] = raw + b'\0' * (end - start + 1 - len(raw))
    return rom


def ips_patch(original, modified):
    """IPS literal records, including the reserved EOF-offset edge case."""
    if len(original) != len(modified) or len(modified) > 0x1000000:
        raise ValueError('IPS export requires equal-size images up to 16 MiB')
    patch, i = bytearray(b'PATCH'), 0
    while i < len(modified):
        if original[i] == modified[i]:
            i += 1
            continue
        start = i - 1 if i == 0x454F46 else i
        i += 1
        while i < len(modified) and original[i] != modified[i] and i - start < 65535:
            i += 1
        chunk = modified[start:i]
        patch += start.to_bytes(3, 'big') + len(chunk).to_bytes(2, 'big') + chunk
    return bytes(patch + b'EOF')


def instruction_rows(original, candidate):
    matcher = difflib.SequenceMatcher(None, [x[1] for x in original], [x[1] for x in candidate], autojunk=False)
    rows = []
    for tag, a, b, c, d in matcher.get_opcodes():
        for i in range(max(b - a, d - c)):
            left = original[a + i] if a + i < b else None
            right = candidate[c + i] if c + i < d else None
            rows.append(dict(tag='=' if tag == 'equal' else '!', rom_addr=f'{left[0]:08X}' if left else None,
                             rom=left[1] if left else None, draft_addr=f'{right[0]:08X}' if right else None,
                             draft=right[1] if right else None))
    return rows


def worker(job_id):
    """Runs in a fresh process so build/scope globals cannot leak between edits."""
    d = job_dir(job_id)
    req = read_json(d / 'request.json')
    f = req['function']
    result = dict(id=job_id, state='running', stage='Compiling with stock agbcc')
    def update(**values):
        result.update(values)
        write_json(d / 'result.json', result)
    try:
        sys.path.insert(0, str(ROOT / 'tools'))
        import build
        import check
        build.CPP += ['-iquote', str((ROOT / f['file']).parent)]
        members = [x for x in index().values() if x['file'] == f['file']]
        start = min(int(x['addr'], 16) for x in members)
        end = max(int(x['addr'], 16) + x['size'] for x in members)
        if end - start != sum(x['size'] for x in members):
            raise ValueError('Unit inventory is not contiguous; a full relocating link is required')
        rom = (ROOT / 'baserom.gba').read_bytes()
        # Validate the baseline against the actual ROM before deriving overlay bounds.
        symbols = check.elf_symbols()
        compiled = []
        symbol_sets = []
        for name, path in [('baseline', d / 'baseline.c'), ('candidate', d / Path(f['file']).name)]:
            obj, elf = d / (name + '.o'), d / (name + '.elf')
            build.build_c(str(path), str(obj))
            address, _ = check.link_object(str(obj), str(elf), syms=symbols, address=start)
            build.sh(['arm-none-eabi-objcopy', '-O', 'binary', '-j', '.text', str(elf), str(d / (name + '.bin'))])
            compiled.append((d / (name + '.bin')).read_bytes())
            object_exports = {p[2] for line in build.sh(['arm-none-eabi-nm', '-g', '--defined-only', str(obj)]).splitlines()
                              if len(p := line.split()) == 3 and p[1] in ('T', 'W') and not p[2].endswith('__addr')}
            exports = {}
            for line in build.sh(['arm-none-eabi-nm', '-g', '--defined-only', str(elf)]).splitlines():
                parts = line.split()
                if len(parts) == 3 and parts[2] in object_exports:
                    exports[parts[2]] = int(parts[0], 16) & ~1
            symbol_sets.append(exports)
            # Writable/static data needs a complete layout, not a text-only overlay.
            for line in build.sh(['arm-none-eabi-objdump', '-h', str(obj)]).splitlines():
                parts = line.split()
                if len(parts) >= 3 and parts[0].isdigit() and parts[1] in ('.data', '.bss', '.rodata') and int(parts[2], 16):
                    raise ValueError('This unit contains non-text storage and needs a full relocating link')
        baseline, candidate = compiled
        target = rom[start - 0x08000000:end - 0x08000000]
        if len(baseline) > len(target) or baseline != target[:len(baseline)]:
            raise ValueError('The current source baseline does not match this ROM; rebuild/refresh the project first')
        # Check every export against the actual built symbol snapshot.
        for name, addr in symbol_sets[0].items():
            if name in symbols and (symbols[name][0] & ~1) != addr:
                raise ValueError('Baseline public symbol addresses differ from the project ELF')
        comparison = check.comparison(target, candidate, start, arm='-marm' in build.cflags_for(str(d / 'baseline.c')),
                                      target_size=len(target))
        comparison.pop('different_addresses')
        rows = instruction_rows(comparison.pop('original'), comparison.pop('candidate'))
        blocked = None
        if len(candidate) > len(target):
            blocked = f'Unit grew to {len(candidate)} bytes; its original span is {len(target)} bytes. A relocating link is required.'
        elif symbol_sets[0] != symbol_sets[1]:
            blocked = 'An existing public function moved or exports changed. A relocating link is required.'
        update(comparison=comparison, rows=rows, can_play=not blocked, play_blocked=blocked,
               source_file=f['file'], stage='Explaining compiler output')
        src = d / Path(f['file']).name
        env = dict(clean_env(), SCOPE_SERVER='0', SCOPE_RUN='forge-' + job_id)
        scope_dir = ROOT / 'build/scope' / ('forge-' + job_id) / f['defined']
        scope_rows = scope_dir / 'diff.json'
        try:
            code, log = run([PYTHON, ROOT / 'tools/scope/scope.py', src, f['defined']], env=env, timeout=35)
            update(scope_log=public_log(log), scope_rows=read_json(scope_rows) if scope_rows.is_file() else [],
                   scope_available=scope_rows.is_file())
            if comparison['status'] != 'match' and scope_rows.is_file():
                code, why = run([PYTHON, ROOT / 'tools/scope/scope.py', src, f['defined'], 'why', 'all'], env=env, timeout=35)
                update(explanation=public_log(why))
        except (OSError, subprocess.TimeoutExpired, ValueError) as e:
            update(scope_log='Compiler diagnostics unavailable: ' + public_log(str(e)))
        if not blocked:
            modified = bytearray(rom)
            at = start - 0x08000000
            modified[at:at + len(candidate)] = candidate
            apply_text_edits(modified, req['text_edits'])
            (d / 'mod.gba').write_bytes(modified)
            (d / 'mod.ips').write_bytes(ips_patch(rom, modified))
            update(patch_url=f'/api/forge/artifact/{job_id}/mod.ips',
                   source_url=f'/api/forge/artifact/{job_id}/source.c',
                   modified_sha256=hashlib.sha256(modified).hexdigest(),
                   base_sha256=hashlib.sha256(rom).hexdigest())
        if req['action'] == 'play':
            if blocked:
                raise ValueError(blocked)
            update(stage='Running headless mGBA')
            preview(d, req, update)
        update(state='done', stage='MATCH' if comparison['status'] == 'match' else 'Compiled · modified code')
    except (Exception, SystemExit) as e:
        update(state='error', stage='Experiment stopped', error=public_log(str(e)))


def preview(d, req, update):
    from PIL import Image
    env = clean_env()
    cp = req['checkpoint']
    if cp['kind'] == 'save':
        env['BOKTAI3_SAV'] = str(ROOT / cp['path'])
    elif cp['kind'] == 'bank':
        sys.path.insert(0, str(ROOT / 'tools/assets'))
        import saves
        save = d / 'checkpoint.sav'
        saves.write_sav(saves.current(saves.read_sgm(cp['path'])), str(save))
        env['BOKTAI3_SAV'] = str(save)
    (d / 'preview.txt').write_text('trace off\nrtc 1760000000\n' + req['script'])
    output = d / 'preview'
    output.mkdir(exist_ok=True)
    code, log = run([ROOT / 'tools/emu/harness', d / 'mod.gba', d / 'preview.txt', output], env=env, timeout=90)
    if code:
        raise ValueError('Emulator preview failed: ' + log)
    shots = []
    for p in sorted(output.glob('*.ppm')):
        dest = p.with_suffix('.png')
        with Image.open(p) as im:
            im.save(dest)
        shots.append(dict(name=p.stem, url=f'/api/forge/artifact/{req["id"]}/{dest.name}'))
    if not shots:
        raise ValueError('Emulator returned no screenshots')
    update(screenshots=shots, preview_log=public_log(log))


def route(handler, method):
    """Small serve.py hook. Forge owns its routes, validation and job state."""
    path = handler.path
    if path not in ('/forge', '/forge.html') and not path.startswith('/api/forge/'):
        return False
    try:
        if method == 'GET':
            if path in ('/forge', '/forge.html'):
                handler.send(200, (HERE / 'forge.html').read_bytes(), 'text/html; charset=utf-8')
            elif path == '/api/forge/functions':
                handler.send(200, list(index().values()))
            elif path.startswith('/api/forge/source/'):
                handler.send(200, source(path.split('/')[-1]))
            elif path == '/api/forge/checkpoints':
                public = [{k: v for k, v in c.items() if k != 'path'} for c in checkpoints(handler.server.trans)]
                handler.send(200, public)
            elif path.startswith('/api/forge/job/'):
                handler.send(200, read_json(job_dir(path.split('/')[-1]) / 'result.json'))
            elif path.startswith('/api/forge/artifact/'):
                parts = path.split('/')
                if len(parts) != 6:
                    raise ValueError('Invalid artifact route')
                d, name = job_dir(parts[4]), parts[5]
                if name == 'mod.ips':
                    file, kind = d / name, 'application/octet-stream'
                elif name == 'source.c':
                    req = read_json(d / 'request.json')
                    file, kind = d / Path(req['function']['file']).name, 'text/plain; charset=utf-8'
                elif re.fullmatch(r'[A-Za-z0-9_-]{1,48}\.png', name):
                    file, kind = d / 'preview' / name, 'image/png'
                else:
                    raise ValueError('Unknown artifact')
                handler.send(200, file.read_bytes(), kind)
            else:
                handler.send(404, {'error': 'Forge route not found'})
        elif method == 'POST':
            # Compile is powerful local tooling. Require browser requests to be same-origin JSON.
            origin = handler.headers.get('Origin')
            if origin and urllib.parse.urlparse(origin).netloc != handler.headers.get('Host'):
                raise ValueError('Forge only accepts same-origin requests')
            if handler.headers.get_content_type() != 'application/json':
                raise ValueError('Send application/json')
            n = int(handler.headers.get('Content-Length', 0))
            if not 0 < n <= MAX_SOURCE * 2:
                raise ValueError('Invalid request size')
            req = json.loads(handler.rfile.read(n))
            if path == '/api/forge/compile':
                handler.send(202, submit(req, handler.server.trans))
            else:
                handler.send(404, {'error': 'Forge route not found'})
    except FileNotFoundError:
        handler.send(404, {'error': 'Forge source, job or artifact not found'})
    except (ValueError, KeyError, TypeError) as e:
        handler.send(400, {'error': str(e)})
    return True


if __name__ == '__main__' and len(sys.argv) == 3 and sys.argv[1] == '--worker':
    worker(sys.argv[2])
