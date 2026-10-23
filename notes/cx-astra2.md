# Campaign worker astra2

Tags: #matching #monsters #actors

- 080D812C Mummy_UpdateBurnDamage: skipped unchanged at 99.9%; two scratch-register bytes remain at 080D8390. Explicit offset and result pins, link-state pointer lifetimes, RAM-literal forms, and extending the health pointer through the local-player query did not improve the baseline. Keeping health live moves the scratch from r5 to r2 and changes a later signed-load scratch; keeping the owner mask live moves it again to r3. No candidate improvement retained.
- 0808B3EC drafted 79.2% as tentative Ghoul_ResumeBehaviorAfterHit. Complete reaction callback reconstructed using the chase/arrival family: three arrival expansions, target retry, hurt animation, and recovery control/state transitions. Full target is 6,236 bytes; candidate is 6,208. Structural differences fell from 451 to 394, total from 929 to 818, by preserving mask evaluation and moving the failed-arrival callback to the shared tail. Explicitly passing a common motion-flags pointer regressed; narrowing the random choice to s16 reduced structural count slightly but worsened overall alignment. Local copies of the chase helpers allow independent work without changing other drafts. No Ghidra export is available.
- 080D2584 drafted 74.8% as tentative Mummy_UpdateDisplacementRecovery. Complete 4,852-byte candidate now covers recoil easing, initial quartered secondary motion, periodic sparks, random route selection, the prepared-recovery callback, and both chase/arrival expansions. Reused local chase helpers from 0808B3EC; an output-pointer easing helper recovers source/current load ordering. Realigned differences improved 884/363 structural to 829/333; candidate extent equals the reviewed target. Widening mask helpers or narrowing retry choices reduced selected structural counts but increased total differences, so those variants were not retained.
- 08087A70 drafted 78.9% as tentative Bat_UpdateKnockbackRecovery. Ported the complete Mummy recovery draft, removing entry motion quartering and random-route selection, changing hit sounds and recovery callback, setting the group bit before arrival, and using a 32-tick resumed route scale. Kept both successful retry transitions. Mask-helper changes improved 779/320 structural differences to 699/301; remaining residue lies in repeated chase helper allocation and control-flow layout.
- 0807AF94 drafted 72.8% as tentative SceneActorMovement_SelectRecoveryRoute. Complete callback covers group gates, unequal-height arrival, heading-dependent cardinal traversals using 0812E118, same-level scans with attribute mask 0x43, two-tick direction preparation, and fallback idle control. Cell-center generation through an inline output pointer preserves the original intermediate height store before replacing it with actor Y. A signed-halfword random choice improved the duplicated turn expansion. Realigned differences improved 855/309 structural to 738/248. No speculative null guard was added for the required subject pointer. Register allocation, helper layout, and four traversal count slots remain.
- 080C8774 Golem_ResumeBehaviorAfterDisplacement: skipped unchanged at 95.8%, 73 total/12 structural differences. Confirmed both RNG loops allocate the table in r8 while the ROM spills it and keeps the wrap mask in r8; PRE stack slots also differ. Tested outer wrap/table lifetimes, pointer volatility, explicit wrap pinning, width changes, and all 24 RNG argument orders. Best alternative order reduced total to 67 but increased structural differences to 14; retained the stronger structural baseline. No source change.

## Wrap-up

The campaign stop marker ended the pass after the Golem attempt. Mimic_Init
(080E87C4) received only its initial assembly read before the marker was noticed;
no candidate was written. The remaining fourteen assigned functions are deferred,
including that initializer. Four complete guarded drafts are committed, two
existing drafts remain unchanged, and there are no new exact matches
(+0.0000 percentage points exact-code gain). All retained drafts were formatted
and measured with the full reviewed function extent; assembly fallbacks remain
active in the normal build.

Batch validation after rebasing onto main: all four draft scores reproduced;
the campaign build wrapper completed with `build/boktai3.gba: OK`.
