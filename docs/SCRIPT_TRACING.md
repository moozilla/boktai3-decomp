# Script calls in a running game

The MGS comparison in [PROCESS_REVIEW.md](PROCESS_REVIEW.md) and the matching
work in [MGS_GCL_MATCHING.md](MGS_GCL_MATCHING.md) identify a useful observation
chain: script command ID → registered native handler → actor registry key →
callback. The harness can now record that chain during an ordinary replay.
This is evidence of specific calls, not proof of an actor's semantic name or
of shared ownership of the entire engine.

## Record and join a replay

Build the harness once with `make -C tools/emu` when no worker is using the
shared executable. Use the verified Japanese U33J ROM. From the repository:

```sh
mkdir -p build/script-trace
cat tests/script_probes.txt tests/newgame_intro.txt > build/script-trace/replay.txt
tools/emu/harness baserom.gba build/script-trace/replay.txt build/script-trace/intro
build/venv/bin/python tools/script_trace.py build/script-trace/intro/probes.tsv \
  --rom baserom.gba > build/script-trace/intro/calls.json
```

For a save replay, set `BOKTAI3_SAV=tests/saves/ShinBok2.sav`. Existing `mark`
commands label events; insert a mark before an interval when recording a new
human playthrough. The Centaur recorder's input scripts can be replayed through
the same harness. The probes are read-only and do not change game memory.

The generic command `probe NAME ADDR` records registers **before** executing
the even hexadecimal instruction address. `unprobe` clears the active probes.
There can be at most 32 active probes; names use letters, digits, `_`, `-` or
`.` and are shorter than 64 characters. Probes still run with `trace off`,
which disables coverage. A trace exceeding 100,000 rows fails explicitly;
narrow the replay rather than treating a truncated run as complete.

`probes.tsv` contains an increasing sequence number, frame, segment, probe
name, instruction PC, execution mode and `r0` through `r15`. Register and PC
values are hexadecimal; sequence and frame are decimal. `r15` is mGBA's
prefetched register value, not the normalized instruction PC column. Raw
traces, joined JSON, screenshots and dumps are ROM-derived local evidence and
must remain under ignored `build/`.

## Sites and checks

| Probe | Address | Evidence collected |
|---|---|---|
| `native_enter` | `0821AC6C` | script ID pointer in r0, entry stack pointer |
| `native_lookup` | `0821AC7C` | decoded command ID in r0 |
| `native_call` | `0821AC9E` | argument pointer r0, Thumb callback r1, table entry r5 |
| `native_return` | `0821ACA2` | native handler's raw return r0 |
| `actor_key` | `0822556A` | registry key passed in r0 |
| `actor_resolved` | `0822556E` | resolved callback or null in r0 |
| `actor_call` | `0822557E` | callback r4, arguments r0 and r1 |
| `actor_return` | `08225582` | callback's raw return r0; the handler ignores it |

The joiner maintains a nested native-call stack. It rejects wrong probe sites
or execution modes, duplicate/out-of-order stages, mismatched stack pointers
and unfinished calls. All three later native sites must have entry SP minus
16. Actor sites must occur inside the verified `chara` handler `08225560`, and
their stack pointer must remain stable. Null actor lookup is a distinct
`missing` result, not a callback. Traces must start and end outside these
calls; a mid-call capture is intentionally rejected. Unknown extra probe
names are ignored after sequence validation.

With `--rom`, the joiner checks command IDs whose script pointer lies in ROM,
resolved command entries that lie in ROM, and all actor lookup results against
the bounded 722-entry registry at `08603300–08604990`. It verifies that registry's
strictly increasing keys, Thumb callbacks and null sentinel. JSON marks each
successful ROM check explicitly. RAM script/table contents are not verified
by this option. A successful join proves consistency of these observed sites;
it is not a complete bytecode decoder, branch trace or argument-type analysis.

## Validated intro observations (2026-10-09 UTC)

The unchanged `tests/newgame_intro.txt` replay produced **698 completed native
calls**, **90 actor lookups and completed callbacks**, **10 distinct native
targets**, and **40 distinct actor targets**. Maximum zero-based native-call
depth was 9 (ten calls simultaneously active). All 698 command IDs and command
table entries, and all 90 actor resolutions, agreed with the ROM. No null actor
lookup occurred in this replay.

| Command ID | Observed calls |
|---|---:|
| `B745` | 282 |
| `0D86` (`if` correspondence) | 247 |
| `9906` (`chara` correspondence) | 90 |
| `4A6F`, `B96E`, `CD3A` | 22 each |
| `121F` | 6 |
| `C8BB` | 4 |
| `22FF` | 2 |
| `D4CB` | 1 |

For example, frame 101 resolves registry key `1B77` to callback `08042314`,
called with r0=`1B77`, r1=0. This establishes an observed callback and its
arguments; it does not identify the on-screen object. The next useful step
is comparing marked scenes and inspecting that callback's state changes.

All 16 intro screenshots were byte-identical to the pre-probe harness output.
The corrected harness also passed all three data-shift scenarios, **35 identical
screenshots** at a 64 KiB shift. Local evidence is in
`build/endurance/root/probe-newgame-final/` and
`build/endurance/root/shift-probe-harness.log`. Synthetic joiner tests run in
the standard `tests/tools` suite and exercise nesting, null lookup, ROM
mismatches, wrong SP/sites and missing/duplicate events.

## Execution sampling correction

Adding the probes exposed an existing execution-coverage offset error. mGBA
0.10.5's `ARMRun` processes events before an instruction, and its Thumb/ARM
step advances the prefetched PC. Between steps the next instruction is r15−2
in Thumb mode or r15−4 in ARM mode. During a memory hook the executing
instruction is r15−4 or r15−8 instead. The old execution sampler reused the
memory-hook formula. The memory-hook formula itself remains unchanged.

Pending events must also be drained before sampling: an IRQ can redirect
execution. Sampling before that drain produced phantom duplicate probes.
A 110-frame control replay with coverage off recorded 223 events at each of
the native sites and 15 at each actor site. Additional probes after the entry
push and stack allocation confirmed SP changes of −12 then −4. After both
corrections, entries, calls and returns balance in the control and full intro.

This follows the primary mGBA implementation:
[ARMRun and step functions](https://github.com/mgba-emu/mgba/blob/0.10.5/src/arm/arm.c)
and [GBA core step](https://github.com/mgba-emu/mgba/blob/0.10.5/src/gba/core.c).
Validated locally against Homebrew mGBA `0.10.5_2`. Recheck sampling when
upgrading mGBA's internal API. **Regenerate older `.exec` files before making
entry-level claims**; previous set-membership reports may contain adjacent
instruction attribution errors. Screenshots and memory-hook pointer evidence
are unaffected by this correction. No names were promoted from old coverage.

During parallel matching, a private harness can be selected for shift tests
with `BOKTAI3_HARNESS=/absolute/path/to/harness`; this avoids rebuilding the
executable that workers share read-only.
