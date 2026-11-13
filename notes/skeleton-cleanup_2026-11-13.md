# Skeleton unit cleanup

Tags: Skeleton, 080C12FC, 080C2BE0, 080C3260, 080C44E4, phase4

Consolidated 13 files from 080C12FC through 080C47B8 into `src/skeleton.c`.
The formatted unit matches all 13500 bytes with the stock compiler.

The complete 0x74C allocation now has the shared actor/group prefix, two contact
colliders, script call counters and parameters, action flags, pursuit timers,
health threshold, navigation probe and destination. Existing sprite, sequence,
tile, profile, terrain and actor fields replace raw offsets and private padded
views. No shared engine header changed. The adjacent generic axis-step routine
uses the common animation context and sprite position, rather than interpreting
its context as a Skeleton collider tail.

Three controller names now use the Skeleton prefix and describe their observed
state transitions. Existing shared movement and terrain names remain. No external
source references required editing. Both the initial contact branch and the
axis-choice branch now use structured conditionals instead of gotos.

The common control helper matches all three controllers. Returning to a behavior
uses one shared Skeleton helper with constant state and phase; passing those
zeros through the general state helper changes address reuse. The recovery
transition matches with ordinary field assignments. The probe's existing
`*(&actor)` expression remains: simplifying it permits constant propagation of
the animation duration and changes 42 non-register instructions. Typed callback,
probe and drawable access match with that expression retained.

The initialization registry-link result is a byte, confirmed against its callee;
using an implicit integer result drops the required byte truncation. Position
snapping must read coordinates unsigned before rounding. All typed inputs and
helper fixes checked individually, then final merged/formatted unit MATCH.
No new concessions or full ROM build.
