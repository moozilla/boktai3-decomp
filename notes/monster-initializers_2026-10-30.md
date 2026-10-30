# Monster initializer family: 17 exact matches

Tags: #matching #monster #compiler #structs #ram #data

All 17 initializers in the first block of the supplied Round 6 queue match in
ordinary C: 67,936 bytes, with no register pins, assembly hints, compiler overrides
or new header selectors. Their reconstruction starts from the ROM's calls,
conditions, access widths, offsets and literal values. The initial matching commits
are separate from the shared-record and helper migrations.

| Function | Address | Bytes |
| --- | --- | ---: |
| Bat_Init | `08084F44` | 3,548 |
| Ghoul_Init | `0808D1D8` | 4,092 |
| Slime_Init | `08090FEC` | 3,632 |
| Bee_Init | `08094764` | 3,548 |
| Ghost_Init | `08099B44` | 4,752 |
| Lun_Init | `0809DD78` | 3,920 |
| Crow_Init | `080A1440` | 3,024 |
| Spider_Init | `080A4AFC` | 4,184 |
| TentacleActor_Init | `080ABDCC` | 3,548 |
| Vorn_Init | `080B43BC` | 4,332 |
| Skeleton_Init | `080C3260` | 4,684 |
| Golem_Init | `080CFC14` | 4,072 |
| Mummy_Init | `080DCC98` | 4,188 |
| Mimic_Init | `080E87C4` | 4,264 |
| Octopus_Init | `080EC3F8` | 3,832 |
| Ivy_Init | `080F0C20` | 4,020 |
| Axe_Init | `080FE68C` | 4,296 |

## Decisions that transferred

- **One sequence interpreter.** The initializers use `DisplaySequence_Step`.
  Its packed control byte preserves the byte-mode value that prevents the wrong
  regmove coalescing in flip handling. Existing sequence consumers remain exact;
  see [the sequence note](monster-init-sequence_2026-10-30.md).
- **Typed script operands and an indexed attack loop.** Halfword-valued readers
  retain early narrowing RTL that affects strength reduction even when the final
  narrowing instructions disappear. The measured loop shrinks from 63 to 56
  instructions between two passes, enabling the required pointer reduction.
- **Byte masks and scalar stores.** A byte targeting-mask parameter gives the OR
  accumulator the required mode. Scalar halfword writes at `0xB2` and `0x9A`
  prevent a zero temporary from surviving across calls and removing `r4` from
  the reload pool. Both Ghost and Lun use the same operation; see
  [the Ghost note](ghost-init_2026-10-30.md).
- **Ownership and lifetime.** Mummy's script context parsing and derived-range
  setup capture their context pointers in separate scopes. Axe's late group
  overrides remain in the caller's group phase. Moving those four stores into a
  broad actor helper created four extra PRE address spills.
- **Storage domains versus arithmetic domains.** Gate duration is a halfword;
  palette IDs and flag-mask queries can be word-valued arithmetic. These
  contracts change early RTL counts without requiring extra final instructions.
  The PRE model is used only after the full physical instruction stream and
  consistent stack-slot identities have been established.

Octopus reproduced the shared physical instruction sequence after correcting its
body receiver. Ivy then reused those operations with its own script fields,
resources, flags and event-dependent behavior reset. This is the concrete reuse
result: the shared operations reproduced multiple independently reviewed ROM
functions.

## Axe's last instruction

The variant byte at `0xED7` is read from keyword `t`, bounded to one, stored, and
used to choose category 12 or 13. A byte-valued bound operation with signed input
matches the ROM's comparison and receiver lifetime:

```c
static inline u8 BoundedVariant(s32 variant)
{
    if (variant > 1)
        return 1;
    return variant;
}
```

Reading the scalar byte directly into that operation removes an extra raw getter
conversion. The target is `ldrb r1, [r1]`; the getter form inserted
`adds r0, r1, #0` before `ldrb r1, [r0]`. Removing that redundant layer completed
all 4,296 bytes. The extra instruction had occupied the target's alignment space,
so function size alone had not distinguished the candidates.

## Shared records, operations, RAM and data

The canonical actor embeds body, attack, targeting, saved-control and full group
components. The [component migration](scene-actor-components_2026-10-30.md) and
[group migration](scene-actor-groups_2026-10-30.md) record the offsets and member
paths. Target and body-position pointers now use typed members; collider, position,
map, resource-header and keyframe records are shared with engine consumers.
The actual body initialization, sprite and movement-probe leaves now use that
record too: their complete 108 bytes remain exact. All 17 initializers use the
same SceneBody API; see [owner validation](scene_body_owner_validation_2026-10-30.md).

The initializer headers now own script operands, attacks, health, targeting,
control, region setup, terrain, gate/range/route setup, graphics and callback
installation. One complete pursuit-body constructor serves Skeleton, Mummy,
Spider, Ghoul and Axe. Nine initializers share palette parsing; six share the
initial-mode parser. Typed callback families share their defaults and common
action setup. There are no consumer-selected initializer implementations.

The remaining grouped body variants are compiler-sensitive source choices:
Golem/Mimic use halfword readers for M/K, Vorn for M, and Ivy/Octopus capture F/R
in halfword locals. Octopus also owns a direct drawable-kind read. Those are not
species behavior differences; making this last body layer uniform remains work.
Ivy's two locals removed redundant intermediate return copies, moving PRE's
measured instruction count from 1613 to 1611 and its table size from 807 to 805.
Octopus similarly reached its required table window after preserving the
conversion order and using the owned scalar kind field.

Two older chase selectors were removed in favor of explicit component operations.
Ten remain; [the shared reaction guide](../docs/SHARED_MONSTER_REACTIONS.md) lists
each retained setting and its measured compiler effect. Existing chase tracking
and group access now uses the same canonical members directly.

Four script-command tables have ordered editable CSV representations: 28
callbacks and 112 bytes. Their codec roundtrips the original bytes, preserves
Thumb pointer bits, and rejects malformed or resized tables. A linker test moves
all 28 callback references under a symbol-address shift. Studio exposes ordered
handlers, code links and direct initializer references for these tables. This
supports the command dispatcher established at `08136F1C`; table order is semantic.
The manager palette query now has one shared word-valued declaration consistent
with its callers; its 28-byte implementation remains exact with a halfword local.

## Verification and remaining queue

After all shared changes, all 17 initializers passed fresh stock compiles and
complete production-unit comparisons at the reviewed extents: zero differing
bytes, zero size deltas, and matching SHA-256 hashes. Expanded-source audits
found no register pins or inline assembly.

The final combined build printed `build/boktai3.gba: OK`, passed the RAM-literal
check, and produced a byte-identical 16 MiB ROM (SHA-1
`2651c5e6875ac60abff734510d152166d211c87c`). The shared actor/group sweep preserved
641 units (624 candidate-length and 17 reviewed-extent checks); sequence sharing
preserved 250 existing units; display records preserved 18 reviewed units; the
older chase cleanup preserved six reviewed units. These are overlapping check
populations, not additional newly matched functions. The final whole-ROM check
covers their combined integration.

The supplied 124-function monster queue has 107 functions remaining after this
initializer block. Its route-resume and pursuit families are the next application
of the shared components. Aggregate progress files are intentionally left out of
this delivery for regeneration by the integrator.
