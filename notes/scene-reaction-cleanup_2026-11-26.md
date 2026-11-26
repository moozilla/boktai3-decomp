# Scene actor reaction cleanup

Tags: #matching #types #phase3 #scene

The actor reaction pass reduced the linter's diagnostic lines from 286 to 247
(13.6%) and charged constant uses from 304 to 262 (13.8%). The scoped linter
reported zero errors. Code coverage remained 100%, a change of zero percentage
points. Each changed translation unit matched its full ROM extent with stock
agbcc.

## Source recovery

- The tracking block at actor +0xDC supplies the distance gate, pending
  contact target, and response reset (08072E50, 0807F4B8, 0807F710,
  0807F81C). The typed predicate already distinguishes boolean tests from
  masked values. All of those tests keep the ROM's mask-first order.
- The chase group contains a timed restoration at actor +0x468. The delayed
  callbacks (0807FDC0, 0807FE80, 0807FF48) now read that member and reuse
  the control installation and state entry operations. Referencing the
  fields through the actor lets agbcc build each address separately.
  Capturing a pointer to the restoration changes that instruction sequence.
  The chase group header check matched all 445 dependents. Removing the
  padded restoration view matched all 104 dependents of its header.
- The contact reaction (0807E9D0) now uses ContactCollider. It writes a
  halfword at actor +0x5CE equal to one third of the contact lock time.
  The timed burst callback (08081C98) reads it and spawns at twice that
  interval and again at the interval. It uses the chase group's effect
  configuration, origin, and route resume callback.
- Eight reaction callbacks use the guarded scene defeat record. Actor
  attachment, Mummy, Skeleton, and Ivy provide independent examples of the
  same link-mode and actor-status checks before recording the pair. The
  helper retains one explicit actor mask and uses the existing boolean
  link-mode test. The canonical record prototype accepts words. The body ID
  is narrowed explicitly where the original call did so. All 13 header
  dependents matched.
- The 29cb05719 and 4c8dcd0ec staging-only changes in these units returned to
  visible mask locals after the direct expressions differed. Those reversions
  leave the steering count unchanged.

## Remaining compiler decisions

- 08072E50 matches with the shared tracking test. Removing its single-pass
  store leaves five register-only instruction differences. Raising the gate
  pointer's global allocation priority reproduces the ROM.
- 08080AF0 matches with the delayed-response type and shared actor transitions.
  Moving the phase value to a direct store changes the position of its first
  load. The common delayed-response initializer writes the final two bytes
  in the opposite order.
- 08080984 still carries the single-pass form. With the loop removed, a
  reload rotation reproduces three of the register differences. The timer
  pointer still receives a different local allocation.
- 08135F70 and 081363F4 carry a display decision union. A signed halfword
  condition and fixed call priority change the argument setup for the damage
  display and the preceding allocation.
