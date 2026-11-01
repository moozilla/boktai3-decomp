# Slime callbacks

Tags: 0808E838 #slime #depin

`Slime_UpdateRandomWait` matches all 360 bytes without pins or barriers.
The byte-sized playback flags after `Anim_SetKeyframes` keep the tile mask
copy emitted by the ROM. The neighboring wait animation uses a word-sized
test and omits that copy.

Tags: 0808E2CC #slime #contact #depin

`Slime_ResolveBodyContact` matches all 640 bytes after restoring the
fallback callee's third argument. `SceneActorBehavior_ApplyContactReaction`
consumes the same actor that the availability check just tested. Keeping
that pointer for the call naturally preserves r2 across the contact flag
load and removes the two pins and the barrier.
