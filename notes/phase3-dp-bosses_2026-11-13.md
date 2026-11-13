# Bosses, Player and Engine core phase 3 results

Tags: #matching #phase3 #bosses #player #tooling

All 19 assigned queue entries were worked in ROM order. Three are now free of
concessions: 15.8% of this queue, a gain of 15.8 percentage points. One additional
function has an exact partial removal. The other 15 retain their original source;
their concession-free drafts and blocking decisions are saved under the dp-bosses
section of `notes/phase3-holdouts.md` and `notes/phase3-drafts/dp-bosses/`.

## Matched changes

- SceneGrid_ScanColumns (0806E7CC), commit `2ec452b56`: eight pins and one operand
  barrier removed. Inline endpoint helpers prevent the long-lived column from
  becoming CSE's class head; the shorter steps life wins allocation naturally.
  Both CSE_NOCANON=column and AGBCC_PRI=steps:+4 confirm the decision. Stock unit MATCH.
- sub_0806F06C (0806F06C), commit `5d7a25ac6`: DEBUG_NOP removed. Repeat the real
  retry increment and exit test on the rejected-X edge; jump2 merges the tail
  after its references have influenced allocation. Position accesses now use
  SceneActorBody fields. AGBCC_PRI=i:+920 confirms the baseline decision. Unit MATCH.
- Vanargand_UpdateCaptureSetup / sub_081A8E08 (081A8E08), commit `5a346898a`:
  height pin removed. Halfword construction in SetPositionAndGetHeight preserves
  the two independently live zeros. Stock unit MATCH.
- Vanargand_ApplyPendingDamage (081ACC24), commit `b4d201708`: partDamage pin and
  operand barrier removed; the original mask pin remains. Narrowing the flags
  read changes flow's relative lifetimes for damage and the health address.
  Stock unit MATCH. The fully pin-free 7/7 draft remains available for follow-up.

These three full removals cover 840 bytes; the partial is a 1196-byte function.
No ROM build, progress regeneration, shared-header change, or new concession was made.

## Nearest remaining decisions

- sub_0813D834 (0813D834): the body is exact; a second case entry adds only an
  initial compare/branch. CSE_NOFOLLOW=2 matches the old baseline. The extra edge
  blocks CSE's switch equality but must disappear later.
- PlayerAction_RefreshFlagState (0813EDDC): one argument copy is placed before
  the sign test instead of inside its nonnegative arm; all other instructions match.
- sub_0819BB18 (0819BB18): 4/4 after fixing the random-table class head; only
  invariant order and the associated reload temporaries differ.
- Hresvelgr_UpdateBoundaryBarrage (081A0BC8): 2/2, the random mask's retry
  allocation chooses sl instead of ip. AGBCC_FORCE=220:12 confirms the decision.
- PlayerParticleCloud_ScatterAnchors / sub_081D3620 (081D3620): 2/2 with the
  correct registers, but the bias adds to random after masking instead of to
  the coordinate before masking. The exact-shape draft needs greater random priority.
- GameState_SelectRamBase (0821B084): CSE puts the known constant base second
  in the add. Hiding it through halfword reconstruction fixes operand order but
  leaves five reconstruction instructions and alignment padding.

The remaining entries have their confirmed knobs, attempted C forms and next
hypotheses in the holdout notes. No search was started after the stop instruction.

## Scope diagnostics

This run fixes two misleading `why` reports encountered in the queue:

- A pure exchange of commutative inputs is reported as operand ordering, with
  the first reordering pass, rather than two allocation failures or an empty
  replacement. The GameState_SelectRamBase add now points to fold_rtx's
  constant-second rule (README fact 87).
- Register-holder lookup accounts for the upper words of multiword allocations.
  In the palette-copy diagnostic (08217430), the u64 bank at r2 also holds r3;
  `why` now names that bank instead of saying the holder is unknown.

Seven focused tests cover these cases and their false-positive boundaries.
Both required suites pass: 407 tests in `tests` (two skipped) and 127 tests in
`tests/tools`. Real-draft checks confirm both corrected explanations. New matched
C-form evidence is recorded as README facts 190–192.
