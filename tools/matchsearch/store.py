"""Transactional search graph. Source alternatives survive plateaus and restarts.

SQLite is the source of truth; generated compiler files are disposable. A source
node can have several parents. Equal machine code does NOT merge source nodes:
two source spellings that compile alike may have different useful descendants.
"""
from __future__ import annotations

from contextlib import contextmanager
from dataclasses import dataclass
import hashlib
import json
import os
from pathlib import Path
import sqlite3
import tempfile
import time
from typing import Any, Iterator
import uuid


SCHEMA = 1


def canonical(value: Any) -> str:
    return json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=False)


def digest(value: str | bytes) -> str:
    return hashlib.sha256(value.encode() if isinstance(value, str) else value).hexdigest()


class ContextChanged(ValueError):
    """A session cannot reuse evidence from another ROM/compiler/header context."""


@dataclass(frozen=True)
class Claim:
    key: str
    owner: str
    generation: int


class Store:
    def __init__(self, directory: Path | str):
        self.directory = Path(directory).resolve()
        self.directory.mkdir(parents=True, exist_ok=True)
        self.db = sqlite3.connect(self.directory / "search.sqlite3", timeout=30, isolation_level=None)
        self.db.row_factory = sqlite3.Row
        # Keep committed state in one database file. WAL-backed databases failed
        # integrity checks after reopening in the managed-workspace experiment.
        # The underlying cause is unresolved; rollback journaling removes the
        # persistent sidecar dependency and supports our short write claims.
        # An existing WAL database must be quiescent for this migration.
        try:
            check = self.db.execute("PRAGMA quick_check(1)").fetchone()[0]
            if check != "ok":
                raise sqlite3.DatabaseError("search database failed integrity check; preserve it and restore a checked backup: " + check)
            mode = self.db.execute("PRAGMA journal_mode=DELETE").fetchone()[0]
            if mode != "delete":
                raise RuntimeError("search storage requires DELETE journaling; close other workers before migration")
            self.db.execute("PRAGMA synchronous=FULL")
        except BaseException:
            self.db.close()
            raise
        self.db.execute("PRAGMA foreign_keys=ON")
        self.db.execute("PRAGMA busy_timeout=30000")
        self.db.executescript("""
            CREATE TABLE IF NOT EXISTS metadata (key TEXT PRIMARY KEY, value TEXT NOT NULL);
            CREATE TABLE IF NOT EXISTS nodes (
                id TEXT PRIMARY KEY, source TEXT NOT NULL, created REAL NOT NULL,
                label TEXT NOT NULL DEFAULT '', retained INTEGER NOT NULL DEFAULT 0,
                retention_reason TEXT NOT NULL DEFAULT '', rejected TEXT NOT NULL DEFAULT '',
                expansions INTEGER NOT NULL DEFAULT 0);
            CREATE TABLE IF NOT EXISTS edges (
                id INTEGER PRIMARY KEY, parent TEXT REFERENCES nodes(id),
                child TEXT NOT NULL REFERENCES nodes(id), move_id TEXT NOT NULL,
                family TEXT NOT NULL, hypothesis TEXT NOT NULL, prediction TEXT NOT NULL,
                metadata TEXT NOT NULL, created REAL NOT NULL,
                UNIQUE(parent, child, move_id));
            CREATE TABLE IF NOT EXISTS evaluations (
                node TEXT PRIMARY KEY REFERENCES nodes(id), context TEXT NOT NULL,
                result TEXT NOT NULL, analysis TEXT NOT NULL, created REAL NOT NULL);
            CREATE TABLE IF NOT EXISTS binaries (
                hash TEXT PRIMARY KEY, code BLOB NOT NULL, rows TEXT NOT NULL);
            CREATE TABLE IF NOT EXISTS evidence (
                id INTEGER PRIMARY KEY, node TEXT NOT NULL REFERENCES nodes(id),
                kind TEXT NOT NULL, payload TEXT NOT NULL, created REAL NOT NULL);
            CREATE TABLE IF NOT EXISTS claims (
                key TEXT PRIMARY KEY, owner TEXT NOT NULL, expires REAL NOT NULL,
                generation INTEGER NOT NULL, state TEXT NOT NULL, result TEXT);
            CREATE TABLE IF NOT EXISTS protections (
                region TEXT PRIMARY KEY, node TEXT NOT NULL REFERENCES nodes(id),
                mode TEXT NOT NULL, reason TEXT NOT NULL);
            CREATE INDEX IF NOT EXISTS edges_parent ON edges(parent);
            CREATE INDEX IF NOT EXISTS edges_child ON edges(child);
            CREATE INDEX IF NOT EXISTS evidence_node ON evidence(node);
        """)
        version = self.meta("schema")
        if version is not None and version != SCHEMA:
            raise ValueError(f"search schema {version} is not supported (expected {SCHEMA})")
        self.set_meta("schema", SCHEMA)

    @contextmanager
    def transaction(self) -> Iterator[None]:
        self.db.execute("BEGIN IMMEDIATE")
        try:
            yield
        except BaseException:
            self.db.execute("ROLLBACK")
            raise
        else:
            self.db.execute("COMMIT")

    def close(self) -> None:
        self.db.close()

    def integrity(self) -> dict[str, Any]:
        """Check the database structure and graph references without repairing it."""
        checks = [row[0] for row in self.db.execute("PRAGMA quick_check")]
        references = [tuple(row) for row in self.db.execute("PRAGMA foreign_key_check")]
        return {"ok": checks == ["ok"] and not references, "checks": checks,
                "foreign_key_errors": references,
                "journal_mode": self.db.execute("PRAGMA journal_mode").fetchone()[0]}

    def backup(self, path: str | Path) -> dict[str, Any]:
        """Publish a checked SQLite snapshot at a new path, without WAL sidecars.

        SQLite's backup API takes a consistent view even while other workers
        are active. Copying the main database file would not provide that
        guarantee. An existing destination is never overwritten.
        """
        if self.db.in_transaction:
            raise ValueError("finish the current transaction before taking a backup")
        path = Path(path).resolve()
        if path.exists():
            raise FileExistsError(path)
        path.parent.mkdir(parents=True, exist_ok=True)
        fd, temporary = tempfile.mkstemp(prefix=path.name + ".", suffix=".tmp", dir=path.parent)
        os.close(fd)
        temporary = Path(temporary)
        try:
            target = sqlite3.connect(temporary)
            try:
                self.db.backup(target)
                target.execute("PRAGMA journal_mode=DELETE")
                checks = [row[0] for row in target.execute("PRAGMA quick_check")]
                references = target.execute("PRAGMA foreign_key_check").fetchall()
                if checks != ["ok"] or references:
                    raise sqlite3.DatabaseError("backup failed integrity check: " + repr((checks, references)))
            finally:
                target.close()
            with temporary.open("rb") as stream:
                os.fsync(stream.fileno())
            # Linking publishes the complete file atomically and refuses to
            # clobber a concurrently created destination.
            os.link(temporary, path)
            temporary.unlink()
            directory_fd = os.open(path.parent, os.O_RDONLY)
            try:
                os.fsync(directory_fd)
            finally:
                os.close(directory_fd)
        finally:
            temporary.unlink(missing_ok=True)
        return {"path": str(path), "bytes": path.stat().st_size, "integrity": "ok",
                "journal_mode": "delete"}

    def __enter__(self) -> Store:
        return self

    def __exit__(self, *_: Any) -> None:
        self.close()

    def meta(self, key: str, default: Any = None) -> Any:
        row = self.db.execute("SELECT value FROM metadata WHERE key=?", (key,)).fetchone()
        return json.loads(row[0]) if row else default

    def set_meta(self, key: str, value: Any) -> None:
        self.db.execute("INSERT INTO metadata VALUES (?,?) ON CONFLICT(key) DO UPDATE SET value=excluded.value",
                        (key, canonical(value)))

    def bind_context(self, identity: dict[str, Any]) -> str:
        stamp = digest(canonical(identity))
        with self.transaction():
            prior = self.meta("context_hash")
            if prior is not None and prior != stamp:
                old = self.meta("context", {})
                changed = sorted(k for k in old.keys() | identity.keys() if old.get(k) != identity.get(k))
                raise ContextChanged("session context changed: " + ", ".join(changed)
                                     + "; use fork to carry sources into a new context")
            self.set_meta("context", identity)
            self.set_meta("context_hash", stamp)
        return stamp

    def resolve(self, name: str | None) -> str:
        if name in (None, "root"):
            result = self.meta("root")
            if result:
                return result
            raise KeyError("session has no root")
        row = self.db.execute("SELECT id FROM nodes WHERE id=?", (name,)).fetchone()
        if row:
            return row[0]
        rows = self.db.execute("SELECT id FROM nodes WHERE substr(id,1,?)=? OR label=?",
                               (len(name), name, name)).fetchall()
        ids = sorted({row[0] for row in rows})
        if len(ids) != 1:
            raise KeyError(f"node {name!r}: {'ambiguous' if ids else 'not found'}")
        return ids[0]

    def add(self, source: str, *, parent: str | None = None, move_id: str = "proposal",
            family: str = "model", hypothesis: str = "", prediction: Any = None,
            metadata: dict[str, Any] | None = None, label: str = "") -> tuple[str, bool]:
        context = self.meta("context_hash")
        if not context:
            raise ContextChanged("bind the compiler context before inserting sources")
        node_id = digest(context + "\0" + source)
        if parent is not None:
            parent = self.resolve(parent)
        with self.transaction():
            cur = self.db.execute("INSERT OR IGNORE INTO nodes(id,source,created,label) VALUES (?,?,?,?)",
                                  (node_id, source, time.time(), label))
            created = cur.rowcount == 1
            if label and not created:
                self.db.execute("UPDATE nodes SET label=? WHERE id=? AND label=''", (label, node_id))
            self.db.execute("INSERT OR IGNORE INTO edges(parent,child,move_id,family,hypothesis,prediction,metadata,created)"
                            " VALUES (?,?,?,?,?,?,?,?)", (parent, node_id, move_id, family, hypothesis,
                            canonical(prediction), canonical(metadata or {}), time.time()))
            if self.meta("root") is None:
                self.set_meta("root", node_id)
        return node_id, created

    def source(self, node: str) -> str:
        return self.db.execute("SELECT source FROM nodes WHERE id=?", (self.resolve(node),)).fetchone()[0]

    def node(self, node: str, *, source: bool = False, analysis: bool = False) -> dict[str, Any]:
        node_id = self.resolve(node)
        row = self.db.execute("SELECT n.*, e.result, e.analysis FROM nodes n LEFT JOIN evaluations e ON e.node=n.id"
                              " WHERE n.id=?", (node_id,)).fetchone()
        result = dict(row)
        if not source:
            result.pop("source")
        result["evaluation"] = json.loads(result.pop("result")) if result["result"] else None
        raw_analysis = result.pop("analysis")
        if analysis:
            result["analysis"] = json.loads(raw_analysis) if raw_analysis else None
        result["parents"] = [dict(r) for r in self.db.execute(
            "SELECT parent,move_id,family,hypothesis,prediction,metadata FROM edges WHERE child=?", (node_id,))]
        for edge in result["parents"]:
            edge["prediction"] = json.loads(edge["prediction"])
            edge["metadata"] = json.loads(edge["metadata"])
        return result

    def record(self, node: str, result: dict[str, Any], analysis: dict[str, Any] | None = None,
               *, claim: Claim | None = None) -> bool:
        node_id = self.resolve(node)
        # A diagnostic build belongs to evidence, never the authoritative evaluation.
        if result.get("diagnostic_knobs") or result.get("build_kind") == "diagnostic":
            raise ValueError("diagnostic builds must use add_evidence, never record")
        with self.transaction():
            if claim is not None:
                owner = self.db.execute("SELECT owner,generation,state FROM claims WHERE key=?", (claim.key,)).fetchone()
                if owner is None or tuple(owner) != (claim.owner, claim.generation, "running"):
                    return False
            self.db.execute("INSERT INTO evaluations VALUES (?,?,?,?,?) ON CONFLICT(node) DO UPDATE SET"
                            " context=excluded.context,result=excluded.result,analysis=excluded.analysis,created=excluded.created",
                            (node_id, self.meta("context_hash"), canonical(result), canonical(analysis or {}), time.time()))
            if claim is not None:
                self.finish_claim(claim, {"node": node_id})
        return True

    def add_evidence(self, node: str, kind: str, payload: Any) -> int:
        cur = self.db.execute("INSERT INTO evidence(node,kind,payload,created) VALUES (?,?,?,?)",
                              (self.resolve(node), kind, canonical(payload), time.time()))
        return cur.lastrowid

    def record_binary(self, code_hash: str, code: bytes, rows: list[Any]) -> None:
        if digest(code) != code_hash:
            raise ValueError("binary cache hash does not match its bytes")
        self.db.execute("INSERT OR IGNORE INTO binaries VALUES (?,?,?)", (code_hash, code, canonical(rows)))

    def binary(self, code_hash: str) -> tuple[bytes, list[Any]] | None:
        row = self.db.execute("SELECT code,rows FROM binaries WHERE hash=?", (code_hash,)).fetchone()
        if not row:
            return None
        code = bytes(row[0])
        if digest(code) != code_hash:
            raise ValueError("corrupt binary cache entry")
        return code, json.loads(row[1])

    def evidence(self, node: str) -> list[dict[str, Any]]:
        return [dict(r, payload=json.loads(r["payload"])) for r in self.db.execute(
            "SELECT id,kind,payload,created FROM evidence WHERE node=? ORDER BY id", (self.resolve(node),))]

    def retain(self, node: str, reason: str, enabled: bool = True) -> None:
        if enabled and not reason:
            raise ValueError("retaining a hypothesis needs a reason")
        self.db.execute("UPDATE nodes SET retained=?,retention_reason=? WHERE id=?",
                        (int(enabled), reason, self.resolve(node)))

    def reject(self, node: str, reason: str) -> None:
        """Policy decision, not an automatic consequence of sampled emulator output."""
        if not reason:
            raise ValueError("a rejection needs a stated reason")
        self.db.execute("UPDATE nodes SET rejected=? WHERE id=?", (reason, self.resolve(node)))

    def protect(self, region: str, node: str, mode: str, reason: str = "") -> None:
        if mode not in ("bytes", "dependencies"):
            raise ValueError("protection mode must be bytes or dependencies")
        self.db.execute("INSERT INTO protections VALUES (?,?,?,?) ON CONFLICT(region) DO UPDATE SET"
                        " node=excluded.node,mode=excluded.mode,reason=excluded.reason",
                        (region, self.resolve(node), mode, reason))

    def protections(self) -> list[dict[str, Any]]:
        return [dict(r) for r in self.db.execute("SELECT * FROM protections ORDER BY region")]

    def claim(self, key: str, *, owner: str | None = None, seconds: float = 120,
              retry_done: bool = False) -> Claim | None:
        """Atomic lease with a fencing generation. Expired workers cannot finish new work."""
        owner = owner or uuid.uuid4().hex
        now = time.time()
        with self.transaction():
            row = self.db.execute("SELECT * FROM claims WHERE key=?", (key,)).fetchone()
            if row and ((row["state"] == "done" and not retry_done) or row["expires"] > now):
                return None
            generation = row["generation"] + 1 if row else 1
            self.db.execute("INSERT INTO claims VALUES (?,?,?,?,?,NULL) ON CONFLICT(key) DO UPDATE SET"
                            " owner=excluded.owner,expires=excluded.expires,generation=excluded.generation,"
                            " state=excluded.state,result=NULL", (key, owner, now + seconds, generation, "running"))
        return Claim(key, owner, generation)

    def finish_claim(self, claim: Claim, result: Any, *, failed: bool = False) -> bool:
        cur = self.db.execute("UPDATE claims SET state=?,result=?,expires=0 WHERE key=? AND owner=? AND generation=?"
                              " AND state='running'", ("failed" if failed else "done", canonical(result),
                              claim.key, claim.owner, claim.generation))
        return cur.rowcount == 1

    def bump_expansions(self, node: str, count: int = 1) -> None:
        self.db.execute("UPDATE nodes SET expansions=expansions+? WHERE id=?", (count, self.resolve(node)))

    def evaluated(self) -> list[dict[str, Any]]:
        return [dict(r, evaluation=json.loads(r["result"])) for r in self.db.execute(
            "SELECT n.id,n.label,n.created,n.retained,n.retention_reason,n.rejected,n.expansions,e.result"
            " FROM nodes n JOIN evaluations e ON n.id=e.node ORDER BY n.created,n.id")]

    def pending(self) -> list[str]:
        return [row[0] for row in self.db.execute(
            "SELECT n.id FROM nodes n LEFT JOIN evaluations e ON e.node=n.id"
            " WHERE e.node IS NULL ORDER BY n.created,n.id")]

    def history(self) -> dict[str, Any]:
        """Portable sources and hypotheses, with no machine code or active scores.

        This works even when the checkout's compiler context no longer matches.
        Saved summaries are historical observations, never imported evaluations.
        """
        config = self.meta("config", {})
        source_path = config.get("source_path", "")
        if source_path and config.get("repo"):
            try:
                source_path = str(Path(source_path).relative_to(config["repo"]))
            except ValueError:
                pass  # external source locations require an explicit import override
        nodes = []
        for row in self.db.execute("SELECT id FROM nodes ORDER BY created,id"):
            node = self.node(row[0], source=True)
            node["historical_evaluation"] = node.pop("evaluation")
            nodes.append(node)
        return {"format": "matchsearch-source-history-v1", "function": config.get("function"),
                "source_path": source_path, "root": self.meta("root"),
                "context_hash": self.meta("context_hash"), "nodes": nodes,
                "historical_protections": self.protections(),
                "policy": "Import sources and hypotheses; recompile under the new context."}

    def import_history(self, history: dict[str, Any]) -> dict[str, str]:
        """Insert a possibly cyclic source graph; never reuse its old verdicts."""
        if history.get("format") != "matchsearch-source-history-v1":
            raise ValueError("unsupported source history format")
        if self.db.execute("SELECT count(*) FROM nodes").fetchone()[0]:
            raise ValueError("source history import requires an empty destination")
        context = self.meta("context_hash")
        if not context:
            raise ContextChanged("bind the new compiler context before importing sources")
        nodes = history["nodes"]
        records = {node["id"]: node for node in nodes}
        if len(records) != len(nodes) or history["root"] not in records:
            raise ValueError("invalid or duplicate source nodes in history")
        ids = {old: digest(context + "\0" + node["source"]) for old, node in records.items()}
        if len(set(ids.values())) != len(ids):
            raise ValueError("source history contains duplicate source states")
        for node in nodes:
            for edge in node.get("parents", []):
                if edge["parent"] is not None and edge["parent"] not in records:
                    raise ValueError("source history contains an absent parent")
        with self.transaction():
            # Insert every node first: inverse moves can return to ancestors.
            for node in nodes:
                self.db.execute("INSERT INTO nodes(id,source,created,label,retained,retention_reason)"
                                " VALUES (?,?,?,?,?,?)", (ids[node["id"]], node["source"],
                                node.get("created", time.time()), node.get("label", ""),
                                int(bool(node.get("retained"))), node.get("retention_reason", "")))
            self.set_meta("root", ids[history["root"]])
            for node in nodes:
                new_id = ids[node["id"]]
                for edge in node.get("parents", []):
                    self.db.execute("INSERT INTO edges(parent,child,move_id,family,hypothesis,prediction,metadata,created)"
                        " VALUES (?,?,?,?,?,?,?,?)", (ids.get(edge["parent"]), new_id, edge["move_id"],
                        edge["family"], edge["hypothesis"], canonical(edge.get("prediction")),
                        canonical(edge.get("metadata", {})), time.time()))
                self.add_evidence(new_id, "imported_history", {
                    "previous_node": node["id"], "previous_context": history.get("context_hash"),
                    "historical_evaluation": node.get("historical_evaluation"),
                    "historical_rejection": node.get("rejected", ""),
                    "policy": "Historical evidence only; no verdict or rejection carried forward."})
            self.set_meta("origin", {"context_hash": history.get("context_hash"), "node_map": ids,
                "historical_protections": history.get("historical_protections", [])})
        return ids

    def stats(self) -> dict[str, Any]:
        results = self.evaluated()
        signatures = {r["evaluation"].get("code_hash") for r in results if r["evaluation"].get("code_hash")}
        return {"nodes": self.db.execute("SELECT count(*) FROM nodes").fetchone()[0],
                "edges": self.db.execute("SELECT count(*) FROM edges").fetchone()[0],
                "evaluated": len(results), "unique_binaries": len(signatures),
                "exact": sum(bool(r["evaluation"].get("exact")) for r in results),
                "exact_pin_free": sum(bool(r["evaluation"].get("exact")) and not
                    any(r["evaluation"].get("concessions", {}).values()) for r in results),
                "rejected": sum(bool(r["rejected"]) for r in results),
                "retained": sum(bool(r["retained"]) for r in results),
                "compiler_ms": round(sum(r["evaluation"].get("compile_ms", 0) for r in results), 3),
                "evaluation_ms": round(sum(r["evaluation"].get("elapsed_ms", 0) for r in results), 3),
                "graph_ms": round(sum(r["evaluation"].get("graph_ms", 0) for r in results), 3)}
