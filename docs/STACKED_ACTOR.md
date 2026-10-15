# Stacked display actor (tentative identity)

`08052130–080537D0` is one coherent actor: allocation `0x388`, singleton
`gUnk_02000118`, one base sprite, a shadow, three stacked display segments,
and eight particles. The descriptive `StackedActor` name does not establish its
in-game identity. Evidence below is static ROM/C review; no execution coverage
is claimed.

`include/stacked_actor.h` records the shared layout. Sprite records are `0x2C`
bytes; particle records are `0x34`. The actor's receiver starts at `0x18`, world
position at `0x98`, height at `0x1C0`, particles at `0x1C8`, five script resource
IDs/arguments at `0x36C–0x37C`, frame counter at `0x382`, and update callback at
`0x384`. Script IDs are integers passed to `Script_Call`, not pointers to script
contexts. Position records contain four signed halfwords and are aligned to four
bytes. The final position halfword is copied with the other coordinates.

The task tick and destructor return `s32` zero; state callbacks return `void`.
The creator passes `(actor, tag, unused)` to the initializer and kills the task
when initialization returns a negative result. The initializer currently returns
zero on every observed path, including an early tether-rejection path before
publishing the singleton. That unusual path is preserved.

The initializer reads `p` for X/Z, then recomputes Y from terrain cells, including
cell-type slopes and the optional cell override. It configures resource `0xBA8`
for the base/shadow/segments and `0x1C1E` for particles. Keywords are `C` pending
script, `c` accepted script, `i` completion script, `o` particle script, `n` its
single argument, `d` base frame (default 6), `a` interaction enabled (default 0),
`P` render mode (default 3), `D` initially disabled, `f` initially active,
`s` appearance/removal behavior, and `w` tether requirement. Keyword meanings are
provisional descriptions of the consumers. Receiver registration uses the tag
and type 10.

The normal idle callback tests a 96-unit X/Z radius, raises lift toward 24 and
animation phase toward 8, and starts player interaction after ten idle frames
when the input and player gates allow it. `player+0x457` is tested for states
`0x1D` and `0x1E`; this field is distinct from actionMode at `0x418`. The tether
predicate requires manager mode 4, phase at most 4, Y separation at most 255,
and squared X/Z separation at most 144400. Its raw coordinate comparisons are
preserved rather than replaced by extra absolute-value tests.

Height advances through seven steps. The three display segments appear in
pairs of height steps, spaced by 342 units, with intermediate offsets of 171.
The fully extended base selects frame `baseFrame+8`. State chains extend the
segments, wait, emit particles, and retract. The script interaction emits eight
falling particles every four frames; the player-release chain emits eight rising
particles and calls the player release helper at frame 16. Retraction can return
to idle or enter a 64-frame disappearance and kill the task. The appearance path
scales and rotates both base and shadow before settling at the world height.

Each particle lives 24 ticks. The updater decrements life, hides/disables at zero,
otherwise adds the signed velocities, selects animation frame `2+((life>>2)&1)`,
and damps each velocity by signed `15/16`, truncating toward zero. Both particle
initializers copy all eight position bytes, consume four successive masked RNG
indices, and publish the final index after the fourth. X/Z vary by `(sample&31)-15`.
Falling particles add a random byte to Y and start with Y velocity
`(sample&31)+24`. The remaining rising initializer starts 768 units higher with
negative Y velocity `-((sample&31)+32)`; it remains an assembly gap pending exact
register allocation, and receives no matching credit.

The message receiver reloads the current manager selector and message count
through the loop, consumes every message, and maps type 6 subcodes 5/6 to lift
raise/lower callbacks. The task then dispatches the state callback and ticks all
eight enabled particles. Destruction removes base/shadow/segments/particles,
unregisters the receiver, and clears a nonzero singleton.

`08052E04` is a hidden 124-byte helper following the 164-byte `08052D60`
particle emitter. Its control flow retracts the actor and selects idle versus
disappearance; it is a separate reviewed function, not data in the emitter. The
integrator records it in both extra-function inventories.

Current new exact work: 17 functions, 4,344 bytes, +0.1798 percentage points of
the `0x24DC0C` denominator. Of these, 1,936 bytes are unpinned and 2,408 retain
minimized matching concessions. Sixteen preexisting exact helpers totaling
1,272 bytes were named and converted to the common layout without byte credit.
All changed functions were checked over their full reviewed extents. The worker
has not run a combined ROM build; the integrator performs that batch check.
