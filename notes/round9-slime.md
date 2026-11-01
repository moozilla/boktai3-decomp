# Slime callbacks

Tags: 0808E838 #slime #depin

`Slime_UpdateRandomWait` matches all 360 bytes without pins or barriers.
The byte-sized playback flags after `Anim_SetKeyframes` keep the tile mask
copy emitted by the ROM. The neighboring wait animation uses a word-sized
test and omits that copy.
