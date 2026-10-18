# Orbiting particles unit consolidation (2026-10-18)

Tags: #particles #types #matching

Range: 080187BC–08018D94 (end exclusive). All nine functions were already
matched; consolidated them into `src/orbiting_particles.c` in ROM order with
`include/orbiting_particles.h`. No newly matched functions, drafts or gaps.
Subsystem coverage remains 100% (+0 percentage points), 1496 exact bytes.

Shared layout: `OrbitingParticles` is the 0x1B4-byte task, with time/duration
at +0x18/+0x1C, speed/rise limit/count/base frame at +0x20..+0x26, resource at
+0x28, origin at +0x2C, and eight `OrbitingParticle` records at +0x34.
Each 0x30-byte orb holds state, angle, rise, radius and a shared wait/animation
timer, then a 0x28-byte `OrbitingParticleSprite` at +8. Sprite position is
+0x18; byte +0xF is provisionally named priority. Reused `SceneVec`, `Task`
and `EngineMap` from existing headers, removing partial views and engine
redeclarations. Existing descriptive function names were already promoted;
no new aliases or symbol-table edits were needed.

Matching adaptations:

* Spawn keeps byte-addressed random-table indexing: typed array indexing
  reverses a literal load and shift. The RAM literal in the initialization
  helper remains the existing checked `RANDOM_TABLE_ADDR` constant.
* Update and destroy retain byte strides between embedded fields, expressed
  using `sizeof(struct OrbitingParticle)`. Update's inactive-slot loop keeps
  its offset temporary with a type-derived member offset: direct indexing
  changes register allocation.
* `depin.py --write` tested the update's existing r2 pin and empty barrier;
  both remain necessary (2 -> 2). No new hints were added.
* `engine.h` declares void-returning task callbacks; these callbacks return
  zero and must retain their result-producing epilogues to match. Explicit
  callback casts preserve the existing engine API without redeclarations.

Validation: individual checks, then the merged checker reports all nine
functions MATCH (1496 bytes). Roughly 20 checker invocations including depin's
three checks. `make format` was run; unrelated formatting edits were removed.
`tools/build.py` printed `build/boktai3.gba: OK`; the progress inventory
retains the baseline matched function and byte counts. No commits or Git-index writes were performed. `merge_unit.py --write`
was run through a local build-directory wrapper replacing only its `git rm`
with filesystem unlinking, as this driver's worker cannot write Git metadata.

Build setup surprise: this checkout lacked `build/res/` and another extracted
manifest source. Extracted the eleven missing build-local sources through
`datafmt.Entry.extract()`. Shared `gen/data.s` also references 578 archive
filenames with entry-index suffixes; fresh extraction used unsuffixed names.
Created the required local aliases from the corresponding extracted entries,
then confirmed every `.incbin` target exists. No shared generated files or
tracked tools were changed.
