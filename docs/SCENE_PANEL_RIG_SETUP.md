# Scene panel rig setup

The reviewed setup cohort is `0805AD18–0805B28C`. It operates on the same
15-sprite rig as the preceding state handlers: one main sprite, eight primary
sprites, and six secondary sprites, each occupying `0x68` bytes. The next
function is an unrelated X/Z proximity helper. The setup code uses the shared `SceneRigSprite` and collider layout;
`scene_panel_rig_setup.h` retains partial parent and payload views.

`0805AD18` removes all fifteen sprites. `0805AD50` initializes the collider at
rig offset `0x620` with dimensions `(128,128,128)`, offset `(0,0,0)`, and owner
position at `0x618`. Packed position padding is preserved. Its callback at
`08059428` receives two contact pointers and the rig owner. The supplied power
is passed to the collider's power/reaction/type setter, followed by refresh.

`0805ADE0` initializes the main sprite with graphics resource hash `ect`, flags
zero, animation frame zero, render mode one, and the current rig position. Its animation
resource uses lookup key `0x922E` with hash `ect_anm`; playback starts at sequence
zero. `0805AF88` initializes six secondary sprites: flags alternate 1024/512,
the first sprite uses render mode two and the others one, phases are `i * 32`,
and animation sequences are `5,4,4,3,3,2`. One random sample supplies each
sprite's low-byte random phase. Odd sprites additionally set flag four.

`0805AE58` initializes the eight primary sprites with the same graphics and
animation lookup. Initial flags alternate 1024/512, animation frame is zero,
render mode is one, current position is copied, and phase is `i * 32`.
Each iteration consumes three fresh random samples: low byte for random phase,
`Mod(sample,3) + 1` for animation sequence, then low bit to conditionally set
flag four. Each sample advances and writes the shared index before its table
access. Calls between samples can change that index, so later reads remain
fresh.

`0805B098` calls the three sprite initializers. `0805B0B4` sets the two packed
payload vectors at `0x6A0/0x6A8` to `(160,160,160)` and `(64,32,64)`, preserving
their padding. Its flat halfword view retains the observed halfword stores.

`0805B0F0` initializes the rig embedded at parent offset `0xC6C`. It copies the
parent center at `0x510` into the origin, subtracts 32 from X and Z, copies the
origin into the current position, and adds 128 to current Y. It advances the
random index, calls `Mod(sample,600)` while discarding the result, advances the
index again, and initializes the sprites. Keyword `L` supplies collider power,
defaulting to five. State becomes zero, angle 160, and turn frame zero.

Five subsequent random samples choose signed turn rate magnitude 8–23,
duration 192–255, speed 192–447, and vertical step 64–127. The shared random
index is written after samples two and five; samples three and four keep the
index local. Motion flag and distance become zero, the halfword at `0x69C`
becomes 600, payload vectors are initialized, and the parent pointer is stored.
The apparent unused initial modulus call and delayed index writes are retained.

All eight reviewed functions now match their full extents. New C covers
1,108 bytes and adds 0.0459 percentage points: main sprite 120 bytes in ordinary
C, plus primary sprites 304, secondary sprites 272, and rig initializer 412
with temporary concessions. Existing C extents typed here total 288 bytes and
add no credit. No function uses an explicit r7 binding.

The primary loop needs a short conditional back edge at the compiler's range
limit. Empty barriers inside it enlarge the compiler's length estimate despite
emitting no bytes. Two compiler-visible C labels preserve the offset and parity
copies instead; an empty read-only asm before the loop references their defined
addresses. There are no computed jumps and no handwritten instructions. The
counter's defined final values are constrained by an empty input asm after the
loop. Separating the animation hash evaluation from the resource lookup keeps
the resource key from occupying r7 across the hash call. Primary, secondary, and rig initializer hints were minimized to 20, 10,
and 7 respectively. Sprite offset `0x10` is animation frame. The driver shows rig offset
`0x68E` is a collider hold timer, not a sprite scale; it sets collider flag four,
counts down, and suppresses the position update while nonzero.
