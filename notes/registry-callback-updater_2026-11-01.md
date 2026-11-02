# SceneActorRegistry_UpdateActors

Stock agbcc matches all 1,164 bytes at `0x08070FA8`. The guarded C started at 66.7% in the incoming round-10 kit and reached 88.4% when the particle loop's field-address spills lined up. The final function uses ordinary C.

The update callback reads the actor's callback slot before loading its context. Both the missing-object check and the engine-pause check then capture the motion field at actor+0x2D4. The ROM keeps this pointer in `sl` across the particle loop. Narrowing each velocity component to a signed halfword before writing its packed field gives the compiler the required register allocation. Naming the first horizontal mask reproduces the subsequent random-index reloads.

The particle check and manager palette check share the 0x2000 mask. The action-word address at actor+0x2CC stays in the final spill slot at sp+0x40. Reusing the flag-field pointer across halfword motion tests and word palette updates preserves that spill and lets the two palette branches compute actor+0x2D0 locally. The regional flags test, region comparison and regional counter access share the game-state getter.

The stock checker verified the full function extent. Differential execution on 160 bounded actor lists reached all 92 ROM blocks. The cases cover an empty list, removal during traversal, individual regional-deactivation gates, particle emission and both palette directions. Run the companion `round10a-registry-fixtures.py` from a provisioned checkout to repeat the comparison.

Tags: 08070FA8 SceneActorRegistry_UpdateActors agbcc register-allocation palette particle
