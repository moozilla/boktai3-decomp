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

The full report at `build/structfamilies/REPORT.md` ranks the top fifty of 350 families and
records 1,818 embedded-type findings.
For `struct Player` it confirms `struct SceneInputWidget` at 0x8C and 0xEC from 12 and four
call sites respectively. Some units use syntax the source parser cannot resolve. The catalog
keeps those diagnostics, along with any layouts obtainable from declarations alone.

## Scan cost

The argument prefilter reduced scan inputs from 4,554 to 3,584. A two-header test uses one
compiler probe on the first pass and zero new probes on the cached pass. Compatible headers can
share a probe and incompatible groups fall back to smaller groups or individual headers.
On `src/grid_attack_actor.c`, three direct Clang preprocessing invocations took 5.304 seconds
versus 10.161 through `cc`, with identical output. The final whole-tree refresh with cached
probes took 1 minute 42 seconds at two jobs.
