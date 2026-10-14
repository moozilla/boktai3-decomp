# Script selector and shared display animation

Two adjacent subsections: **08042198–08042320** (script-entry selector,
tentative) and **08042320–080427FC** (display animation wrapper). This batch
adds **13 exact functions**, **0.055 percentage points** (1,336 bytes), bringing
the branch to **23.001%**: 6,352 / 11,465 functions and 555,562 / 2,415,354 bytes.
Claude's PR #83 contributes 17 separate exact helpers, 1,064 bytes and
0.044 percentage points; they are incorporated in the overall figures and
combined validation.
Both subsections now have complete exact function coverage; preserved nonfunction
padding/data is not counted as new C. No new NONMATCHING draft is introduced.

## Script-entry selector

The 0x30-byte list-4 task stores a script start, two decoded operand values,
selection index, repeat delay and previous direction. The existing 08042198
cursor routine now uses this shared view and the name `ScriptSelector_MoveSelection`.
It restores the script PC, scans pairs using 0821AA88/0821AA00, and keeps the
selected pair. The second value is passed to Script_Call when A triggers.
The display meaning of the first value remains unconfirmed.

Up/down changes selection, with a 20-frame delay on a new direction and a
four-frame repeat delay thereafter. Input halfword 1 bit 2 repeatedly advances
until the cursor stops changing; it therefore selects the final available entry.
Input halfword 2 bit 1 calls the selected script and kills the task. These are
observed offsets/masks; the third halfword's general input meaning is not yet
named. The cursor clamps negative indices to zero and assigns the scan index
when the PC runs out; do not assume the stored index always denotes a live pair.

Initialization writes 0x40 to gUnk_03004DA0, requires script keyword `s`, saves
the PC, resets index/direction and selects the first pair. It loads resource
92B3/519C and copies eight words from +0x1F4 to gUnk_030051A0. The resource and
global's gameplay/rendering identities remain provisional. Factory allocation
uses list 4, size 0x30, the update callback and no destructor. Initialization
failure marks the task for deletion and returns null.

## Display wrapper layout and constructors

The wrapper is **0x94 bytes**. Its common prefix has kind, entry index, variant,
flags and finished byte. Kind 1 uses a sprite at +0x28 and sequence state at
+0x54; kind 2 uses a 32-byte resource header at +0x0C and a keyframe object at
+0x2C. +0x8C points to the position storage; +0x90 points to the selected display's
flag word. These pointers let position and flag helpers work across both kinds.
The shared header provides observed partial views of each underlying format.

Sequence construction clears the wrapper, sets kind 1, loads AnimData and an
optional sequence resource (922E/id), initializes the sprite and copies two
position words to +0x44. A zero sequence ID stores a null resource pointer.
Keyframe construction clears the wrapper, sets kind 2, loads CB05/id, copies and
relocates the 32-byte resource header, initializes the keyframe object and sets
position/flag pointers to +0x4C/+0x34. It preserves the initializer's return
value. The kind is already set if a later resource operation fails; the matched
C intentionally preserves that behavior.

Entry selection rejects inactive wrappers. Kind 1 stores variant/flags and
starts the requested sequence, folding bit 0 into its sequence options. Kind 2
adds the stored byte variant to the entry index, configures keyframes, clears
finished unless option bit 0 is set, and sets/clears display flag 4 from the
wrapper's flags byte. Destruction removes the appropriate sprite/animation and
clears kind, without clearing every field.

## Playback and queries

The sequence command halfword contains **frame ID in bits 6–15**, flip bits in
bits 4–5 and duration weight in bits 0–3. Playback writes the frame and derives
display flags 4/8 from command flips relative to sequence flags. It advances a
16-bit elapsed counter. Sequence flag 4 selects reverse traversal; flag 8 holds
at an endpoint instead of wrapping. Reaching an endpoint sets finished, while
ordinary advancement clears it. After a transition it computes byte duration as
`(speed * weight) >> 6`, with zero coerced to one. The speed field is a halfword.
The sequence resource being null skips playback and returns the current finished
byte. Kind 2 delegates stepping to Anim_Step and stores its result in finished;
inactive kind 0 returns -1.

The completion query returns the wrapper's finished byte for kind 1, otherwise
keyframe status bit 2. The frame-boundary query returns sequence index when its
elapsed counter is zero, or keyframe frame when status bit 4 is set. These query
paths do not have the setters' inactive guard; adding one would change behavior.

Priority and palette/attribute setters dispatch to the appropriate object view.
Tile selection dispatches to AnimData_SelectTiles or 08219A80. Their consumers
confirm the tile-source pointer at wrapper +0x18 (sequence) / +0x74 (keyframes)
and tile index at +0x12 / +0x66. The position setter returns -1 when inactive;
on success the original returns the incoming wrapper pointer, retained explicitly
in C. Set/clear/test flag helpers use the +0x90 pointer, with test returning zero
for an inactive wrapper.

## Evidence and matching work

New reviewed extents: 080421E8 144; 08042278 100; 080422DC 56; 08042320 168;
080423C8 308; 080424FC 40; 08042524 52; 08042588 136; 08042610 160;
080426B0 36; 08042738 36; 0804275C 60; 08042798 40. Each addition passed a full
resolved-byte check and has a separate single-unit matching commit. Existing
cursor, destruction, flag, tile-source and tile-index helpers were rechecked
following type cleanup; their bytes earn no additional credit.

Playback was developed from readable C using regdiag to distinguish allocation
from control flow. Explicit narrow counters, command widths, the signed duration
product and shared index-store paths reproduced the original instructions.
Temporary hints reduced from **47 to 11** via depin before acceptance. Volatile
halfword reads retain the observed packed-command access width. The selector
update retains one hint; position setter retains one; both constructors, all
other new helpers and selector initialization use no hints.

Earlier checks used truncated extents (92 instead of 100 for selector init, 164
instead of 168 for entry selection). They were mismatches, never accepted.
Both were corrected against the full original body/pool before matching commits.
Exact acceptance uses the extents listed above, not the preliminary values.

The complete combined build printed **`build/boktai3.gba: OK`**
(`build/script-display-final-combined.log`); format, inventory and RAM-literal
validation passed. The hidden 0804271C boundary retains its file-local alias,
as required by the name promoter. Next
contiguous subsection starts at **080427FC**: collision callbacks and the
adjacent actor/sprite initialization beginning at 08042984. Its collision IDs,
angles, reaction timers and dual colliders are distinct from this display API.
