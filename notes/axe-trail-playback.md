# Axe trail playback

Tags: #monsters #matching

Axe_EmitMotionTrail matches all 588 bytes without its five register pins. Each of the four
96-byte trail slots carries a sprite and a playback sequence. PlayerCue_StepSequence takes
those as separate arguments and reads the command table through the sequence. Recomputing
the sequence address from the slot index also reproduces the original register allocation.
