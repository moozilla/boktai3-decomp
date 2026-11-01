# Scene NPC state effect

Tags: #matching #scene-actor #particles

SceneNpc_OnStateEffectUpdate matches all 612 bytes. The NPC projects its attachment's position, advances a 128-frame triangular palette pulse, and emits one particle every fourth frame while attached. Two random samples choose its velocity and a third chooses a lifetime from 15 through 30.

The four-frame test takes the bounded palette clock. Capturing the horizontal sine sample before scaling gives the sine table and speed their original registers. A copied sample-origin value stays in a saved register until the velocity pointer takes its place. The literal's offset is measured from the physical random table, as in the other particle sample readers.
