# Temporary animations and contact-burst effect, October 14

Continue in ROM order through **08047BB4–080483A0**. The temporary animation
pool and contact-burst controller are fully exact; the directional-particle
callback is exact and its spawner is a guarded draft.

Exact code rises **23.746% → 23.807%**, **+0.0604 percentage points**
(+1,460 bytes, ten new functions). There are **6,463 / 11,471 functions**
and **575,018 / 2,415,354 exact bytes**. Code without register pinning is
**19.432%**; Register-pinned is **4.375%**, included above. Seventy-three
guarded drafts remain excluded from exact credit.

| Family | New exact functions | New bytes | Existing functions typed |
| --- | ---: | ---: | ---: |
| Temporary animation pool | 4 | 444 | 2 |
| Directional particles | 1 | 56 | 0 |
| Contact-burst controller | 5 | 960 | 4 |

Four new functions need no hints. Six retain minimized concessions:
pool initialization (three hints), entry configuration (one), particle update
(six), contact callback (one), contact animation (three), and contact-controller
update (two). `tools/depin.py --write` minimized every hinted exact candidate.
The initializer's final call-argument ordering was fixed by removing the
resource pointer's r8 pin. No handwritten instruction bodies were used.

## Temporary animation pool

The singleton at **gUnk_020000A8** allocates **0x678 bytes on task list 10**.
Sixteen **0x64 slots** begin at 0x38, following the task and a 32-byte relocated
resource. Each has an active byte, a signed 16-bit lifetime and a **0x60
animation entry**. Initialization obtains resource **CB05/DE23**, copies and
relocates its header, clears an eight-byte local position and initializes each
animation with options **0/0/2/0/60**. Missing resource or negative animation
initialization result fails task creation.

An active slot decrements its lifetime each update, removes its animation at
zero or below and clears active. Destruction checks the animation's separate
**registered byte at slot offset 8**, removes registered entries and clears the
singleton. Finding an available slot tests the pool's **active byte at offset
0**. These two notions of liveness are distinct.

Entry creation selects a sequence with argument 27, ORs caller flags, optionally
sets a positive palette index and palette-data pointer, then registers on list
one if flag 128 is set or list zero otherwise. A missing explicit palette uses
**gUnk_030042E4 + index * 32**. The lifetime argument truncates to the stored
signed halfword. The shared `ProjectedDisplayEntry` now exposes flags at 8,
palette index at 0x3A and palette-data pointer at 0x48; its 0x60 size is unchanged.

## Contact-burst controller and particles

`include/contact_burst.h` describes a **0xC4 task on list 8**. The name and exact
actor identity remain tentative. A static owner constructor near **0802C5xx**
passes its position at owner offset 0x8A4 and stores this task at **0x90C**, next
to the contact-sequence pool and orbiting contact actor. The controller holds
an enabled byte, previous enabled byte, contact timer/direction, animation
frame counter, fade byte, caller-owned world-position pointer, graphics at
0x28, sprite at 0x44 and collider at 0x70.

Initialization uses graphics **4147**, sprite flags **0x211**, palette index
**0x164**, idle frame 27 and priority one. Collider flags are **0x4005**, bounds
**(96,128,96)** and offset **(0,128,0)**. Packed coordinate initialization
preserves unspecified padding. Its original success path leaves r0 from the
final void `Collider_Refresh` call; the creator tests only its sign. This is
retained under the worker playbook's stale-result pattern rather than assigning
a new success value.

Enabling starts fade at one and sound **3B6**. The sprite projects the world
position with signed truncation toward zero:
`x=3*(world.x-world.z)/32`, `sum=3*(world.x+world.z)/32`,
`vertical=3*world.y/32`, `y=sum-vertical`, `depth=sum+vertical`.
Camera offsets and screen center **(120,80)** are applied, then depth increases
by 50. Fade rises by two toward 64; sprite flag two stays set while rising,
and clears when capped. A pending contact temporarily disables the collider
with flag four; otherwise the collider follows the world position.

A collision sets an **eighteen-update timer**. Flag 256 in the originating
collider uses its stored direction byte; otherwise `Math_Atan2` uses the other
collider's x/z displacement. Idle animation alternates frames 27/28 every four
updates. Contact animation chooses a pair from **32/33, 34/35, 36/37** every
eight controller updates, and plays **1BF or 1C0** when the contact timer is a
multiple of eight. The frame-selection sample uses the RNG high byte; sound
selection uses its low byte.

On an enabled-to-disabled transition, eight particles are spawned with angles
`index*32 + (random & 15)`, frames **29–31**, then sound **3B7** plays. The
sprite is hidden and collider disabled. `include/directional_particles.h`
recovers the shared sprite-pool position at 0x28 and appended motion at 0x6C:
velocity occupies eight bytes and lifetime follows at 0x74. Its exact callback
adds velocity into the three halfword coordinates and requests removal when
pool age reaches lifetime.

The **08047DF4 spawner** remains a **53.3% guarded draft**: lifetime **10–17**,
speed **24–31**, sine-table velocity scaled by 4096, zero y velocity,
owner-relative offsets **x/z [-64,+63]**, **y [16,47]**, and the resource/frame/
palette/activation calls are recovered. Stack and register allocation differ.
The original additionally materializes an unused **0x0403B714** literal into
ip and copies it to r5 before overwriting it; its source-level reason is
unresolved and the draft does not manufacture a use. No exact credit or new
emulator claims.

Next contiguous work starts **080483A0–080485FC**, the singleton RFU link-strength
indicator. Its consumers read `RfuLinkStatus.strength` and connected-slot masks,
so the subsystem identity is supported by the recovered SDK layout. Continue
toward **25% exact or 60% usage remaining**; the latest allowance is **78% remaining**.
