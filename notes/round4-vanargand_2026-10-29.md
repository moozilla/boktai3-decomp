# Vanargand initialization — exact ordinary C

Tags: #matching #vanargand #palette #motion-track #regalloc #081AE0C0

`Vanargand_Init` (formerly `sub_081AE0C0`) at `0x081AE0C0`
matches its complete **5,064-byte** ROM extent, including literal data.
Target and final candidate SHA-256:

`dbc25a363b92843b4fd070e2ef128a30060a649656786fb3dd59186dc05a03d6`

The final function has no register pins, inline assembly, barriers, compiler
overrides, or `NONMATCHING` fallback. Its frozen evaluator result is exact;
the uninstrumented production compiler, assembler, and real linker independently
reproduce the same bytes from the complete translation unit.

## Recovered structure

- The word at actor `+0x570` is a target count. Captured target-player,
  target-position, and player-list bases express the ROM's counted initialization
  loop. The existing player layout supplies its position at `+0x30`.
- One coordinate writer expresses the same three halfword stores for the body
  and detached part. Shared palette restoration preserves the actual palette
  reference across state changes. Ordinary interpolation and paired color stores
  follow the already matched `Vanargand_UpdatePaletteTransition` implementation.
- A focused [motion-track header](../include/motion_track.h) shares the proven
  `0x38`-byte track and `0x08`-byte vector layouts with
  `MotionTrack_Initialize` (`0x081B9334`). Vanargand uses the component reset in
  observed store order. The initializer retains its existing vector-clear
  implementation; the header extraction preserves all **104** of its ROM bytes.
- Callback types now distinguish state callbacks returning `void` from damage
  and requested-mode callbacks returning `s32`, each with one object argument.
  Health parameters, presentation mode, and target count have names supported by
  their observed uses. Unknown bytes and uncertain fields remain explicitly
  opaque or offset-named.

## Compiler-sensitive interfaces retained

The forced alternate palette is a one-word preset passed by reference to a
selection helper, while runtime restoration passes an already resolved bank to
the shared palette implementation. This distinction keeps the correct pending
animation constant lifetime in modes 1 and 3. A single scalar interface shared
that value too far; passing every bank through a record introduced five extra
copies. The accepted record is a useful input descriptor, not evidence that the
original developers used this exact API.

`ReadScriptHalfword` returns `u16` at all nine static script-value sites whose
destinations already truncate to a halfword. Full-word configuration and packed
coordinate reads keep their original domains. This genuine value boundary
removes the final eight differing bytes, all of which had been stack offsets.
Same-byte compiler traces show 2,219 to 2,246 real pre-GCSE instructions and a
PRE table size of 1,109 to 1,123. The palette/variant address buckets change to
1,011/825, producing the ROM's variant-before-palette spill order. These numbers
explain this candidate's compilation; they do not establish the historical
source's intermediate representation. Declaration order, receiver bindings,
coordinate formals, and palette interpolation scopes also remain matching
choices.

## Validation and limits

Evidence is retained under `build/round4/vanargand/`:
`final-session-gate.json`, `final-full-tu.json`,
`final-layout-gate.json`, `motion-track-full-tu.json`, and
`frozen-stock-verification.json`. All **91** layout assertions passed with
target AGBCC, covering the actor fields, embedded components, selected sizes,
and player position. The full-TU initializer hash is
`861ab71ca86467313d33d3f5fce0364c2ed416341adcbe752770c6e5d01eb3f6`.
Source hypotheses and compiler diagnostics are retained separately from the
final source. A new context was created after adding the shared header.

Byte equality is the acceptance result for this compiler, flags, external symbol
map, and ROM extent. It does not uniquely recover historical C, validate every
provisional field name, or prove that every neighboring Vanargand view can be
merged. Only the demonstrated motion-track overlap was extracted. Combined
whole-ROM validation belongs to the integration gate.
