"""A persistent, production-clean compiler oracle for matching search.

The cheap path is scope's existing fork server and relocator.  A byte match is
independent of all disassembly scores.  Diagnostic knobs have a separate verdict
and can never produce ``exact=True``.  Detailed RTL and execution checks are
requested lazily, rather than paid for at every search edge.

This module deliberately does not choose a scalar search reward.  It exposes
bytes, scope's alignment, and decoded rows for the dependency-aware comparator.
"""

from __future__ import annotations

import ast
import contextlib
import dataclasses
import functools
import hashlib
import importlib
import io
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import signal
import subprocess
import sys
import tempfile
import threading
import time
from typing import Any, Mapping


_SCOPE_LOCK = threading.RLock()
_KNOB_PREFIXES = ("AGBCC_", "CSE_")
_CONTROL_PREFIXES = ("AGBCC_", "CSE_", "SCOPE_")
_C_TOKEN = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|[A-Za-z_]\w*|\S', re.S)
_CPP_ENVIRONMENT = ("CPATH", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH", "OBJC_INCLUDE_PATH",
                    "GCC_EXEC_PREFIX", "COMPILER_PATH", "SDKROOT")
_C_TOKENS = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*', re.S)
_DYNAMIC_INCLUDE_CONTEXT = re.compile(r"\b(?:__LINE__|__COUNTER__|__FILE__|__BASE_FILE__|__INCLUDE_LEVEL__)\b")
_SEMANTIC_MODEL_VERSION = 3
_SEMANTIC_CONTRACT_MODEL = "frozen-baseline-address-contracts-v1"
_SEMANTIC_TIMEOUT_SECONDS = 30


def _concessions(preprocessed: str, names: tuple[str, ...]) -> dict[str, int]:
    """Count expanded concessions in the target and referenced inline bodies.

    This is conservative lexical reachability, not optimizer reachability:
    taking a helper's address or referring to it on a dead C path still counts.
    Strings, comments and CPP line markers cannot manufacture a function or asm.
    An unknown target declarator falls back to the entire translation unit;
    unrecognized helper bodies remain in the conservative outside-body scan.
    """
    text = re.sub(r'^\s*#.*$', '', _without_comments(preprocessed), flags=re.M)
    tokens = _C_TOKEN.findall(text)
    functions: dict[str, tuple[bool, list[str]]] = {}
    inline_names: set[str] = set()
    outside: list[str] = []

    def after_group(at: int, opening: str, closing: str) -> int:
        depth = 0
        for pos in range(at, len(tokens)):
            depth += (tokens[pos] == opening) - (tokens[pos] == closing)
            if depth == 0:
                return pos + 1
        return len(tokens)

    i = boundary = depth = 0
    while i < len(tokens):
        token = tokens[i]
        if (depth == 0 and token not in {'asm', '__asm', '__asm__', '__attribute__', '__attribute'}
                and re.fullmatch(r'[A-Za-z_]\w*', token) and tokens[i + 1:i + 2] == ['(']):
            end = after_group(i + 1, '(', ')')
            while end < len(tokens) and tokens[end] in {'__attribute__', '__attribute'}:
                if tokens[end + 1:end + 2] != ['(']:
                    break
                end = after_group(end + 1, '(', ')')
            inline = bool({'inline', '__inline', '__inline__'} & set(tokens[boundary:i]))
            if inline and tokens[end:end + 1] in ([';'], ['{']):
                inline_names.add(token)
            if tokens[end:end + 1] == ['{']:
                stop = after_group(end, '{', '}')
                header = tokens[boundary:end]
                functions[token] = (bool({'inline', '__inline', '__inline__'} & set(header)),
                                    tokens[boundary:stop])
                boundary = i = stop
                continue
        depth += (token == '{') - (token == '}')
        if depth == 0 and token in {';', '}'}:
            outside.extend(tokens[boundary:i + 1])
            boundary = i + 1
        i += 1

    selected = {name for name in names if name in functions}
    if selected:
        pending = list(selected)
        while pending:
            refs = set(functions[pending.pop()][1])
            for name, (inline, _) in functions.items():
                if (inline or name in inline_names) and name in refs and name not in selected:
                    selected.add(name)
                    pending.append(name)
        active = [token for name in sorted(selected) for token in functions[name][1]]
    else:
        active = tokens

    def count(parts: list[str], *, outside_body: bool = False) -> dict[str, int]:
        found = {'register_pins': 0, 'asm_barriers': 0, 'inline_assembly': 0}
        for index, token in enumerate(parts):
            # an empty `do { } while (0)` acts as a barrier: its loop notes end cse blocks and weight refs
            if token == 'do' and parts[index + 1:index + 7] == ['{', '}', 'while', '(', '0', ')']:
                found['asm_barriers'] += 1
                continue
            # so does an if with nothing in its body; DEBUG_PRINT() (include/debug.h) leaves `{ ; }`
            if token == 'if' and parts[index + 1:index + 2] == ['(']:
                depth, at = 0, index + 1
                while at < len(parts):
                    depth += (parts[at] == '(') - (parts[at] == ')')
                    at += 1
                    if depth == 0:
                        break
                if parts[at:at + 2] == ['{', '}'] or parts[at:at + 1] == [';']:
                    found['asm_barriers'] += 1
                continue
            if token not in {'asm', '__asm', '__asm__'}:
                continue
            at = index + 1
            while at < len(parts) and parts[at] in {'volatile', '__volatile', '__volatile__', 'inline', 'goto'}:
                at += 1
            if parts[at:at + 1] != ['(']:
                continue
            at += 1
            strings = []
            while at < len(parts) and parts[at].startswith('"'):
                try:
                    strings.append(ast.literal_eval(parts[at]))
                except (SyntaxError, ValueError):
                    # Unknown C string extensions are still a concession.
                    strings.append(parts[at][1:-1])
                at += 1
            if not strings:
                continue
            assembly = ''.join(strings)
            if outside_body and assembly.lstrip().startswith('.include '):
                continue  # the unit's unrelated INCLUDE_ASM placeholders
            if parts[at:at + 1] == [')'] and re.fullmatch(r'r\d+|sb|sl|fp|ip|sp|lr', assembly):
                found['register_pins'] += 1
            else:
                found['asm_barriers' if not assembly else 'inline_assembly'] += 1
        return found

    result = count(active)
    # Global register bindings can affect the target without a body-local pin.
    # Unknown helper syntax is also retained here rather than certified clean.
    if selected:
        for key, value in count(outside, outside_body=True).items():
            result[key] += value
    return result


def _semantic_call_arities(contracts: Mapping[str, Any], names: Mapping[int, str]
                          ) -> tuple[dict[str, int], dict[str, dict[str, Any]]]:
    """Adapt address contracts to semcheck's selected display names.

    ELF aliases are labels, not signatures.  In particular a ``foo__addr``
    label must inherit a prototype only when it denotes the same address.
    Unknown signatures are deliberately absent: semcheck then observes all
    four argument registers and its outgoing-stack heuristic.  A capped arity
    of four does not imply that a callee has no stack arguments.
    """
    arities: dict[str, int] = {}
    provenance: dict[str, dict[str, Any]] = {}
    for callee, count in contracts.get("arities", {}).items():
        if (not isinstance(callee, str) or not re.fullmatch(r"#0x[0-9a-fA-F]+", callee)
                or not isinstance(count, int) or isinstance(count, bool) or not 0 <= count <= 4):
            raise ValueError("invalid frozen call-register contract")
        address = int(callee[1:], 16)
        if address > 0xFFFFFFFF or address & 1:
            raise ValueError("call-register contract requires an aligned 32-bit callee address")
        record = contracts.get("provenance", {}).get(callee, {})
        declarations = record.get("declarations", [])
        counts = {item.get("parameter_count") for item in declarations}
        exact_count = next(iter(counts)) if len(counts) == 1 else None
        if not isinstance(exact_count, int) or isinstance(exact_count, bool) or exact_count < count:
            exact_count = None
        # infer_call_contracts preserves a full parameter count in provenance,
        # although its graph API intentionally caps core-register arities at 4.
        # semcheck uses >4 to retain its stack-argument comparison.
        observed_count = count if count < 4 else (exact_count if exact_count is not None else 5)
        label = names.get(address, f"0x{address:08X}")
        for alias in (label, "via " + label):
            arities[alias] = observed_count
            provenance[alias] = {
                "callee": callee, "selected_name": label,
                "register_arguments": count, "parameter_count": exact_count,
                "stack_arguments": ("none-declared" if observed_count <= 4 else
                                    "declared" if exact_count is not None else "unknown"),
                "symbols": list(record.get("symbols", [])),
                "basis": record.get("basis", "frozen baseline call-register contract"),
            }
    return arities, provenance


def _without_comments(source: str) -> str:
    # Translation phase 2 precedes comment removal, including // continuations.
    source = source.replace("\\\r\n", "").replace("\\\n", "")
    return _C_TOKENS.sub(lambda m: "".join("\n" if c == "\n" else " " for c in m[0])
                         if m[0].startswith(("/*", "//")) else m[0], source)


def _file_stamp(path: Path) -> tuple[int, int, int, int] | None:
    try:
        st = path.stat()
    except FileNotFoundError:
        return None
    return (st.st_dev, st.st_ino, st.st_mtime_ns, st.st_size)


def _serialized(method):
    """Scope has process-global caches; parallel workers must be processes."""
    @functools.wraps(method)
    def call(*args, **kwargs):
        with _SCOPE_LOCK:
            return method(*args, **kwargs)
    return call


def clean_environment(env: Mapping[str, str] | None = None) -> dict[str, str]:
    """Keep network/tool configuration, remove inherited compiler experiments."""
    return {k: v for k, v in (os.environ if env is None else env).items()
            if not k.startswith(_CONTROL_PREFIXES)}


def byte_distance(target: bytes, candidate: bytes) -> int:
    """Position-wise unequal bytes, including the longer sequence's tail."""
    return sum(a != b for a, b in zip(target, candidate)) + abs(len(target) - len(candidate))


def _sha(data: bytes | str) -> str:
    if isinstance(data, str):
        data = data.encode()
    return hashlib.sha256(data).hexdigest()


def _run_semantic_process(request: Mapping[str, Any], *, cwd: Path,
                          timeout: float = _SEMANTIC_TIMEOUT_SECONDS,
                          command: list[str] | None = None) -> dict[str, Any]:
    """Run the actual check in a child; a capability probe alone is insufficient.

    The optional command is a private testing seam for real signal/timeout
    regressions. Production always runs this module's JSON worker.
    """
    command = command or [sys.executable, str(Path(__file__).resolve()), '--semantic-worker']
    isolation = {'mode': 'subprocess', 'timeout_seconds': timeout}
    unavailable = {'verdict': 'unavailable', 'trials': 0, 'proof': False,
                   'model_version': _SEMANTIC_MODEL_VERSION, 'isolation': isolation}
    try:
        child = subprocess.run(command, input=json.dumps(request), cwd=cwd,
                               env=clean_environment(), capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return {**unavailable, 'detail': f'Semantic worker exceeded {timeout:g} seconds.',
                'isolation': {**isolation, 'termination': 'timeout'}}
    except OSError as exc:
        return {**unavailable, 'detail': f'Cannot start semantic worker: {exc}'}
    if child.returncode:
        detail = f'Semantic worker exited with status {child.returncode}.'
        if child.returncode < 0:
            number = -child.returncode
            try:
                name = signal.Signals(number).name
            except ValueError:
                name = 'unknown signal'
            detail = f'Native semantic worker exited with signal {number} ({name}).'
            isolation['signal'] = number
        return {**unavailable, 'detail': detail, 'isolation': {**isolation, 'returncode': child.returncode},
                'stderr': child.stderr[-2000:]}
    try:
        result = json.loads(child.stdout)
        if (not isinstance(result, dict) or result.get('verdict') not in
                {'sampled_agree', 'counterexample', 'inconclusive', 'unavailable'}
                or result.get('proof') is not False):
            raise ValueError('unexpected result schema')
    except (TypeError, ValueError) as exc:
        return {**unavailable, 'detail': f'Invalid semantic worker response: {exc}',
                'stderr': child.stderr[-2000:]}
    result['isolation'] = {**isolation, 'returncode': 0}
    return result


def _semantic_worker(request: Mapping[str, Any]) -> dict[str, Any]:
    """Child-only adapter to semcheck, with all physical and ABI inputs frozen."""
    semcheck = importlib.import_module('semcheck')
    engine_hash = _sha(Path(semcheck.__file__).read_bytes())
    if engine_hash != request['engine_sha256']:
        raise ValueError('semantic engine changed before execution')
    trials = request['trials']
    if not isinstance(trials, int) or isinstance(trials, bool) or not 1 <= trials <= 10000:
        raise ValueError('semantic trials must be an integer between 1 and 10000')
    rom = Path(request['rom_path']).read_bytes()
    if _sha(rom) != request['rom_sha256']:
        raise ValueError('frozen semantic ROM snapshot changed')
    baseline = request['baseline_preprocessed']
    actual = next((name for name in request['own_names']
                   if semcheck.param_kinds(baseline, name) is not None), request['function'])
    kinds = semcheck.param_kinds(baseline, actual)
    returns = semcheck.returns_value(baseline, actual)
    input_contract = {
        'function': actual, 'parameter_kinds': kinds, 'check_return_register': returns,
        'basis': 'frozen baseline definition; syntactic sampling heuristic, not an input/precondition proof',
        'definition_found': kinds is not None,
    }
    names = {int(address): name for address, name in request['names'].items()}
    # Oracle normally reloads the mutable inventory from disk. This single-use
    # child instead supplies the address and extent already frozen by Evaluator.
    original_inventory = semcheck.check.inventory
    function = request['function']
    semcheck.check.inventory = lambda: ({function: function}, {function: request['address']},
                                       {function: request['target_size']})
    try:
        oracle = semcheck.Oracle(function, trials, returns, rom, names=names,
                                  arity=request['arities'], kinds=kinds)
        verdict, detail = oracle.check(bytes.fromhex(request['code_hex']))
    finally:
        semcheck.check.inventory = original_inventory
    called = sorted({call[0] for _, run in oracle.runs for call in run.calls})
    return {'verdict': {'EQUIVALENT': 'sampled_agree', 'DIFFERENT': 'counterexample',
                        'INCOMPLETE': 'inconclusive'}[verdict],
            'detail': detail, 'trials': trials, 'block_coverage_percent': oracle.coverage,
            'proof': False, 'model_version': _SEMANTIC_MODEL_VERSION,
            'engine': {'module': 'semcheck', 'sha256': engine_hash},
            'input_contract': input_contract, 'observed_target_callees': called}


@dataclasses.dataclass(slots=True)
class Evaluation:
    status: str
    exact: bool
    diagnostic_match: bool
    function: str
    address: int
    source_hash: str
    environment_hash: str
    target_size: int
    candidate_size: int = 0
    byte_mismatches: int | None = None
    instruction_diff: int | None = None
    moved_branches: int = 0
    shape_distance: int | None = None
    code_hash: str | None = None
    elapsed_ms: float = 0.0
    compile_ms: float = 0.0
    error: str | None = None
    concessions: dict[str, int] = dataclasses.field(default_factory=dict)
    diagnostic_knobs: dict[str, str] = dataclasses.field(default_factory=dict)
    resolved_diagnostic_knobs: dict[str, str] = dataclasses.field(default_factory=dict)
    semantic: dict[str, Any] | None = None
    code: bytes = dataclasses.field(default=b"", repr=False)
    rows: list[tuple] = dataclasses.field(default_factory=list, repr=False)
    data_ranges: list[tuple[int, int]] = dataclasses.field(default_factory=list, repr=False)

    @property
    def pin_free(self) -> bool:
        return not any(self.concessions.values())

    def to_dict(self, *, include_rows: bool = False, include_code: bool = False) -> dict[str, Any]:
        """Compact JSON-safe summary; large bytes/listings are opt-in."""
        out = {f.name: getattr(self, f.name) for f in dataclasses.fields(self)
               if f.name not in {"code", "rows", "data_ranges"}}
        out["pin_free"] = self.pin_free
        if include_rows:
            out["rows"] = self.rows
            out["data_ranges"] = self.data_ranges
        if include_code:
            out["code_hex"] = self.code.hex()
        return out


def _load_scope(repo: Path):
    # scope.py's CLI convenience re-execs Python when capstone is missing.
    # That is unsafe as an imported library (notably ``python -`` loses its
    # program), so fail honestly before importing that CLI module.
    try:
        importlib.import_module("capstone")
    except ImportError:
        raise RuntimeError("capstone is unavailable in this interpreter; use build/venv/bin/python") from None
    for path in (repo / "tools", repo / "tools" / "scope"):
        if str(path) not in sys.path:
            sys.path.insert(0, str(path))
    # Scope and its build/check imports keep ROOT in module state.  Mixing two
    # repositories in one process would silently resolve against the first one.
    scope = importlib.import_module("scope")
    if Path(scope.ROOT).resolve() != repo:
        raise ValueError("one repository per evaluator process; start another worker for another checkout")
    return scope, importlib.import_module("fast")


class Evaluator:
    """Persistent per-function oracle.  One worker process may own several.

    ``source_path`` names the original translation unit and fixes its compiler
    variant/flags.  Candidate strings do not get to alter those flags.  Ordinary
    source strings retain their macros and are preprocessed; AST moves can pass
    ``preprocessed=True`` to reuse their already-preprocessed translation unit.

    Project include files and the baseline's other header dependencies are
    frozen at initialization.  Adding a dependency outside that inventory needs
    a new baseline/session.  A changed include search result invalidates cached
    evaluations even when every previously used header remains unchanged.
    """

    def __init__(self, repo: str | Path, function: str, source_path: str | Path,
                 workdir: str | Path | None = None):
        self.repo = Path(repo).resolve()
        self.source_path = Path(source_path)
        if not self.source_path.is_absolute():
            self.source_path = self.repo / self.source_path
        self.source_path = self.source_path.resolve()
        self.original_source = self.source_path.read_text()
        self._closed = False
        self._semantic_contracts: dict[str, Any] | None = None
        self._semantic_binding_hash: str | None = None
        self._semantic_baseline_source = self.original_source
        self._semantic_baseline_preprocessed: str | None = None
        self._semantic_input: dict[str, Any] | None = None
        self._fast: dict[bool, Any] = {}
        self._last_preprocessed = ""
        self._cpp_cache: dict = {}
        self._header_cache: dict = {}
        self._pre_cache: dict = {}
        self.workdir = Path(workdir) if workdir is not None else self.repo / "build" / "matchsearch" / "compiler"
        self.workdir.mkdir(parents=True, exist_ok=True)
        self.workdir = Path(tempfile.mkdtemp(prefix=f"worker-{os.getpid()}-", dir=self.workdir)).resolve()
        self._input = self.workdir / "candidate.c"
        self._input.write_text(self.original_source)
        with _SCOPE_LOCK:
            self.scope, self.fast = _load_scope(self.repo)
            self.build = self.scope.build
            self.cpp = list(self.build.CPP) + ["-iquote", str(self.source_path.parent)]
            elf = self.repo / "build" / "boktai3.elf"
            if not elf.is_file() or elf.stat().st_size == 0:
                raise FileNotFoundError("baseline ELF is missing or empty; finish the baseline ROM build before opening a session")
            by_label, addresses, sizes = self.scope.inventory()
            self.function = by_label.get(function, function)
            if self.function not in addresses:
                raise KeyError(f"unknown ROM function: {function}")
            self.address = addresses[self.function]
            self.own_names = tuple(dict.fromkeys((function, self.function, f"sub_{self.address:08X}")))
            compiler_kind = Path(self.build.compiler_for(str(self.source_path))).name
            if compiler_kind != "agbcc":
                raise ValueError(f"fork evaluator requires agbcc Thumb, source uses {compiler_kind}")
            self.flags = tuple(self.build.cflags_for(str(self.source_path)))
            self.production_compiler = Path(self.build.compiler_for(str(self.source_path)))
            self.assembler = list(self.build.AS) + ["-I", str(self.repo)]
            self.compiler = Path(self.scope.INSTR)
            if not self.compiler.is_file():
                raise FileNotFoundError(f"instrumented compiler missing: run sh tools/scope/build_agbcc.sh ({self.compiler})")
            self.rom_symbols = self.scope.elf_syms()
            # scope's CLI memoizes ROM bytes for its whole process lifetime.
            # An independent new evaluator must not inherit that snapshot after
            # the file or BOKTAI3_ROM selection changes.
            romlib = importlib.import_module("romlib")
            try:
                self.rom_path = Path(romlib.baserom_path()).resolve()
                self.rom = romlib.load_rom(verify=True)
            except SystemExit as exc:
                raise ValueError(f"ROM validation failed: {exc}") from None
            offset = self.address - 0x08000000
            self.target_code = bytes(self.rom[offset:offset + sizes[self.function]])
            if len(self.target_code) != sizes[self.function]:
                raise ValueError("target ROM function extends outside supplied ROM")
            self._freeze_target_classification()
            self.target_rows = self.scope.decode(self.target_code, self.address, self.target_data)
            self.identity = self._identity()
            self.environment_hash = _sha(json.dumps(self.identity, sort_keys=True))

    def _freeze_target_classification(self) -> None:
        """Freeze both ROM data ranges and the optional assembly that selects them.

        A missing generated file is an input too: adding it can make scope stop
        using its literal-pool fallback, changing disassembly and ranking while
        leaving the ROM and candidate bytes identical.
        """
        path = self.repo / 'build' / 'asm' / 'nonmatching' / (self.function + '.s')
        before = _file_stamp(path)
        contents = path.read_bytes() if before is not None else None
        self.target_data = list(self.scope.rom_data(self.function, self.address, self.target_code))
        if _file_stamp(path) != before:
            raise RuntimeError('target assembly changed while opening the evaluator; retry with a stable build')
        self._target_asm_input = (path, before)
        self._target_classification = {
            'mode': 'thumb',
            'generated_assembly': {
                'path': os.path.relpath(path, self.repo),
                'present': contents is not None,
                'sha256': _sha(contents) if contents is not None else None,
            },
            'resolved_data_ranges': [list(pair) for pair in self.target_data],
        }

    def _identity(self) -> dict[str, Any]:
        """Hash dependencies once when opening a session, never per candidate."""
        project_headers = sorted(p for p in (self.repo / "include").rglob("*") if p.is_file())
        extra_headers = self._freeze_include_inputs(project_headers)
        deps = [*project_headers, *extra_headers,
                self.repo / "asm" / "macros.inc", self.repo / "tools" / "build.py",
                self.repo / "tools" / "scope" / "fast.py",
                self.repo / "tools" / "scope" / "scope.py",
                self.repo / "tools" / "scope" / "agbcc.patch",
                self.repo / "symbols" / "functions.csv",
                self.repo / "symbols" / "progress_functions.csv",
                self.repo / "symbols" / "ram.csv",
                self.rom_path,
                self.repo / "build" / "boktai3.elf"]
        dep_hash = hashlib.sha256()
        self._dependency_stamps: list[tuple[Path, int, int]] = []
        for path in deps:
            if path.is_file():
                st = path.stat()
                self._dependency_stamps.append((path, st.st_mtime_ns, st.st_size))
                dep_hash.update(os.path.relpath(path, self.repo).encode() + b"\0")
                dep_hash.update(path.read_bytes())
        compiler_stat = self.compiler.stat()
        self._dependency_stamps.append((self.compiler, compiler_stat.st_mtime_ns, compiler_stat.st_size))
        production_stat = self.production_compiler.stat()
        self._dependency_stamps.append((self.production_compiler, production_stat.st_mtime_ns, production_stat.st_size))
        tool_hashes = {}
        for name, command in (("assembler", self.assembler), ("preprocessor", self.cpp)):
            resolved = shutil.which(command[0])
            if resolved is None:
                raise FileNotFoundError(f"{name} executable not found: {command[0]}")
            path = Path(resolved).resolve()
            st = path.stat()
            self._dependency_stamps.append((path, st.st_mtime_ns, st.st_size))
            tool_hashes[name] = {"executable": str(path), "sha256": _sha(path.read_bytes())}
        identity = {"version": 2, "function": self.function, "address": self.address,
                "rom_sha256": _sha(self.rom), "target_sha256": _sha(self.target_code),
                "target_classification": self._target_classification,
                "concession_model": "preprocessed-target-and-referenced-inline-definitions-v1",
                "compiler_sha256": _sha(self.compiler.read_bytes()),
                "production_compiler_sha256": _sha(self.production_compiler.read_bytes()),
                "flags": list(self.flags), "cpp": self.cpp, "tool_executables": tool_hashes,
                "defines": list(self.build.DEFINES), "assembler": list(self.build.AS),
                "dependencies_sha256": dep_hash.hexdigest(),
                "source_unit": os.path.relpath(self.source_path, self.repo),
                "production_knobs": {}}
        if any(value is not None for value in self._include_environment.values()):
            identity["include_environment"] = self._include_environment
        self.validate_environment()
        return identity

    def _include_signature(self, source: str) -> str:
        text = _without_comments(source)
        if re.search(r"\b__has_include(?:_next)?\b", text):
            raise ValueError("dynamic __has_include probing is outside the frozen include contract")
        if self._dynamic_include_context or _DYNAMIC_INCLUDE_CONTEXT.search(text) or "??" in text:
            # Line/counter-sensitive directives need a new scan after body edits.
            return _sha(source)
        return _sha("\n".join(line.strip() for line in text.splitlines()
                              if re.match(r"\s*(?:#|%:)", line)))

    def _scan_include_inputs(self, source: str) -> tuple[set[Path], dict[Path, tuple | None]]:
        """Ask the real CPP for dependencies and expanded lookup spellings once.

        -MD supplies actual inputs independently of user #line directives; -dI
        supplies macro-expanded include names; -v supplies the driver's real
        quote/bracket search order, including configured include paths.
        """
        self._input.write_text(source)
        dependency_file = self.workdir / "includes.d"
        command = self.cpp + ["-E", "-dI", "-v", "-MD", "-MF", str(dependency_file),
                              "-MT", "matchsearch", "-nostdinc", "-undef"]
        command += list(self.build.DEFINES) + ["-DNONMATCHING", "-I", "include", "-iquote", ".", str(self._input)]
        proc = subprocess.run(command, cwd=self.repo, env=clean_environment(), capture_output=True,
                              text=True, timeout=30)
        if proc.returncode:
            raise ValueError("preprocessor dependency scan failed: " + proc.stderr[-6000:])
        raw = dependency_file.read_text().replace("\\\n", "")
        dependencies = {Path(os.path.abspath(self.repo / name.replace("$$", "$")))
                        for name in shlex.split(raw.split(":", 1)[1])}
        dependencies.discard(self._input)
        quote_dirs: list[Path] = []
        bracket_dirs: list[Path] = []
        collecting = None
        for line in proc.stderr.splitlines():
            if line == '#include "..." search starts here:':
                collecting = quote_dirs
            elif line == "#include <...> search starts here:":
                collecting = bracket_dirs
            elif line == "End of search list.":
                collecting = None
            elif collecting is not None and line.startswith(" "):
                directory = line.strip()
                if directory.endswith(" (framework directory)"):
                    raise ValueError("framework includes are outside the frozen include contract")
                collecting.append(Path(os.path.abspath(self.repo / directory)))
        if not bracket_dirs:
            raise ValueError("cannot determine preprocessor include search paths")
        lookups: dict[Path, tuple | None] = {}
        physical = self._input
        for line in proc.stdout.splitlines():
            marker = re.match(r'^#\s+\d+\s+("(?:\\.|[^"\\])*")(.*)$', line)
            if marker:
                name = json.loads(marker[1])
                flags = marker[2].split()
                # Unflagged #line changes the logical name, not include lookup.
                if not name.startswith("<") and ("1" in flags or "2" in flags):
                    physical = Path(os.path.abspath(self.repo / name))
                continue
            include = re.match(r'^#\s*(include(?:_next)?)\s+([<"])([^>"]+)[>"]', line)
            if not include:
                continue
            if include[1] == "include_next":
                raise ValueError("include_next is outside the frozen include contract")
            name = include[3]
            roots = bracket_dirs
            if include[2] == '"':
                roots = [physical.parent, *quote_dirs, *bracket_dirs]
                if physical == self._input:
                    # scope's fast header cache preprocesses a stand-in here.
                    roots = [self.workdir / "cache", *roots]
            if Path(name).is_absolute():
                roots = [Path("/")]
            for directory in dict.fromkeys(roots):
                path = Path(os.path.abspath(directory / name))
                stamp = _file_stamp(path)
                lookups[path] = stamp
                if stamp is not None:
                    if path not in dependencies:
                        raise ValueError("include lookup differs from CPP dependencies: " + str(path)
                                         + "; remove headers from the private compiler work directory")
                    break
        return dependencies, lookups

    def _freeze_include_inputs(self, project_headers: list[Path]) -> list[Path]:
        self._include_environment = {key: os.environ.get(key) for key in _CPP_ENVIRONMENT}
        self._include_inventory = {Path(os.path.abspath(p)) for p in project_headers}
        self._include_signatures: set[str] = set()
        self._include_lookups: dict[Path, tuple | None] = {}
        headers, lookups = self._scan_include_inputs(self.original_source)
        extras = sorted(headers - self._include_inventory)
        self._include_inventory.update(headers)
        text = "\n".join(path.read_text(errors="replace") for path in self._include_inventory)
        if re.search(r"\b__has_include(?:_next)?\b", _without_comments(text)):
            raise ValueError("dynamic __has_include probing is outside the frozen include contract")
        self._dynamic_include_context = bool(_DYNAMIC_INCLUDE_CONTEXT.search(text))
        self._include_lookups.update(lookups)
        self._include_signatures.add(self._include_signature(self.original_source))
        return extras

    @_serialized
    def validate_source_environment(self, source: str) -> None:
        """Validate dependencies before accepting even a stored byte verdict.

        Most moves preserve preprocessing directives, so their validation is
        a small text scan and filesystem stamps, without invoking CPP again.
        """
        self.validate_environment()
        signature = self._include_signature(source)
        if signature in self._include_signatures:
            return
        headers, lookups = self._scan_include_inputs(source)
        unknown = sorted(headers - self._include_inventory)
        if unknown:
            names = ", ".join(os.path.relpath(p, self.repo) for p in unknown[:8])
            raise ValueError("candidate introduces headers outside the frozen inventory: " + names
                             + "; use a new baseline/session or fork")
        self._include_lookups.update(lookups)
        self._include_signatures.add(signature)
        self.validate_environment()

    def validate_environment(self) -> None:
        """Call at transaction boundaries; stale sessions must not reuse scores."""
        if hasattr(self, '_target_asm_input'):
            path, stamp = self._target_asm_input
            if _file_stamp(path) != stamp:
                raise RuntimeError('target assembly classification input changed; open a new search session: '
                                   + os.path.relpath(path, self.repo))
        for path, mtime, size in self._dependency_stamps:
            st = path.stat()
            if (st.st_mtime_ns, st.st_size) != (mtime, size):
                raise RuntimeError(f"compiler dependency changed; open a new search session: {os.path.relpath(path, self.repo)}")
        if hasattr(self, "build"):
            flags = tuple(self.build.cflags_for(str(self.source_path)))
            compiler = Path(self.build.compiler_for(str(self.source_path)))
            if flags != self.flags or compiler != self.production_compiler:
                raise RuntimeError("translation unit compiler/flags changed; open a new search session")
        for key, value in getattr(self, "_include_environment", {}).items():
            if os.environ.get(key) != value:
                raise RuntimeError(f"preprocessor environment changed: {key}; open a new search session")
        for path, stamp in getattr(self, "_include_lookups", {}).items():
            if _file_stamp(path) != stamp:
                raise RuntimeError("include lookup changed; open a new search session: "
                                   + os.path.relpath(path, self.repo))

    @contextlib.contextmanager
    def _scope_context(self, *, work: Path | None = None):
        # Scope's globals and environment are process-local.  Workers share files
        # only through unique directories, and in-process calls serialize here.
        with _SCOPE_LOCK:
            saved = {k: v for k, v in os.environ.items() if k.startswith(_CONTROL_PREFIXES)}
            old_work = self.scope.WORK
            old_cpp = self.build.CPP
            old_cpp_cache, old_headers, old_pre = self.scope._CPP1, self.scope._HEADS, self.scope._PRE
            for key in saved:
                del os.environ[key]
            self.scope.WORK = str(work or self.workdir)
            self.build.CPP = self.cpp
            self.scope._CPP1, self.scope._HEADS, self.scope._PRE = self._cpp_cache, self._header_cache, self._pre_cache
            try:
                yield
            finally:
                self.scope.WORK = old_work
                self.build.CPP = old_cpp
                self.scope._CPP1, self.scope._HEADS, self.scope._PRE = old_cpp_cache, old_headers, old_pre
                for key in tuple(os.environ):
                    if key.startswith(_CONTROL_PREFIXES):
                        del os.environ[key]
                os.environ.update(saved)

    @_serialized
    def preprocess(self, source: str) -> str:
        if self._closed:
            raise RuntimeError("evaluator is closed")
        self.validate_source_environment(source)
        self._input.write_text(source)
        # fast_preprocess is scope's safe header cache: macro-dependent bodies
        # fall back to real cpp, so a transformed macro call is never guessed.
        with self._scope_context():
            pre = self.scope.fast_preprocess(str(self._input), {}, self.function)
        self._last_preprocessed = pre
        return pre

    def _compiler(self, debug: bool):
        if debug not in self._fast:
            self._fast[debug] = self.fast.Fast(
                str(self.compiler), list(self.flags) + (["-g"] if debug else []),
                self.assembler, self.rom_symbols, self.address, str(self.workdir),
                fixup=self.scope.inline_macros)
        return self._fast[debug]

    def _error_details(self, pre: str, debug: bool, knobs: Mapping[str, str]) -> str:
        path = self.workdir / "failed.i"
        path.write_text(pre)
        env = clean_environment()
        env.update(knobs)
        proc = subprocess.run([str(self.compiler), *self.flags, *(["-g"] if debug else []),
                               str(path), "-o", os.devnull], cwd=self.repo,
                              env=env, capture_output=True, text=True, timeout=30)
        return proc.stderr[-8000:] or "compiler or assembler failed; inspect source and dependencies"

    def _validate_source_settings(self, source: str) -> None:
        flags_match = re.search(r"^// CFLAGS: (.*)$", source, re.M)
        flags = tuple(flags_match[1].split() + ["-fhex-asm"]) if flags_match else tuple(self.build.CFLAGS)
        compiler_match = re.search(r"^// COMPILER: (\S+)\s*$", source, re.M)
        compiler = compiler_match[1] if compiler_match else "agbcc"
        if flags != self.flags or compiler != "agbcc":
            raise ValueError("candidate changes translation-unit compiler/flags; propose C within the frozen build context")

    @_serialized
    def source_names(self, source: str) -> tuple[str, ...]:
        """Include recovered address-named leaves inside the frozen ROM span.

        Scope uses the same grouping for drafts whose inventory extent contains
        a trailing function. Keep known, independently inventoried neighbors out.
        """
        leaves = []
        for match in re.finditer(r"^[^\n;#{}]*\b(sub_([0-9A-Fa-f]{8}))\s*\([^;{]*\)\s*\{",
                                 _without_comments(source), re.M):
            name, address = match[1], int(match[2], 16)
            if self.address < address < self.address + len(self.target_code):
                leaves.append(name)
        if not leaves:
            return self.own_names
        _, addresses, _ = self.scope.inventory()
        return tuple(dict.fromkeys((*self.own_names, *(name for name in leaves if name not in addresses))))

    @_serialized
    def evaluate(self, source: str, *, preprocessed: bool = False,
                 diagnostic_knobs: Mapping[str, str] | None = None,
                 semcheck_trials: int = 0) -> Evaluation:
        if self._closed:
            raise RuntimeError("evaluator is closed")
        started = time.perf_counter()
        knobs = {str(k): str(v) for k, v in (diagnostic_knobs or {}).items()}
        if any(not k.startswith(_KNOB_PREFIXES) for k in knobs):
            raise ValueError("diagnostic_knobs accepts only AGBCC_* / CSE_* settings")
        # The server protocol is whitespace-delimited.  No arbitrary paths or
        # control bytes may become another knob or a second compiler request.
        if any(re.search(r"\s|\x00", k + v) or "=" in k for k, v in knobs.items()):
            raise ValueError("invalid diagnostic knob name/value")
        result = Evaluation("compile_error", False, False, self.function, self.address,
                            _sha(source), self.environment_hash, len(self.target_code),
                            diagnostic_knobs=knobs)
        try:
            if not preprocessed:
                self._validate_source_settings(source)
            else:
                self.validate_environment()
            pre = source if preprocessed else self.preprocess(source)
            self._last_preprocessed = pre
            own = self.source_names(pre)
            result.concessions = _concessions(pre, own)
            resolved = knobs
            if any(re.search(r"(^|,)[A-Za-z_]", knobs.get(k, "")) for k in ("AGBCC_PRI", "AGBCC_FORCE")):
                self._input.write_text(source)
                trace_work = self.workdir / "traces" / _sha(source)[:16]
                sink = io.StringIO()
                with self._scope_context(work=trace_work), contextlib.redirect_stdout(sink), contextlib.redirect_stderr(sink):
                    resolved = self.scope.resolve_knobs(str(self._input), self.function, knobs)
            result.resolved_diagnostic_knobs = dict(resolved)
            debug = bool(resolved) and self.scope.needs_g(resolved)
            compile_started = time.perf_counter()
            with self._scope_context():
                server = self._compiler(debug)
                code, error = server.bytes(pre, own, resolved)
            result.compile_ms = (time.perf_counter() - compile_started) * 1000
            self.validate_environment()
            if code is None:
                result.error = error + "\n" + self._error_details(pre, debug, resolved)
            else:
                result.code = code
                result.code_hash = _sha(code)
                result.candidate_size = len(code)
                result.byte_mismatches = byte_distance(self.target_code, code)
                same = code == self.target_code
                result.exact = same and not knobs
                result.diagnostic_match = same and bool(knobs)
                result.status = "exact" if result.exact else "diagnostic_match" if same else "compiled"
                result.data_ranges = list(server.data)
                result.rows = self.scope.decode(code, self.address, result.data_ranges)
                if same:
                    result.instruction_diff = result.shape_distance = 0
                else:
                    pairs, _ = self.scope.align(self.target_rows, result.rows)
                    result.moved_branches = sum(tag == "→" for _, _, tag in pairs)
                    result.instruction_diff = sum(tag not in {"=", "→"} for _, _, tag in pairs)
                    result.shape_distance = self.scope.shape_distance(self.target_rows, result.rows)
                if semcheck_trials:
                    result.semantic = self._semantic(pre, code, semcheck_trials)
        except (SystemExit, OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as exc:
            result.error = f"{type(exc).__name__}: {exc}"
        result.elapsed_ms = (time.perf_counter() - started) * 1000
        return result

    @_serialized
    def verify(self, source: str) -> dict[str, Any]:
        """Recompile an acceptance candidate with the uninstrumented compiler.

        This is deliberately outside the cheap loop.  Batch landing must still
        use the project's complete build, which checks unit boundaries too.
        """
        self.validate_environment()
        self._validate_source_settings(source)
        pre = self.preprocess(source)
        path = self.workdir / "verify.i"
        path.write_text(pre)
        asm_path = self.workdir / "verify.s"
        process = subprocess.run([str(self.production_compiler), *self.flags, str(path), "-o", str(asm_path)],
                                 cwd=self.repo, env=clean_environment(), capture_output=True, text=True, timeout=60)
        if process.returncode:
            return {"exact": False, "method": "production compiler", "error": process.stderr[-8000:]}
        obj, error = self.fast.assemble(self.assembler, self.scope.inline_macros(asm_path.read_text()),
                                       str(self.workdir), tag="verify")
        if obj is None:
            return {"exact": False, "method": "production compiler", "error": error}
        code, _, _, missing = self.fast.place(obj, self.source_names(pre), self.rom_symbols, self.address)
        if code is None or missing:
            return {"exact": False, "method": "production compiler", "error": "unresolved: " + ", ".join(missing)}
        self.validate_environment()
        return {"exact": code == self.target_code, "method": "production compiler + scope relocator",
                "code_hash": _sha(code), "byte_mismatches": byte_distance(self.target_code, code),
                "candidate_size": len(code), "target_size": len(self.target_code)}

    @_serialized
    def bind_call_contracts(self, contracts: Mapping[str, Any], *, baseline_source: str | None = None) -> None:
        """Freeze a session's address contracts for diagnostic execution.

        Session callers pass their stored baseline source, including on reopen.
        Standalone callers need not invoke this method: the first semantic
        check infers contracts from the constructor's original-source snapshot.
        Candidate prototypes never change the target oracle or its trial input
        model.  This binding does not change compilation or byte acceptance.
        """
        snapshot = json.loads(json.dumps(dict(contracts), sort_keys=True))
        if not isinstance(snapshot.get("arities"), dict):
            raise ValueError("frozen call contracts require an address-keyed arities mapping")
        _semantic_call_arities(snapshot, {})  # Validate before changing any state.
        source = self.original_source if baseline_source is None else baseline_source
        if not isinstance(source, str):
            raise ValueError("semantic baseline_source must be raw source text")
        binding = _sha(json.dumps(snapshot, sort_keys=True) + "\0" + _sha(source))
        previous = getattr(self, "_semantic_binding_hash", None)
        if previous is not None:
            if previous != binding:
                raise ValueError("semantic baseline contracts are frozen; use a new evaluator to rebind")
            return
        self._semantic_contracts = snapshot
        self._semantic_binding_hash = binding
        self._semantic_baseline_source = source
        self._semantic_baseline_preprocessed = None
        self._semantic_input = None

    def _semantic_baseline(self) -> str:
        """Read the frozen baseline once without replacing candidate metadata."""
        pre = getattr(self, "_semantic_baseline_preprocessed", None)
        if pre is not None:
            return pre
        source = getattr(self, "_semantic_baseline_source", self.original_source)
        previous_pre = self._last_preprocessed
        input_path = getattr(self, "_input", None)
        previous_input = input_path.read_bytes() if input_path is not None and input_path.is_file() else None
        try:
            pre = self.preprocess(source)
        finally:
            self._last_preprocessed = previous_pre
            if previous_input is not None:
                input_path.write_bytes(previous_input)
        if getattr(self, "_semantic_contracts", None) is None:
            from . import contracts
            inferred = contracts.infer_call_contracts(pre, self.rom_symbols)
            inferred["inference_version"] = _sha(Path(contracts.__file__).read_bytes())
            self.bind_call_contracts(inferred, baseline_source=source)
        expected = self._semantic_contracts.get("preprocessed_sha256")
        if expected is not None and expected != _sha(pre):
            raise ValueError("frozen call contracts do not match the supplied semantic baseline source")
        self._semantic_baseline_preprocessed = pre
        return pre

    def _semantic(self, pre: str, code: bytes, trials: int) -> dict[str, Any]:
        # Candidate declarations never choose the ROM input/callee model. Even
        # importing the native backend happens only inside the bounded child.
        if not isinstance(trials, int) or isinstance(trials, bool) or not 1 <= trials <= 10000:
            raise ValueError("semantic trials must be an integer between 1 and 10000")
        try:
            baseline = self._semantic_baseline()
        except (OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as exc:
            return {"verdict": "inconclusive", "detail": f"semantic baseline unavailable: {exc}",
                    "proof": False, "model_version": _SEMANTIC_MODEL_VERSION}
        names: dict[int, str] = {}
        for label, symbol in self.rom_symbols.items():
            address, kind = (symbol[0], symbol[1] if len(symbol) > 1 else None) if isinstance(symbol, (tuple, list)) else (symbol, None)
            if kind == "FUNC" or label.startswith(("sub_", "_call_via")):
                names.setdefault(address & ~1, label)
        arities, provenance = _semantic_call_arities(self._semantic_contracts, names)
        contracts = {
            "model": _SEMANTIC_CONTRACT_MODEL,
            "binding_sha256": self._semantic_binding_hash,
            "preprocessed_sha256": self._semantic_contracts.get("preprocessed_sha256"),
            "inference_version": self._semantic_contracts.get("inference_version"),
            "assumption": self._semantic_contracts.get("assumption", "frozen baseline declarations; not ROM-callee proof"),
            "known_addresses": len(self._semantic_contracts["arities"]),
            "unknown_declarations": len(self._semantic_contracts.get("unknown", [])),
            "unknown_policy": "all core argument registers and outgoing-stack heuristic remain observed",
            "warnings": list(self._semantic_contracts.get("warnings", [])),
        }
        try:
            engine = self.repo / 'tools' / 'semcheck.py'
            engine_hash = _sha(engine.read_bytes())
            # A private snapshot avoids depending on a mutable ROM path or on
            # another process's module cache. It is removed on success, fault,
            # and timeout, and is never part of exported candidate evidence.
            with tempfile.TemporaryDirectory(prefix='semantic-', dir=getattr(self, 'workdir', None)) as directory:
                rom_path = Path(directory) / 'rom.bin'
                rom_path.write_bytes(self.rom)
                request = {
                    'function': self.function, 'own_names': list(self.own_names),
                    'address': self.address, 'target_size': len(self.target_code),
                    'baseline_preprocessed': baseline, 'code_hex': code.hex(), 'trials': trials,
                    'names': names, 'arities': arities, 'engine_sha256': engine_hash,
                    'rom_path': str(rom_path), 'rom_sha256': _sha(self.rom),
                }
                result = _run_semantic_process(request, cwd=self.repo)
        except OSError as exc:
            result = {'verdict': 'unavailable', 'detail': f'Semantic worker inputs unavailable: {exc}',
                      'trials': 0, 'proof': False, 'model_version': _SEMANTIC_MODEL_VERSION}
        called = result.pop('observed_target_callees', [])
        contracts['observed_target_callees'] = [
            {'name': name, **provenance.get(name, {
                'register_arguments': 'unknown (all r0-r3 observed)',
                'stack_arguments': 'unknown', 'basis': 'no frozen address contract',
            })} for name in called]
        if 'input_contract' in result:
            self._semantic_input = result['input_contract']
        result.update({
            'requested_trials': trials,
            'model': 'arbitrary initial memory, disjoint pointer arguments, stubbed callees',
            'call_contracts': contracts,
            'pruning': 'counterexample requires validation against function preconditions, input model, and declared callee contracts',
        })
        return result

    @_serialized
    def diagnose(self, source: str, *, limit: int = 24, names: list[str] | None = None) -> dict[str, Any]:
        """Lazy compiler facts.  They describe a debug compile, not match proof.

        Full dumps remain under the returned directory.  The important condition
        is ``debug_changes_code``: when true, source/UID annotations must not be
        silently treated as annotations of the production instruction stream.
        """
        if limit < 0:
            raise ValueError("diagnostic limit must be nonnegative")
        production = self.evaluate(source)
        if production.error:
            return {"error": production.error, "production": production.to_dict()}
        self._input.write_text(source)
        output = io.StringIO()
        trace_work = self.workdir / "traces" / _sha(source)[:16]
        with self._scope_context(work=trace_work), contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            try:
                code, _, info = self.scope.compile_draft_g(str(self._input), self.function, {})
                wd = info["wd"]
                allocations = self.scope.alloc_records(wd, self.function)
                pseudo_names = self.scope.pseudo_names(wd, self.function)
                conflicts = self.scope.conflicts(wd)
                reloads = self.scope.reload_records(wd, self.function)
                states = self.scope.spill_states(wd, self.function)
                inherited = self.scope.inherit_records(wd, self.function)
                pre = self.scope.pre_records(wd, self.function)
                rows = []
                for pseudo, rec in allocations.items():
                    pc, hc = conflicts.get(pseudo, (set(), set()))
                    row = {"pseudo": pseudo, "name": pseudo_names.get(pseudo),
                           "allocator": "global" if rec[0] == "G" else "local",
                           "priority": rec[2], "hard_register": rec[6],
                           "pseudo_conflicts": sorted(pc), "hard_conflicts": sorted(hc)}
                    if rec[0] == "G":
                        row.update(rank=rec[1], refs=rec[3], live_length=rec[4], calls_crossed=rec[5])
                    else:
                        row.update(block=rec[1], birth=rec[3], death=rec[4], refs=rec[5])
                    rows.append(row)
                is_named = lambda r: bool(r["name"]) and not r["name"].startswith("=")
                rows.sort(key=lambda r: (not is_named(r), r["allocator"], r.get("rank", r.get("block", 0)), r["pseudo"]))
                full_path = Path(wd) / "feedback.json"
                full_path.write_text(json.dumps({"allocations": rows, "reloads": reloads, "spill_states": states,
                                                "inherited_reloads": inherited, "pre": pre}, indent=2))
                selected = rows if names is None else [r for r in rows if r["name"] in names or str(r["pseudo"]) in names]
                compact = []
                for row in selected[:max(0, limit)]:
                    row = dict(row)
                    pc = row.pop("pseudo_conflicts")
                    row["pseudo_conflict_count"] = len(pc)
                    row["pseudo_conflict_sample"] = pc[:8]
                    compact.append(row)
                return {"production": production.to_dict(), "debug_changes_code": code != production.code,
                        "annotation_authority": "debug compile only" if code != production.code else "same bytes as production",
                        "directory": os.path.relpath(wd, self.repo),
                        "full_feedback": os.path.relpath(full_path, self.repo),
                        "allocation_count": len(rows), "named_allocation_count": sum(map(is_named, rows)),
                        "allocation_rows_omitted": len(selected) - len(compact), "allocations": compact,
                        "missing_names": sorted(set(names or []) - {str(r["pseudo"]) for r in selected}
                                                - {r["name"] for r in selected if r["name"]}),
                        "reload_count": len(reloads), "reloads": [
                            {"event": r[0], "uid": r[1], "register": r[2], "line": r[3],
                             "state": states.get(r[0])} for r in reloads[:limit]],
                        "inherited_reloads": inherited[:limit], "pre_count": len(pre), "pre": pre[:limit],
                        "compiler_sources": self.compiler_references(), "messages": output.getvalue()[-4000:]}
            except (SystemExit, OSError, ValueError, RuntimeError) as exc:
                return {"error": f"{type(exc).__name__}: {exc}", "messages": output.getvalue()[-4000:],
                        "production": production.to_dict()}

    def compiler_references(self) -> dict[str, dict[str, Any]]:
        refs = {}
        for key, filename, token in (
                ("allocation_priority", "global.c", "allocno_compare (v1p, v2p)"),
                ("allocation_constraints", "global.c", "find_reg (allocno, losers"),
                ("unsafe_forcing", "global.c", "force_allocno (allocno, regno)"),
                ("reload_choice", "reload1.c", "allocate_reload_reg (chain, r, last_reload"),
                ("pre", "gcse.c", "pre_delete ()"),
                ("liveness", "flow.c", "life_analysis (f, nregs, file)")):
            path = self.compiler.parent / filename
            if path.is_file():
                text = path.read_text(errors="replace")
                index = text.find(token)
                refs[key] = {"path": os.path.relpath(path, self.repo),
                             "line": text.count("\n", 0, index) + 1 if index >= 0 else None,
                             "symbol": token.split(" (")[0]}
        return refs

    @_serialized
    def close(self) -> None:
        if self._closed:
            return
        with self._scope_context():
            for server in self._fast.values():
                server.close()
        self._closed = True

    def __enter__(self):
        return self

    def __exit__(self, *_):
        self.close()


if __name__ == '__main__':
    if sys.argv[1:] != ['--semantic-worker']:
        raise SystemExit('private evaluator worker; use python -m matchsearch for the public interface')
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    try:
        import resource
        resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    except (ImportError, OSError, ValueError):
        pass
    try:
        # Keep the worker's transport valid even if an imported tool reports a
        # warning to stdout. Native signals remain visible to the parent.
        with contextlib.redirect_stdout(io.StringIO()):
            result = _semantic_worker(json.load(sys.stdin))
    except (Exception, SystemExit) as exc:
        result = {'verdict': 'unavailable', 'detail': f'{type(exc).__name__}: {exc}',
                  'trials': 0, 'proof': False, 'model_version': _SEMANTIC_MODEL_VERSION}
    print(json.dumps(result), flush=True)
