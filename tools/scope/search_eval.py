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


class StaleContextError(RuntimeError):
    """An evaluation must be retried on a node in the new compiler context."""


def digest(data):
    return hashlib.sha256(data.encode() if isinstance(data, str) else data).hexdigest()


def module_identity(module):
    """Hash loaded function bodies, not a newer source file underneath a batch.

    Normalize filenames/line positions so equivalent worktrees share identities.
    Build configuration values are fingerprinted separately in the context.
    """
    import marshal
    import types

    def portable(code):
        return code.replace(co_filename=Path(code.co_filename).name, co_firstlineno=0,
            co_consts=tuple(portable(c) if isinstance(c, types.CodeType) else c for c in code.co_consts))
    bodies = []
    for name, value in sorted(vars(module).items()):
        value = getattr(value, "__wrapped__", value)
        code = getattr(value, "__code__", None)
        if code and getattr(value, "__module__", None) == module.__name__:
            bodies.append(name.encode() + marshal.dumps(portable(code)))
    return digest(b"".join(bodies))


def preprocessed_concessions(pre, function=None, aliases=()):
    """Count sites after CPP, scoped to the target and reachable inline bodies.

    This counts source sites, not machine instructions or the number of inline
    expansions. Address-taken inline helpers are included conservatively. An
    unsupported definition spelling reports UNKNOWN instead of false pin-free.
    """
    token = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*|[A-Za-z_]\w*|[^\s]', re.S)
    tokens = [t for t in token.findall(re.sub(r'^\s*#.*$', '', pre, flags=re.M))
              if not t.startswith(('/*', '//'))]
    # A top-level brace following a balanced function parameter list starts a
    # definition; aggregate bodies and initializer braces are not definitions.
    definitions, depth, start, i = {}, 0, 0, 0
    while i < len(tokens):
        t = tokens[i]
        if t == '{' and depth == 0:
            prefix = tokens[start:i]
            name = None
            if prefix and prefix[-1] == ')':
                j, parens = len(prefix)-1, 1
                while j and parens:
                    j -= 1
                    parens += (prefix[j] == ')') - (prefix[j] == '(')
                if j and re.fullmatch(r'[A-Za-z_]\w*', prefix[j-1]):
                    name = prefix[j-1]
            end, nested = i+1, 1
            while end < len(tokens) and nested:
                nested += (tokens[end] == '{') - (tokens[end] == '}')
                end += 1
            if name and nested == 0:
                definitions[name] = (tokens[i+1:end-1], bool(set(prefix) & {'inline', '__inline', '__inline__'}))
            i, start = end, end
            continue
        if t == ';' and depth == 0:
            start = i+1
        i += 1
    function = next((n for n in (function, *aliases) if n in definitions), function)
    roots = [function] if function is not None else [name for name, (_, inline) in definitions.items() if not inline]
    if (function is not None and function not in definitions) or not roots:
        return dict(verdict="UNKNOWN", pins=None, barriers=None, by_function={},
                    detail="The selected preprocessed function definition could not be located.")
    reached, todo, by_function = set(), roots, {}
    while todo:
        name = todo.pop()
        if name in reached:
            continue
        reached.add(name)
        body = definitions[name][0]
        todo += [t for t in set(body) if t in definitions and definitions[t][1] and t not in reached]
        pins = barriers = 0
        for j, t in enumerate(body):
            if t not in {'asm', '__asm', '__asm__'}:
                continue
            k = j+1
            if k < len(body) and body[k] in {'volatile', '__volatile', '__volatile__'}:
                k += 1
            if k+1 >= len(body) or body[k] != '(':
                continue
            k += 1
            strings = []
            while k < len(body) and body[k].startswith('"'):
                strings.append(body[k][1:-1])
                k += 1
            if not strings:
                continue
            def unescape(m):
                value = m[0][1:]
                if value.startswith('x'):
                    return chr(int(value[1:], 16) & 255)
                if value[0] in '01234567':
                    return chr(int(value, 8) & 255)
                return {'n': '\n', 'r': '\r', 't': '\t', 'b': '\b', 'f': '\f', 'v': '\v', 'a': '\a'}.get(value, value)
            template = ''.join(re.sub(r'\\(?:x[0-9a-fA-F]+|[0-7]{1,3}|.)', unescape, s) for s in strings)
            if re.fullmatch(r'r(?:1[0-5]|[0-9])|a[1-4]|v[1-8]|sb|sl|fp|ip|sp|lr|pc', template):
                # A function ABI asm label is at file scope; inside a body this
                # syntax must belong to a register declaration to count a pin.
                prior = body[:j]
                boundary = max((n for n, b in enumerate(prior) if b in {';', '{', '}'}), default=-1)
                if 'register' in prior[boundary+1:]:
                    pins += 1
            elif not template:
                barriers += 1
        by_function[name] = dict(pins=pins, barriers=barriers)
    return dict(verdict="CONCESSIONS" if any(v[k] for v in by_function.values() for k in ('pins', 'barriers')) else "NONE_FOUND",
                pins=sum(v['pins'] for v in by_function.values()),
                barriers=sum(v['barriers'] for v in by_function.values()),
                by_function=by_function, scope="preprocessed target and referenced inline helpers; source sites")


def validate_knobs(knobs):
    """Reject misspellings and atoi-style silent no-ops before recording evidence."""
    name = r"(?:[A-Za-z_]\w*(?:#\d+)?|\d+)"
    integer = r"[+-]?\d+"
    formats = {
        **{k: name + ":" + integer + "(?:," + name + ":" + integer + ")*" for k in ("AGBCC_PRI", "AGBCC_FORCE")},
        **{k: r"[^,:\n]+:" + integer + r"(?:,[^,:\n]+:" + integer + ")*" for k in ("AGBCC_PRI_LABEL", "AGBCC_FORCE_LABEL")},
        "AGBCC_SPILL_LABEL": r"[^,:\n]+#\d+:" + integer + r"(?:,[^,:\n]+#\d+:" + integer + ")*",
        "AGBCC_SPILL": r"\d+:" + integer + r"(?:,\d+:" + integer + ")*",
        **{k: r"\d+(?:,\d+)*" for k in ("AGBCC_NOGIV", "AGBCC_GIV", "AGBCC_NOMOVE", "AGBCC_NOTIE")},
        "CSE_KEEP": name + "(?:," + name + ")*", "CSE_NOCANON": name,
        "CSE_NOFOLLOW_JUMP": r"\d+(?::[12])?(?:,\d+(?::[12])?)*",
        **{k: r"\d+" for k in ("AGBCC_NOPRE", "CSE_NOFOLLOW", "CSE_WHY", "CSE_DEBUG", "CSE_PATH", "CSE_PATHS", "AGBCC_GCSE_T", "AGBCC_NOSTRICT", "AGBCC_PRE_TRACE", "AGBCC_LOOP_TRACE")}}
    for key, value in knobs.items():
        if key not in formats:
            raise ValueError("unknown search experiment knob: " + key)
        if not re.fullmatch(formats[key], str(value)):
            raise ValueError(f"invalid {key} value {value!r}; numeric UIDs/events are required except named PRI/FORCE and CSE pseudo selectors")


class Evaluator:
    def __init__(self, directory):
        # Import scope as a library: no scope client, no external resident's state.
        import scope
        import fast
        import search_structure
        import search_trace
        self.scope = scope
        self.directory = Path(directory).resolve()
        self.directory.mkdir(parents=True, exist_ok=True)
        self.oracles = {}
        self._hashes = {}
        self.loaded_scope = scope.IMPLEMENTATION_ID
        self.loaded_fast = fast.IMPLEMENTATION_ID
        self.loaded_structure = getattr(search_structure, "IMPLEMENTATION_ID", digest(Path(search_structure.__file__).read_bytes()))
        self.loaded_trace = getattr(search_trace, "IMPLEMENTATION_ID", digest(Path(search_trace.__file__).read_bytes()))
        import sys
        orchestrator = sys.modules.get("search") or sys.modules.get("__main__")
        self.loaded_orchestrator = module_identity(orchestrator) if orchestrator else None
        semantic_module = sys.modules.get("semcheck")
        self.loaded_semantic = getattr(semantic_module, "IMPLEMENTATION_ID", digest((ROOT / "tools/semcheck.py").read_bytes()))
        import romlib
        self.support = {module.__name__: module_identity(module) for module in (scope.build, scope.check, romlib)}

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
        if os.path.basename(s.build.compiler_for(str(path))) != "agbcc":
            raise ValueError("this session needs the Thumb agbcc compiler")
        flags = s.build.cflags_for(str(path))
        # Paths produced below ROOT already have a canonical parent. Resolving
        # every header and every repeated include walks thousands of directory
        # components per request; resolve each distinct quoted path only once.
        deps = sorted(set((ROOT / "include").rglob("*.h")))
        resolutions = {}
        def resolve(path):
            if path not in resolutions:
                resolutions[path] = path.resolve()
            return resolutions[path]
        # Include unit-local headers too. Imported edits are recontextualized.
        local = (ROOT / origin).resolve().parent
        # Unit-local include chains need not live under include/. Discover quoted
        # paths recursively; system includes are disabled by the build flags.
        # Scanning every neighbor of a file under src/fn/ costs more than a warm
        # compile and discovers nothing that these actual include paths omit.
        queue = [(source, local)] + [(p.read_text(errors="replace"), p.parent) for p in deps]
        visited = set(deps)
        while queue:
            text, directory = queue.pop()
            for name in re.findall(r'^\s*#\s*include\s*"([^"\n]+)"', text, re.M):
                child = resolve(directory / name)
                if not child.is_file():
                    child = resolve(ROOT / "include" / name)
                if child.is_file() and child not in visited:
                    visited.add(child)
                    deps.append(child)
                    queue.append((child.read_text(errors="replace"), child.parent))
        headers = [(os.path.relpath(p, ROOT), self.file_hash(p)) for p in deps]
        symbols = [(str(p.relative_to(ROOT)), self.file_hash(p))
                   for p in sorted((ROOT / "symbols").glob("*")) if p.is_file()]
        import romlib
        rom_path = romlib.baserom_path()
        macros = ROOT / "asm/macros.inc"
        asm_macros = dict(present=macros.is_file(),
                          sha256=self.file_hash(macros) if macros.is_file() else None)
        # Freeze configuration values, not references to mutable build lists.
        # Scope's CPP and fork-server caches otherwise keep old definitions or
        # assembler commands while a new context claims to have used new ones.
        toolchain = dict(flags=list(flags), cpp=list(s.build.CPP),
            cpp_binary=self.file_hash(shutil.which(s.build.CPP[0])),
            defines=list(s.build.DEFINES), assembler_command=list(s.build.AS),
            assembler=self.file_hash(shutil.which(s.build.AS[0])), asm_macros=asm_macros)
        assembly = ROOT / "build/asm/nonmatching" / (rom_name + ".s")
        assembly_identity = dict(path=str(assembly.relative_to(ROOT)),
                                 present=assembly.is_file(),
                                 sha256=self.file_hash(assembly) if assembly.is_file() else None)
        common_environment = digest(json.dumps([headers, symbols, self.file_hash(s.INSTR),
            self.file_hash(rom_path), self.file_hash(ROOT / "build/boktai3.elf"), toolchain]))
        environment = digest(json.dumps([common_environment, assembly_identity]))
        if (getattr(s, "_SEARCH_ENVIRONMENT", None) != common_environment or
                getattr(self, "_environment", environment) != environment):
            # Scope's preprocessing cache keys source mtimes, not header contents;
            # its live fork servers also retain the old symbol resolver.
            self.close()
            s._FAST.clear()
            s._PRE.clear()
            s._CPP1.clear()
            s._HEADS.clear()
            s._ROMBYTES.clear()
            s._MEMO.clear()
            self.oracles.clear()
            rom_name = s.rom_name(str(path), function)
            _, addresses, sizes = s.inventory()
        self._environment = environment
        # These caches belong to the imported scope module, not this Evaluator.
        # A fresh session/evaluator must validate caches populated by another one.
        s._SEARCH_ENVIRONMENT = common_environment
        address, size = addresses[rom_name], sizes[rom_name]
        # The source's compiler directive was checked above; the target mode is
        # in functions.csv. is_arm() additionally scans every source unit to
        # locate its owner, which is unnecessary for an immutable copied unit.
        if any(a == address and mode == "arm" for a, mode in s.arm_starts()):
            raise ValueError("the search environment currently serves Thumb agbcc functions")
        target = s.rom_bytes()[s.off(address):s.off(address) + size]
        classification = dict(generated_assembly=assembly_identity,
                              data=[list(r) for r in s.rom_data(rom_name, address, target)])
        import importlib.util
        import sys
        packages = str(ROOT / "build/search-sol/python")
        if packages not in sys.path:
            sys.path.insert(0, packages)
        spec = importlib.util.find_spec("unicorn")
        backend = []
        if spec and spec.origin:
            package = Path(spec.origin).parent
            backend = [(str(p.relative_to(package)), self.file_hash(p)) for p in sorted(package.rglob("*"))
                       if (p.name == "__init__.py" or p.suffix in (".so", ".dylib", ".dll")) and p.is_file()]
        semantic_runtime = dict(python=sys.version, emulator=digest(json.dumps(backend)),
                                worker=self.file_hash(ROOT / "tools/scope/search_semantic.py"))
        if getattr(self, "semantic_runtime", None) != semantic_runtime:
            self.semantic_failure = None
        self.semantic_runtime = semantic_runtime
        return dict(version=VERSION, function=function, rom_name=rom_name,
                    address=address, size=size, target_hash=digest(target),
                    target_classification=classification,
                    rom_hash=self.file_hash(rom_path),
                    compiler=self.file_hash(s.INSTR), flags=toolchain["flags"],
                    assembler=toolchain["assembler"], assembler_flags=toolchain["assembler_command"][1:],
                    assembler_command=toolchain["assembler_command"], asm_macros=asm_macros,
                    cpp=toolchain["cpp"], cpp_binary=toolchain["cpp_binary"], defines=toolchain["defines"],
                    headers=digest(json.dumps(headers)), symbols=digest(json.dumps(symbols)),
                    elf=self.file_hash(ROOT / "build/boktai3.elf"),
                    evaluator=IMPLEMENTATION, orchestrator=self.loaded_orchestrator,
                    scope=self.loaded_scope, fast=self.loaded_fast,
                    structure=self.loaded_structure, trace=self.loaded_trace, semantic=self.loaded_semantic,
                    semantic_runtime=semantic_runtime,
                    support=self.support,
                    origin=os.path.relpath(ROOT / origin, ROOT))

    def materialize(self, source, origin):
        # Preserve quoted-include lookup even though snapshots live under build/.
        local = (ROOT / origin).resolve().parent
        text = re.sub(r'^(\s*#\s*include\s*)"([^"\n]+)"',
                      lambda m: m[1] + '"' + os.path.relpath(local / m[2], ROOT) + '"'
                      if (local / m[2]).exists() else m[0], source, flags=re.M)
        # Concurrent workers may compile different nodes of one session. Never
        # share a mutable candidate.c between their preprocessing operations.
        directory = self.directory / "sources"
        directory.mkdir(exist_ok=True)
        path = directory / (digest(text) + ".c")
        if not path.exists():
            import tempfile
            with tempfile.NamedTemporaryFile(mode="w", dir=directory, prefix=".source-", delete=False) as temporary:
                temporary.write(text)
                staged = Path(temporary.name)
            staged.replace(path)
        return path

    def evaluate(self, source, function, origin, knobs=None, semantic_trials=0, expected_context=None):
        s = self.scope
        s.WORK = str(self.directory / "traces")
        knobs = dict(knobs or {})
        validate_knobs(knobs)
        # Neither the caller's environment nor a prior experiment may influence exact.
        saved = {k: os.environ.pop(k) for k in list(os.environ)
                 if k.startswith(("AGBCC_", "CSE_", "SCOPE_TRANSFORM"))}
        output = io.StringIO()
        start = time.perf_counter()
        try:
            actual_context = self.context(source, function, origin)
            if expected_context is not None and expected_context != actual_context:
                raise StaleContextError("Compiler inputs changed before compilation; refresh the session context.")
            path = self.materialize(source, origin)
            compile_started = time.perf_counter()
            with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
                effective_knobs = s.resolve_knobs(str(path), function, knobs)
                compiled = s.fast_compile(str(path), function, effective_knobs)
                if compiled is None:
                    # Capture actual diagnostics and keep failures in the graph.
                    code, address, info = s.compile_draft(str(path), function, effective_knobs)
                    data = info.get("data", ())
                else:
                    code, address, data = compiled
            compile_ms = (time.perf_counter() - compile_started) * 1000
            name = s.rom_name(str(path), function)
            size = s.inventory()[2][name]
            target = s.rom_bytes()[s.off(address):s.off(address) + size]
            target_data = actual_context["target_classification"]["data"]
            target_rows = s.decode(target, address, target_data)
            rows = s.decode(code, address, data)
            pairs, shifted = s.align(target_rows, rows)
            counts = s.count_text([{"tag": t} for _, _, t in pairs])
            exact = not knobs and code == target
            result = dict(status="exact" if exact else "different", exact=exact,
                          experiment=bool(knobs), experiment_exact=bool(knobs) and code == target,
                          experiment_build="diagnostic -g UID stream" if s.needs_g(knobs) else "normal no-g build",
                          knobs=knobs, effective_knobs=effective_knobs, compile_ms=round(compile_ms, 3),
                          elapsed_ms=0, counts="MATCH" if exact else counts,
                          code_hash=digest(code), code_hex=code.hex(), target_hash=digest(target),
                          byte_length=len(code), target_length=len(target),
                          byte_differences=sum(a != b for a, b in zip(code, target)) + abs(len(code)-len(target)),
                          shifted_branches=shifted, rows=rows, target_rows=target_rows,
                          current_data=data, target_data=target_data,
                          differences=[dict(target=target_rows[x] if x is not None else None,
                                            current=rows[y] if y is not None else None, tag=t)
                                       for x, y, t in pairs if t != "="],
                          semantic=dict(verdict="EXACT" if exact else "NOT_RUN", trials=0))
            result["concessions"] = preprocessed_concessions(s.fast_preprocess(str(path), func=function), function,
                aliases=(name, f"sub_{address:08X}"))
            if s.needs_g(knobs):
                with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
                    result["diagnostic_baseline_differs"] = s.g_sensitive(str(path), function)
            if semantic_trials and not knobs and not exact:
                try:
                    result["semantic"] = self.semantic(source, function, target, code, address,
                                                       path, semantic_trials)
                except (ImportError, SystemExit) as e:
                    result["semantic"] = dict(verdict="UNAVAILABLE", trials=0, detail=str(e), proof=False)
            result["diagnostics"] = output.getvalue()[-8000:]
            # The graph's node context describes one resolved classification and
            # compiler environment. Never attach newly classified or newly built
            # evidence to that old node if dependencies changed during the work.
            if actual_context != self.context(source, function, origin):
                raise StaleContextError("Compiler inputs changed during compilation; refresh the session context.")
            result["elapsed_ms"] = round((time.perf_counter()-start)*1000, 3)
            return result
        except StaleContextError:
            raise
        except (SystemExit, OSError, RuntimeError, ValueError) as e:
            return dict(status="experiment_error" if knobs else "compile_error", exact=False, experiment=bool(knobs),
                        knobs=knobs, diagnostics=output.getvalue()[-8000:] + "\n" + str(e),
                        elapsed_ms=round((time.perf_counter()-start)*1000, 3),
                        semantic=dict(verdict="NOT_RUN", trials=0))
        finally:
            os.environ.update(saved)

    def semantic(self, source, function, target, code, address, path, trials):
        import subprocess
        import sys
        if not 1 <= trials <= 10000:
            raise ValueError("semantic trials must be between 1 and 10000")
        if getattr(self, "semantic_failure", None):
            return dict(verdict="UNAVAILABLE", trials=0, detail=self.semantic_failure, proof=False)
        s = self.scope
        payload = dict(source=source, pre=s.preprocess(str(path), func=function), function=function,
                       rom_name=s.rom_name(str(path), function), code_hex=code.hex(), address=address, trials=trials)
        try:
            child = subprocess.run([sys.executable, str(ROOT / "tools/scope/search_semantic.py")],
                input=json.dumps(payload), capture_output=True, text=True, cwd=ROOT, timeout=10)
        except subprocess.TimeoutExpired:
            return dict(verdict="TIMEOUT", trials=0, detail="Differential execution exceeded 10 seconds.", proof=False)
        if child.returncode:
            detail = (f"Native semantic worker exited with signal {-child.returncode}." if child.returncode < 0
                      else f"Semantic worker exited with status {child.returncode}.")
            self.semantic_failure = detail + " " + child.stderr[-1000:]
            return dict(verdict="UNAVAILABLE", trials=0, detail=self.semantic_failure, proof=False)
        try:
            result = json.loads(child.stdout)
        except ValueError:
            return dict(verdict="UNAVAILABLE", trials=0, detail="Invalid semantic worker response: " + child.stderr[-1000:], proof=False)
        return result

    def diagnose(self, source, function, origin, command="why", argument="all"):
        s = self.scope
        s.WORK = str(self.directory / "traces")
        path = self.materialize(source, origin)
        output = io.StringIO()
        saved = {k: os.environ.pop(k) for k in list(os.environ)
                 if k.startswith(("AGBCC_", "CSE_", "SCOPE_TRANSFORM"))}
        try:
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
        finally:
            os.environ.update(saved)
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
