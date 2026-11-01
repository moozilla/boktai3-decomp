# NamedObject_CreateFromScript

The guarded C now scores 99.7% over the 1,856-byte function, up from 66.2%.
It advances the shared player-cue sequence and supplies its real 16-bit sequence
and 8-bit row arguments. The final proximity check receives the route cursor
and the body's world position, with the signed speed as both X and Z limits.
The sprite and route cursor reuse a pointer after the sprite's final x adjustment.

The 1,856-byte candidate retains every instruction with a live result after
realignment. It omits a redundant zero in r7 at +0x692 between loading the flags
and setting the terrain-registration bit. This shifts branch and literal offsets,
and the assembler emits padding at the end. The default build retains assembly.
