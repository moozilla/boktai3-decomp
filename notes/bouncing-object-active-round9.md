# BouncingObject_TickActive

All 1,584 bytes match with stock agbcc. Gravity reads the signed sprite height
before querying the terrain. After that test, the handler keeps the sprite
position and velocity pointers separately across the bounce and wall checks.
The candidate position adds velocity to position in a packed vector.

The two colliders use the object-owned position. A blink changes the sequence
but preserves its elapsed time and command index before advancing the shared
player-cue playback routine.

Tags: #matching #round9b #bouncing-object
