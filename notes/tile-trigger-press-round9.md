# TileTrigger_StepPress

All 344 bytes match using the shared frame-sequence helper. A separate pointer
to the sprite preserves the input across the press callback. The trigger gets
30 frames before it sets state 5 and marks entry into that state.

## TileTrigger_ApplyContactEffects

All 936 bytes match. The scan preserves each nearby player's contact flag and
switches eligible actors to the sink callback. For a controlled actor with
motion flag 0x2000 it emits three parts at the registry's effect origin,
then clears the two actor timers.

Keeping the callback in a local before the scan lets agbcc load it at the
point of installation. The particle helper uses the same pointer for the
random table during sampling and the effect group during emission.

Tags: 08106E98, 081078B0, tile-trigger, matching
