# Tether actor entry, interaction and script controls

11 new exact functions: **+0.0608 percentage points**, reaching **23.978%**
(6,490 / 11,471 functions; 579,146 / 2,415,354 bytes).
Without register pinning: **19.533%**; Register-pinned: **4.445%**.
These include the 1,468 new bytes, with 424 bytes requiring no hints.
Actor identity and state meanings remain provisional.

## Exact contiguous entry and state subsection

**08049168–0804945C** is complete, including three previously exact helpers.
New matches are 49180 (68), 491C4 (112), 49234 (36), 49258 (76),
492F0 (116), and 49364 (248). Existing helpers: state setter 49168 (24),
pending-effect wrapper 492A4 (12), map-path probe 492B0 (64).

- Entry dispatch uses kind + base except kind 9. The normal path mirrors
  headings above 4 using 8 - heading and sets flag 1. The kind-9 path uses
  entry 44, four quantized sectors, variant 1 for sectors 1/2 and flag 1 for
  sectors 2/3. API entry indices are not necessarily sprite frame numbers.
- State is byte 255, with frame counter at 256 reset by the setter.
- Pending halves at 384/386 are cleared before updating the 47C handle with
  helper 66DB8, using the configuration at 44. Configuration/channel meanings
  are not established; the helper is not named as a particle emitter.
- The path probe initializes its cache at 1D8 for position 50, then tests the
  script-supplied target at 410. The underlying map routines enumerate regions
  and cache positions; this is not an actor hitbox with XYZ bounds.
- Interaction-state selection restores 40C from 40E; 40A gates it, along with
  status flags 3E4. It updates **global 02000488**, preserving the distinction
  from the passed actor. Resulting states are 0, 3 or 4.
- Auxiliary-state entry copies position to 434 and raises Y by 384, creates
  the auxiliary sprite, then places it on the map surface. The low nibble of
  the terrain byte is height * 256. Surface kind 1 subtracts low Z, kind 2
  subtracts low X. Out-of-range cells use cell 0. It removes registration 244
  when flag 254 is set, applies pending work, and enters state 5.

## Public interaction and script controls

Additional new matches: 497F0 (40), 49CD8 (116), 49D4C (176),
4A15C (200), 4A294 (280). Twenty-seven already exact public helpers have
provisional names, with checked field views where they preserve the original
code generation. The
script-path setter, detached-state entry, status remover and script-output
routine retain their exact byte views. Some transfer helpers also deliberately
retain local two-word views. Their complete pre-existing object extents were
checked, so these refactors earn no new exact bytes.

- 3E4 is a **status bitmask**, with tag at 3E6; it also suppresses moving and
  palette/auxiliary effects. Bit 4 compatibility is tested by 49818.
- Availability 49CD8 rejects lock 3FC, blend request 472, status flags 3E4,
  and cooldown 41E. It admits states 0/1, or 3/4 with pending half 384.
- 49D4C treats state 1 separately (lock 1, cooldown 300); states 0/2 and
  pending states 3/4 reselect their interaction state and use cooldown 600.
  Both clear 480 and update handle 47C with configuration 44, mode 1,
  output pointer 480, and numeric arguments 200/250/330.
- 4A15C enters state 15, sets step 470 to 16 and timer 46A to 29.
  Heading 46C points relative to the player at 30 using atan2, or uses a
  random heading if X/Z coincide. The random branch uses its saved actor,
  while the angle and final timer stores reload the global actor.
- 4A294 reads `p` and `d`. Position 258 receives script XYZ, then **Y is
  replaced by terrain height** before it is copied to actor position 50,
  the display-position pointer at 118, and origin 42C. `d` sets heading 260.
- Input command 4A0CC writes request 240, variant 241 and value 242 for kind 9.
  These overlap the tail of the wider map-path storage, whose complete
  internal layout remains unknown. No universal cache type was invented.

## Larger recovered drafts

4945C (**77.4%**, 904-byte reviewed extent) adjusts kind-9 movement using
adjacent cell heights and eight direction sectors. Odd/even paths inspect
one/two neighbors, and only eligible movement is adjusted by the sine table
scaled by 02000550. Signed products truncate toward zero. The current draft
still differs in register allocation and common-tail layout.

49840 (**65.6%**, 764-byte extent) is a nearby interaction test: squared X/Z
radius 48,400 (220 units), state/status compatibility, and cell adjacency with
Y tolerance 255. For diagonal/nonadjacent cells it tests the two orthogonal
corner routes with terrain-height tolerance 127. Its packed local initializes
only X/Z; neither C helper reads its unwritten Y or padding. The draft still
needs allocation and branch-layout matching. Both are guarded; **no exact
credit**. Alongside the earlier tether interpolation, they provide subsystem
understanding without masking the remaining work.

## Validation and continuation

**52 complete post-promotion extent checks passed**, covering every exact
actor-header user and external status cleanup. Format, RAM-literal, inventory,
registry, ten archives and text-bank checks passed. The combined build printed
**`build/boktai3.gba: OK`** in `build/tether-interaction-combined.log`.
No new emulator claims.

Continue at **0804A430**, the kind/script initializer, then 4A4C8, 4A600 and
4A700. These feed the following state callbacks through the 4D5CC constructor.
Stop at 25% exact or 60% account usage remaining. Latest check: 75% remaining.
