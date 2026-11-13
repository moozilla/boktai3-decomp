#!/usr/bin/env python3
"""Play the Boktai 3 source port in a browser: one command.

    python3 port/serve.py            (or: make port-demo)

Builds build/port/game/game.wasm (cached; needs build/boktai3.elf from make or
tools/worktree.sh --here), serves port/web/ on http://127.0.0.1:PORT and opens
it. The page loads baserom.gba from the repository root automatically; nothing
is uploaded anywhere and nothing derived from the ROM is written outside build/.

Saves: the game's EEPROM is written to build/port/saves/boktai3.sav as you
play. The page can also start from tests/saves/*.sav or build/saves/*.sav
(python3 tools/assets/saves.py bank --make builds the story checkpoints).
"""
import argparse
import http.server
import json
from pathlib import Path
import socketserver
import subprocess
import sys
import threading
import urllib.parse
import webbrowser

ROOT = Path(__file__).resolve().parents[1]
WEB = ROOT / 'port/web'
WASM = ROOT / 'build/port/game/game.wasm'
ROM = ROOT / 'baserom.gba'
SAVES = ROOT / 'build/port/saves'
WRITABLE = 'boktai3.sav'
CHECKPOINTS = [ROOT / 'tests/saves', ROOT / 'build/saves']
TYPES = {'.html': 'text/html; charset=utf-8', '.js': 'text/javascript', '.wasm': 'application/wasm',
         '.json': 'application/json', '.gba': 'application/octet-stream', '.sav': 'application/octet-stream'}


def save_files():
    names = []
    for d in CHECKPOINTS:
        if d.is_dir():
            names += sorted(p.name for p in d.glob('*.sav') if p.stat().st_size == 8192)
    return names


def find_save(name):
    if '/' in name or '\\' in name or not name.endswith('.sav'):
        return None
    for d in [SAVES] + CHECKPOINTS:
        if (d / name).is_file():
            return d / name
    return None


class Handler(http.server.BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        pass

    def send(self, code, body=b'', kind='text/plain'):
        self.send_response(code)
        # SharedArrayBuffer (input, pacing, audio) needs cross-origin isolation.
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cache-Control', 'no-store')
        self.send_header('Content-Type', kind)
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        if self.command != 'HEAD':
            self.wfile.write(body)

    def do_GET(self):
        path = urllib.parse.unquote(urllib.parse.urlparse(self.path).path).lstrip('/') or 'index.html'
        if path == 'config.json':
            body = {'local': True, 'rom': ROM.is_file(), 'writable': WRITABLE,
                    'hasWritable': (SAVES / WRITABLE).is_file(), 'saves': save_files()}
            return self.send(200, json.dumps(body).encode(), TYPES['.json'])
        if path == 'game.wasm':
            return self.file(WASM)
        if path == 'rom.gba':
            return self.file(ROM)
        if path.startswith('saves/'):
            found = find_save(path[6:])
            return self.file(found) if found else self.send(404)
        target = (WEB / path).resolve()
        if target.parent != WEB.resolve():
            return self.send(404)
        return self.file(target)

    do_HEAD = do_GET

    def do_PUT(self):
        path = urllib.parse.unquote(urllib.parse.urlparse(self.path).path).lstrip('/')
        length = int(self.headers.get('Content-Length', 0))
        data = self.rfile.read(length)
        if path != 'saves/' + WRITABLE or length != 8192:
            return self.send(403)
        SAVES.mkdir(parents=True, exist_ok=True)
        tmp = SAVES / (WRITABLE + '.tmp')
        tmp.write_bytes(data)
        tmp.replace(SAVES / WRITABLE)
        return self.send(204)

    def file(self, path):
        if not path or not path.is_file():
            return self.send(404)
        return self.send(200, path.read_bytes(), TYPES.get(path.suffix, 'application/octet-stream'))


class Server(socketserver.ThreadingMixIn, http.server.HTTPServer):
    daemon_threads = True
    allow_reuse_address = True


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--port', type=int, default=8765)
    ap.add_argument('--no-build', action='store_true', help='serve the existing game.wasm')
    ap.add_argument('--no-open', action='store_true', help='do not open a browser')
    args = ap.parse_args()
    if not ROM.is_file():
        print('note: baserom.gba is not in the repository root; the page will ask for the ROM file.')
    if not args.no_build:
        print('Building the game for WebAssembly (cached after the first build)...', flush=True)
        if subprocess.run([sys.executable, str(ROOT / 'port/build_game.py')]).returncode:
            sys.exit('build failed')
    if not WASM.is_file():
        sys.exit(f'{WASM} is missing; run python3 port/build_game.py')
    for port in range(args.port, args.port + 20):
        try:
            server = Server(('127.0.0.1', port), Handler)
            break
        except OSError:
            continue
    else:
        sys.exit('no free port')
    url = f'http://127.0.0.1:{port}/'
    print(f'Boktai 3 source port: {url}  (Ctrl+C to stop)', flush=True)
    if not args.no_open:
        threading.Timer(0.3, webbrowser.open, [url]).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
