# CircleWindow_GenerateScanlines (08018F38): holdout

Tags: #matching #phase3

The match keeps `asm("" : : "r"(relativeTop))` to hold `savedRadius - p->y` alive. The ROM
computes it (`subs r0, r3, r0` at +0x36, after the bottom clamp, from a separate copy of the
radius in r3) and never reads it. flow deletes dead sets, so the ROM's value had a use when
flow ran that a later pass removed: combine (README fact 94) or jump2 (fact 57).

* `CircleWindow_GenerateScanlines.c`: no asm; 7 ROM / 5 draft differ (only the dead set and
  the radius copy feeding it, plus the r3/r0 choices around them).
* `CircleWindow_GenerateScanlines.fact57.c`: `if (savedRadius - p->y > 0) out = ...; else
  out = ...;`. jump2 cross-jumps only 4 insns of the arms because each arm's reload of `bank`
  (r8) takes the next spill register (r3, then r0). `AGBCC_SPILL=4:1` brings it to 8 differing;
  the rest is the sb/sl order of `square` and `bank * 4`. Identical arms are contrived C anyway.
* Ruled out: the first loop counting `radius - y` up to 0 (`for (i = savedRadius - p->y; i < 0;
  ...)`): keeps the value live as the biv init but the loop is no longer reversed onto `top`
  (6/6); guarding loop 1 with `radius - y < 0` (18/20); identical arms holding loop 1 (25/40);
  a `?:` with equal arms and `i = 0` in both arms (folded before flow).
* Next idea: two uses of the value that combine both folds (i2 kept because its result does
  not die at the first i3), e.g. a loop-2 induction value seeded from `radius - y`.

Store: build/matchsearch/CircleWindow_GenerateScanlines (local to the pilot worktree).
