# Link play and shared runtime recovery

Tags: #link #system #engine #script #types #matching #phase3

The lane reduced source steering debt from 344 to 312 constant uses. Weighted
warnings fell from 384 to 352 with zero errors. Owned headers retained 18
warnings and 18 constant uses. One source count correction accounts for the
decimal digit counter below. Exact-match coverage was unchanged.

## Recovered layouts

- The RFU controller's existing `struct Engine` replaced 28 private views.
  The merger verified the views whose smaller size did not affect their use.
  Nine views still address a scratch span that crosses the shared roster's
  typed members. The peer roster builder still has a callback type conflict.
- Five link-player units now use the shared session record for the local
  player index. The matching layouts were linked by their declarations at the
  same RAM pointer, 020004B4. The scanner distinguishes objects stored inline,
  including arrays, from objects held through a pointer.
- Shared types consolidate sprite component, handshake, record panel, solar
  measurement, and related task views. The mechanical pass also moved wireless
  controller offsets to typed fields and replaced four exact private copies.
- The wireless item manager owns a `SpriteRenderResource` at +0x20. Its queue
  routines use direct subscripts for the incoming and local item groups. Four
  resource-header dependents matched.
- The wireless scene actor owns a `SpatialEntry` at +0x56C. Its position setter
  and hit-byte reset confirm the layout. All 114 header dependents matched.
  `ScenePool_UpdateOccupiedActor` (0803C114) uses the shared actor layout,
  `ContactPosition`, and the existing entry-state operation that resets the
  frame counter and clears the state workspace.
- The link route reaction (0813727C) now uses the shared chase group, route, and
  target types. Its timed restoration is addressed from +0x460, the start of
  the previous saved-control record. The two-record view keeps the original
  base and names the already recovered restoration fields. Starting from
  +0x468 produces the same stores with different instructions.
- The script session reset and settings paths use `SessionSave`. Save reads
  identify its selected-slot byte at +0x08, and the save-request accessor
  identifies the byte at +0x09. The header checks its 0x28-byte extent. All
  five header dependents matched after naming the two bytes.

## Steering and compiler evidence

- `SolarCalibration_Initialize` (081DBE04) passes a one-entry map index array
  and writes direct reset values and graphics arguments. `Menu_DrawText`
  (081DC054) masks the escaped glyph directly and loads both character bytes
  before combining them. Combining the shift and low-byte load in one expression
  placed the shift ahead of the load. The complete calibration unit matches with
  one remaining warning, down from 22.
- Two mutually exclusive reaction tests in `Player_ApplyContact` (08243090)
  read the collider field directly. Independent tests outside that chain load
  the mask before the field. Their direct forms reverse the order.
- The peer menu reuses its three-field exit transition and existing sequence
  playback, including both orientation tests and frame timing. The complete
  peer panel unit matches with all six weighted constant uses removed.
- `SceneMotion_InitState` uses the shared proximity volume and the game's SDK
  fill wrapper for its eight-byte position. The original leaves the emitter
  pointer in r0. Explicit returns remove the ROM's extra move into the store
  register, so the explicit return is under `UBFIX`. Both builds compiled.
- The digit counter in `Text_DrawNumber` (08215F38) is advanced by the DIGIT
  macro. Its zero is an actual counter initializer. The unused macro parameter
  was removed and the false positive was documented.

The type tools now support reviewed shorter pointer-only views, tagged
typedefs, and typed RAM declaration evidence. Thirty focused tooling tests
passed. Every changed source unit passed the stock unit check. The combined
ROM build belongs to batch integration.

## Remaining decisions

Direct background visibility operations change the shadow load order and spill
rotation. Direct affine division forms keep the calculation but allocate the
multiply and signed rounding differently. Direct wireless item reset loops
coalesce zeros for the state timer and pending counters, moving a zero across the
count load. The item descriptor reset still hoists its fill zero after the loop
pointer is initialized. The script pause latch retains three single-pass scopes
because simpler forms do the same work with a different three-register rotation.
The script game-state base still uses the known-zero term described in the
[hard-site notes](hard-sites_2026-11-23.md).

The worker's `build/REPORT.md` indexes the family previews, individual
refusals, stock checks, and scope logs. The affine search keeps both scalar
candidates under `build/matchsearch/lane-Bg_SetAffineTransform/`.
