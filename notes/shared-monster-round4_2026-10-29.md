# Round 4: shared monster operations and five exact matches

Tags: #matching #scene-actor #monsters #golem #skeleton #ghoul #mummy #vanargand #compiler #tooling #080DA97C #0808B3EC #08078EE8 #081AE0C0 #080D2584

This batch starts at the supplied main bundle, `f62b776228cf3a3c468e059a185b4cc022f23b11`
(PR 887). All five reserved targets are now ordinary C matches, with no register
pins, assembly barriers, handwritten instructions, or compiler overrides. Each
new match has its own commit; tool fixes and shared extraction are separate.

| Function | Address | Bytes | Decisive finding |
| --- | --- | ---: | --- |
| Mummy_SelectMovementResponse | 080DA97C | 6,316 | source-local responses, shared chase interfaces, 45-slot PRE composition |
| Ghoul_ReactToContact | 0808B3EC | 6,236 | shared chase implementation; failed responses merge after allocation |
| SceneActor_UpdateStatus | 08078EE8 | 5,956 | actor component ownership, halfword flags, normalized predicates and three PRE identities |
| Vanargand_Init | 081AE0C0 | 5,064 | typed palette/motion APIs, halfword script values and two PRE slots |
| Mummy_UpdateDisplacementRecovery | 080D2584 | 4,852 | narrow member updates, byte route count and 32-slot PRE composition |

## What is shared

The four chase headers contain one set of actor-related route, player, terrain,
tracking, flag, and control records, with the existing canonical actor layout.
Golem and Skeleton no longer keep copied helper layers. Ghoul and both Mummy
functions use the same implementation; the status updater reuses its component,
geometry, control/state, and flag operations. Both Mummy functions use one random
route-selection helper. Vanargand shares `motion_track.h` with an already matched
motion initializer, rather than forcing its separate family into chase code.

Seventeen common records and 61 named inline operations are defined centrally.
Twelve named selectors retain measured value-domain and ownership alternatives.
Typed/generic receiver wrappers, raw/masked flag APIs, and one shared one-pointer
random-table view remain compiler-sensitive choices. They are documented, not
presented as uniquely recovered historical interfaces. Monster-specific outer
responses remain separate.

The new halfword flag component is explicitly packed and aligned to two bytes:
its real actor fields include `+0x2D6`. Target-compiler assertions and all affected
exact compiles verify this detail. The status updater also uses a two-byte-aligned
signed palette-countdown view without changing existing canonical raw field types.

## What transfers to other matches

Shared source was useful beyond removing copies. Mummy's recovered halfword
member operation resolved the status updater's final physical-register ties.
Separate terminating responses let GCC perform late tail sharing, fixing Ghoul
and Mummy without early hand-joined exits. The PRE model then solved permutations
of otherwise correct address spills.

The status updater is a concrete warning against ranking only by byte distance:
its seven-byte candidate had impossible relative PRE identities at every odd
table size. Changing the actor/registry binding produced 35 differing stack
bytes but compatible relative identities. The first measured predicate
composition compiled exactly; shared cleanup remained exact with final
`N=2667`, `T=1333`. No source instruction-count padding was used. The rule and its
preconditions are documented in `tools/scope/PRE_STACK_ORDER.md`.

## Acceptance

All five targets pass ordinary, stock, and complete-unit checks. The full
12,492-byte Golem unit and 6,372-byte Skeleton unit remain exact; the 104-byte
motion initializer affected by the Vanargand header change also remains exact.
The status migration has 385 passing target layout assertions and Vanargand has
91. The five new targets compile without warnings. Golem's existing pointer and
prototype warnings were reproduced with its old headers as a control.

One final combined build prints `build/boktai3.gba: OK`; the RAM-literal check
passes. A direct full-file comparison confirms all 16,777,216 ROM bytes, SHA-1
`2651c5e6875ac60abff734510d152166d211c87c`. Aggregate reports were regenerated once
for this bundle-based integration. Regenerate them on a receiving branch that
also includes other workers' commits.

## Tool fixes and review materials

The landing failure came from comparing a resolved source path to an unresolved
repository path. Canonicalizing the repository fixes symlinked checkouts while
retaining the guard against a source symlink escaping `src/`. A separate scope
fix preserves multiline inline record-return helpers during diagnostic trimming.
All 15 focused tests pass: 11 LocalLanding, two text-interface, and two diagnostic
source tests. This is not a claim about the entire historic matchsearch suite
or a native macOS run.
All 19 touched C/header files pass formatting. The whole-tree formatter still
flags 59 untouched files whose bytes equal the supplied base. A final cosmetic
Vanargand formatting pass preserves its complete-unit exact bytes; no semantic
source change follows the combined ROM check.

Per-function notes: `docs/GHOUL_CONTACT_RESPONSE.md`,
`docs/MUMMY_MOVEMENT_RESPONSE.md`, `docs/MUMMY_DISPLACEMENT_RECOVERY.md`,
`docs/SCENE_ACTOR_STATUS.md`, and `notes/round4-vanargand_2026-10-29.md`.
`docs/SHARED_MONSTER_REACTIONS.md` records the common interfaces and remaining
matching choices. The delivery includes the ordered patches, compact evidence,
portable source histories, and a self-contained HTML explainer with a measured
45-address PRE model. Historical source-node results require reevaluation after
changing headers, symbols, or compiler context.
