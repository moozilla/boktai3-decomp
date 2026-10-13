# Wireless menus and result panels

This batch continues directly after the completed wireless connection controller.
The new sources recover the following contiguous menu families. Names are
provisional, based on their C behavior and neighboring callers.

- Wireless text encoding: the packet-name alphabet, two-byte character decoding,
  string conversion and name decoding. All named functions match.
- Wireless status menu: background/palette setup, prompts, disconnect waiting
  and task lifecycle. All named functions match.
- Peer panel: five drawables, a name buffer and connected-player selection.
  Initialization, packet-name updates, input and cleanup now all match.
- Packet exchange: queued packet publication, completion/error callbacks, task
  setup and singleton access. The menu uses these callbacks for trade/selection.
- Trade and selection menu: masked left/right selection, handshake, countdown,
  packed score exchange, round resolution, outcome acknowledgment and results.
  Its large initializer parses script-defined lists and assigns unused slots;
  the input handler now matches, completing its named task callbacks.
- Wireless game item/display controllers: movement, arrival/merge counters,
  task dispatch, queue publication and display callbacks. Several larger state
  handlers remain; the new item layout is shared by their drafts.
- Result panel: staged rolling numbers, games/wins counters, score reveal,
  input dismissal, 22 drawable objects and four text windows. Its initializer
  now matches, including descriptor copies and initial animation advancement.
- Following scene/result objects: initial slot lookup, vector transfer, camera
  proximity and resource-list attachment. Interpretation remains tentative.

The corresponding objdiff groups preserve the original denominator and leave
unmatched spans in their groups. This batch does not claim those groups are
fully complete or reinterpret unnamed assembly tails as C.

## Recovered layouts

Trade/menu fields include timer +0x24, state/entry flags +0x28/+0x29, result
packet fields +0x38, task/window references +0x60 and +0xD0, selection masks
+0x70/+0x74/+0x78, and script-derived lists at +0x7C/+0xBC. The initializer
uses script keywords 0x77/0x67/0x73/0x70, saves listener handles, clears slot
state and initializes score weights. These views are local rather than one
universal struct for all menus.

Result-panel timer/state/entry are at +0x18/+0x1C/+0x1D. Descriptor blocks are
+0x30/+0x50, sprite +0x70, glyph resource +0x9C, 22 drawables start +0xB8 with
0x60-byte stride, callback/resource are +0x8F8/+0x8FC, four windows +0x900 and
copied exchange data +0x910/+0x920. The singleton is gUnk_02000104.

Item records have a 0xB0 stride, padded vector fields at +0x0C/+0x14, sprite
+0x24 and drawing +0x50. Movement copies drawable coordinates into the sprite,
then arrival handlers update capped counters and publish the pending queue.

## Matching lessons

Pinning r7 can suppress its save/restore or make the backend treat it as a frame
pointer. Let a normal live local occupy r7; preserve other allocation choices
with scoped constraints. This solved both name-reset and packed-score loops.
The peer initializer matched after removing forced owner memory constraints:
ordinary typed descriptor access restores the compiler's argument setup order.
Explicitly live fixed registers must not overlap during different logical
stages; temporary drawing and sprite cursors require separate lifetimes.

Large initializers benefit from actual descriptor copies, typed animation
records and stack objects with observed field order. The result initializer
reuses the peer initializer's animation behavior rather than handwritten
instructions. Exact checks still review the complete function and literal pool.

## Validation

Each production translation unit passed a reviewed-extent exact check after
formatting. One focused existing report test confirmed that grouping preserves
code totals and register-pinning categories. Combined ROM validation printed **`build/boktai3.gba: OK`**.
The first split found standalone callbacks duplicated inside contiguous units;
redundant standalone copies were removed. Progress counted them only once. No tooling or regression suite was added.
