# External research review and next experiments

Reviewed 2026-10-08 against the local U33J ROM and generated assembly. This
review adds a concrete script-system lead: Boktai 3 has the same 16-bit name
hash, several command IDs, and closely corresponding parser/dispatcher
structures as Metal Gear Solid's reconstructed GCL library. This supports
comparing that subsystem. It does **not** establish shared rendering,
collision, scheduler, or whole-engine ancestry.

No matching C, symbols, build files, or generated assembly were changed in
this review. No ROM build or emulator replay was run. The small read-only
audit tool described below passed seven synthetic tests and was exercised on
three local tables and one bounded bytecode example. Raw outputs and external
checkouts remain under ignored `build/process-review/`.

## Verdict on the supplied research

| Claim | Assessment and consequence |
|---|---|
| Boktai shares a Castlevania / GBA Zone of the Enders engine | Unsupported by the supplied citations or the primary source code examined here. Shared OAM registers and similar loops are not distinctive evidence. Do not put this in worker instructions. |
| Register swaps indicate ARM SDT / ADS compilation | Unsupported inference. The existing exact agbcc matches and controlled compiler probes are stronger evidence. A register allocation miss alone identifies neither compiler nor calling convention. The ongoing, independently verified libgcc compiler-variant work is a useful example of the evidence required. |
| Boktai uses a custom bytecode interpreter | Supported by actual decoder and dispatch code, independently of the historical narrative. Its existence was already known locally; SolDec provides specific formats to compare. |
| A large switch or pointer loop must be the interpreter | False as a classification rule. Require byte-stream progression, decoded operand types, dispatch destinations, and callers. `0821AF2C` meets this standard; arbitrary switches do not. |
| Cartridge constraints caused most dungeon logic to be scripted | Not established by these sources. Script bytes are not matched native code bytes and cannot count toward the current native matching goal. |

The [translated 2003 Nintendo Dream interview](https://shmuplations.com/boktai/)
describes design, production, and team size; it does not substantiate the
compiler or engine-sharing claims. This is an interview translation, not
binary or original-source evidence for those technical assertions.

## Pinned external references

* [SolDec README](https://github.com/Prof9/SolDec/blob/0d93dbc12ad61826a7fac5b88d1726caadf485d8/README.md),
  revision `0d93dbc12ad61826a7fac5b88d1726caadf485d8`: explicitly tested only
  with Boktai 1. Its example offset belongs to that game, not Boktai 3.
* [SolDec reader](https://github.com/Prof9/SolDec/blob/0d93dbc12ad61826a7fac5b88d1726caadf485d8/InstructionReader.cs)
  and [instruction model](https://github.com/Prof9/SolDec/blob/0d93dbc12ad61826a7fac5b88d1726caadf485d8/Instruction.cs):
  useful format hypotheses, incomplete control support and fixed RAM bases.
* FoxdieTeam `mgs_reversing`, revision
  `f54dbb2a58adfc2755403296c9ebb653fbec277b`:
  [basic commands](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/basic.c),
  [game commands](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/game/script.c),
  [name hash](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgv/strcode.c),
  [parser](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/parse.c),
  [dispatcher](https://github.com/FoxdieTeam/mgs_reversing/blob/f54dbb2a58adfc2755403296c9ebb653fbec277b/source/libgcl/command.c).
  These are another project's reconstructed source, not recovered original
  Konami source. No external source was copied into matching units.

## Concrete Boktai 3 evidence

`sub_08219BD8` starts at zero, rotates its 16-bit accumulator left five bits,
adds an unsigned byte, truncates to 16 bits, and continues to NUL. This is the
algorithm in MGS `GV_StrCode`. Multiple independent correspondences make this
more useful than an isolated short hash collision:

* `0821ABE8` registers a linked command-table node; `0821AC30` searches nodes
  containing `{next, count, entries}` and entries `{id, function}`.
* `0821AC6C` reads a little-endian command ID, finds the native callback, sets
  up its argument/keyword pointers, calls it, and restores the keyword stack.
* `0821AF2C` dispatches high-nibble `0x30` to expression evaluation, `0x60`
  to native commands, and `0x70` to procedure calls, advancing by the decoded
  payload length. This closely corresponds to GCL's block executor, with
  different byte encoding details.
* `0821B738` follows condition/block pairs and `e`/`i` alternatives, matching
  the structure of MGS's `if` command. `0821B7B0` compares `c` and `d` keyword
  cases. `0821B82C` stores an optional result and returns the interpreter's
  stop value, 1.

The six-entry table at `0861418C` is registered by `0821B90C`. Hash candidates
and observed behavior are separated below; these are proposals for later
symbol review, not changes to `symbols/functions.csv`.

| ID | Native target | Hash candidate | Static behavior observed |
|---|---|---|---|
| `0D86` | `0821B794` | `if` | Selects a block via `0821B738`, executes it if non-null. |
| `4A6F` | `0821B7B0` | `switch` | Compares `c` case values and accepts `d` default. |
| `64C0` | `0821B80C` | `eval` | Decodes a value and stores the VM result. |
| `121F` | `0821B8AC` | `call` | Collects procedure arguments and invokes `0821AD08`. |
| `CD3A` | `0821B82C` | `return` | Stores optional result and returns 1. |
| `B96E` | `0821B858` | `print` | Processes string operands; **visible output not established**. |

The eight-entry table at `08E8791C`, registered by `082257E4`, adds hash
candidates `mesg→082255BC`, `chara→08225560`, `trap→08225624`,
`load→08225508`, `map→08225620`, and `restart→082257A0`; IDs `B745` and
`0BB3` remain unnamed here. SolDec itself leaves `B745` unnamed. The `map`
target is only `bx lr`, so its hash is emphatically not proof that it loads a
map. `08225504`, the `0BB3` target, returns zero. Neither short target has a
generated function-start directive in the reviewed assembly.

The `chara` candidate at `08225560` consumes a script value and looks it up
through `08225884` / `08225844`. Those search the table at `08603300`, whose
end is found by `0822580C` using a null callback. A local bounded scan finds
**722 entries, 722 distinct Thumb targets**, ending at the null record at
`08604990`. Thirty targets lack generated function-start directives. That is
a boundary-audit queue, not thirty confirmed new unmatched functions: first
check current source, aliases, progress accounting, and tail-sharing.

## SolDec compatibility boundaries

The following are verified against Boktai 3 code, rather than assumed from
SolDec:

* `0821A66C`: lengths 0–12 are inline in the low nibble; D/E/F select 1/2/3
  following little-endian bytes. Length is relative to the returned payload
  pointer, not the opcode address.
* `0821A6F0`: compact integers C0–FF yield `(opcode & 0x3F) - 1`; the scalar
  widths and parameter/variable families agree with SolDec's main scheme.
* `0821B2B0`: memory selectors choose pointers held at `02000710`, `02000700`,
  or `02000708`. Do **not** copy SolDec's fixed Boktai 1 RAM bases.
* B3 operand `07` is length-prefixed inline text, and `0E` calls
  `Text_LookupString` with a two-byte ID. SolDec rejects both rather than
  decoding them.
* B3 `0821A6C0` reads a separate one/two-byte big-endian 15-bit command
  argument/keyword offset. This is distinct from the compact envelope length.

SolDec discards the decoded envelope lengths while recursively reading,
throws on unsupported control/keyword forms, and its CLI silently catches
exceptions. Its loops can also fail to terminate on truncated input because
EOF yields `Invalid`, not an end instruction. Therefore partial C-looking
output is not a completeness or round-trip check. A B3 adaptation should
preserve unknown payloads, record exact byte spans, bound nesting and reads,
and report failures explicitly before attempting pretty printing.

## Ranked follow-up experiments

1. **Join native command and actor registries to script locations.** Start
   with the two registered command tables and the 722-target registry above.
   Add an opt-in trace at native dispatch `0821AC6C`, capturing frame, segment,
   bytecode pointer, command ID, resolved native target, and call nesting.
   Add a trace at `08225560` for the resolved registry key/target. Replay the
   existing new-game and menu scenarios. Success: at least one observed chain
   from a named debug-label-bearing script span to an actual native callback,
   with arguments and a screenshot. This provides context for matching large
   actor functions that first-reader coverage alone cannot identify.
2. **Adapt a bounded B3 decoder before a decompiler.** Start from known script
   blocks, distinguish native command hashes from numeric procedure IDs, add
   the B3 memory/text differences, preserve unknown spans and all length
   fields, and require byte-exact decode/re-encode. Test a condition, switch,
   string reference, and nested procedure call with runtime traces. Report
   decoded bytes and unsupported spans separately. A script map can then
   expose which callbacks implement scenes, flags, or transitions; neither
   successful parsing nor a debug string alone proves their behavior.
3. **Audit missing registry targets in an isolated boundary pass.** The 30
   actor entries plus two command targets give evidence-backed seeds. Inspect
   each target's ARM/Thumb bit and instructions, callers, existing aliases,
   and preserved data tails. Regenerate only outside active workers; full
   build and shift tests must pass. Record denominator changes separately
   from matching gains. This is more directed than another blind ROM scan.
4. **Use the command map to design coverage, then matching batches.** Choose
   a script path with an unobserved registry target, reproduce the necessary
   scene through ordinary inputs, and mark transition and steady segments
   separately. Measure newly executed native bytes, retained near-matches,
   and subsequently matched bytes per bounded batch. Use already available
   `watch`/`dump` before adding a general debugger. A read-only script trace
   should precede warps or flag mutation; arbitrary scene jumps can bypass
   initialization and produce misleading behavior.
5. **Keep compiler escalation controlled.** For a near-match family, retain
   its source, flags, compiler revision, assembly diff, and best permuter
   result. Compare variants on several positive controls and the actual
   residuals. The active libgcc work already follows this approach. Do not
   spend a worker budget acquiring ADS merely because registers differ.

The first two experiments can improve semantic context without prematurely
renaming hundreds of functions. None has a measured native-byte throughput
gain yet. Judge them by successful callback attribution and the matches they
enable, not by the amount of generated script text.

## Reproduce the small audit

`tools/soldec_audit.py` is an independent read-only audit, not a port of
SolDec or GCL. All CLI offsets are **file offsets**, unlike the bus addresses
in the tables above. It accepts explicitly bounded tables and packet streams,
retains colliding hash candidates, and never edits symbols. Its packet mode
skips opaque payloads: it is not a recursive parser or a semantic validator.

```sh
build/venv/bin/python -m unittest discover -s tests/tools -p 'test_soldec_audit.py'
build/venv/bin/python tools/soldec_audit.py hash if switch eval call return print
build/venv/bin/python tools/soldec_audit.py table baserom.gba \
  --offset 0x61418c --count 6 --word if --word switch --word eval \
  --word call --word return --word print --asm gen/code_sym.s
build/venv/bin/python tools/soldec_audit.py table baserom.gba \
  --offset 0xe8791c --count 8 --asm gen/code_sym.s
build/venv/bin/python tools/soldec_audit.py table baserom.gba \
  --offset 0x603300 --count 722 --asm gen/code_sym.s
build/venv/bin/python tools/soldec_audit.py packets baserom.gba \
  --start 0xdd6ba3 --end 0xdd6d55
```

The last example is the payload of the bounded block envelope at `08DD6BA0`:
six top-level records, including native ID `4A6F`, end exactly at the supplied
boundary after a zero marker. This tests length handling only. Its nested
switch body remains opaque, and this pass did not demonstrate runtime
execution of that block. Save ROM-derived JSON only under ignored `build/`.

## Orchestrator integration validation

The parent reproduced the builtin-table and bounded-envelope audits, reviewed
the seven synthetic tests, and included this ROM-free tool in the normal test
suite. The surrounding batch passed a complete bit-identical ROM build and
all 35 shift-test screenshots. The audit does not modify the ROM or symbols.
