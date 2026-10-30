# Shared monster sequence stepping and four initializer matches

Tags: 08084F44 08090FEC 08094764 080A1440 Bat_Init Slime_Init Bee_Init Crow_Init DisplaySequenceState PRE stack-order animation

Bat_Init (3548 bytes), Slime_Init (3632), Bee_Init (3548), and Crow_Init
(3024) each pass a fresh stock compiler check and the complete translation-unit
comparison at their reviewed ROM extents. They contain no register pins,
instruction assembly, volatile animation reads, or compiler-option overrides.
Combined ROM validation belongs to the batch integration.

## One sequence implementation

`include/display_sequence.h` now owns the existing `DisplaySequenceState` and
`SceneActorControlMotionDrawable` definitions. Their fields and layout are
unchanged. The former headers include these definitions, preserving existing
consumers. `include/display_sequence_inline.h` supplies one `DisplaySequence_Step`
implementation used by all four initializers. It replaces four private copies
and removes their unused getters and temporary declarations.

The four initializer units still match after extraction. All 250 existing C
units that name either moved record also passed normal-build unit comparisons.
The private checkout initially lacked fallback assembly for 39 of those units;
the required 41 function fragments were regenerated from its existing code
listing before those checks were repeated. No full ROM build was run here.

## Why the control byte is a packed value

The ROM loads the sequence control byte before loading a flip mask. It then
copies the mask before the AND. The previous object getter preserved the copy
but put the mask load first. A scalar `u8` snapshot fixed load order but lost
the copy: the old compiler promotes scalar locals and parameters to word-mode
pseudos, allowing regmove to merge the input and result.

A packed control value keeps the byte in aggregate QI mode:

```c
union DisplaySequenceControl control;
u32 bit, first;
control.raw = state->flags;
bit = 1;
first = (u8)(control.raw & bit);
if (first != (((*command & 0x30) >> 4) & bit))
    owner->flags |= 4;
else
    owner->flags &= ~4;
```

The union describes the actual four control bits: horizontal flip, vertical
flip, backwards playback, and hold at the endpoint. Its one-byte size is
checked. The canonical state's stored `flags` field remains `u8`; other users
do not have to change. Both flip blocks match, including the vertical block's
narrowing shifts. This is a supported source interface, not a claim that the
original source used these particular names.

## Stored updates recover the timing arithmetic

The sequence elapsed and index fields are halfwords. Direct updates retain
their C expression-value narrowing:

```c
if (++state->elapsed >= state->duration) {
    state->elapsed = 0;
    /* Backwards playback and endpoint handling. */
    if (++state->index >= state->frameCount) {
        /* Loop or hold at the endpoint. */
    }
}
```

Those updates generate and share the ROM's halfword mask. An earlier explicit
word temporary plus `& 0xFFFF` was semantically equivalent but gave local
allocation a constant-equivalent register to remove and rematerialize later.
The compiler traces showed the unwanted movement occurring in local allocation,
after CSE, GCSE, combine, and flow. Direct field expressions removed the need
for the explicit mask and cached duration.

The command stream is ordinary `const u16 *`. Word-valued command and mask
interfaces preserve the ROM's halfword read when only its low four bits are
used. A direct narrow expression can instead become a byte load. Frame weight
is stored as a byte and reread for the speed multiplication, matching the ROM.

## Stack order is a separate constraint

For these initializers, PRE spills actor-relative addresses in the hash order
described in `tools/scope/PRE_STACK_ORDER.md`. Once the physical instructions
matched, the measured receiver pseudos and target slot order constrained the
early instruction count, rather than a raw byte-distance score.

Bat's 57 addresses allowed T=641,643,645,647, or N=1280..1295. Consistent typed
script readers for byte and halfword stored fields put the candidate in that
interval without changing final instructions. Bee required T=639 and
N=1276..1279; Crow required T=521 and N=1040..1043. The packed control snapshots
removed two early instructions. The final shared index accessor leaves Bee
inside its interval and Crow at its upper endpoint. All four use the same
sequence function; there are no per-species sequence selectors.

## Other recovered contracts

The optional attack parser uses halfword readers for three stored arrays and
a word reader for flags. Its forward loop has an explicit exit after entry
four. These types retain enough early operations to prevent strength reduction
of the first array while the other arrays become pointer walks. Later passes
remove redundant conversions. The target's separate first-array byte cursor
is therefore generated from one C index, not maintained artificially.

The primary callback initializer writes the engine's default command-handler
table before a species override. The eliminated default store still introduces
its address to PRE before the next field. Omitting that common phase changed
both repeated address-setup paths even though the final store was dead.

Field +0x250 is a script command-handler table, not a movement route table.
`SceneActorScript_OnCommand` reads the first halfword of a command packet,
indexes the table, and invokes its function with the actor and command pointer.
The initializer declarations now use a typed table pointer. The shared member
should be named `commandHandlers`; older local install-helper names remain to
be cleaned up with the common actor integration.

## ROM facts corrected during reconstruction

- Slime's post-animation bit-8 test reads species flags at +0x40A. Its half-health
  threshold at +0x3DA is the signed initial health shifted right once.
- Bee's `P` command clears +0x3DC. Its script variant is at +0x9A, and its saved
  initial position is at +0x488.
- Crow's script variant is also at +0x9A. The ROM sets behavior mask 0x200 and
  status mask 0x02100408 after installing motion flags.
- Widget palette writes target the drawable's embedded widget and palette
  pointer. They do not target actor-relative offsets with the same numbers.

The source was reconstructed against these ROM operations. Earlier rough
drafts were not treated as evidence of the source's original design.
