"""The Recorder page: python -m tools.recorder serve NAME [--port 8765].

A timeline of the session's screens; per frame, the writes as typed fields with the C
statement that made them; click a field for its history and its why-slice.
JSON endpoints (all GET): /api/session, /api/screen?f=N (PNG), /api/writes?f=N,
/api/history?where=W&from=A&to=B, /api/why?where=W&frame=F[&check=1].
"""
import contextlib
import io
import json
import struct
import zlib
from collections import OrderedDict
from http.server import ThreadingHTTPServer, BaseHTTPRequestHandler
from pathlib import Path
from urllib.parse import urlparse, parse_qs

import numpy as np

HERE = Path(__file__).resolve().parent


def png(rgb555, width=240, height=160):
    px = np.frombuffer(rgb555, np.uint16).reshape(height, width)
    r = ((px & 31) << 3).astype(np.uint8)
    g = (((px >> 5) & 31) << 3).astype(np.uint8)
    b = (((px >> 10) & 31) << 3).astype(np.uint8)
    rows = np.dstack([r, g, b]).reshape(height, width * 3)
    raw = b''.join(b'\0' + rows[y].tobytes() for y in range(height))

    def chunk(kind, data):
        c = struct.pack('>I', len(data)) + kind + data
        return c + struct.pack('>I', zlib.crc32(kind + data) & 0xFFFFFFFF)
    return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress(raw, 6)) + chunk(b'IEND', b''))


class App:
    def __init__(self, name):
        from facts import Facts
        from lift import Types
        from session import Session
        self.session = Session(name) if Path(name).exists() else Session(Path(HERE.parents[1] / 'build/recorder/sessions' / name))
        self.facts, self.types = Facts(), Types()
        self.maps = OrderedDict()

    def objmap(self, frame):
        from heap import Namer, HeapNamer
        from slicer import Slicer
        if not hasattr(self, 'heap'):
            self.slicer = Slicer(self.session, self.facts)
            self.heap = HeapNamer(self.session, self.facts, self.types, self.slicer)
        om = self.maps.get(frame)
        if om is None:
            om = self.maps[frame] = Namer(self.session, self.facts, self.types, frame, slicer=self.slicer, heap=self.heap)
            while len(self.maps) > 8:
                self.maps.popitem(last=False)
        return om

    def session_info(self):
        s = self.session
        screens = sorted(int(p.stem) for p in (s.path / 'screens').glob('*.rgb'))
        s.writes
        per_frame = np.diff(s.frame_start)
        return {'name': s.name, 'frames': len(s), 'meta': s.meta, 'screens': screens,
                'writes': [int(x) for x in per_frame], 'keys': [int(k) for k in s.frames['keys']],
                'solar': [int(x) for x in s.frames['solar']]}

    def screen(self, f):
        s = self.session
        screens = sorted(int(p.stem) for p in (s.path / 'screens').glob('*.rgb'))
        best = max((x for x in screens if x <= f), default=screens[0] if screens else None)
        if best is None:
            return None
        return png((s.path / 'screens' / f'{best:08d}.rgb').read_bytes())

    def frame_writes(self, f):
        from lift import join
        cli = _cli()
        s = self.session
        sl = s.writes_in(f, f)
        om = self.objmap(f)
        heap = s.meta.get('heapBase', 0)
        groups = OrderedDict()
        for i in range(sl.start, sl.stop):
            a, w, site = int(s.addr[i]), int(s.width[i]), int(s.writes['site'][i])
            if a < heap:
                continue
            label, path, leaf, base = om.at(a, w)
            name = join(label, path)
            key = (name, site)
            g = groups.get(key)
            if g is None:
                fn, loc, text, lv = cli.statement(self.facts, site)
                g = groups[key] = {'place': name, 'addr': a, 'width': w, 'fn': fn, 'loc': loc, 'text': text, 'count': 0,
                                   'object': label}
            g['count'] += 1
            g['last'] = (int(s.writes['value'][i]) >> (8 * max(0, g['addr'] - a))) & 0xFFFFFFFF
            if len(groups) > 600:
                break
        # each field's value at the end of the frame, at the field's own width
        for g in groups.values():
            _, _, leaf, _ = om.at(g['addr'], g['width'])
            w, signed = cli.leaf_info(self.types, leaf) if leaf is not None else (min(g['width'], 4), False)
            w = min(w, 4)
            g['width'] = w
            v = om.ram.read(g['addr'], w, signed)
            g['value'] = v if v is not None else g['last'] & ((1 << (8 * w)) - 1)   # IO: not in RAM snapshots
        return {'frame': f, 'rows': int(sl.stop - sl.start), 'writes': list(groups.values()),
                'keys': int(s.frames['keys'][f]), 'solar': int(s.frames['solar'][f])}


def _cli():
    import importlib.util
    import sys
    if 'recorder_cli' in sys.modules:
        return sys.modules['recorder_cli']
    spec = importlib.util.spec_from_file_location('recorder_cli', HERE / '__main__.py')
    mod = importlib.util.module_from_spec(spec)
    sys.modules['recorder_cli'] = mod
    spec.loader.exec_module(mod)
    return mod


def run_cli(argv):
    out = io.StringIO()
    with contextlib.redirect_stdout(out), contextlib.redirect_stderr(io.StringIO()):
        try:
            _cli().main(argv)
        except SystemExit as e:
            if e.code not in (0, None):
                print(e.code)
    return out.getvalue()


SESSIONS = HERE.parents[1] / 'build/recorder/sessions'
_apps = {}


def sessions():
    """Recorded sessions, newest first."""
    out = [p for p in SESSIONS.glob('*') if (p / 'meta.json').exists()] if SESSIONS.exists() else []
    return sorted(out, key=lambda p: -(p / 'meta.json').stat().st_mtime)


def app_for(name=None):
    if not name:
        found = sessions()
        if not found:
            raise FileNotFoundError('no recorded sessions in build/recorder/sessions')
        name = found[0].name
    app = _apps.get(name)
    if app is None:
        app = _apps[name] = App(name)
    return app


def page():
    return (HERE / 'web' / 'index.html').read_bytes()


def api(sub, q, app=None):
    """(code, body, content type) for the Recorder API path SUB (after the API prefix)."""
    try:
        if sub == '/sessions':
            return 200, [{'name': p.name, 'frames': json.loads((p / 'meta.json').read_text()).get('frames')} for p in sessions()], None
        app = app or app_for(q.get('session'))
        if sub == '/session':
            return 200, app.session_info(), None
        if sub == '/screen':
            data = app.screen(int(q.get('f', 0)))
            return (200, data, 'image/png') if data else (404, {'error': 'no screens'}, None)
        if sub == '/writes':
            return 200, app.frame_writes(int(q['f'])), None
        if sub == '/history':
            argv = ['history', str(app.session.path), q['where'], '--changes', '--limit', q.get('limit', '200')]
            if 'from' in q:
                argv += ['--from', q['from']]
            if 'to' in q:
                argv += ['--to', q['to']]
            return 200, {'text': run_cli(argv)}, None
        if sub == '/why':
            argv = ['why', str(app.session.path), q['where'], q['frame']] + (['--check'] if q.get('check') else [])
            return 200, {'text': run_cli(argv)}, None
        return 404, {'error': 'not found'}, None
    except Exception as e:  # reported to the page
        return 500, {'error': f'{type(e).__name__}: {e}'}, None


def serve(name, port=8765):
    app = app_for(name)

    class Handler(BaseHTTPRequestHandler):
        def log_message(self, *a):
            pass

        def send(self, body, kind=None, code=200):
            kind = kind or 'application/json'
            if isinstance(body, (dict, list)):
                body = json.dumps(body).encode()
            elif isinstance(body, str):
                body = body.encode()
            self.send_response(code)
            self.send_header('Content-Type', kind)
            self.send_header('Content-Length', str(len(body)))
            self.end_headers()
            self.wfile.write(body)

        def do_GET(self):
            u = urlparse(self.path)
            q = {k: v[0] for k, v in parse_qs(u.query).items()}
            if u.path == '/':
                return self.send(page(), 'text/html; charset=utf-8')
            if u.path.startswith('/api/'):
                code, body, kind = api(u.path[4:], q, None if q.get('session') else app)
                return self.send(body, kind, code)
            self.send({'error': 'not found'}, code=404)

    srv = ThreadingHTTPServer(('127.0.0.1', port), Handler)
    print(f'Recorder: http://127.0.0.1:{port}/  ({app.session.name}, {len(app.session)} frames)')
    srv.serve_forever()
