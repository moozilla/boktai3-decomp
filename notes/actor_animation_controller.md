# Actor animation controller (081E34D0-081EAB64)

## End state

One unit: `src/actor_animation_controller.c`, 98 functions, `tools/check.py
src/actor_animation_controller.c` exits 0 (30356 bytes, exact). Shared types in
`include/actor_animation_controller.h`: `ActorAnimController` (the 0xFA4-byte
controller), `ActorChildSlot`/`ActorChildCallbackView` (the 16-entry child
array, viewed from its own start and from each slot's callback field),
`ActorSprite`, `ActorAnim`, and `InteractionPartner`.

93 of the 98 functions were already matching C using one-off per-function
partial structs (`struct G`/`struct S`/`struct Obj`/... with hand-picked `pN`/
`gapNN` padding). All of them now go through the shared header. 5 were gaps
with no C at all; one (`sub_081E42C4`) got a 87.6% NONMATCHING draft, the
other four (`sub_081E728C`, `sub_081E9BA4`, `sub_081E9E44`, `sub_081EA4DC`)
are plain `INCLUDE_ASM`.

## Header layout: how it was recovered

The pre-existing per-function structs already agreed on most field offsets
(cross-checked across ~15 of them by hand), but two assumptions turned out to
be wrong when actually compiled with the project's agbcc:

* **No trailing struct padding is skipped — but it also isn't absent.**
  `ActorSprite` sums to 0x22 bytes by field widths, but agbcc still rounds the
  *embedded-member* size up to 0x24 (its own alignment, driven by the leading
  `u32 flags`). This only showed up by probing agbcc directly (writing a
  throwaway `Probe()` function and reading its `.s` output), not by reasoning
  about the C standard — the two assumptions needed canceled each other out
  by coincidence until I checked.
* **Anonymous unions are silently dropped.** agbcc (gcc 2.9) warns
  `unnamed struct/union that defines no instances` and the union contributes
  zero bytes to the struct, unlike every anonymous-union-supporting compiler
  I'm used to. `ActorSprite.y`/`.angle` had to become a single plain `s16 y`
  field instead; callers that want the rotation reading use
  `*(u16 *)&p->obj.y`.

Everything else (the movement-scratch region at 0xE7B-0xF50, the palette-fade
block at 0xEE8-0xF50 that time-shares those same bytes with movement fields
that stop earlier, the idle-restore fields at 0xF6E-0xF86) was cross-validated
against 3-5 independent functions each before being written into the header,
then re-verified against the real compiler with small throwaway `Probe()`
functions (see git history of this file's early commits if that scratch code
is still present; it isn't checked in).

## Traps found while moving functions onto the shared header

These all compiled and still matched before the move (each one used its own
narrow struct), and would have been *silent* semantic bugs after a blind
type-rename (right offset range, wrong exact field) rather than compile
errors, so they're worth listing:

* `sub_081E5648`: a local `p->variant` (u32 @ 0xFA0 in the shared header)
  actually meant the *other* variant byte at 0xF77 (`variant8` in the header).
  Both names existed in different original structs; only one file needed the
  fix.
* `sub_081E7C3C`: `p->handle` here was actually `completionHandle` (0xF54,
  not 0xF50) — the original struct's gap arithmetic placed it one slot later
  than every other function's `handle`. Showed up as a 48-byte mismatch, not
  a compile error, because `handle` is a real field in the shared struct too.
* `sub_081EAAAC`: its local `struct Obj { u32 a, b, flags; }` is an unrelated
  12-byte struct (unknown object, offsets `p+0x238`/`p+0x2c0`), not the same
  "Obj" as the sprite. Left local instead of merging into `ActorSprite`.
* `sub_081E4B0C`: the only function that actually needed the *old* per-byte
  `mode:4`/`xb:4` bitfield shape (register pinning is sensitive to the load
  width agbcc picks for a bitfield read). Kept its own `struct G` locally
  rather than forcing the shared header's halfword `mode:4`/`phase:12` here;
  everything else uses the header's version.
* Several "`g->xa`/`g->b`/`g->e2c`/`g->e32`/`g->c`/`g->d`/`g->e`" single-letter
  fields (21 files, mostly `Actor_BeginAnimationPhase*`) were a second,
  independently-named view of `mode`/`oldMode`/`anim.frame`/`anim.timer`/
  `flags`/`savedFrame`/`savedTimer`. Renamed in bulk once the mapping was
  confirmed by hand on one file.
* Static inline helpers (`take`, `frame_factor`, `mode_bits`, `child_take`,
  `rotate`) are duplicated across many files under the same name with small
  per-function differences (mostly presence/absence of a register-pin
  concession). `merge_unit.py` correctly flagged these as conflicts; resolved
  by renaming the minority variant to `_v2`/`_v3` per symbol rather than
  picking a single version (the differences are real register-pinning
  concessions, not accidental drift).
* A handful of forward-declared external functions/globals
  (`Actor_EnsureController`, `gUnk_020003C8`, `Task_Kill`,
  `Anim_LoadKeyframe`, `Sound_PlaySong`, `Script_GetKeywordValue`, several
  `Actor_*` callbacks) were declared with mismatched types across different
  original files (`void *` vs a real struct pointer, `s32` vs `u32`, `u16[]`
  vs a scalar `s32` for `gUnk_03005260`). None of this affects codegen
  (everything is a 4-byte value on this ABI) but agbcc rejects the outright
  type conflict once merged into one translation unit; resolved by picking
  one canonical declaration per symbol and casting at the odd call sites
  (`*(s32 *)gUnk_03005260` where the scalar reading was needed).

## Register-pinned conflicts from merge_unit.py

`merge_unit.py` found real differences (not just name collisions) between
functions sharing a static helper name. All were genuine per-function
matching concessions, not something to unify away:

* `take`/`frame_factor`: 1-2 files omit the `register ... asm("r1")` hint
  the other 24-27 files use for the same helper.
* `mode_bits`: 5 files use a 2-statement body (`u32 result = ...; return
  result;`), 11 use a 1-statement `return mask & v;` body.
* `child_take`: 3 distinct shapes (direct `c->control`, a plain `u8 *flags`
  temp, a register-pinned `u8 *flags`).
* `rotate`: the 6-rotating-animation variant needed an entirely different
  body (no `Rotator` reinterpretation, works through `p->obj.y` directly).

## What's still unfinished

* `sub_081E728C` (949 lines of asm), `sub_081E9BA4` (154 lines, a 16-child
  double loop against an unidentified global table + a call to
  `sub_08249240`), `sub_081E9E44` (497 lines), `sub_081EA4DC` (293 lines,
  called from `Actor_EnsureController` right after `Task_Create`, likely the
  controller's own init/setup) are still bare `INCLUDE_ASM`. None were
  attempted in depth beyond confirming they're gaps, not already-matched
  code; `sub_081EA4DC` is probably the best next target given it's called
  from code already understood here.
* `sub_081E42C4` (idle-wait timer/phase-save, tentatively
  `Actor_UpdateIdleWait`) is a 87.6% NONMATCHING draft. The remaining diff is
  pool-constant materialization/reuse around the `delay`/`interrupted`/
  `savedPhase` triple and the `phase << 4` mode write — structurally right,
  register/constant-sharing choice is off by one pool slot's worth of code
  size (4 bytes). Tried reusing a single pointer via subtraction/addition to
  mimic the original's apparent constant reuse; made things worse, so left
  as the simpler form.
* `sub_081E3B6C` (`Actor_ProjectAndClampToScreen`) keeps a small local `Vec`
  struct for its own 3-`s16` helper type; not merged into `ActorSprite` since
  it's single-purpose and already matches.
* Many `unkNNNN` padding ranges in the header (notably 0xE34-0xE74 "motion
  state", most of 0xE7B-0xE8C and 0xE92-0xEC8) are still unidentified. Only
  named what at least one matched function actually dereferenced.
* `ActorChildSlot`'s own `unk04[0x28]`/`sprite[0x80]`/`unkB2[0x1c]` are
  unexplored; only `flags`, `fn`, `index`, `control`, `variant`, `effect`
  are confirmed.

## Check iterations

Rough order of magnitude: ~150-200 `tools/check.py` calls total across the
header design, per-file rewrites, and the final merge's declaration-conflict
cleanup (merge_unit.py's conflict list was resolved iteratively, each fix
re-triggering the next conflict it had been masking). The environment was
shared with several other concurrent unit-worker farm sessions, which made
individual `check.py` calls intermittently slow (seconds to tens of seconds)
under CPU contention; no correctness issues came from that, just wall-clock.
