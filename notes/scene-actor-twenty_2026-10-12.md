# Scene actors and dialogue: reaching 20%

50 functions raised reported coverage from 19.656% to 20.001%
(+0.345 percentage points). Source changes only; one combined ROM build.
`build/scene-actor-twenty-combined.log` printed `build/boktai3.gba: OK`.

## Complete contiguous runs

- 08030164–08030710: actor proximity, path, phase-window and completion actions.
- All named manager functions at 08030710–08030864: update, teardown, init, create.
- Dialogue controls around 0803092C–08031964 are substantially recovered, with
  remaining script initialization and slow/input-driven text callbacks.

The names describe current interpretations. Scene actor movement uses fixed
point x/z values at 0x68/0x6c; path state is rooted at 0x19c. The dialogue
controller's VM begins at 0x18, script pointer is 0x1a4, animation frame state
is 0x2b8, and callback is 0x2cc. These are separate observed object layouts.

## Matching patterns

- Return a materialized result from proximity helpers and outer target checks;
  sequential failure labels preserve both boolean layers. An ordinary result
  can be preferable to a fixed register, which may introduce byte copies.
- Callback constants often need loading before the destination address.
  Small inline setters with different parameter orders help; local marked
  pinning closes remaining allocation differences.
- Keep zero/one initialization after constructing a field pointer when the
  original computes the pointer before materializing the constant.
- Empty asm constraints preserve required narrow conversions and reloads.
  Remove unnecessary barriers: their size estimates can make agbcc expand a
  short conditional branch even when the emitted empty asm has no bytes.
- Do not infer nested structure size from modern host alignment. The compiler
  rounded a char-only state view to four bytes; explicit padding had to account
  for its actual compiled size.

## Retained candidates

Dedicated branch: `codex/wip-scene-actor-twenty`. Only draft C outside `src/`.

- 0802D44C: 292-byte initializer, five register-only differences. Changing
  vectors to bitfields or reordering collider wrapper arguments did not help.
  The bounded permuter attempt failed with an AST assertion and was stopped.
- 0802DD6C: sound triggers tied to animation frames; pointer/literal allocation
  differences remain. Candidate is exact-sized but not an exact match.
- 0802EA90: target approach and facing update. Semantics recovered; allocation
  and instruction-order differences remain.
- 080311F0: dialogue script options, palette, rectangle, scrolling and choices.
  The draft needs failure-label placement and register/order corrections.

Other ignored drafts remain locally. No draft was merged or counted.
