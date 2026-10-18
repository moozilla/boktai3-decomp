# Player action phases foundry run (2026-10-18)

Tags: #player #matching #foundry

Candidates remain in `wip/foundry/`; per-function results and checker snapshots are
in `build/foundry/`. No production sources or commits were added by this run.

- 08150B34: 94.3% fuzzy in three checks; CORE distance one, a facing reload
  remains. Timed action phase folds the facing direction and subtracts thirty
  from the counter at +0x4E8 before switching modes.
- 08150C5C: 95.0% fuzzy in three checks (including one compile failure); CORE
  distance one, the same facing reload. Recoil speed at +0x598 sets X/Z
  velocities through the direction sine table and decays by three quarters.
- 081511E0: exact in three checks. Four action phases use a mode-dependent
  animation speed and launch a directional effect on the observed cue frame.
  Explicit if/else speed selection reproduces the original constant ordering.
- 08151310: exact in one check. Six action phases, including a fifteen-frame
  hold, reproduce the jump table directly.
- 08151988: exact in one check. Three phases use base animation zero; the
  gameplay identity of action 0x11 remains tentative.
- 08156CAC: exact in three checks. A signed GameState field at +0x502 gates
  particles and doubles cadence above 2499. Player +0x4D0 counts up past fifteen,
  then emits one particle and resets. Modes three/four use collider position;
  other modes use player position with the second component raised by 360.

All six use the existing Player header. Added recoilSpeed (+0x598) and
particleCadenceCounter (+0x4D0) by replacing padding; added
GameState.playerParticleRate (+0x502) the same way. Header checks passed for
all 16 Player consumers and 17 GameState consumers. The particle candidate
uses the existing signed ContactPackedVector type. Provisional function and
callee aliases are file-local; no register pins or instruction bodies were used.
