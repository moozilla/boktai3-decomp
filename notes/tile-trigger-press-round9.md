# TileTrigger_StepPress

All 344 bytes match using the shared frame-sequence helper. A separate pointer
to the sprite preserves the input across the press callback. The trigger gets
30 frames before it sets state 5 and marks entry into that state.

## TileTrigger_ApplyContactEffects

The guarded C at 0x08106E98 compiles to 936 bytes, the full ROM extent, with
74.2% stock instruction similarity. It shares the press actor layout and the
registry control, state and random-sample helpers. The remaining differences
include register allocation through the actor loop and packed-vector stores.
The default build uses the ROM assembly.

All 400 differential cases returned in both bodies. The comparison covered
the full tile and actor buffers, final random cursor and all seven external
call types, including the position, velocity and scale of 258 particle spawns.
The cases reached 37 of the 40 block starts reported by the ROM decoder.

Tags: 08106E98, 081078B0, tile-trigger, NONMATCHING
