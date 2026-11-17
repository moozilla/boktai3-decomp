# Round 14: field-worker compiler-warning review

Date: 2026-11-17. Source baseline for these trials: `0ff9046b8` on
`codex/round14-fields` (the seven preceding commits remove the nine assigned
raw-field/volatile errors). **None of the stage 2 drafts below is a stock match;
no stage 2 production source or lint suppression was changed.**

The adjacent `round14-fields-natural-candidates.patch` preserves one ordinary
candidate for each of the nine assigned units. It is a research checkpoint, not
an accepted source patch. Apply it only in a disposable worktree. It removes the
identified compiler-warning forms, but does not claim to resolve every other
allocation-only construct already present in those functions.

Compilation used stock agbcc with the repository's ordinary Thumb options and
independent reference ROM. The scope compiler was used only to diagnose causes.
Numbers below count differing ROM / draft instructions, not bytes. A matching
code size does not imply matching bytes. Scope's shape score can count insertions
and deletions differently from its register-aware instruction pairing.

## Outcome by function

| Unit | Ordinary removal | Best or most informative natural trial | Remaining decision |
| --- | --- | --- | --- |
| `sub_08014CA4` / FragmentGroup_SpawnRadial | 106 / 106 | Global table sampler: 75 / 75 | PRE hoists two group-array bases; constant equivalence lowers table priority. |
| `sub_080154EC` / AnimatedParticles_Create | 5 / 1 | Correct external prototype produces the same result | Jump2 merges two sequence-setting call tails which the ROM keeps separate. |
| `sub_08016118` / FadingParticleGroup_Update | 43 / 43 | Removing the additional impossible lifetime guard: 48 / 46 | The clean expression reuses the earlier vertical-velocity load; pointer allocation also changes. |
| `sub_0804373C` / ContactActor_UpdateRetreat | 13 / 8 | Removing the associated origin, union and once-loops: 85 / 86 | A high-register spill choice and the dead symbolic-origin lifetime. |
| `sub_0805B0F0` / ScenePanel_InitRig | 12 / 12 | Typed samples, indices, mask and lookup helper all preserve that result | Constant-equivalent table loses r4 to mask. |
| `sub_080AB968` / TentacleActor_UpdateRetraction | 3 / 12 | A single advance call removes 18 ROM instructions | Four equal call tails do not all merge late enough to preserve both absolute-value calculations. |
| `sub_080AEE08` / Vorn_UpdateDefeat | 115 / 118 | Shared six-spoke burst helper: 133 / 130 | Local order of kind/spread/origin, global table eviction and reload rematerialization. |
| `sub_080BD1F0` / Skeleton_UpdateDefeat | 28 / 28 | Removing the two unrelated unions too: 178 / 168 | Shared origin takes sl, displacing the later table and removing its stack slot. |
| `sub_08234540` / SpiralBurst_InitParticles | 48 / 44 | Direct global sampler: 25 / 25 | The retained table copy, table/frame allocation, and two cursor stores. |

## Evidence and attempted natural structures

### Fragment groups

The three zero identities are the table formation and the `initialized[i]` /
`enabled[i]` bases. The direct replacements introduce PRE expressions for group
base + 0x20 and + 0x2c and spill their hoisted values. The table receives a
constant-equivalence lifetime penalty. On the ordinary draft, **diagnostic-only**
`AGBCC_NOPRE=1,2 AGBCC_NOEQUIV=table` reproduces all 704 bytes. This isolates the
three decisions; those options are not a source solution or build setting.

Tried actual byte destination pointers, a conventional indexed group/particle
loop following the updater's structure, manager-array addressing, global table
and global cursor samplers, narrow sample return types, and a multi-statement
advance/sample macro. None matched. The direct global sampler is closer but
leaves the two group-base PRE decisions. Five early candidates were also saved
in `build/matchsearch/round14-fragment` during the session.

### Animated particles

Removing the handle unions, impossible unsigned guard, and pointer-zero uses
leaves 1,164 bytes instead of 1,176. Two formerly separate `Anim_SetSequence`
call tails jump to the final branch's call. The real callee returns `u32` and has
`u16`, `u8`, `u8` trailing parameters; correcting the local prototype does not
change this result. A branch-local initializer helper gives 8 / 3 differences;
separate actual animation locals give 7 / 2; a combined initialize-and-step
helper expands too much playback code (146 / 337).

### Fading particles

The ROM reloads signed vertical velocity after the z-position store and then
subtracts two. Removing the zero pointer reassignment lets CSE retain the earlier
velocity load and changes pointer allocation (43 / 43, shape 4). Actual field
pointers for the destination and gravity store, packed signed velocity fields,
independent sprite/velocity/position cursors, a vector movement helper, and an
ordinary lifetime decrement were tested. They do not restore the ROM. The
additional impossible negative lifetime guard was removed in the preserved
draft as well; the result is 48 / 46, not an accepted match.

### Contact retreat

Removing only `origin & 3` leaves 13 / 8 differences. The symbolic origin is then
unused; the ROM and draft also disagree on the high register selected for a
spilled value. A full ordinary version removes the origin capture, the union
between a numeric mask and cursor pointer, and the one-statement loops, uses
`gRandomTable`, and keeps ordinary index advance/read operations. It produces
85 / 86 differences. A CSE canonicalization probe did not recover the ROM.

### Panel rig

Direct `table = gRandomTable` produces the right instruction sequence and only
swaps table/mask registers. Table pseudo 130 has 6 references and priority 1016;
mask pseudo 131 has 6 and priority 1153. The table's flow lifetime is 59
instructions, doubled to 118 by constant equivalence. A **diagnostic-only**
`AGBCC_PRI=table:+138` gives all 412 bytes exactly.

Tried actual u16 sample/index/mask types, a typed table lookup helper, separate
logical batches, and a direct global sampler. The first group preserves the
12 / 12 register mismatch; the global sampler has 16 / 16 differences and extra
cursor stores. The multi-statement macro experiment described below moves the
wrong lifetimes and is worse.

### Tentacle retraction

The ordinary pointer version preserves all four existing dominant-axis/sign
branches and removes every handle-union/unsigned-negative/zero-offset scaffold.
It produces 344 bytes versus the ROM's 324. Three call tails merge, but the final
fallthrough's flow USE differs and leaves another call and branches. Collapsing
the identical calls to one `Advance` also removes the two absolute-value
calculations (18 ROM instructions absent). The actual advance callee returns
void, so a fabricated return value is not justified. The original source
operation that explains the redundant axis work is still unresolved.

### Vorn defeat

The three identities manufacture extra uses of spark kind, spread and origin.
Without them, `spread` is locally allocated to r4 rather than r5; `origin` moves
to r5 instead of r6; the global table/work scalar takes r6 instead of r4. Reload
also changes rematerialization of constants. The real six-spoke pattern is
shared with Skeleton: directions 0x20, 0x40, 0x5e, 0x80, 0xc4 and 0xf0, each with
a fresh random count 1..4. A shared inline burst helper and an actual
random-count-plus-spawn statement macro were tested; they produce 133 / 130 and
146 / 145 differences respectively. Neither was introduced into shared headers.

### Skeleton defeat

Removing the two impossible negative arms, the zero sound argument and the dead
actor-to-table initializer keeps 7,924 bytes but changes 28 instructions on each
side. The shared position origin takes sl instead of a stack slot; the stack
frame shrinks from 0x9c to 0x98. The later table consequently spills. Separating
the context/one and flags/one unions into their actual semantic values gives
178 / 168 differences. Including the real shared SparkParticle prototype gives
identical output to that separated version. The preserved patch includes the
union separation, so its expected result is 178 / 168.

### Spiral burst

Removing all three table identities leaves 48 / 44 differences. A direct global
sampler reaches 25 / 25 at the same 428-byte size, but omits the retained table
copy, swaps frame/table r5/r6 and stores two intermediate global cursor values
which the ROM keeps only in registers. An actual advance/sample macro with a
direct global table gives 53 / 53; a cached table version gives 68 / 64. The
existing unrelated sine-pointer/sample union is not resolved by this review.

## Shared RNG operation audit

There is concrete repeated source evidence for an index advance followed by a
table sample: `contact_burst.c` contains both statements in one do/while block;
`ScenePanelRig_TickWandering`, `ScenePanelRig_RandomizeMovement`, projectile
impact and the particle groups repeat the same operation. Accordingly, trials
used a conventional statement macro with two real operations:

1. Advance the supplied cursor modulo 1024.
2. Read the table at the new cursor into the requested output.

Both global-cursor and locally batched-cursor forms were tested. This differs
from merely wrapping one assignment to raise its reference weight. On the rig,
the genuine operation instead assigns the table to r7 and input to r8; on
fragments it does not resolve PRE; on spiral it leaves the table/cursor issues
above. These trials do not establish the exact original macro. No private
weighting wrapper, new identity, diagnostic override or suppression was accepted.

## Reproduction

From a provisioned worktree, put the adjacent patch into a disposable branch and
use the ordinary `tools/check.py` or `tools/scope/scope.py` file checks. For the
first function, ROM inventory uses `sub_08014CA4`; the C source aliases its
readable name with a macro. The runtime drafts for this session are under
`build/round14-fields`, and scope reports under `build/scope`. Those ignored
outputs are convenient diagnostics, not the durable record: this note and the
adjacent patch are tracked.
