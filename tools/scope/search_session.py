"""Durable source search state; the caller, rather than a score, chooses the policy.

A node is exactly (source text, compile context). Moves are separate edges, so
transpositions share source/evaluation storage and undoing a move need not invent
another program. Such an undo can form a cycle: this is a state graph, while its
append-only event log is the chronological history. No evaluation removes a node
from the frontier. A policy may park a branch and return to it later.

SQLite WAL permits separate workers to read and add branches concurrently. Keep
sessions under build/: they contain source snapshots and ROM-derived feedback.
The context must fingerprint everything outside the source that affects a result
(compiler, flags, headers/preprocessed prefix, target bytes, evaluator version).
"""
from __future__ import annotations

import contextlib
import datetime
import difflib
import hashlib
import json
import os
from pathlib import Path
import sqlite3
import tempfile
import time
from typing import Any
import uuid


SCHEMA_VERSION = 1
STATES = {"open", "expanded", "parked", "pruned", "solved"}


def _json(value: Any) -> str:
    return json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=False,
                      allow_nan=False)


def fingerprint(value: Any) -> str:
    """Stable SHA-256 for a JSON value (dict order never changes the key)."""
    return hashlib.sha256(_json(value).encode("utf-8")).hexdigest()


def source_hash(source: str) -> str:
    return hashlib.sha256(source.encode("utf-8")).hexdigest()


def _now() -> str:
    return datetime.datetime.now(datetime.timezone.utc).isoformat(timespec="microseconds")


class Session:
    """A persistent search graph. Use one instance per worker/thread.

    Methods returning nodes use dictionaries to keep the API and JSON CLI alike.
    References accept full hashes, unambiguous hash prefixes, ROOT, and HEAD.
    Constructing a Session creates its directory and schema; initialize() supplies
    the function, root program, and compile context exactly once.
    """

    def __init__(self, path: str | os.PathLike, timeout: float = 30):
        self.path = Path(path)
        self.owner = uuid.uuid4().hex
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self.db = sqlite3.connect(str(self.path), timeout=timeout, isolation_level=None)
        self.db.row_factory = sqlite3.Row
        self.db.execute("PRAGMA foreign_keys=ON")
        self.db.execute(f"PRAGMA busy_timeout={int(timeout * 1000)}")
        self.db.execute("PRAGMA journal_mode=WAL")
        self.db.execute("PRAGMA synchronous=FULL")
        version = self.db.execute("PRAGMA user_version").fetchone()[0]
        if version not in (0, SCHEMA_VERSION):
            self.close()
            raise ValueError(f"unsupported search session schema {version}")
        self.db.executescript("""
            BEGIN IMMEDIATE;
            CREATE TABLE IF NOT EXISTS metadata (
                key TEXT PRIMARY KEY, value TEXT NOT NULL
            );
            CREATE TABLE IF NOT EXISTS contexts (
                id TEXT PRIMARY KEY, payload TEXT NOT NULL, created TEXT NOT NULL
            );
            CREATE TABLE IF NOT EXISTS sources (
                id TEXT PRIMARY KEY, source TEXT NOT NULL
            );
            CREATE TABLE IF NOT EXISTS nodes (
                id TEXT PRIMARY KEY,
                source_hash TEXT NOT NULL REFERENCES sources(id),
                context_id TEXT NOT NULL REFERENCES contexts(id),
                created TEXT NOT NULL,
                UNIQUE(source_hash, context_id)
            );
            CREATE TABLE IF NOT EXISTS edges (
                id TEXT PRIMARY KEY,
                parent TEXT NOT NULL REFERENCES nodes(id),
                child TEXT NOT NULL REFERENCES nodes(id),
                move TEXT NOT NULL, created TEXT NOT NULL
            );
            CREATE INDEX IF NOT EXISTS edges_parent ON edges(parent);
            CREATE INDEX IF NOT EXISTS edges_child ON edges(child);
            CREATE TABLE IF NOT EXISTS evaluations (
                seq INTEGER PRIMARY KEY AUTOINCREMENT,
                cache_key TEXT NOT NULL,
                node_id TEXT NOT NULL REFERENCES nodes(id),
                evaluator TEXT NOT NULL, options TEXT NOT NULL,
                result TEXT NOT NULL, created TEXT NOT NULL
            );
            CREATE INDEX IF NOT EXISTS eval_cache ON evaluations(cache_key, seq);
            CREATE INDEX IF NOT EXISTS eval_nodes ON evaluations(node_id, seq);
            CREATE TABLE IF NOT EXISTS frontier (
                node_id TEXT PRIMARY KEY REFERENCES nodes(id),
                state TEXT NOT NULL, priority REAL NOT NULL DEFAULT 0,
                reason TEXT NOT NULL DEFAULT '', updated TEXT NOT NULL
            );
            CREATE TABLE IF NOT EXISTS notes (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                node_id TEXT REFERENCES nodes(id), kind TEXT NOT NULL,
                text TEXT NOT NULL, data TEXT NOT NULL, created TEXT NOT NULL
            );
            CREATE INDEX IF NOT EXISTS notes_nodes ON notes(node_id, id);
            CREATE TABLE IF NOT EXISTS events (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                kind TEXT NOT NULL, node_id TEXT REFERENCES nodes(id),
                data TEXT NOT NULL, created TEXT NOT NULL
            );
            CREATE TABLE IF NOT EXISTS leases (
                cache_key TEXT PRIMARY KEY, owner TEXT NOT NULL,
                expires REAL NOT NULL
            );
            PRAGMA user_version=1;
            COMMIT;
        """)

    def close(self):
        self.db.close()

    def __enter__(self):
        return self

    def __exit__(self, *_):
        self.close()

    @contextlib.contextmanager
    def _transaction(self):
        self.db.execute("BEGIN IMMEDIATE")
        try:
            yield
        except BaseException:
            self.db.execute("ROLLBACK")
            raise
        else:
            self.db.execute("COMMIT")

    def _event(self, kind, node_id=None, **data):
        self.db.execute("INSERT INTO events(kind,node_id,data,created) VALUES(?,?,?,?)",
                        (kind, node_id, _json(data), _now()))

    def metadata(self) -> dict:
        return {row["key"]: json.loads(row["value"])
                for row in self.db.execute("SELECT * FROM metadata ORDER BY key")}

    def _set_metadata(self, values):
        for key, value in values.items():
            self.db.execute("INSERT INTO metadata(key,value) VALUES(?,?) "
                            "ON CONFLICT(key) DO UPDATE SET value=excluded.value",
                            (key, _json(value)))

    def set_metadata(self, values: dict | None = None, **kwargs):
        values = dict(values or {}, **kwargs)
        reserved = {"function", "root", "head"} & values.keys()
        if reserved:
            raise ValueError(f"reserved metadata keys: {', '.join(sorted(reserved))}")
        with self._transaction():
            self._set_metadata(values)
            self._event("metadata", keys=sorted(values))

    def _context(self, value: dict | str) -> str:
        if isinstance(value, str):
            if not self.db.execute("SELECT 1 FROM contexts WHERE id=?", (value,)).fetchone():
                raise KeyError(f"unknown context: {value}")
            return value
        if not isinstance(value, dict):
            raise TypeError("compile context must be a dict or an existing context hash")
        key = fingerprint(value)
        self.db.execute("INSERT OR IGNORE INTO contexts VALUES(?,?,?)", (key, _json(value), _now()))
        return key

    def context(self, ref="HEAD") -> dict:
        node = self._resolve(ref)
        row = self.db.execute("SELECT payload FROM contexts WHERE id=?", (node["context_id"],)).fetchone()
        return json.loads(row[0])

    def _insert_node(self, source, context_id):
        if not isinstance(source, str):
            raise TypeError("source must be C text")
        src_hash = source_hash(source)
        key = fingerprint({"source": src_hash, "context": context_id})
        self.db.execute("INSERT OR IGNORE INTO sources VALUES(?,?)", (src_hash, source))
        inserted = self.db.execute("INSERT OR IGNORE INTO nodes VALUES(?,?,?,?)",
                                   (key, src_hash, context_id, _now())).rowcount > 0
        if inserted:
            self.db.execute("INSERT INTO frontier(node_id,state,updated) VALUES(?,?,?)",
                            (key, "open", _now()))
        return key, inserted

    def initialize(self, function: str, source: str, context: dict,
                   metadata: dict | None = None) -> dict:
        """Create the root atomically; repeated identical initialization is harmless."""
        with self._transaction():
            meta = self.metadata()
            if "root" in meta:
                root = self._resolve("ROOT")
                if (meta["function"] != function or root["source_hash"] != source_hash(source)
                        or root["context_id"] != fingerprint(context)):
                    raise ValueError("session already initialized with a different function, source, or context")
                key = root["id"]
            else:
                values = dict(metadata or {})
                if {"function", "root", "head"} & values.keys():
                    raise ValueError("metadata includes reserved keys")
                context_id = self._context(context)
                key, _ = self._insert_node(source, context_id)
                values.update(function=function, root=key, head=key)
                self._set_metadata(values)
                self._event("initialize", key, function=function)
        return self.node(key)

    def _resolve(self, ref=None):
        ref = ref or "HEAD"
        if ref.upper() in ("HEAD", "ROOT"):
            row = self.db.execute("SELECT value FROM metadata WHERE key=?", (ref.lower(),)).fetchone()
            if not row:
                raise ValueError("session is not initialized")
            ref = json.loads(row[0])
        rows = self.db.execute("SELECT * FROM nodes WHERE id=?", (ref,)).fetchall()
        if not rows:
            if not ref or any(c not in "0123456789abcdef" for c in ref.lower()):
                raise KeyError(f"unknown node: {ref}")
            rows = self.db.execute("SELECT * FROM nodes WHERE id LIKE ? LIMIT 2", (ref.lower() + "%",)).fetchall()
        if len(rows) != 1:
            raise KeyError(f"{'ambiguous' if rows else 'unknown'} node: {ref}")
        return dict(rows[0])

    @staticmethod
    def _edge(row):
        data = dict(row)
        data["move"] = json.loads(data["move"])
        return data

    @staticmethod
    def _evaluation(row):
        data = dict(row)
        data["options"] = json.loads(data["options"])
        data["result"] = json.loads(data["result"])
        return data

    def node(self, ref="HEAD") -> dict:
        node = self._resolve(ref)
        node["edges"] = [self._edge(row) for row in self.db.execute(
            "SELECT * FROM edges WHERE child=? ORDER BY created,id", (node["id"],))]
        node["parents"] = list(dict.fromkeys(edge["parent"] for edge in node["edges"]))
        node["evaluations"] = [self._evaluation(row) for row in self.db.execute(
            "SELECT * FROM evaluations WHERE node_id=? ORDER BY seq", (node["id"],))]
        node["frontier"] = dict(self.db.execute("SELECT * FROM frontier WHERE node_id=?",
                                                (node["id"],)).fetchone())
        return node

    def source(self, ref="HEAD") -> str:
        node = self._resolve(ref)
        return self.db.execute("SELECT source FROM sources WHERE id=?", (node["source_hash"],)).fetchone()[0]

    def add(self, source: str, parent="HEAD", move: dict | str | None = None,
            context: dict | str | None = None, note: str | None = None,
            checkout: bool = True) -> dict:
        """Retain a proposed program, even when equal/worse; return its canonical node.

        A duplicate program/context reuses its node but records the parent and move.
        Adding a child does not close its parent: another spelling may be valuable.
        """
        with self._transaction():
            previous = self._resolve(parent)
            context_id = self._context(context) if context is not None else previous["context_id"]
            key, inserted = self._insert_node(source, context_id)
            move = {"kind": "edit"} if move is None else ({"kind": move} if isinstance(move, str) else move)
            edge_id = fingerprint({"parent": previous["id"], "child": key, "move": move})
            self.db.execute("INSERT OR IGNORE INTO edges VALUES(?,?,?,?,?)",
                            (edge_id, previous["id"], key, _json(move), _now()))
            if note:
                self.db.execute("INSERT INTO notes(node_id,kind,text,data,created) VALUES(?,?,?,?,?)",
                                (key, "decision", note, _json({"edge": edge_id}), _now()))
            if checkout:
                self._set_metadata({"head": key})
            self._event("add", key, parent=previous["id"], edge=edge_id, new=inserted)
        return self.node(key)

    def cache_key(self, ref="HEAD", evaluator="compile", options=None) -> str:
        node = self._resolve(ref)
        return fingerprint({"node": node["id"], "evaluator": evaluator, "options": options or {}})

    def evaluation(self, ref="HEAD", evaluator="compile", options=None) -> dict | None:
        key = self.cache_key(ref, evaluator, options)
        row = self.db.execute("SELECT * FROM evaluations WHERE cache_key=? ORDER BY seq DESC LIMIT 1",
                              (key,)).fetchone()
        return self._evaluation(row) if row else None

    def evaluate(self, ref, result: dict, evaluator="compile", options=None) -> dict:
        """Append feedback. Re-evaluations remain auditable; the latest is cached.

        Keep evaluator/schema versions in evaluator or options. Semantic status and
        per-region/compiler feedback are unrestricted JSON inside result.
        """
        node = self._resolve(ref)
        key = self.cache_key(node["id"], evaluator, options)
        with self._transaction():
            cursor = self.db.execute("INSERT INTO evaluations(cache_key,node_id,evaluator,options,result,created) "
                                     "VALUES(?,?,?,?,?,?)", (key, node["id"], evaluator,
                                     _json(options or {}), _json(result), _now()))
            seq = cursor.lastrowid
            self.db.execute("DELETE FROM leases WHERE cache_key=?", (key,))
            self._event("evaluate", node["id"], evaluation=seq, evaluator=evaluator)
        return self._evaluation(self.db.execute("SELECT * FROM evaluations WHERE seq=?", (seq,)).fetchone())

    record_evaluation = evaluate

    def claim(self, ref="HEAD", owner=None, evaluator="compile", options=None,
              ttl: float = 120) -> bool:
        """Lease an uncached evaluation; expired/crashed workers can be replaced."""
        if ttl <= 0:
            raise ValueError("lease ttl must be positive")
        owner = owner or self.owner
        key = self.cache_key(ref, evaluator, options)
        now = time.time()
        with self._transaction():
            if self.db.execute("SELECT 1 FROM evaluations WHERE cache_key=? LIMIT 1", (key,)).fetchone():
                return False
            self.db.execute("DELETE FROM leases WHERE cache_key=? AND expires<=?", (key, now))
            row = self.db.execute("SELECT owner FROM leases WHERE cache_key=?", (key,)).fetchone()
            if row and row[0] != owner:
                return False
            self.db.execute("INSERT INTO leases VALUES(?,?,?) ON CONFLICT(cache_key) DO UPDATE SET expires=excluded.expires",
                            (key, owner, now + ttl))
        return True

    def release(self, ref="HEAD", owner=None, evaluator="compile", options=None):
        owner = owner or self.owner
        key = self.cache_key(ref, evaluator, options)
        with self._transaction():
            self.db.execute("DELETE FROM leases WHERE cache_key=? AND owner=?", (key, owner))

    def set_frontier(self, ref="HEAD", state="open", priority: float = 0, reason=""):
        if state not in STATES:
            raise ValueError(f"frontier state must be one of {', '.join(sorted(STATES))}")
        node = self._resolve(ref)
        with self._transaction():
            self.db.execute("UPDATE frontier SET state=?,priority=?,reason=?,updated=? WHERE node_id=?",
                            (state, priority, reason, _now(), node["id"]))
            self._event("frontier", node["id"], state=state, priority=priority, reason=reason)
        return self.node(node["id"])

    def frontier(self, limit: int | None = None, include_closed=False) -> list[dict]:
        """Caller priorities only: default order is insertion, never best-score-only."""
        sql = "SELECT n.id FROM nodes n JOIN frontier f ON n.id=f.node_id"
        if not include_closed:
            sql += " WHERE f.state='open'"
        sql += " ORDER BY f.priority DESC,n.created,n.id"
        params = ()
        if limit is not None:
            if limit < 0:
                raise ValueError("frontier limit cannot be negative")
            sql += " LIMIT ?"
            params = (limit,)
        return [self.node(row[0]) for row in self.db.execute(sql, params).fetchall()]

    def checkout(self, ref, path: str | os.PathLike | None = None) -> dict:
        node = self._resolve(ref)
        if path is not None:
            _atomic_text(Path(path), self.source(node["id"]))
        with self._transaction():
            self._set_metadata({"head": node["id"]})
            self._event("checkout", node["id"])
        return self.node(node["id"])

    def diff(self, a="ROOT", b="HEAD") -> str:
        left, right = self._resolve(a), self._resolve(b)
        return "".join(difflib.unified_diff(self.source(left["id"]).splitlines(keepends=True),
                                         self.source(right["id"]).splitlines(keepends=True),
                                         fromfile=left["id"][:12] + ".c", tofile=right["id"][:12] + ".c"))

    def history(self, ref="HEAD") -> list[dict]:
        """All ancestors, once each; handles undo cycles and transpositions."""
        key = self._resolve(ref)["id"]
        rows = self.db.execute("""WITH RECURSIVE ancestry(id) AS (
            SELECT ? UNION SELECT e.parent FROM edges e JOIN ancestry a ON e.child=a.id
            ) SELECT n.id FROM nodes n JOIN ancestry a ON n.id=a.id ORDER BY n.created,n.id""", (key,)).fetchall()
        return [self.node(row[0]) for row in rows]

    def note(self, text: str, ref=None, kind="observation", data=None) -> dict:
        node_id = self._resolve(ref)["id"] if ref is not None else None
        with self._transaction():
            cursor = self.db.execute("INSERT INTO notes(node_id,kind,text,data,created) VALUES(?,?,?,?,?)",
                                     (node_id, kind, text, _json(data or {}), _now()))
            key = cursor.lastrowid
            self._event("note", node_id, note=key, note_kind=kind)
        row = dict(self.db.execute("SELECT * FROM notes WHERE id=?", (key,)).fetchone())
        row["data"] = json.loads(row["data"])
        return row

    def notes(self, ref=None) -> list[dict]:
        if ref is None:
            rows = self.db.execute("SELECT * FROM notes ORDER BY id").fetchall()
        else:
            rows = self.db.execute("SELECT * FROM notes WHERE node_id=? ORDER BY id",
                                   (self._resolve(ref)["id"],)).fetchall()
        return [dict(row, data=json.loads(row["data"])) for row in rows]

    def events(self, after=0) -> list[dict]:
        return [dict(row, data=json.loads(row["data"])) for row in self.db.execute(
            "SELECT * FROM events WHERE id>? ORDER BY id", (after,)).fetchall()]

    def export(self, path: str | os.PathLike | None = None) -> dict:
        """Consistent, portable JSON snapshot including every branch and evaluation."""
        self.db.execute("BEGIN")
        try:
            result = {"schema": SCHEMA_VERSION, "metadata": self.metadata(),
                      "contexts": {row["id"]: json.loads(row["payload"]) for row in self.db.execute("SELECT * FROM contexts")},
                      "sources": {row["id"]: row["source"] for row in self.db.execute("SELECT * FROM sources")},
                      "nodes": self.frontier(include_closed=True),
                      "edges": [self._edge(row) for row in self.db.execute("SELECT * FROM edges ORDER BY created,id")],
                      "notes": self.notes(), "events": self.events()}
        finally:
            self.db.execute("ROLLBACK")
        if path is not None:
            _atomic_text(Path(path), json.dumps(result, indent=2, ensure_ascii=False, allow_nan=False) + "\n")
        return result


def _atomic_text(path: Path, text: str):
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix="." + path.name + "-", dir=path.parent)
    try:
        with os.fdopen(fd, "w", encoding="utf-8", newline="") as stream:
            stream.write(text)
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)
