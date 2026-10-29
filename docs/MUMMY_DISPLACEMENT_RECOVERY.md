# Mummy displacement recovery: 080D2584

Tags: #matching #mummy #scene-actor #regalloc #080D2584

`Mummy_UpdateDisplacementRecovery` (formerly `sub_080D2584`) is an ordinary-C match for **100% of its 4,852 bytes**, adding
**0.2013 percentage points** of unpinned code coverage. It uses no register pins, assembly hints,
handwritten instructions, or compiler overrides.

## Behavior and shared source

The callback eases recoil displacement by one quarter per update, clamps each
axis to ±200, and emits a periodic particle burst. When displacement stops and
the animation latch permits recovery, it selects another route, enters the
timed recovery callback, or resumes pursuit through the shared arrival and
terrain operations. The separate success and failure responses retain their
original callback, state, and timer order.

The source uses the canonical actor and chase records. Only three small
displacement operations remain local. The new halfword flag component and
random-route selection live once in the shared headers; the existing response
consumers retain their verified interfaces. The selected coordinate, flag,
saved-control, and route-count interfaces are measured matching-source choices,
not proof of the original authors' exact helper boundaries.

## What decided the match

A cast to a scalar `u16 *` had erased the compiler's narrow struct-member
read-modify-write expansion for group flags. Restoring an actual halfword member
retained the required zero-value lifetimes. That made `r7` enter the global
reload-register set and restored the particle table reload. A typed flag
component preserves the same member semantics while expressing the real field
input separately. Artificially fusing control/state transitions also changed
pressure, but moved state selection earlier than the ROM and was rejected.

A byte-valued route-count accessor then corrected the competing mask lifetimes.
After this, all instructions, physical registers, branches, and literals agreed;
only 22 bytes of stack offsets differed. The address-PRE model matched all
32 spilled field identities. A measured interaction between promoted coordinate
values and scalar output stores corrected an initially nonadditive source model.
The resulting seven-interface composition was exact: **1,947 pre-PRE
instructions, 973 hash buckets**, with collision order by first discovery.
No compiler table size or pseudo number is encoded in the C.

## Verification

The isolated ordinary compile, uninstrumented stock compiler, and complete
translation-unit production pipeline all reproduce the exact ROM slice.
Preprocessing, compilation, and assembly are warning-free. Formatting and
migration to the canonical shared headers preserve the same bytes. This unit
contains one emitted function; the parent integration owns the combined ROM
build and the shared-header checks against existing consumers.

Code SHA-256:
`35bfecff298bb46cabfee39a06bb66be6aafeff4915a2c1b7bcaaa8539f5552a`

Worker-source SHA-256:
`070f69cab5cf53cb96c97a4fc03212877a0ea3ca0fe5114b6fcac9652840d0ba`

The 85-node portable history, rejected controls, private diagnostic traces,
checked database backups, and full compiler gates are retained under
`build/round4/mummy_recovery/`. Start with `exact-summary.json`,
`source-history-canonical.json`, and `full-tu-shared-final/verification.json`.
