# Scene NPC state effect

Tags: #matching #scene-actor #particles

SceneNpc_OnStateEffectUpdate at 081D0928 projects the attached display position, advances a 128-frame triangular palette pulse, and emits a particle every fourth frame while attached. The first two random samples control velocity and the third sets a lifetime from 15 to 30. The Q12 products truncate toward zero.

The guarded 608-byte C candidate has 92.0% similarity against the 612-byte ROM body. The palette-frame mask and the transfers between the saved random origin and the velocity pointer account for the remaining differences. A differential Thumb check compared actor and display memory, the random cursor, and external calls across all 256 combinations of two activation flags, two actor states, the palette flag, eight frames and two height inputs.
