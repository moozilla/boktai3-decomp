# Tether actor unit consolidation

Tags: #tether-actor #unit #matching #types

Range: **080485FC–0804D620**, end exclusive. Identity remains tentative.
The 113 source files are consolidated in ROM order in `src/tether_actor.c`;
shared layouts remain in `include/tether_actor.h`. No shared documentation,
symbol-name table, generated assembly, or git metadata is edited.

## Types and contracts

The existing 0x484-byte `TetherActor` layout is reused, including its seven
0x28-byte segments, movement and interaction probes, display, two colliders,
charge, state, palette and directed-movement fields. The formerly opaque
0x43C–0x463 state/effect tuning halfwords now have tentative field names based
on the initializer at 0804AA08. Packed collider initialization pairs are shared
header types rather than anonymous H/Z declarations.

The duplicate player view is replaced by `struct Player` from `player.h`:
heading/side at 0x3A2/0x3A3, facing at 0x3A4 and tether mode/phase/frame at
0x457/0x458/0x45A. Global state accesses use `GameState`, including saved kind
at 0x5AA and the signed savedZ interpretation at 0x5B0. Charge callbacks reuse
`ContactCollider` (contactFlags at 0xA, statRewardFlags at 0x34, unknown3C).
The callback's interpretation of unknown3C as contact power is tentative.

Singleton accessors now use the shared actor fields and aligned position copies,
with no local P/Q/W/Obj/status-owner object views. The script initialization
chain now declares and forwards its entry argument: separate translation units
previously relied on r1 surviving into `TetherActor_InitKindFromScript`.
The third constructor argument remains unused in the initializer.

## Matching adaptations

Some pre-existing pinned functions require byte-based address calculations,
unsigned halfword views or explicit two-word vector copies to preserve address
lifetimes, signedness and register allocation. Each retained calculation is a
local matching adaptation; direct field substitutions were tested against the
whole range. Display positions still need a coordinate view because their
shared engine type exposes words rather than named coordinates. Existing
barriers and pins are retained only where the greedy minimizer still needs
them; no new hints or instruction bodies were introduced.

`merge_unit.py --write` was invoked with its git-rm subprocess replaced by
ordinary source-file unlinking, because the driver owns git metadata. Its merge
and conflict checks were used normally. Differently shaped inline terrain
helpers have distinct local names; shared helpers are outside draft guards so
exact functions can use them.

## Remaining drafts

All five pre-existing guarded drafts were attempted (about eight source-shape
variants each, plus baseline and type checks). None became exact. They remain
useful source drafts, including the ones below the usual new-draft threshold;
the default build still uses the original assembly. Fresh scores are recorded
in the unit by `tools/check.py --nonmatching --record`. Some previous fuzzy
annotations were stale even when checking the original source and header under
the present toolchain.

- 08048644 UpdateSegments: packed endpoint caching, stack/register allocation.
- 0804945C AdjustSlopeMovement: terrain branch layout, truncation and allocation.
- 08049840 TestNearbyInteraction: distance/status gates and orthogonal terrain
  routes understood; branch layout and register lifetimes differ.
- 0804B964 LimitPlayerDistance: distance limiter understood; allocation differs.
- 0804BF04 UpdateRoamingState: complete state behavior; scratch allocation differs.

No new gap functions or newly exact functions. Exact coverage is unchanged
(**+0.000 percentage points**); assembly fallbacks are not counted as C progress.

## Validation

- `make format` completed; `git diff --check` passed.
- Whole-unit exact check: **20,516 bytes, zero differences**, covering the
  complete assigned range with assembly fallbacks in their original positions.
- 108 exact C functions retained: **84.597% of this subsystem**, unchanged
  (**+0.000 percentage points**; 17,356 exact C bytes).
- Guarded draft scores recorded in ROM order: **64.8%, 29.5%, 65.6%, 67.9%,
  82.9%**. Drafts remain excluded from exact credit.
- `tools/depin.py src/tether_actor.c --write`: **136 → 136 existing hints**.
- Rough check effort: 113 initial per-file checks, about 45 draft checks,
  26 direct-field trials, 136 hint-removal trials and combined-unit checks.
- `tools/progress.py`: matching count and bytes unchanged; no coverage loss.
- `build/venv/bin/python tools/build.py`: **`build/boktai3.gba: OK`**.
  The first builds exposed missing local resource inputs. Extracted the ten
  archive manifests into ignored `build/res/`, then supplied the older indexed
  filenames expected by shared `gen/data.s` using checked hash/index mappings.
  No shared generated source or tool was changed. C compilation was reused.
- Changes are left uncommitted for the driver.
