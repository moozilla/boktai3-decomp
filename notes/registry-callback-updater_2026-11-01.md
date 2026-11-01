SceneActorRegistry_UpdateActors remains guarded at 87.7% with stock agbcc, up from 66.7% in the incoming draft. Scope finds 73 ROM and 67 draft instructions different, down from 157 and 163. The draft is twelve bytes shorter than the 1,164-byte ROM function.

Taking the update-callback address before loading its context fixes the invocation sequence. The missing-object test is evaluated before capturing the motion flags. The regional deactivation path and the palette transition path both use the motion and adjacent state words.

The 0x44-byte frame, particle-counter slot at sp+0x2c, and velocity-pointer spill at sp+0x30 agree with the ROM. The field-address slots for actor+0x47c and actor+0xaa remain reversed at sp+0x34 and sp+0x38. Scope's PRE model puts the first viable bucket count at 245; the draft uses 243. The ROM also recomputes the motion-flags address after the pause test, while the draft keeps the first computation.

Both binaries returned identically over 32 bounded-list emulations covering 50 of 72 ROM blocks. Null heads, sentinel-only lists and chains of one to five actors exercised update and destroy callbacks, particle emission, and palette changes. The stock random-memory trial 0 enters a six-node cycle via address 0x6 and exhausts the emulator's instruction cap.
