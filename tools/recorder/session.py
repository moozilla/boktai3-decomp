"""A recorded session on disk (written by tools/recorder/runner.mjs record).

frames.bin  32-byte rows: frame, keys, solar, flags, hash lo, hash hi, write rows, 0
writes.bin  16-byte rows: frame, site, address | wcode << 28, value   (wcode: width 1 << wcode)
writes.z    the same rows compacted (compact()): chunks of up to 1M rows, each
            [rows u32, compressed length u32] + zlib(frame column, site, addrw, value)
snap/       gzip: [count, stack pointer, (address, length) * count] then the bytes
edits.json  [{frame, bytes: [[address, value], ...]}]: memory the script changed at the
            end of that frame, after the RAM hash was taken
"""
import gzip
import json
import struct
import zlib
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
SESSIONS = ROOT / 'build/recorder/sessions'
EWRAM, IWRAM = 0x02000000, 0x03000000
FRAME = np.dtype([('frame', '<u4'), ('keys', '<u4'), ('solar', '<u4'), ('flags', '<u4'),
                  ('lo', '<u4'), ('hi', '<u4'), ('writes', '<u4'), ('pad', '<u4')])
WRITE = np.dtype([('frame', '<u4'), ('site', '<u4'), ('addrw', '<u4'), ('value', '<u4')])
KEYS = ['A', 'B', 'SELECT', 'START', 'RIGHT', 'LEFT', 'UP', 'DOWN', 'R', 'L']


def key_names(mask):
    return '+'.join(k for i, k in enumerate(KEYS) if mask >> i & 1) or '-'


class Ram:
    """EWRAM + IWRAM as one byte array, addressed by GBA address."""

    def __init__(self, ewram, iwram):
        self.ewram = np.frombuffer(bytearray(ewram), np.uint8).copy()
        self.iwram = np.frombuffer(bytearray(iwram), np.uint8).copy()

    def copy(self):
        return Ram(self.ewram.tobytes(), self.iwram.tobytes())

    def region(self, address):
        if EWRAM <= address < EWRAM + 0x40000:
            return self.ewram, address - EWRAM
        if IWRAM <= address < IWRAM + 0x8000:
            return self.iwram, address - IWRAM
        return None, None

    def read(self, address, width, signed=False):
        mem, off = self.region(address)
        if mem is None or off + width > len(mem):
            return None
        v = int.from_bytes(mem[off:off + width].tobytes(), 'little')
        if signed and v >> (8 * width - 1):
            v -= 1 << (8 * width)
        return v

    def bytes(self, address, length):
        mem, off = self.region(address)
        return None if mem is None else mem[off:off + length].tobytes()

    def apply(self, addrs, widths, values):
        """Apply write rows in order (later rows win)."""
        for k in (1, 2, 4):
            pass
        # expand to bytes, keeping order
        n = len(addrs)
        if not n:
            return
        reps = widths.astype(np.int64)
        idx = np.repeat(np.arange(n), reps)
        offs = np.arange(len(idx)) - np.repeat(np.cumsum(reps) - reps, reps)
        byte_addr = addrs[idx].astype(np.int64) + offs
        byte_val = ((values[idx].astype(np.uint64) >> (8 * offs.astype(np.uint64))) & 0xFF).astype(np.uint8)
        for mem, base in ((self.ewram, EWRAM), (self.iwram, IWRAM)):
            sel = (byte_addr >= base) & (byte_addr < base + len(mem))
            if sel.any():
                # numpy assigns repeated indices in order; the last write wins
                mem[byte_addr[sel] - base] = byte_val[sel]


class Session:
    def __init__(self, path):
        path = Path(path)
        if not path.exists() and (SESSIONS / str(path)).exists():
            path = SESSIONS / str(path)
        self.path = path
        self.name = path.name
        self.meta = json.loads((path / 'meta.json').read_text())
        self.frames = np.fromfile(path / 'frames.bin', FRAME)
        self._writes = None
        self.edits = {e['frame']: e['bytes'] for e in json.loads((path / 'edits.json').read_text())}
        self.snapshots = sorted(int(p.name[:8]) for p in (path / 'snap').glob('*.gz'))
        self._snap_cache = {}

    def __len__(self):
        return len(self.frames)

    @property
    def writes(self):
        if self._writes is None:
            w = read_writes(self.path)
            self._writes = w
            self.addr = (w['addrw'] & 0x0FFFFFFF).astype(np.uint32)
            self.width = (1 << ((w['addrw'] >> 28) & 3)).astype(np.uint32)
            self.frame_start = np.searchsorted(w['frame'], np.arange(len(self.frames) + 1))
        return self._writes

    def writes_in(self, first, last):
        """Row slice of the writes of frames first..last inclusive."""
        self.writes
        return slice(self.frame_start[first], self.frame_start[last + 1])

    def snapshot(self, frame):
        if frame in self._snap_cache:
            return self._snap_cache[frame]
        raw = gzip.decompress((self.path / 'snap' / f'{frame:08d}.gz').read_bytes())
        n, sp = struct.unpack_from('<II', raw, 0)
        off = 8 + 8 * n
        parts = {}
        for i in range(n):
            a, ln = struct.unpack_from('<II', raw, 8 + 8 * i)
            parts[a] = raw[off:off + ln]
            off += ln
        self._snap_cache = {frame: parts}
        return parts

    def ram_snapshot(self, frame):
        parts = self.snapshot(frame)
        return Ram(parts[EWRAM], parts[IWRAM])

    def ram_at(self, frame, after_edits=False):
        """RAM at the end of FRAME (before the script's edits there unless after_edits)."""
        s = max((x for x in self.snapshots if x <= frame), default=None)
        if s is None:
            raise ValueError(f'no snapshot at or before frame {frame}')
        ram = self.ram_snapshot(s)
        self.writes
        for k in range(s + 1, frame + 1):
            sl = self.writes_in(k, k)
            ram.apply(self.addr[sl], self.width[sl], self._writes['value'][sl])
            if k < frame or after_edits:
                self._apply_edits(ram, k)
        if frame == s and not after_edits:
            pass  # a snapshot holds the state after its edits; frames with edits are snapshots too
        return ram

    def _apply_edits(self, ram, frame):
        for a, v in self.edits.get(frame, ()):
            mem, off = ram.region(a)
            if mem is not None:
                mem[off] = v

    def verify(self, log=print):
        """Write log completeness: each snapshot = the previous one + the logged writes."""
        bad = 0
        for a, b in zip(self.snapshots, self.snapshots[1:]):
            ram = self.ram_at(b, after_edits=True)
            want = self.ram_snapshot(b)
            for name, x, y, base in (('EWRAM', ram.ewram, want.ewram, EWRAM), ('IWRAM', ram.iwram, want.iwram, IWRAM)):
                diff = np.nonzero(x != y)[0]
                if len(diff):
                    bad += 1
                    log(f'frames {a}..{b}: {name} differs at {len(diff)} bytes, first {base + int(diff[0]):08X} '
                        f'(log {x[diff[0]]:02X}, snapshot {y[diff[0]]:02X})')
        return bad


CHUNK = 1 << 20


def read_writes(path):
    path = Path(path)
    if (path / 'writes.bin').exists():
        return np.fromfile(path / 'writes.bin', WRITE)
    parts = []
    with open(path / 'writes.z', 'rb') as f:
        while True:
            head = f.read(8)
            if len(head) < 8:
                break
            n, size = struct.unpack('<II', head)
            cols = np.frombuffer(zlib.decompress(f.read(size)), np.uint32).reshape(4, n)
            w = np.empty(n, WRITE)
            for i, name in enumerate(WRITE.names):
                w[name] = cols[i]
            parts.append(w)
    return np.concatenate(parts) if parts else np.empty(0, WRITE)


def compact(path, log=print):
    """writes.bin -> writes.z (column-chunked zlib, about a tenth of the size)."""
    path = Path(path)
    src = path / 'writes.bin'
    if not src.exists():
        return
    w = np.memmap(src, WRITE, 'r')
    tmp = path / 'writes.z.tmp'
    with open(tmp, 'wb') as f:
        for i in range(0, len(w), CHUNK):
            c = w[i:i + CHUNK]
            data = zlib.compress(b''.join(np.ascontiguousarray(c[name]).tobytes() for name in WRITE.names), 1)
            f.write(struct.pack('<II', len(c), len(data)))
            f.write(data)
    n = len(w)
    del w
    before = src.stat().st_size
    tmp.rename(path / 'writes.z')
    src.unlink()
    log(f'compacted {path.name}: {n} writes, {before >> 20} MB -> {(path / "writes.z").stat().st_size >> 20} MB')
