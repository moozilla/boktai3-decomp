# Grid attack animation and targets

Tags: #matching #bosses 0802B7C8

GridAttack_UpdateAnimationAndTargets matches all 820 bytes with the shared
PlayerCue_StepSequenceEnded helper. The target pointers, squared distances and
bearings are indexed one-entry arrays. Taking the address of a scalar instead
makes agbcc advance three pointers through the loop. The draw and transform
positions have signed halfword coordinates and four-byte alignment, so the
shadow position copy still uses two word loads and stores.
