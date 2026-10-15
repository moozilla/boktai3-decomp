# Scene panel rig setup

The reviewed setup cohort is `0805AD18–0805B28C`. It operates on the same
15-sprite rig as the preceding state handlers: one main sprite, eight primary
sprites, and six secondary sprites, each occupying `0x68` bytes. The next
function is an unrelated X/Z proximity helper. `scene_panel_rig_setup.h` adds
local setup views without changing the integrator's shared rig structure.

`0805AD18` removes all fifteen sprites. `0805AD50` initializes the collider at
rig offset `0x620` with dimensions `(128,128,128)`, offset `(0,0,0)`, and owner
position at `0x618`. Packed position padding is preserved. Its callback at
`08059428` receives two contact pointers and the rig owner. The supplied power
is passed to the collider's power/reaction/type setter, followed by refresh.

`0805ADE0` initializes the main sprite with graphics resource hash `ect`, flags
zero, scale zero, render mode one, and the current rig position. Its animation
resource uses lookup key `0x922E` with hash `ect_anm`; playback starts at sequence
zero. `0805AF88` initializes six secondary sprites: flags alternate 1024/512,
the first sprite uses render mode two and the others one, phases are `i * 32`,
and animation sequences are `5,4,4,3,3,2`. One random sample supplies each
sprite's low-byte random phase. Odd sprites additionally set flag four.

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

Exact new extents currently committed are main sprite 120 bytes (ordinary C),
secondary sprites 272 bytes (10 minimized hints), and rig initializer 412 bytes
(7 minimized hints). These 804 bytes add 0.0333 percentage points. Existing C
extents typed here total 288 bytes and add no credit. The primary eight-sprite
initializer remains a matching investigation and is not credited. No function
uses an explicit r7 binding.
