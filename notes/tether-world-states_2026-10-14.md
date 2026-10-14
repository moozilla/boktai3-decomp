# Tether world position, player boundary and state callbacks

Eight new exact functions add **0.1240 percentage points**, reaching **24.184%**
(6,506 / 11,471 functions; 584,118 / 2,415,354 bytes).
Without register pinning: **19.554%**; Register-pinned: **4.629%**.
New exact bytes: 2,996, including 196 without hints. Actor identity remains tentative.

The contiguous **0804AF80–0804B964** subsection is fully exact: 2,532 bytes,
including 1,972 newly exact and three existing typed state/value callbacks.
Four later helpers through **0804BEE4** add another 1,024 exact bytes.
The intervening 4B964 distance limiter is a guarded **97.9% draft** (384 bytes),
with no exact credit. Four existing methods also received descriptive names.

## World position and movement-cache contract

- **4AF80 (588):** unless status flags are set or state is 5, computes terrain Y
  from the cell's low-nibble height and high-nibble slope. Display XYZ adds the
  offsets at 37E/380/382 and reloads the display pointer between stores. Updates
  the charge collider, snapshots the current movement cache, applies the kind-9
  body movement or refreshes other kinds' terrain cache, and stores the snapshot
  into the previous fields. Kind 9 also invokes the flagged-cell clamp. Finally
  updates sampled position, copies world position to target 3F0, and writes the
  heading into the target's final halfword at 3F6.
- **4B1CC (344):** the first call sets `terrainCheckReady` at 464 and returns 0.
  State 5 or nonzero status flags suppress the test. Terrain attribute 0x40
  returns 1; otherwise state 1 returns 0. Other states query the properties
  again and return whether terrain Y differs from world Y by more than 255.
  The repeated dynamic-property query is retained.
- Existing **4B324 (280)** now names and types next-state selection: a requested
  kind-9 action selects 11, other kinds can enter the auxiliary terrain state,
  and player tether mode 6 switches the 0/1 and 16/17 state pairs.

**380 is `displayOffsetY`**, correcting its former `auxiliaryTimer` name.
The current/previous cell, height, slope and attributes occupy the 16-byte
movement cache at 120. The world updater confirms those paired semantics.

## Lock, following and player boundary

- **4B43C (336):** freezes on global flag 0x200. `lockFrame` at 3FE selects shake
  amplitude 16 through frame 63, then `(frame >> 2) + 16` with glow mode 2.
  Two independent random-table samples drive X/Z offsets. `lockSoundFrame`
  at 404 gates sound 2B9 every eighth tick using another random low bit.
  At frame 128 clears the lock, writes tentative `transitionReason` at 406
  (2 in state 1, otherwise 1), enters state 2, and copies 402 into 400.
- Existing **4B58C (88)** ticks the primary interaction value and reselects
  interaction state when it reaches zero, subject to global freeze, enabled,
  reset and status gates. Existing **4B5E4 (192)** is the idle-state callback;
  it clears charge-collider flag 4, selects entry 0, and dispatches locked
  shake, charge-ready shake, blend reset or the interaction-value tick.
  Its `SetEntry(actor, 0)` ABI is now explicit and remains exact.
- **4B6A4 (704):** blocks player movement when terrain attribute 1 is set or
  player/actor terrain heights differ by more than 255. For adjacent cells it
  places the player two units inside the actor's cell edge. Otherwise copies
  actor X/Z. It recomputes cell properties as the original does.
- **4BAE4 (76):** absent status flags, ticks secondary value; at zero reloads
  its maximum, enables interaction lock, and resets lock frame to zero.
- **4BB30 (420):** enters state 1, invalidates interaction reset for distances
  over 256, clears glow mode, and either limits player distance or snaps to the
  cached target (marking movement for differences over two units). Restores
  heading, selects the moving kind-5 animation or normal entry, then dispatches
  shake, blend reset or secondary-lock tick. **The original's second reset
  comparison uses actor X against target Z**, and that quirk is preserved.

Guarded **4B964** computes a 70-unit facing offset from player facing at 3A4,
checks a 150-unit radius, computes a radial target, and applies 3:1 smoothing
with signed division toward zero. It calls the boundary blocker and copies
player position into the cache at 168. The whole 384-byte extent is reviewed;
four scratch-register differences (five bytes) remain. A bounded permuter
run found no improvement. Greedy removal reduced 54 draft hints to 16 while
preserving the same score. No instruction bodies or artificial ROM literals.

## Candidate step checks

- **4BCD4 (332):** rejects masked terrain attributes, attribute 0x80 with
  GameState flags868 bit 0x10, terrain-height differences over 50, or a nonzero
  result from 08100AD4. Its caller supplies origin, candidate and attribute mask.
- **4BE20 (196, no hints):** tests a pure X or Z displacement directly. For
  diagonals, separately tests X and Z before testing the combined displacement.
  It copies all eight position bytes before modifications, preserving padding;
  the mixed bitfield/halfword views reproduce the original store widths.
- Existing **4BEE4 (32)** tests whether both signed deltas are within 32.

Seven new matches retain minimized concessions: AF80 16 hints, B1CC 2,
B43C 1, B6A4 4, BAE4 3, BB30 2 and BCD4 4. BE20 needs none.
No explicit r7 binding. Low-register bindings are not relied upon across calls;
values that survive calls use the ABI's preserved registers. Coverage remains
unmeasured; these contracts come from reviewed code and exact-byte checks.

## Validation and continuation

The combined build printed **`build/boktai3.gba: OK`** in
`build/tether-world-states-combined.log`. **80 complete post-promotion extent
checks passed**, covering exact actor-header users and the renamed wrappers.
Format, inventory and RAM checks, plus all twelve data round trips, pass.
Main PR 130's progress Action also passed at dffdb2a8.
Continue at **0804BF04**, the state-3 callback, followed by 4C104/4C304 and the
remaining contiguous state handlers through the constructor at 4D5CC. The
4B964 guarded allocation draft remains available for a later scratch-register
pass. Continue until **25% exact or 60% usage remaining**; latest usage is
**72% remaining**. Claude's separate checkout owns main.
