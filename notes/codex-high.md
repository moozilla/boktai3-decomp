# Codex high: task 0808AF38

Tags: #matching #ghoul #slime #register-pinning

Branch: `cloud/0808AF38`. Worked solo, offline, using the worktree's Python
and per-file checker only. No full build, generated-file edits, network actions,
or direct Git mutations. Commits were queued through `build/commit.sh`.

## Results

- **0808E838 matched**, `Slime_UpdateRandomWait`, 360 bytes, register-pinned.
  Consumes `field2B2`, plays profile entry 15, chooses a duration from RNG bits
  0–5 (zero becomes 64), and selects 0808E9A0 with control kind 0x23 when
  `field2B3` is set. Both preceding 0808E660 and following 0808E9A0 reference
  this state; the adjacent setup functions identify the Slime family.
  The animation selector is an inlined specialization of the shared profile
  animation logic at 08076818. Keeping the selection-address load separate
  from the saved pointer reproduced the otherwise missing register copy.
  An empty constraint on the drawable and narrow register hints reproduced
  the surviving constant-mask branch. `tools/depin.py --write` reduced 14
  hints to five; formatting and a final `check.py --target-size 360` retained
  byte equality. No handwritten instruction bodies.
- **0808EC80 drafted 88.6%**, `Slime_ConvergeReactionMotion`, reviewed extent
  728 bytes. On entry quarters secondary X/Z motion and selects profile entry
  three. Each update converges the saved X/Z displacement by a clamped
  quarter-step; once settled and animation-ready, health/context flags select
  idle control or 0808E660. The matched shared relative 08082D80 explains the
  convergence arithmetic. Remaining realigned difference: 40 instructions,
  four ignoring registers. The principal allocation mismatch is the flags
  pointer versus saved-X pointer; one pointer copy and a boolean copy also
  differ. Explicit 32-bit mask temporaries avoid unwanted narrow constants;
  a typed sequence view preserves the halfword timer store at offset 14.
- **0808E2CC drafted 88.1%**, `Slime_ResolveBodyContact`, reviewed extent
  640 bytes, candidate 632 bytes. Registered by `Slime_InitColliders`
  (0808F840) as its body callback. Rejects unavailable actors; contact flag
  0x800 adjusts movement relative to the terrain-cell center and cardinal
  direction, arms the primary collider if the destination ground height
  differs, otherwise moves X/Z and clears their velocity. Other contacts
  forward to `SceneActorBehavior_ApplyContactReaction`. Remaining realigned
  difference: 45 instructions, four ignoring registers. These four are the
  health-address copy and three high-register motion-pointer copies in the
  initial vector addition. Reversing the source operands of the shared Add
  inline fixed load order; an inline signed angle-division helper removed
  early result copies. Pinning the whole vector pointers made allocation
  worse, so that variant was discarded.
- **0808CC48 drafted 86.1%**, `Ghoul_ResumePendingRoute`, reviewed extent
  508 bytes, candidate 504 bytes. Called first by `Ghoul_Update` (0808D16C).
  Context bit zero at 0x628 requests a restart after the 0x648 countdown.
  It resets the actor from the route origin, saves a 30-frame delayed control,
  starts the route, attempts movement to context position 0x62C, and consumes
  the pending bit. Remaining realigned difference: 36 instructions, twelve
  ignoring registers. Cached route-slot and motion-flag pointer lifetimes
  differ. Returning the pending pointer through an inline output parameter
  reproduces its stack slot better than computing it twice. Mask temporaries
  keep the intended 32-bit arithmetic before halfword stores.
- **0808AF38 stopped below draft threshold**. Best complete candidate was
  66.7% fuzzy (candidate 816 bytes versus body/pool extent 812), with 154
  realigned differing instructions, 60 ignoring registers. Tentative role:
  `Ghoul_SelectRetreatRoute`. Computes a target-facing vector using Atan2
  and the sine table, scans route points for a negative dot product, excludes
  the current point, otherwise chooses a random different point, snaps its
  terrain height and attempts movement. Important original quirk: a rejected
  point in the negative-dot-product branch advances the index without
  advancing the point pointer. Candidate preserves that behavior. Bitfield
  vector writes plus signed-halfword pointer reads in the dot-product inline
  recover stack storage, but origin-coordinate hoisting/spills and route
  failure/control-reset allocation remain substantially different. Explicit
  volatile origin locals and broader pointer pinning regressed. Scratch
  candidates remain only in ignored `wip/`; no below-threshold source added.
- **0808B3EC stopped after structural investigation**, no qualifying draft.
  This 2,738-instruction callback is installed during the Ghoul initializer.
  Read with `asmat.py`, `condense.py`, and `relatives.py`; the Ghidra export is
  absent in this checkout. Three repeated chase/arrival expansions dominate
  the body, interleaved with sound, reaction flags and Ghoul-specific paths.
  `include/scene_actor_chase.h` supplies useful `Chase_Arrive`, terrain height,
  route and state/control templates; 080E4468 explains 38% of the target's
  instruction runs, with further matches in 080CEDF0 and 080C4A9C. The repeated
  expansions start around the terrain snap sequences at 0808B4B0, 0808BD54
  and the later third path. No whole-function candidate was ready to check;
  assembly remains authoritative.

## Validation and retention

The three drafts are guarded by `NONMATCHING`, retain `INCLUDE_ASM` in their
`#else`, and have scores recorded with `check.py --nonmatching --record` using
the reviewed extents above. Their default assembly paths also passed per-file
checks. Depinning was attempted before wrapping; the minimizer only accepts
exact candidates and skipped the three drafts. One exact match and three
drafts were committed separately. Names and inferred behavioral roles are
provisional, with file-local aliases and low-confidence comments. Full-ROM
batch validation remains for the integrator.
