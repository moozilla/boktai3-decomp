# Private record recovery, 2026-11-25

Tags: #tooling #matching #process

`tools/structfamilies.py` combines agbcc layout probes with typed call arguments, RAM declarations,
and ROM report units. A layout match alone can show a related shared view. The family graph requires
object-use evidence. The report ranks copies times unexcused warnings in the distinct source units
and records ROM load sites, names, type conflicts, and embedded types. The catalog keeps the evidence
for each call site.

`tools/structmerge.py` previews by default. For each conflicting field, it considers ROM load width
and extension first, copy votes second, and takes the name from the most specific declaration.
It checks the generated type's size, alignment, and direct member offsets with stock agbcc. Apply
checks each unit through keepcheck and rechecks all dependents of a newly generated header.
Private dependencies, unproved bitfield groups, differing object extents, and ambiguous edits
remain review items.

## Scratch trials

The trials used copies under `build/structmerge_trial/`. All source proposals in these three
families converted. The original source and header trees were frozen.

| Family | New shared type | Units and dependents matched | Concession warnings before → after |
|---|---|---:|---:|
| `Vec-f27fac1d` | `struct VecRecovered` | 3/3 | 114 → 114 |
| `EffectPosition-6f825dd0` | `struct EffectPosition` | 5/5 | 10 → 10 |
| `Anim-c4a8fee4` | `struct Anim` | 4/4 | 8 → 8 |

There were zero errors before and after in the trial units. The vector tag uses the `Recovered`
suffix because a shared `struct Vec` already exists. The other generated types use their family's
dominant tag. The linter counts cover the affected source units and the generated headers.
The trials recover types so future cleanup can tackle the parked constants separately.

The follow-up converted all three `Args-3a9bc39e` units to `struct ScriptCallArguments`.
The two original bitfield declarations had different type spellings and measured the same
16-bit span. They voted together, and the emitted bitfield kept its measured position and width.
All three header dependents matched. The scoped linter reported six warnings and zero errors
before and after.

The full report at `build/structfamilies/REPORT.md` ranks the top fifty of 394 families and
records 1,818 embedded-type findings.
For `struct Player` it confirms `struct SceneInputWidget` at 0x8C and 0xEC from 12 and four
call sites respectively. Some units use syntax the source parser cannot resolve. The catalog
keeps those diagnostics, along with any layouts obtainable from declarations alone.

## Nested member follow-up

The scanner now carries evidence from related parents to compatible private types embedded
at the same named member slot. The refreshed catalog grew from 350 to 394 families. It found
two `SceneDistanceEntry` declarations through the `linked` array at offset 0xDF0 in their
parents. Both units matched with the shared child header. A new scan of those units then
produced a shared `SceneDistanceSet` header, and both units matched again. The linter counted
six warnings and zero errors before and after across the three reviewed units.

The wider parent proposal exposed a separate layout at 0xDF0 in `RatatoskOrbRelocation`.
That unit uses an orb union, and the header it brings in conflicts with
`SunOrb_GetPositionFromManager` declarations in the two distance units. The exact-match gate
rolled back the wider proposal.

## Orb overlay follow-up

The report now lists direct member alternatives at aggregate layout conflicts. At 0xDF0 it
shows two declarations of `linked` as a four-element `SceneDistanceEntry` array and one
declaration of `orbs` as `union RatatoskLinkedOrbs`.

A manual scratch trial gave the distance and relocation layouts separate members of the
same 0x60-byte union. The 0xF30-byte shared parent is named `struct RatatoskOrbDistanceView`
because the canonical Ratatosk header already describes that region differently. The
distance routines use the anchor at 0x36C and the typed declaration of
`SunOrb_GetPositionFromManager` from `sun_orb_effect.h`.
Stock agbcc verified the layout. All three units and header dependents matched
(081F2E88, 081F2EE0, 081F8988), with six warnings and zero errors before and after in the
scoped linter. The generated scratch header is
`build/structmerge_trial/include/recovered/ratatosk_orb_distance_view.h`.

## Scan cost

The argument prefilter reduced scan inputs from 4,554 to 3,584. A two-header test uses one
compiler probe on the first pass and zero new probes on the cached pass. Compatible headers can
share a probe and incompatible groups fall back to smaller groups or individual headers.
On `src/grid_attack_actor.c`, three direct Clang preprocessing invocations took 5.304 seconds
versus 10.161 through `cc`, with identical output. The final whole-tree refresh with cached
probes took 1 minute 42 seconds at two jobs. The nested-evidence refresh took 3 minutes
1 second and ranked the newly discovered families as well.

A focused two-unit SceneDistance scan took 55.59 seconds with all 444 shared headers in scope
and 30.75 seconds with `--shared-header include/scene_vec.h`. Both runs found the same two
private family IDs. The focused report records the selected header because it limits which
shared layouts and embedded-type findings can appear.

Grouping ROM load counts by field offset reduced median Markdown rendering time from
0.1899 to 0.0388 seconds for the top fifty and from 0.5069 to 0.2485 seconds for all 394
families. These are five alternating runs on the same loaded catalog. Both rendered reports
were byte-for-byte identical to the previous renderer.
