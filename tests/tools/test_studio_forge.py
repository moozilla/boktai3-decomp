"""Forge boundary tests use synthetic ROMs; optional compiler tests use local inputs."""
import hashlib
import http.server
import io
import json
import os
from pathlib import Path
import struct
import sys
import time
import unittest
from types import SimpleNamespace
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/studio'))
sys.path.insert(0, str(ROOT / 'tools'))
import forge


def apply_ips(original, patch_data):
    if not patch_data.startswith(b'PATCH'):
        raise ValueError('bad IPS header')
    rom, at = bytearray(original), 5
    while patch_data[at:at + 3] != b'EOF':
        offset = int.from_bytes(patch_data[at:at + 3], 'big')
        length = int.from_bytes(patch_data[at + 3:at + 5], 'big')
        at += 5
        rom[offset:offset + length] = patch_data[at:at + length]
        at += length
    return bytes(rom)


class PatchTests(unittest.TestCase):
    def test_patch_round_trips_separated_and_long_edits(self):
        before = b'\0' * 140000
        after = bytearray(before)
        after[0] = 1
        after[50:135000] = b'\x99' * 134950
        after[-1] = 3
        self.assertEqual(apply_ips(before, forge.ips_patch(before, after)), after)
        self.assertEqual(forge.ips_patch(before, before), b'PATCHEOF')

    def test_patch_handles_reserved_eof_offset(self):
        before = b'\0' * (0x454F46 + 3)
        after = bytearray(before)
        after[0x454F46] = 1
        self.assertEqual(apply_ips(before, forge.ips_patch(before, after)), after)

    def test_reject_size_changes(self):
        with self.assertRaises(ValueError):
            forge.ips_patch(b'a', b'aa')


class InputTests(unittest.TestCase):
    def test_scripts_are_bounded_and_cannot_read_or_write_arbitrary_paths(self):
        for script in ('loadstate ../../secret', 'shot ../outside', 'record movie.mp4', 'poke 02000000 1',
                       'wait 7201', 'hold A 999999999', 'tap X', 'shot ' + 'x' * 49,
                       '\n'.join('shot x' for _ in range(13))):
            with self.subTest(script=script), self.assertRaises(ValueError):
                forge.validate_script(script)
        self.assertEqual(forge.validate_script('hold A+UP 20\ntap START'), 'hold A+UP 20\ntap START\nshot result\n')

    def test_job_ids_are_opaque(self):
        for query in ('../outside', '/tmp/x', 'a' * 31, 'z' * 32, None):
            with self.assertRaises(ValueError):
                forge.job_dir(query)

    def test_keep_preprocessor_and_toolchain_settings_fixed(self):
        baseline = '#include "global.h"\n// CFLAGS: -O2\nint f(void) { return 1; }\n'
        forge.validate_source(baseline.replace('return 1', 'return 2'), baseline)
        for source in (baseline.replace('-O2', '-O0'), baseline + '#include "/etc/passwd"\n', '', 'x\0y'):
            with self.assertRaises(ValueError):
                forge.validate_source(source, baseline)

    def test_no_matching_knobs_in_production_environment(self):
        with patch.dict(os.environ, {'AGBCC_FORCE': '100:0', 'AGBCC': 'other', 'CSE_KEEP': '101',
                                     'SCOPE_RUN': 'other', 'CPP': 'other', 'BOKTAI3_SAV': 'other'}):
            env = forge.clean_env()
            self.assertFalse(any(k in env for k in ('AGBCC_FORCE', 'AGBCC', 'CSE_KEEP', 'SCOPE_RUN', 'CPP', 'BOKTAI3_SAV')))

    def test_stale_source_and_bad_actions_do_not_enqueue(self):
        f = dict(addr='08000000', source='int f(void){return 1;}', sha256='original', file='src/f.c')
        with patch.object(forge, 'source', return_value=f):
            with self.assertRaisesRegex(ValueError, 'changed on disk'):
                forge.submit(dict(function='f', source=f['source'], sha256='stale'))
            with self.assertRaisesRegex(ValueError, 'Action'):
                forge.submit(dict(function='f', source=f['source'], sha256='original', action='shell'))

    def test_text_edits_reject_duplicate_and_non_integer_ids(self):
        for edits in ([dict(id=1, text='a'), dict(id=1, text='b')], [dict(id='1', text='a')], [dict(id=True, text='a')]):
            with self.assertRaises(ValueError):
                forge.validate_text_edits(edits)

    def test_text_changes_keep_string_offsets_and_rom_size(self):
        from datafmt import text_bank
        # A synthetic bank with a string and a binary record; no game bytes.
        rom = bytearray(b'\0' * 64)
        struct.pack_into('<II', rom, 0, 0x80000000, 8)
        rom[16:24] = b'LONGTXT\0'
        with patch.multiple(text_bank, TABLE=0x08000000, BANK=0x08000010), patch.object(text_bank, 'charmap', return_value={}):
            forge.apply_text_edits(rom, [dict(id=0, text='NEW')])
            self.assertEqual(rom[16:24], b'NEW\0\0\0\0\0')
            self.assertEqual(len(rom), 64)
            with self.assertRaisesRegex(ValueError, 'budget'):
                forge.apply_text_edits(rom, [dict(id=0, text='EXCESSIVELY LONG')])
            with self.assertRaisesRegex(ValueError, 'binary'):
                forge.apply_text_edits(rom, [dict(id=1, text='x')])


class HTTPTests(unittest.TestCase):
    def request(self, method, path, body=b'', headers=None):
        # Exercise BaseHTTPRequestHandler's real parser and response writer without
        # binding a listening socket (the managed worker sandbox forbids that).
        class Socket:
            def __init__(self, request):
                self.request = io.BytesIO(request)
                self.response = bytearray()
            def makefile(self, *args):
                return self.request
            def sendall(self, data):
                self.response.extend(data)
        class Handler(http.server.BaseHTTPRequestHandler):
            def send(self, status, body, kind='application/json'):
                self.send_response(status)
                self.send_header('Content-Type', kind)
                self.end_headers()
                self.wfile.write(json.dumps(body).encode() if isinstance(body, (dict, list)) else body)
            def do_GET(self):
                forge.route(self, 'GET')
            def do_POST(self):
                forge.route(self, 'POST')
            def log_message(self, *args):
                pass
        headers = dict(headers or {})
        headers.update(Host='localhost:8642', **{'Content-Length': str(len(body))})
        raw = f'{method} {path} HTTP/1.0\r\n' + ''.join(f'{k}: {v}\r\n' for k, v in headers.items()) + '\r\n'
        sock = Socket(raw.encode() + body)
        Handler(sock, ('127.0.0.1', 1), SimpleNamespace(trans='/nonexistent'))
        head, data = bytes(sock.response).split(b'\r\n\r\n', 1)
        return int(head.split()[1]), data

    def test_page_and_unknown_job(self):
        code, body = self.request('GET', '/forge')
        self.assertEqual(code, 200)
        self.assertIn(b'Build & play', body)
        code, _ = self.request('GET', '/api/forge/job/' + '0' * 32)
        self.assertEqual(code, 404)

    def test_reject_cross_origin_and_non_json_compile_requests(self):
        for headers in ({'Content-Type': 'text/plain'}, {'Content-Type': 'application/json', 'Origin': 'http://elsewhere.invalid'}):
            code, _ = self.request('POST', '/api/forge/compile', b'{}', headers)
            self.assertEqual(code, 400)

    def test_reject_artifact_traversal_and_rom_downloads(self):
        for suffix in ('../../baserom.gba', '0' * 32 + '/mod.gba', '0' * 32 + '/request.json'):
            code, _ = self.request('GET', '/api/forge/artifact/' + suffix)
            self.assertIn(code, (400, 404))


@unittest.skipUnless(os.environ.get('BOKTAI3_FORGE_INTEGRATION'), 'opt-in local agbcc/ROM integration')
class CompilerTests(unittest.TestCase):
    def experiment(self, transform=lambda x: x, **extra):
        f = forge.source('Boot_ClearReservedWords')
        source = transform(f['source'])
        job = forge.submit(dict(function=f['addr'], source=source, sha256=f['sha256'], **extra))
        deadline = time.monotonic() + 180
        while time.monotonic() < deadline:
            result = forge.read_json(forge.job_dir(job['id']) / 'result.json')
            if result['state'] in ('done', 'error'):
                return result
            time.sleep(.2)
        self.fail('Forge job did not finish')

    def test_original_matches_and_real_mod_patch_reconstructs_snapshot(self):
        original = self.experiment()
        self.assertEqual(original['state'], 'done', original)
        self.assertEqual(original['comparison']['status'], 'match')
        self.assertTrue(original['scope_available'])
        modified = self.experiment(lambda s: s.replace('u32 zero = 0;', 'u32 zero = 1;'))
        self.assertEqual(modified['state'], 'done', modified)
        self.assertEqual(modified['comparison']['status'], 'mismatch')
        self.assertTrue(modified['can_play'])
        d = forge.job_dir(modified['id'])
        rom = (ROOT / 'baserom.gba').read_bytes()
        patched = apply_ips(rom, (d / 'mod.ips').read_bytes())
        self.assertEqual(hashlib.sha256(patched).hexdigest(), modified['modified_sha256'])
        self.assertNotEqual(patched, rom)
        self.assertIn('explanation', modified)

    def test_compile_error_is_a_job_result(self):
        result = self.experiment(lambda s: s.replace('u32 zero = 0;', 'u32 zero = ;'))
        self.assertEqual(result['state'], 'error')
        self.assertNotIn('patch_url', result)


if __name__ == '__main__':
    unittest.main()
