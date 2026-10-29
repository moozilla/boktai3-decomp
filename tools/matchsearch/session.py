"""Model-facing stateful search, with cheap slate expansion and lazy diagnosis."""
from __future__ import annotations

from collections import Counter
import copy
import dataclasses
import difflib
import json
from pathlib import Path
import re
import time
from typing import Any
from types import SimpleNamespace

from . import frontier
from .store import Store, canonical, digest


def _json(value: Any) -> Any:
    if hasattr(value, "to_dict"):
        return value.to_dict()
    if dataclasses.is_dataclass(value):
        return dataclasses.asdict(value)
    return value


class Session:
    """One target, one frozen environment, arbitrarily many source hypotheses.

    Evaluation happens outside SQLite write transactions. Independent workers
    can share the graph; leases suppress duplicate expansions, and per-evaluator
    compiler directories prevent object/trace collisions. Detailed traces and
    semantic trials are evidence, not substitutes for ordinary byte matching.
    """

    def __init__(self, directory: str | Path, *, evaluator: Any = None,
                 baseline_source: str | None = None):
        self.store = Store(directory)
        config = self.store.meta("config")
        if not config:
            self.store.close()
            raise ValueError("session not initialized; use create")
        try:
            if evaluator is None:
                from .evaluator import Evaluator
                evaluator = Evaluator(config["repo"], config["function"], config["source_path"],
                                      workdir=self.store.directory / "compiler")
            self.evaluator = evaluator
            self.store.bind_context(evaluator.identity)
            from . import boundaries, constraints, contracts
            from .constraints import analyze
            contract_version = digest(Path(contracts.__file__).read_bytes())
            frozen = self.store.meta("call_contracts")
            if frozen is None or frozen.get("inference_version") != contract_version:
                if baseline_source is None:
                    baseline_source = (self.store.source("root") if self.store.meta("root")
                                       else Path(config["source_path"]).read_text())
                evaluator.validate_environment()
                inferred = contracts.infer_call_contracts(
                    evaluator.preprocess(baseline_source), evaluator.rom_symbols)
                inferred["inference_version"] = contract_version
                evaluator.validate_environment()
                with self.store.transaction():
                    frozen = self.store.meta("call_contracts")
                    if frozen is None or frozen.get("inference_version") != contract_version:
                        self.store.set_meta("call_contracts", inferred)
                        frozen = inferred
            self.call_contracts = frozen
            self.call_arities = frozen["arities"]
            if hasattr(evaluator, "bind_call_contracts"):
                semantic_baseline = baseline_source
                if semantic_baseline is None:
                    semantic_baseline = (self.store.source("root") if self.store.meta("root")
                                         else Path(config["source_path"]).read_text())
                evaluator.bind_call_contracts(frozen, baseline_source=semantic_baseline)
            analysis_files = [Path(module.__file__) for module in (constraints, contracts, boundaries)]
            analysis_files.append(Path(__file__))
            stackflow = Path(__file__).with_name("stackflow.py")
            if stackflow.exists():
                analysis_files.append(stackflow)
            self.analysis_version = digest(b"".join(path.name.encode() + b"\0" + path.read_bytes()
                for path in analysis_files) + canonical(self.call_arities).encode())
            self.target = analyze(evaluator.target_rows, name=evaluator.function,
                                  address=evaluator.address, call_arities=self.call_arities)
        except BaseException:
            if evaluator is not None:
                evaluator.close()
            self.store.close()
            raise
        self._catalogs: dict[tuple, Any] = {}
        self._comparison_cache: dict[str, tuple[Any, dict[str, Any]]] = {}
        self._boundary_cache: dict[tuple, dict[str, Any]] = {}

    @classmethod
    def create(cls, directory: str | Path, *, repo: str | Path, function: str,
               source_path: str | Path, label: str = "baseline") -> Session:
        repo = Path(repo).resolve()
        source_path = Path(source_path)
        if not source_path.is_absolute():
            source_path = repo / source_path
        source_path = source_path.resolve()
        config = {"repo": str(repo), "function": function, "source_path": str(source_path)}
        with Store(directory) as store:
            existing = store.meta("config")
            if existing and existing != config:
                raise ValueError("session was initialized for a different target/source")
            if existing and store.meta("root"):
                raise ValueError("session already initialized; open it to continue")
            # An interrupted setup can retry with exactly the same context.
            store.set_meta("config", config)
        session = cls(directory)
        node_id, _ = session.store.add(source_path.read_text(), family="baseline", label=label,
                                      hypothesis="supplied source baseline")
        try:
            session.evaluate(node_id)
        except BaseException:
            session.close()
            raise
        return session

    @classmethod
    def from_history(cls, directory: str | Path, history: dict[str, Any], *,
                     repo: str | Path, source_path: str | Path | None = None) -> Session:
        """Carry a source graph into a new physical context and reevaluate its root."""
        if history.get("format") != "matchsearch-source-history-v1":
            raise ValueError("unsupported source history format")
        repo = Path(repo).resolve()
        path = Path(source_path or history["source_path"])
        if not path.is_absolute():
            path = repo / path
        baseline = next(node["source"] for node in history["nodes"] if node["id"] == history["root"])
        config = {"repo": str(repo), "function": history["function"], "source_path": str(path.resolve())}
        import_hash = digest(canonical(history))
        with Store(directory) as store:
            existing = store.meta("config")
            if existing is not None and (existing != config or store.meta("history_import_hash") != import_hash):
                raise ValueError("destination belongs to a different source history or configuration")
            if existing is None and store.stats()["nodes"]:
                raise ValueError("history import requires a new destination directory")
            store.set_meta("config", config)
            store.set_meta("history_import_hash", import_hash)
        session = cls(directory, baseline_source=baseline)
        try:
            if session.store.meta("root") is None:
                session.store.import_history(history)
            session.evaluate("root")
        except BaseException:
            session.close()
            raise
        return session

    @classmethod
    def fork(cls, previous: str | Path, directory: str | Path, *,
             repo: str | Path | None = None, source_path: str | Path | None = None) -> Session:
        with Store(previous) as old:
            history = old.history()
            repo = repo or old.meta("config")["repo"]
        return cls.from_history(directory, history, repo=repo, source_path=source_path)

    def close(self) -> None:
        try:
            self.evaluator.close()
        finally:
            self.store.close()

    def __enter__(self) -> Session:
        return self

    def __exit__(self, *_: Any) -> None:
        self.close()

    def _validate(self) -> None:
        self.evaluator.validate_environment()

    def _comparison(self, rows: list[Any]) -> tuple[Any, dict[str, Any]]:
        from .constraints import analyze, compare
        analysis = analyze(rows, name=self.evaluator.function, address=self.evaluator.address,
                           call_arities=self.call_arities)
        return analysis, _json(compare(self.target, analysis))

    @staticmethod
    def _summary(result: dict[str, Any], comparison: dict[str, Any], analysis: Any) -> dict[str, Any]:
        result = dict(result)
        distances = comparison.get("vector", {})
        if not isinstance(distances, dict):
            distances = {}
        result["distances"] = {
            "dependency": distances.get("dependencies", 0),
            "control": distances.get("control", 0),
            "operations": distances.get("instructions", 0) + distances.get("operands", 0),
            "calls": distances.get("calls", 0), "memory": distances.get("memory", 0),
            "data": distances.get("data", 0),
        }
        regions = comparison.get("regions", [])
        result["region_summary"] = dict(Counter(r.get("verdict", "unresolved") for r in regions))
        result["exact_regions"] = [r.get("id", str(i)) for i, r in enumerate(regions)
                                    if r.get("verdict") in ("exact", "byte_exact", "bytes_exact")]
        result["structure_hash"] = comparison.get("fingerprints", {}).get("dependencies")
        if not result["structure_hash"]:
            serialized = _json(analysis)
            result["structure_hash"] = serialized.get("fingerprints", {}).get("dependencies")
        result["build_kind"] = "production"
        result["analysis_is_equivalence_proof"] = False
        return result

    def evaluate(self, node: str, *, force: bool = False, preprocessed_source: str | None = None) -> dict[str, Any]:
        self._validate()
        node_id = self.store.resolve(node)
        source = self.store.source(node_id)
        self.evaluator.validate_source_environment(source)
        prior = self.store.node(node_id)
        previous = prior["evaluation"]
        if previous is not None and previous.get("analysis_version") == self.analysis_version and not force:
            return {"id": node_id, "cached": True, "evaluation": prior["evaluation"]}
        # Completed evaluation leases can reopen after analyzer upgrades or
        # rollbacks. Move leases remain one-shot. One key also serializes old
        # and new analyzer workers against the authoritative node evaluation.
        claim = self.store.claim("evaluate:" + node_id, retry_done=True)
        if claim is None:
            current = self.store.node(node_id)
            valid = bool(current["evaluation"] and current["evaluation"].get("analysis_version") == self.analysis_version)
            return {"id": node_id, "cached": valid,
                    "pending_elsewhere": not valid, "evaluation": current["evaluation"]}
        try:
            # The persisted landing source is authoritative. A preprocessed AST
            # candidate is only an optimization after its real-source compile
            # has established the same bytes; callers cannot smuggle a mismatch.
            cached_binary = self.store.binary(previous["code_hash"]) if previous and previous.get("code_hash") and not force else None
            if cached_binary is not None:
                code, rows = cached_binary
                evaluation = SimpleNamespace(code=code, rows=rows, code_hash=previous["code_hash"])
                result = dict(previous)
            else:
                evaluation = self.evaluator.evaluate(source)
                self._validate()  # shared headers/compiler must not change during evaluation
                result = evaluation.to_dict()
                if evaluation.code:
                    self.store.record_binary(evaluation.code_hash, evaluation.code, evaluation.rows)
            result["analysis_version"] = self.analysis_version
            tokens = re.findall(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*|[A-Za-z_]\w*|\d+|[^\s]',
                                source, re.S)
            result["source_complexity"] = sum(not token.startswith(("/*", "//")) for token in tokens)
            comparison: dict[str, Any] = {}
            if evaluation.code:
                graph_started = time.perf_counter()
                cached_graph = self._comparison_cache.get(evaluation.code_hash)
                if cached_graph is None:
                    analysis, comparison = self._comparison(evaluation.rows)
                    if len(self._comparison_cache) >= 64:
                        self._comparison_cache.pop(next(iter(self._comparison_cache)))
                    self._comparison_cache[evaluation.code_hash] = (analysis, copy.deepcopy(comparison))
                else:
                    analysis, prior_comparison = cached_graph
                    comparison = copy.deepcopy(prior_comparison)
                result["graph_cached"] = cached_graph is not None
                for region in comparison.get("regions", []):
                    region["id"] = f"block:{region['target_block']}"
                    tr = region.get("target_range")
                    cr = region.get("candidate_range")
                    if tr and cr:
                        target_slice = self.evaluator.target_code[tr[0]:tr[1]]
                        candidate_slice = evaluation.code[cr[0]:cr[1]]
                        region["candidate_bytes_hash"] = digest(candidate_slice)
                        region["bytes_equal"] = candidate_slice == target_slice
                        # Exact bytes within an aligned region are a local fact;
                        # its entry register bindings may still differ globally.
                        if region["bytes_equal"]:
                            region["verdict"] = "byte_exact"
                        cb = region.get("candidate_block")
                        if cb is not None and "candidate_dependency_hash" not in region:
                            block = analysis.blocks[cb]
                            region["candidate_dependency_hash"] = digest(canonical({
                                "instructions": [analysis.instructions[i].shape for i in block.instructions],
                                "dependencies": [analysis.dependencies[i] for i in block.instructions],
                                "successors": block.successors,
                            }))
                result = self._summary(result, comparison, analysis)
                result["protection_changes"] = self._protection_changes(comparison)
                result["graph_ms"] = (time.perf_counter() - graph_started) * 1000
            self._validate()
            if not self.store.record(node_id, result, comparison, claim=claim):
                return {"id": node_id, "superseded_lease": True,
                        "evaluation": self.store.node(node_id)["evaluation"]}
            return {"id": node_id, "cached": False, "cached_compile": cached_binary is not None, "evaluation": result}
        except BaseException as exc:
            self.store.finish_claim(claim, {"error": str(exc)}, failed=True)
            raise

    def _protection_changes(self, comparison: dict[str, Any]) -> list[dict[str, Any]]:
        regions = {str(r.get("id", i)): r for i, r in enumerate(comparison.get("regions", []))}
        changed = []
        for protection in self.store.protections():
            previous = self.store.node(protection["node"], analysis=True)
            if (previous["evaluation"] or {}).get("analysis_version") != self.analysis_version:
                changed.append({**protection, "status": "stale", "reason": "refresh reference node analysis"})
                continue
            prior = previous["analysis"] or {}
            old_regions = {str(r.get("id", i)): r for i, r in enumerate(prior.get("regions", []))}
            old = old_regions.get(protection["region"])
            new = regions.get(protection["region"])
            key = "candidate_bytes_hash" if protection["mode"] == "bytes" else "candidate_dependency_hash"
            # Unknown alignment is reported, never treated as a preserved region.
            if not old or not new or not old.get(key) or old.get(key) != new.get(key):
                changed.append({**protection, "status": "unresolved" if not old or not new or not old.get(key)
                                else "changed", "new_verdict": (new or {}).get("verdict")})
        return changed

    def propose(self, source: str, *, parent: str = "root", hypothesis: str,
                prediction: Any = None, label: str = "", family: str = "model",
                move_id: str | None = None, metadata: dict[str, Any] | None = None) -> dict[str, Any]:
        if not hypothesis.strip():
            raise ValueError("a proposal needs the source/compiler hypothesis it tests")
        parent = self.store.resolve(parent)
        node_id, created = self.store.add(source, parent=parent, move_id=move_id or "model:" + digest(source)[:16],
            family=family, hypothesis=hypothesis, prediction=prediction, label=label, metadata=metadata)
        evaluated = self.evaluate(node_id)
        evaluated.update(created=created, delta=self.delta(parent, node_id))
        return evaluated

    def _catalog(self, node: str, *, max_candidates: int = 256, edit_function: str | None = None):
        from .moves import MoveCatalog
        node_id = self.store.resolve(node)
        function = edit_function or self.evaluator.function
        key = (node_id, function, max_candidates)
        if key not in self._catalogs:
            source = self.store.source(node_id)
            preprocessed = self.evaluator.preprocess(source)
            catalog = MoveCatalog.from_source(source, function,
                preprocessed=preprocessed, repo_root=self.evaluator.repo, max_candidates=max_candidates)
            if len(self._catalogs) >= 12:
                self._catalogs.pop(next(iter(self._catalogs)))
            self._catalogs[key] = catalog
        return self._catalogs[key]

    def moves(self, node: str = "root", *, families: list[str] | None = None, region: Any = None,
              limit: int = 64, max_candidates: int = 256, include_source: bool = False,
              edit_function: str | None = None) -> dict[str, Any]:
        self._validate()
        catalog = self._catalog(node, max_candidates=max_candidates, edit_function=edit_function)
        moves = []
        for move in catalog.moves(families=families, region=region, limit=limit):
            item = move.to_dict(include_source=include_source)
            if not include_source:
                item = {k: v for k, v in item.items() if k not in ("source", "function_source", "preprocessed_source")}
            moves.append(item)
        return {"node": self.store.resolve(node), "catalog": _json(catalog.summary()), "moves": moves}

    def expand(self, node: str = "root", *, hypothesis: str, ids: list[str] | None = None,
               families: list[str] | None = None, region: Any = None, limit: int = 32,
               max_candidates: int = 256, prediction: Any = None,
               edit_function: str | None = None) -> dict[str, Any]:
        """Compile a policy-selected, bounded slate, retaining every outcome."""
        self._validate()
        if not hypothesis.strip():
            raise ValueError("expansion needs a hypothesis")
        node_id = self.store.resolve(node)
        catalog = self._catalog(node_id, max_candidates=max_candidates, edit_function=edit_function)
        candidates = list(catalog.moves(families=families, region=region, limit=max_candidates))
        wanted = set(ids) if ids is not None else None
        if wanted is not None:
            known = {m.id for m in candidates}
            if missing := wanted - known:
                raise KeyError("move IDs not present in this parent/filter: " + ", ".join(sorted(missing)))
            candidates = [m for m in candidates if m.id in wanted]
        candidates = candidates[:limit]
        results = []
        start = time.perf_counter()
        for move in candidates:
            claim = self.store.claim("move:" + node_id + ":" + move.id)
            if claim is None:
                results.append({"move": move.id, "already_claimed_or_done": True})
                continue
            try:
                metadata = _json(move)
                metadata = {k: v for k, v in metadata.items()
                            if k not in ("source", "function_source", "preprocessed_source")}
                metadata["edited_function"] = catalog.function
                result = self.propose(move.source, parent=node_id, hypothesis=hypothesis,
                    prediction=prediction, family=move.family, move_id=move.id, metadata=metadata)
                result["move"] = move.id
                results.append(result)
                self.store.finish_claim(claim, {"node": result["id"]})
            except Exception as exc:
                self.store.finish_claim(claim, {"error": str(exc)}, failed=True)
                results.append({"move": move.id, "error": str(exc)})
        self.store.bump_expansions(node_id, len(candidates))
        return {"parent": node_id, "hypothesis": hypothesis, "results": results,
                "elapsed_ms": round((time.perf_counter() - start) * 1000, 3),
                "frontier": self.frontier()}

    def delta(self, a: str, b: str) -> dict[str, Any]:
        self.evaluate(a)
        self.evaluate(b)
        ra, rb = self.store.node(a), self.store.node(b)
        ea, eb = ra["evaluation"] or {}, rb["evaluation"] or {}
        if any(e.get("analysis_version") != self.analysis_version for e in (ea, eb)):
            return {"from": ra["id"], "to": rb["id"], "pending_elsewhere": True,
                    "numeric": {}, "note": "comparison waits for current-version analysis"}
        scalars = ("instruction_diff", "moved_branches", "byte_mismatches", "candidate_size")
        change = {k: eb[k] - ea[k] for k in scalars
                  if isinstance(ea.get(k), (int, float)) and isinstance(eb.get(k), (int, float))}
        for key in ("dependency", "control", "operations"):
            x, y = ea.get("distances", {}).get(key), eb.get("distances", {}).get(key)
            if isinstance(x, (int, float)) and isinstance(y, (int, float)):
                change[key] = y - x
        return {"from": ra["id"], "to": rb["id"], "numeric": change,
                "same_binary": bool(ea.get("code_hash")) and ea.get("code_hash") == eb.get("code_hash"),
                "region_before": ea.get("region_summary", {}), "region_after": eb.get("region_summary", {}),
                "concessions_before": ea.get("concessions"), "concessions_after": eb.get("concessions")}

    def compare(self, a: str, b: str, *, include_source_diff: bool = True) -> dict[str, Any]:
        output = self.delta(a, b)
        if include_source_diff:
            output["source_diff"] = "".join(difflib.unified_diff(self.store.source(a).splitlines(True),
                self.store.source(b).splitlines(True), fromfile=self.store.resolve(a)[:12],
                tofile=self.store.resolve(b)[:12]))
        output["before"] = self.store.node(a, analysis=True)
        output["after"] = self.store.node(b, analysis=True)
        return output

    def boundaries(self, node: str = "root", *, region: str | None = None, policy: str = "strict",
                   limit: int = 12, detail_limit: int = 8, include_equal: bool = False) -> dict[str, Any]:
        """Inspect call-anchored regions and their explicit value obligations."""
        from .boundaries import compare_boundaries
        from .constraints import analyze
        if policy not in {"strict", "abi-private"}:
            raise ValueError("stack policy must be strict or abi-private")
        if limit < 1 or detail_limit < 0 or detail_limit > 256:
            raise ValueError("positive limit and detail_limit between 0 and 256 required")
        result = self.evaluate(node)
        evaluation = result.get("evaluation") or {}
        if result.get("pending_elsewhere") or not evaluation.get("code_hash"):
            return {"node": result["id"], "unavailable": True, "evaluation": evaluation}
        cached = self.store.binary(evaluation["code_hash"])
        if cached is None:
            raise ValueError("binary cache missing; force an evaluation before boundary analysis")
        code, rows = cached
        key = (evaluation["code_hash"], policy, detail_limit)
        if key not in self._boundary_cache:
            options = {"stack_policy": policy} if policy != "strict" else {}
            target = (self.target if policy == "strict" else analyze(self.evaluator.target_rows,
                name=self.evaluator.function, address=self.evaluator.address, call_arities=self.call_arities, **options))
            candidate = analyze(rows, name=self.evaluator.function, address=self.evaluator.address,
                                call_arities=self.call_arities, **options)
            report = compare_boundaries(target, candidate, limit=len(target.instructions) + 1,
                                        detail_limit=detail_limit, include_equal=True)
            for item in report["regions"]:
                tr, cr = item["target_range"], item["candidate_range"]
                item["local_bytes_equal"] = bool(tr and cr and
                    self.evaluator.target_code[tr[0]:tr[1]] == code[cr[0]:cr[1]])
            report["analysis_version"] = self.analysis_version
            if len(self._boundary_cache) >= 8:
                self._boundary_cache.pop(next(iter(self._boundary_cache)))
            self._boundary_cache[key] = report
            self.store.add_evidence(result["id"], "boundary_summary", {
                field: value for field, value in report.items() if field != "regions"})
        report = copy.deepcopy(self._boundary_cache[key])
        if region is not None:
            report["regions"] = [r for r in report["regions"] if r["id"] == region]
            if not report["regions"]:
                raise KeyError(f"unknown region {region!r}")
        else:
            report["regions"] = [r for r in report["regions"]
                if include_equal or r["status"] != "corresponding-under-alignment"][:limit]
        self._validate()
        return {"node": result["id"], **report}

    def diagnose(self, node: str, *, knobs: dict[str, str] | None = None, limit: int = 12,
                 names: list[str] | None = None, full: bool = False) -> dict[str, Any]:
        self._validate()
        source = self.store.source(node)
        if knobs:
            evaluation = self.evaluator.evaluate(source, diagnostic_knobs=knobs)
            evidence = evaluation.to_dict()
            evidence["build_kind"] = "diagnostic"
            if evaluation.code:
                _, evidence["comparison"] = self._comparison(evaluation.rows)
            kind = "counterfactual"
        else:
            evidence = self.evaluator.diagnose(source, limit=limit, names=names)
            kind = "compiler_trace"
        self._validate()
        evidence_id = self.store.add_evidence(node, kind, evidence)
        if not full and "comparison" in evidence:
            evidence = dict(evidence)
            comparison = evidence["comparison"]
            evidence["comparison"] = {"vector": comparison.get("vector"),
                "region_summary": dict(Counter(r["verdict"] for r in comparison.get("regions", []))),
                "issues": comparison.get("issues", [])[:8],
                "warnings": comparison.get("warnings", []), "semantic_proof": False}
        return {"node": self.store.resolve(node), "kind": kind, "evidence_id": evidence_id,
                "evidence": evidence, "full_evidence": "request evidence with this node and evidence_id"}

    def decision_delta(self, a: str, b: str, *, names: list[str] | None = None, limit: int = 16,
                       include_constants: bool = False) -> dict[str, Any]:
        """Compare named compiler facts, keeping pseudo-number ambiguity explicit."""
        self._validate()
        traces = []
        for node in (a, b):
            found = [item["payload"] for item in self.store.evidence(node) if item["kind"] == "compiler_trace"]
            trace = found[-1] if found else self.diagnose(node, limit=limit, names=names, full=True)["evidence"]
            if not trace.get("full_feedback") and not trace.get("error"):
                trace = self.diagnose(node, limit=limit, names=names, full=True)["evidence"]
            full_path = trace.get("full_feedback")
            if full_path:
                path = Path(full_path)
                if not path.is_absolute():
                    path = self.evaluator.repo / path
                if path.is_file():
                    raw = json.loads(path.read_text())
                    trace = {**trace, **raw}
            traces.append(trace)

        def named(trace: dict[str, Any]) -> tuple[dict[str, Any], list[str]]:
            groups: dict[str, list[dict[str, Any]]] = {}
            for row in trace.get("allocations", []):
                if row.get("name") and (names is None or row["name"] in names):
                    if names is None and not include_constants and row["name"].startswith("="):
                        continue
                    groups.setdefault(row["name"], []).append(row)
            return {key: rows[0] for key, rows in groups.items() if len(rows) == 1}, [
                key for key, rows in groups.items() if len(rows) != 1]

        before, ambiguous_a = named(traces[0])
        after, ambiguous_b = named(traces[1])
        changes = []
        for name in sorted(before.keys() & after.keys()):
            def fields(row):
                if "record" in row:
                    rec = row["record"]
                    fields = dict(zip(("allocator", "rank", "priority", "refs", "live_length", "calls_crossed", "hard_register"), rec))
                    fields["allocator"] = {"G": "global", "L": "local"}.get(fields["allocator"], fields["allocator"])
                    return fields
                return {key: row.get(key) for key in
                        ("allocator", "rank", "priority", "refs", "live_length", "calls_crossed", "hard_register")}
            left, right = fields(before[name]), fields(after[name])
            difference = {key: [left[key], right[key]] for key in left if left[key] != right[key]}
            if difference:
                changes.append({"name": name, "changes": difference})

        def reload_summary(trace):
            reloads = trace.get("reloads", [])
            if reloads and not isinstance(reloads[0], dict):
                states = trace.get("spill_states", {})
                reloads = [{"register": r[2], "state": states.get(str(r[0]), states.get(r[0], {}))} for r in reloads]
            return {"events": trace.get("reload_count", len(reloads)),
                    "available_sets": sorted({tuple((row.get("state") or {}).get("regs", [])) for row in reloads}),
                    "chosen_registers": dict(Counter(row.get("register") for row in reloads))}
        payload = {"from": self.store.resolve(a), "to": self.store.resolve(b),
                   "changed_named_values": changes[:limit], "total_named_changes": len(changes),
                   "ambiguous_names": sorted(set(ambiguous_a + ambiguous_b)),
                   "new_names": sorted(after.keys() - before.keys())[:limit],
                   "removed_names": sorted(before.keys() - after.keys())[:limit],
                   "reload_before": reload_summary(traces[0]), "reload_after": reload_summary(traces[1]),
                   "debug_changes_code": [t.get("debug_changes_code") for t in traces],
                   "compiler_sources": traces[1].get("compiler_sources", {}),
                   "interpretation": "named-value correspondence is evidence; unnamed pseudos are not paired by number"}
        self.store.add_evidence(b, "decision_delta", payload)
        return payload

    def semantic(self, node: str, *, trials: int = 8) -> dict[str, Any]:
        self._validate()
        if trials < 1 or trials > 10000:
            raise ValueError("trials must be between 1 and 10000")
        evaluation = self.evaluator.evaluate(self.store.source(node), semcheck_trials=trials)
        self._validate()
        payload = evaluation.semantic or {"verdict": "UNAVAILABLE", "error": evaluation.error}
        self.store.add_evidence(node, "sampled_execution", payload)
        return {"node": self.store.resolve(node), "semantic": payload,
                "policy": "evidence only; no automatic pruning without validated input/callee contracts"}

    def verify(self, node: str) -> dict[str, Any]:
        self._validate()
        evidence = self.evaluator.verify(self.store.source(node))
        self.store.add_evidence(node, "production_verification", evidence)
        return {"node": self.store.resolve(node), "verification": evidence}

    def frontier(self, limit: int = 12) -> list[dict[str, Any]]:
        epoch = self.store.meta("frontier_epoch", 0)
        self.store.set_meta("frontier_epoch", epoch + 1)
        rows = [r for r in self.store.evaluated()
                if r["evaluation"].get("analysis_version") == self.analysis_version]
        return frontier.select(rows, limit=limit, epoch=epoch)

    def refresh(self, *, nodes: list[str] | None = None, limit: int = 256) -> dict[str, Any]:
        """Refresh stale graph metadata or evaluate imported pending sources."""
        ids = nodes if nodes is not None else self.store.pending() + [r["id"] for r in self.store.evaluated()
            if r["evaluation"].get("analysis_version") != self.analysis_version]
        results = []
        for node in ids[:limit]:
            result = self.evaluate(node)
            results.append({"id": result["id"], "cached_compile": result.get("cached_compile", False),
                            "cached": result.get("cached", False),
                            "pending_elsewhere": result.get("pending_elsewhere", False)})
        return {"refreshed": results, "remaining_stale": self._stale_count(),
                "remaining_pending": len(self.store.pending())}

    def _stale_count(self) -> int:
        return sum(r["evaluation"].get("analysis_version") != self.analysis_version for r in self.store.evaluated())

    def status(self, *, limit: int = 12) -> dict[str, Any]:
        self._validate()
        return {"directory": str(self.store.directory), "config": self.store.meta("config"),
                "context_hash": self.store.meta("context_hash"), "root": self.store.resolve("root"),
                "analysis_version": self.analysis_version, "stale_analysis": self._stale_count(),
                "pending_evaluations": len(self.store.pending()),
                "call_contracts": {"known": len(self.call_arities), "unknown": len(self.call_contracts["unknown"]),
                    "assumption": self.call_contracts["assumption"], "warnings": self.call_contracts["warnings"]},
                "analysis_note": "refresh stale nodes before comparing their metrics; byte verdicts remain valid",
                "stats": self.store.stats(), "frontier": self.frontier(limit),
                "protections": self.store.protections()}

    def export(self, node: str, path: str | Path) -> dict[str, Any]:
        self._validate()
        path = Path(path).resolve()
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(self.store.source(node))
        result = self.store.node(node)
        return {"path": str(path), "node": result["id"], "evaluation": result["evaluation"],
                "next": "run the independent production per-unit check before landing"}

    def dispatch(self, request: dict[str, Any]) -> Any:
        """Small JSON protocol for an LLM/tool client; no shell evaluation."""
        op = request.get("op", "status")
        args = dict(request.get("args", {}))
        if op in {"status", "moves", "expand", "propose", "compare", "diagnose", "semantic", "evaluate",
                  "frontier", "export", "verify", "decision_delta", "refresh", "boundaries"}:
            if op == "propose" and "file" in args:
                args["source"] = Path(args.pop("file")).read_text()
            return getattr(self, op)(**args)
        if op == "node":
            return self.store.node(**args)
        if op == "source":
            return {"source": self.store.source(args.get("node", "root"))}
        if op == "evidence":
            found = self.store.evidence(args.get("node", "root"))
            if "evidence_id" in args:
                return next(item for item in found if item["id"] == args["evidence_id"])
            if args.get("full"):
                return found
            return [{"id": item["id"], "kind": item["kind"], "created": item["created"]} for item in found]
        if op in {"retain", "reject", "protect"}:
            if op == "protect":
                self.evaluate(args["node"])
            getattr(self.store, op)(**args)
            return {"ok": True}
        if op == "target":
            return _json(self.target)
        if op == "context":
            return self.store.meta("context")
        if op == "contracts":
            return self.call_contracts
        if op == "history":
            return self.store.history()
        if op in {"integrity", "backup"}:
            return getattr(self.store, op)(**args)
        raise ValueError(f"unknown search operation: {op}")
