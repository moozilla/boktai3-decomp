"""Export a session's typed history to DuckDB (default) or SQLite.

    python -m tools.recorder export NAME [--db FILE] [--sqlite]

Tables
  frames(frame, keys, solar, flags, hash_lo, hash_hi, writes)
  sites(site, kind, function, file, line, text, lvalue)       every write site in the log
  writes(seq, frame, site, addr, width, value)                 the write log, in order
  globals(name, addr, size, type)                              RAM globals (types.json)
  objects(frame, addr, size, type, label)                      typed objects at each snapshot frame
  fields(type, off, size, path)                                scalar leaves of every type used above
Views
  typed_writes: each write named `object.path` through the latest snapshot's objects and the
  globals (falls back to the raw address). DuckDB only (it needs range joins to be fast).

  duckdb build/recorder/sessions/field/history.duckdb \\
    "select frame, name, value, function, line from typed_writes where name like 'Player#%.position.x' limit 5"
"""
import csv
import json
import os
import tempfile
from pathlib import Path

import numpy as np

from facts import Facts, Inst, Function, Block, KIND_NAMES
from lift import Types, ObjectMap


def _leaves(types, t, off=0, path='', depth=0, out=None):
    out = [] if out is None else out
    d = types.resolve(t)
    if d is None or depth > 6:
        return out
    k = d.get('k')
    if k in ('struct', 'union'):
        for f in d.get('fields', []):
            if 'bits' not in f:
                _leaves(types, f['type'], off + f['off'], f"{path}.{f['name']}", depth + 1, out)
    elif k == 'array':
        n, es = d.get('n', 0), types.sizeof(d['of'])
        for i in range(min(n, 64) if es else 0):
            _leaves(types, d['of'], off + i * es, f'{path}[{i}]', depth + 1, out)
    else:
        out.append((off, types.sizeof(t) or 4, path))
    return out


def export(session, db=None, sqlite=False, log=print):
    from slicer import c_line
    from facts import lvalue_text
    s = session
    facts, types = Facts(), Types()
    db = Path(db) if db else s.path / ('history.sqlite' if sqlite else 'history.duckdb')
    if db.exists():
        db.unlink()
    s.writes
    tmp = Path(tempfile.mkdtemp(prefix='recexport', dir=s.path))
    try:
        # frames
        f = s.frames
        nwrites = np.diff(s.frame_start)
        np.savetxt(tmp / 'frames.csv', np.stack([f['frame'], f['keys'], f['solar'], f['flags'], f['lo'], f['hi'], nwrites], 1),
                   fmt='%d', delimiter=',')
        # writes
        w = s.writes
        arr = np.stack([np.arange(len(w)), w['frame'], w['site'], s.addr, s.width, w['value']], 1).astype(np.int64)
        with open(tmp / 'writes.csv', 'w') as fh:
            for i in range(0, len(arr), 1 << 20):
                np.savetxt(fh, arr[i:i + (1 << 20)], fmt='%d', delimiter=',')
        log(f'  {len(w)} writes')
        # sites
        with open(tmp / 'sites.csv', 'w', newline='') as fh:
            cw = csv.writer(fh)
            for site in np.unique(w['site']):
                x = facts.site(int(site))
                fn = x if isinstance(x, Function) else getattr(x, 'fn', None)
                if isinstance(x, Inst):
                    unit, line, text = c_line(facts, x)
                    lv = lvalue_text(text, x.col) if text else None
                else:
                    unit, line, text, lv = (fn.unit.name if fn else None), None, None, None
                cw.writerow([int(site), KIND_NAMES[int(site) >> 29], fn.name if fn else None, unit, line,
                             (text or '').strip() or None, lv])
        # globals, objects, fields
        used_types = set()
        with open(tmp / 'globals.csv', 'w', newline='') as fh:
            cw = csv.writer(fh)
            for a, size, name, t in types.static:
                tn = t if isinstance(t, str) else json.dumps(t)
                used_types.add(tn)
                cw.writerow([name, a, size, tn])
        with open(tmp / 'objects.csv', 'w', newline='') as fh:
            cw = csv.writer(fh)
            for frame in s.snapshots:
                om = ObjectMap(types, s.ram_snapshot(frame))
                for a, size, tname, label in om.objects:
                    used_types.add(tname)
                    cw.writerow([frame, a, size, tname, label])
        with open(tmp / 'fields.csv', 'w', newline='') as fh:
            cw = csv.writer(fh)
            for tn in used_types:
                t = json.loads(tn) if tn.startswith('{') else tn
                for off, size, path in _leaves(types, t):
                    cw.writerow([tn, off, size, path])
        schema = {
            'frames': 'frame INTEGER, keys INTEGER, solar INTEGER, flags INTEGER, hash_lo BIGINT, hash_hi BIGINT, writes INTEGER',
            'writes': 'seq BIGINT, frame INTEGER, site BIGINT, addr BIGINT, width INTEGER, value BIGINT',
            'sites': 'site BIGINT, kind TEXT, function TEXT, file TEXT, line INTEGER, text TEXT, lvalue TEXT',
            'globals': 'name TEXT, addr BIGINT, size INTEGER, type TEXT',
            'objects': 'frame INTEGER, addr BIGINT, size INTEGER, type TEXT, label TEXT',
            'fields': 'type TEXT, off INTEGER, size INTEGER, path TEXT',
        }
        if sqlite:
            import sqlite3
            con = sqlite3.connect(db)
            for name, cols in schema.items():
                con.execute(f'CREATE TABLE {name} ({cols})')
                n = len(cols.split(','))
                with open(tmp / f'{name}.csv', newline='') as fh:
                    rows = ([None if v == '' else v for v in r] for r in csv.reader(fh))
                    con.executemany(f'INSERT INTO {name} VALUES ({",".join("?" * n)})', rows)
            con.execute('CREATE INDEX writes_addr ON writes(addr)')
            con.execute('CREATE INDEX writes_frame ON writes(frame)')
            con.commit()
            con.close()
        else:
            import duckdb
            con = duckdb.connect(str(db))
            for name, cols in schema.items():
                con.execute(f'CREATE TABLE {name} ({cols})')
                con.execute(f"INSERT INTO {name} SELECT * FROM read_csv('{tmp / (name + '.csv')}', header=false, "
                            f"columns={{{', '.join(repr(c.split()[0]) + ': ' + repr(c.split()[1]) for c in cols.split(', '))}}})")
            con.execute('''
                CREATE VIEW typed_writes AS
                WITH snap AS (SELECT DISTINCT frame FROM objects),
                w AS (SELECT w.*, (SELECT max(frame) FROM snap WHERE snap.frame <= w.frame) AS snap FROM writes w),
                named AS (
                  SELECT w.seq, o.label || CASE WHEN f.off IS NULL THEN printf('+0x%X', w.addr - o.addr) ELSE coalesce(f.path, '') END AS name, o.size AS osize
                  FROM w JOIN objects o ON o.frame = w.snap AND w.addr >= o.addr AND w.addr < o.addr + o.size
                  LEFT JOIN fields f ON f.type = o.type AND w.addr - o.addr >= f.off AND w.addr - o.addr < f.off + f.size
                  UNION ALL
                  SELECT w.seq, g.name || CASE WHEN f.off IS NULL THEN printf('+0x%X', w.addr - g.addr) ELSE coalesce(f.path, '') END, 1 << 30
                  FROM w JOIN globals g ON w.addr >= g.addr AND w.addr < g.addr + g.size
                  LEFT JOIN fields f ON f.type = g.type AND w.addr - g.addr >= f.off AND w.addr - g.addr < f.off + f.size
                ),
                best AS (SELECT seq, arg_min(name, osize) AS name FROM named GROUP BY seq)
                SELECT w.seq, w.frame, coalesce(b.name, printf('0x%08X', w.addr)) AS name, w.addr, w.width, w.value,
                       s.function, s.file || ':' || s.line AS line, s.text
                FROM writes w LEFT JOIN best b USING (seq) LEFT JOIN sites s USING (site)
            ''')
            con.close()
    finally:
        for p in tmp.iterdir():
            p.unlink()
        os.rmdir(tmp)
    log(f'  -> {db} ({db.stat().st_size >> 20} MB)')
    return db
