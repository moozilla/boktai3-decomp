"""The search environment: unmodified build bytes, compiler evidence, semantic trials.

Only bytes decide a match. Knobs are counterfactual experiments stored separately;
debug-note builds and semantic trials never promote a candidate to exact.
"""
import contextlib
import hashlib
import io
import json
import os
from pathlib import Path
import re
import shutil
import time

ROOT = Path(__file__).resolve().parents[2]
VERSION = 1
IMPLEMENTATION = hashlib.sha256(Path(__file__).read_bytes()).hexdigest()


def digest(data):
    return hashlib.sha256(data.encode() if isinstance(data, str) else data).hexdigest()


class Evaluator:
    def __init__(self, directory):
        # Import scope as a library: no scope client, no external resident's state.
        import scope
        self.scope = scope
        self.directory = Path(directory).resolve()
        self.directory.mkdir(parents=True, exist_ok=True)
        self.oracles = {}
        self._hashes = {}
        self.loaded_scope = digest(Path(scope.__file__).read_bytes())
        self.loaded_fast = digest((ROOT / "tools/scope/fast.py").read_bytes())

    def file_hash(self, path):
        path = Path(path)
        st = path.stat()
        key = (str(path), st.st_mtime_ns, st.st_size)
        if key not in self._hashes:
            self._hashes[key] = digest(path.read_bytes())
        return self._hashes[key]

    def context(self, source, function, origin):
        """Content identities, never a timestamp-only evaluation cache key.

        All headers are conservative dependencies. Origin determines quoted includes;
        compiler directives are preserved in every snapshot and covered by its hash.
        """
        s = self.scope
        path = self.materialize(source, origin)
        rom_name = s.rom_name(str(path), function)
        _, addresses, sizes = s.inventory()
        if rom_name not in addresses:
            raise ValueError(f"unknown ROM function: {function}")
        if s.is_arm(rom_name, addresses[rom_name]):
            raise ValueError("the search environment currently serves Thumb agbcc functions")
        if os.path.basename(s.build.compiler_for(str(path))) != "agbcc":
            raise ValueError("this session needs the Thumb agbcc compiler")
        flags = s.build.cflags_for(str(path))
        deps = sorted((ROOT / "include").rglob("*.h"))
        # Include unit-local headers too. Imported edits are recontextualized.
        local = (ROOT / origin).resolve().parent
        deps += sorted(p for p in local.glob("*.h") if p not in deps)
        headers = [(os.path.relpath(p, ROOT), self.file_hash(p)) for p in deps]
        symbols = [(str(p.relative_to(ROOT)), self.file_hash(p))
                   for p in sorted((ROOT / "symbols").glob("*")) if p.is_file()]
        address, size = addresses[rom_name], sizes[rom_name]
        target = s.rom_bytes()[s.off(address):s.off(address) + size]
        return dict(version=VERSION, function=function, rom_name=rom_name,
                    address=address, size=size, target_hash=digest(target),
                    rom_hash=self.file_hash(ROOT / "baserom.gba"),
                    compiler=self.file_hash(s.INSTR), flags=flags,
                    assembler=self.file_hash(shutil.which(s.build.AS[0])),
                    cpp=s.build.CPP, defines=s.build.DEFINES,
                    headers=digest(json.dumps(headers)), symbols=digest(json.dumps(symbols)),
                    elf=self.file_hash(ROOT / "build/boktai3.elf"),
                    evaluator=IMPLEMENTATION, scope=self.loaded_scope, fast=self.loaded_fast,
                    origin=os.path.relpath(ROOT / origin, ROOT))

    def materialize(self, source, origin):
        # Preserve quoted-include lookup even though snapshots live under build/.
        local = (ROOT / origin).resolve().parent
        text = re.sub(r'^(\s*#\s*include\s*)"([^"\n]+)"',
                      lambda m: m[1] + '"' + os.path.relpath(local / m[2], ROOT) + '"'
                      if (local / m[2]).exists() else m[0], source, flags=re.M)
        path = self.directory / "candidate.c"
        if not path.exists() or path.read_text() != text:
            path.write_text(text)
        return path

    def evaluate(self, source, function, origin, knobs=None, semantic_trials=0):
        s = self.scope
        s.WORK = str(self.directory / "traces")
        knobs = dict(knobs or {})
        bad = [k for k in knobs if not k.startswith(("AGBCC_", "CSE_"))]
        if bad:
            raise ValueError("unsupported experiment keys: " + ", ".join(bad))
        # Neither the caller's environment nor a prior experiment may influence exact.
        saved = {k: os.environ.pop(k) for k in list(os.environ)
                 if k.startswith(("AGBCC_", "CSE_", "SCOPE_TRANSFORM"))}
        output = io.StringIO()
        start = time.perf_counter()
        try:
            path = self.materialize(source, origin)
            with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
                compiled = s.fast_compile(str(path), function, knobs)
                if compiled is None:
                    # Capture actual diagnostics and keep failures in the graph.
                    code, address, info = s.compile_draft(str(path), function, knobs)
                    data = info.get("data", ())
                else:
                    code, address, data = compiled
            compile_ms = (time.perf_counter() - start) * 1000
            name = s.rom_name(str(path), function)
            size = s.inventory()[2][name]
            target = s.rom_bytes()[s.off(address):s.off(address) + size]
            target_rows = s.decode(target, address, s.rom_data(name, address, target))
            rows = s.decode(code, address, data)
            pairs, shifted = s.align(target_rows, rows)
            counts = s.count_text([{"tag": t} for _, _, t in pairs])
            exact = not knobs and code == target
            result = dict(status="exact" if exact else "different", exact=exact,
                          experiment=bool(knobs), experiment_exact=bool(knobs) and code == target,
                          knobs=knobs, compile_ms=round(compile_ms, 3),
                          elapsed_ms=0, counts="MATCH" if exact else counts,
                          code_hash=digest(code), code_hex=code.hex(), target_hash=digest(target),
                          byte_length=len(code), target_length=len(target),
                          byte_differences=sum(a != b for a, b in zip(code, target)) + abs(len(code)-len(target)),
                          shifted_branches=shifted, rows=rows, target_rows=target_rows,
                          current_data=data, target_data=s.rom_data(name, address, target),
                          differences=[dict(target=target_rows[x] if x is not None else None,
                                            current=rows[y] if y is not None else None, tag=t)
                                       for x, y, t in pairs if t != "="],
                          semantic=dict(verdict="EXACT" if exact else "NOT_RUN", trials=0))
            if semantic_trials and not knobs and not exact:
                try:
                    result["semantic"] = self.semantic(source, function, target, code, address,
                                                       path, semantic_trials)
                except (ImportError, SystemExit) as e:
                    result["semantic"] = dict(verdict="UNAVAILABLE", trials=0, detail=str(e), proof=False)
            result["diagnostics"] = output.getvalue()[-8000:]
            result["elapsed_ms"] = round((time.perf_counter()-start)*1000, 3)
            return result
        except (SystemExit, OSError, RuntimeError, ValueError) as e:
            return dict(status="compile_error", exact=False, experiment=bool(knobs),
                        knobs=knobs, diagnostics=output.getvalue()[-8000:] + "\n" + str(e),
                        elapsed_ms=round((time.perf_counter()-start)*1000, 3),
                        semantic=dict(verdict="NOT_RUN", trials=0))
        finally:
            os.environ.update(saved)

    def semantic(self, source, function, target, code, address, path, trials):
        import sys
        local_packages = str(ROOT / "build/search-sol/python")
        if local_packages not in sys.path:
            sys.path.insert(0, local_packages)
        import semcheck
        s = self.scope
        pre = s.preprocess(str(path), func=function)
        arity = semcheck.arities(pre)
        kinds = semcheck.param_kinds(pre, function)
        check_return = semcheck.returns_value(source, function)
        name = s.rom_name(str(path), function)
        key = (name, digest(target), trials, check_return, tuple(kinds or []), digest(pre[:pre.find(function)]))
        oracle = self.oracles.get(key)
        if oracle is None:
            oracle = self.oracles[key] = semcheck.Oracle(name, trials, check_return,
                rom=s.rom_bytes(), arity=arity, kinds=kinds)
        verdict, detail = oracle.check(code, address)
        return dict(verdict=verdict, detail=detail, trials=trials,
                    target_coverage=round(oracle.coverage, 2), proof=False)

    def diagnose(self, source, function, origin, command="why", argument="all"):
        s = self.scope
        s.WORK = str(self.directory / "traces")
        path = self.materialize(source, origin)
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            if command == "why":
                s.cmd_why_all(str(path), function, {}) if argument == "all" else s.cmd_why(str(path), function, int(argument), {})
            elif command == "life":
                s.compile_draft(str(path), function, {})
                s.cmd_life(argument.split(), function)
            elif command in ("frame", "hoist", "giv", "pre", "probe", "spill"):
                getattr(s, "cmd_" + command)(str(path), function, {})
            else:
                raise ValueError("unknown compiler view: " + command)
        return output.getvalue()

    def close(self):
        for fast in self.scope._FAST.values():
            fast.close()


def decision_delta(parent, child):
    """Keep a vector of facts, not a scalar value function masquerading as policy."""
    keys = ("status", "exact", "byte_length", "byte_differences", "counts", "code_hash")
    delta = {k: {"before": parent.get(k), "after": child.get(k)}
             for k in keys if parent.get(k) != child.get(k)}
    a, b = parent.get("structure", {}), child.get("structure", {})
    for k in ("summary", "constraints"):
        if a.get(k) != b.get(k):
            if k == "constraints":
                delta["constraints"] = {name: {"before": a.get(k, {}).get(name, {}).get("same"),
                    "after": b.get(k, {}).get(name, {}).get("same")}
                    for name in b.get(k, {}) if a.get(k, {}).get(name, {}).get("same") != b[k][name].get("same")}
            else:
                delta["structure." + k] = {"before": a.get(k), "after": b.get(k)}
    return delta
