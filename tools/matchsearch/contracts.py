"""Conservative call-register contracts from the frozen baseline translation unit.

These are declared ABI assumptions, not facts proved from ROM callees.  The
caller must reuse this result for both target and candidates.  Unknown calls
retain the analyzer's default r0-r3 dependencies and arbitrary memory effects.

Counting C parameters is insufficient for aggregate results and multiword
arguments.  We reduce dependencies only for explicit, nonvariadic prototypes
whose arguments are pointers or ordinary <=32-bit integer values under agbcc's
32-bit ARM ABI.  Other signatures deliberately remain unknown.
"""

from __future__ import annotations

import hashlib
from collections import defaultdict
from pathlib import Path
import re
from typing import Any, Mapping


_TOKENS = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*', re.S)
_INCLUDE_ASM = re.compile(
    r'^\s*(?:__asm__|asm)\s*\(\s*"\.include (?:\\.|[^"\\])*"\s*\)\s*;[ \t]*$', re.M)
_INTEGER_WORDS = {"char", "short", "int", "long", "signed", "unsigned", "_Bool"}


def _blank(text: str) -> str:
    return "".join("\n" if char == "\n" else " " for char in text)


def _resolve(tp: Any, typedefs: Mapping[str, Any], ca: Any,
             seen: frozenset[str] = frozenset()) -> tuple[Any, str | None]:
    """Resolve only outer typedefs; pointer pointees do not affect word count."""
    reason = "type attributes require ABI review" if getattr(tp, "gcc_attributes", None) else None
    if isinstance(tp, ca.TypeDecl):
        resolved, nested_reason = _resolve(tp.type, typedefs, ca, seen)
        return resolved, reason or nested_reason
    if isinstance(tp, ca.IdentifierType) and len(tp.names) == 1:
        name = tp.names[0]
        if name in typedefs:
            if name in seen:
                return tp, "cyclic typedef"
            decl = typedefs[name]
            if getattr(decl, "gcc_attributes", None):
                reason = reason or "typedef attributes require ABI review"
            resolved, nested_reason = _resolve(decl.type, typedefs, ca, seen | {name})
            return resolved, reason or nested_reason
    return tp, reason


def _kind(tp: Any, typedefs: Mapping[str, Any], ca: Any, *, parameter: bool) -> str:
    tp, reason = _resolve(tp, typedefs, ca)
    if reason:
        return reason
    if isinstance(tp, ca.PtrDecl):
        return "word"
    if parameter and isinstance(tp, (ca.ArrayDecl, ca.FuncDecl)):
        return "word"  # C adjusts array/function parameters to pointers.
    if isinstance(tp, ca.Enum):
        return "word"
    if isinstance(tp, ca.IdentifierType):
        words = list(tp.names)
        if words == ["void"]:
            return "void"
        if words and set(words) <= _INTEGER_WORDS and words.count("long") <= 1:
            return "word"
    return "unsupported or multiword ABI type"


def _prototype(decl: Any, fn_type: Any, typedefs: Mapping[str, Any], ca: Any
               ) -> tuple[int | None, str | None, int | None]:
    if "static" in (decl.storage or []):
        return None, "static declaration cannot be tied to a global symbol by name alone", None
    if getattr(decl, "asmlabel", None) is not None:
        return None, "assembler-renamed declaration requires symbol resolution", None
    if getattr(decl, "gcc_attributes", None):
        return None, "function attributes require ABI review", None
    if fn_type.args is None:
        return None, "old-style empty parameter list is not a prototype", None
    params = fn_type.args.params or []
    if any(isinstance(param, ca.EllipsisParam) for param in params):
        return None, "variadic prototype retains all argument-register dependencies", None
    if not params or any(not isinstance(param, (ca.Decl, ca.Typename)) for param in params):
        return None, "old-style or untyped parameters", None
    return_kind = _kind(fn_type.type, typedefs, ca, parameter=False)
    if return_kind not in {"word", "void"}:
        return None, "return type may require a hidden argument: " + return_kind, None
    kinds = [_kind(param.type, typedefs, ca, parameter=True) for param in params]
    if kinds == ["void"] and getattr(params[0], "name", None) is None:
        return 0, None, 0
    for param, kind in zip(params, kinds):
        if getattr(param, "gcc_attributes", None):
            return None, "parameter attributes require ABI review", len(params)
        if kind != "word":
            return None, "parameter type: " + kind, len(params)
    return min(4, len(params)), None, len(params)


def _symbol(value: Any) -> tuple[int | None, str | None]:
    kind = None
    if isinstance(value, (tuple, list)):
        if not value:
            return None, "empty symbol record"
        if len(value) > 1 and isinstance(value[1], str):
            kind = value[1]
        value = value[0]
    if not isinstance(value, int) or isinstance(value, bool) or not 0 <= value <= 0xFFFFFFFF:
        return None, "invalid symbol address"
    if kind is not None and kind not in {"FUNC", "NOTYPE"}:
        return value & ~1, "symbol is not a function"
    return value & ~1, None


def infer_call_contracts(preprocessed: str, rom_symbols: Mapping[str, Any]) -> dict[str, Any]:
    """Return ``arities``, per-callee ``provenance``, ``unknown``, and warnings.

    ``rom_symbols`` accepts symbol -> integer address, or the project's
    symbol -> (address, ELF type) tuples.  Keys in ``arities`` match the scope
    disassembler exactly, e.g. ``#0x81a6e9c``; Thumb marker bits are cleared.
    An arity means the leading core argument registers consumed, capped at
    four.  Stack and memory dependencies must remain conservative regardless.

    Parse failure returns no reductions.  Calls, function-pointer objects,
    block-scope declarations, and unmapped names never create contracts.
    Conflicting or unknown declarations sharing one address veto a reduction.
    The input hash permits the session to freeze and audit the assumed source.
    """
    result: dict[str, Any] = {
        "arities": {}, "provenance": {}, "unknown": [], "warnings": [],
        "preprocessed_sha256": hashlib.sha256(preprocessed.encode()).hexdigest(),
        "assumption": "Frozen baseline declarations and agbcc 32-bit ARM ABI; not ROM-callee proof.",
    }
    # Share the project's GNU C frontend without reparsing for every candidate.
    from .moves import _load_parser
    try:
        ca, (parser_cls, _), parser_kind = _load_parser(Path(__file__).resolve().parents[2])
        result["parser"] = parser_kind
        parse_text = _INCLUDE_ASM.sub(lambda match: _blank(match.group()), preprocessed)
        parse_text = _TOKENS.sub(lambda match: _blank(match.group())
                                 if match.group().startswith(("/*", "//")) else match.group(), parse_text)
        ast = parser_cls().parse(parse_text, filename="baseline.c")
    except Exception as exc:
        result["warnings"].append(f"Prototype parse failed; all calls retain unknown arity: {exc}")
        return result

    typedefs: dict[str, Any] = {}
    records: dict[str, list[dict[str, Any]]] = defaultdict(list)
    for node in ast.ext:
        if isinstance(node, ca.Typedef):
            typedefs[node.name] = node
            continue
        definition = isinstance(node, ca.FuncDef)
        decl = node.decl if definition else node
        if not isinstance(decl, ca.Decl) or not decl.name or decl.name not in rom_symbols:
            continue
        fn_type, type_reason = _resolve(decl.type, typedefs, ca)
        if not isinstance(fn_type, ca.FuncDecl):
            continue
        address, symbol_reason = _symbol(rom_symbols[decl.name])
        callee = f"#0x{address:x}" if address is not None else None
        arity, reason, count = _prototype(decl, fn_type, typedefs, ca)
        reason = type_reason or symbol_reason or reason
        record = {
            "symbol": decl.name, "callee": callee,
            "location": str(decl.coord) if decl.coord else None,
            "declaration_kind": "definition" if definition else "prototype",
            "parameter_count": count, "register_arguments": arity if reason is None else None,
            "reason": reason,
        }
        if callee is None:
            result["unknown"].append(record)
        else:
            records[callee].append(record)

    for callee, declarations in sorted(records.items()):
        arities = {item["register_arguments"] for item in declarations}
        counts = {item["parameter_count"] for item in declarations}
        if None in arities or len(arities) != 1 or len(counts) != 1:
            reasons = sorted({item["reason"] for item in declarations if item["reason"]})
            if len(arities - {None}) > 1 or len(counts - {None}) > 1:
                reasons.append("conflicting prototype parameter counts at one address")
            result["unknown"].append({"callee": callee, "reasons": reasons,
                                      "declarations": declarations})
            continue
        arity = next(iter(arities))
        result["arities"][callee] = arity
        result["provenance"][callee] = {
            "register_arguments": arity,
            "symbols": sorted({item["symbol"] for item in declarations}),
            "declarations": declarations,
            "basis": "baseline explicit prototype; one core word per parameter",
        }
    return result
