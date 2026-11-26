# Scene camera receiver and registered reaction owner

Tags: #matching #types #phase3 #scene #camera

The registry passes the actor body initialized by `SceneBody_Init` to
the motion-mode callback at 08080BE0. `SceneActorBody.owner` at +0x3C
replaces the callback's padded view. The hit path uses the shared down
transition for its health, motion and reaction flag writes. All 980
bytes match. The scoped linter drops the parked zero and the explicit
halfword mask argument.

The camera task allocated at 082275D8 embeds a 0x34-byte
`SceneMessageReceiver` at +0x20. Initialization and destruction pass
that receiver to the scene message manager. The command path passes
its embedded queue to the engine and reads the declared dispatch phase
and command kind. At 08226930, indexing the packet halfwords from the
start of the command matches all 332 bytes. Taking a separate payload
base at +0x08 changes the allocation through the switch. The completion
wrappers use the declared mode and rate fields. The shared script
declarations replace an older unsigned prototype in the cue reader.
All 14 changed camera units and all 28 camera-header dependents match.

The steering actor pool loads its 0x1C-byte `ContactSwarmGraphics`
resource at +0x18 in 0806CE98. The travel and bobbing sprite paths at
0806C7B0 and 0806CA3C pass its address to the sprite initializer. All
three units and all 26 steering-header dependents match.

The direct down predicate and facing simplification at 08080BE0 change
the instruction shape. The timed burst at 08081C98 still has a separate
flags pointer whose lifetime spans effect setup. Trials that combine
its gate reset or simplify its heading or palette selection change
instruction shape or allocation. Both callbacks keep their exact forms
for those paths.
