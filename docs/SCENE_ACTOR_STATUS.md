# SceneActor_UpdateStatus — exact status update reconstruction

Tags: #matching #scene-actor #compiler #regalloc #08078EE8

`SceneActor_UpdateStatus` at **0x08078EE8** is an ordinary C exact match: all **5,956 bytes** agree with the ROM. It advances route and attachment countdowns, updates collision placement and environmental effects, manages palette flashes/blinks, applies reaction transitions, releases contacts, emits particles, checks recovery routing, updates target facing, and copies heading to both colliders. It returns `s32` value `1`; both existing caller declarations now reflect that contract.

## Recovered source structure

The draft was a starting hypothesis. ROM inspection corrected the unstable-state flag field, effect subtype mask, death-control pointer, death-path state/control ordering, flash-expiry return, signed timers, palette-sum promotion, and actual route/output fields. The implementation uses the canonical `SceneActorRegistryResetView` and shared chase geometry, flag, control, and state operations. Temporary duplicate actor/group/terrain layouts were removed; the migration passed 385 target-compiler layout assertions.

The source boundaries that resolved the difficult compiler differences have ordinary purposes:

- Collision, reaction, and recovery each obtain the group view belonging to their actor operation. This replaces the draft's unnecessarily long-lived group argument.
- A scene-environment value snapshot supplies normalized mode and cached solar level, with real consumers of both fields. Its optimized branch explains the otherwise unused mode-mask instruction without assembly or artificial equal branches.
- Palette phases own separate countdown values; category-specific recovery actions have separate branches; online player input is captured at its actual call. These reproduce register lifetimes and late branch sharing.
- The wake operation captures a read-only registry identity for its later defeat record. Direct signed `selection |= -1` reproduces the narrow constant spill without an explicit stack object.
- The motion query retains its real halfword member and `u16` mask through both interfaces, reusing the independently matched shared flag component. All final normalized status/behavior query results are consumed as conditions, preserving their meaning.

The final address spill order followed from actor/registry input ownership and these real predicate interfaces. No source stack offsets, allocation overrides, dummy inputs, register pins, inline assembly, or barriers are used.

## Final acceptance and compiler evidence

The renamed, formatted receiving source was checked in a fresh context. Ordinary evaluation, independent stock compilation, and a production full translation-unit link all produced zero differing bytes. Direct comparison with the ROM and separately captured zero-warning compilation agree on this SHA-256:

```text
bb9ae48e4f155462fbce705cb75c2d19b12cc3c7f459e71b80bb9c4a6903cc0b
```

The checked source SHA-256 is `fcc5ae6df54d52d69ea48a2d91288d57bb8042c4245ec181bdf852ed913aaa76`. Its byte-neutral diagnostic trace records **2,667 real instructions before GCSE**, **1,333 expression buckets**, and these three PRE-generated address spills:

| Source address role | Reaching pseudo | Stack slot | Reload store UID |
|---|---:|---:|---:|
| Actor group, actor + 0x3D4 | 4627 | 0x3C | 10827 |
| Actor motion flags, actor + 0x2D4 | 4635 | 0x40 | 10835 |
| Registry identity motion flags, identity + 0x2D4 | 4636 | 0x44 | 10826 |

The same instruction UIDs were checked before and after reload; `debug_changes_code=false` makes this trace evidence about the accepted production bytes. These facts explain this source/compiler combination, not a unique historical source.

Compact acceptance and trace data are in `build/round4/dispatch/final-gate-summary.json`; full-TU details are in `final-full-tu.json`, with object/ELF/binary artifacts under the recorded directory. `final_receiving_gate.py` and `final_full_tu_gate.py` reproduce the checks. Because the frozen baseline ELF still had the old name, the full-TU gate used the checker's existing explicit-address linker API after confirming the new CSV entry and old ELF symbol agree at 0x08078EE8. No shared ELF was changed. The integrated batch also passes the combined whole-ROM build and RAM-literal check.
