# OrbitEffect_UpdateManager (081FE2C4): exact match

Tags: #matching #orbit-effect #compiler #loop #giv #gcse #pre #reload

**Exact match in ordinary C.** `OrbitEffect_UpdateManager`, 896 bytes,
compiles byte-identically (`tools/check.py src/fn/sub_081FE2C4.c`: MATCH, 0
differing bytes). It has no register pins, barriers, inline assembly or compiler
overrides. Commit `d0f08886f` on `codex/opus-ratatosk-defeat`. The unit's
guarded draft was at 77.9% (80 ROM / 78 draft instructions differing in scope,
shape 64). Round 8's best, `build/handoff/frontiers/damage-and-orbit/`, was at
60 / 58 (shape 52). 27 drafts were compiled, besides the unit's and round
8's two. They are kept in `build/scope/drafts/oe/` (worktree scratch, not
committed). The scope change from this session is commit `1acd301de`.

## The deciding source properties

1. **One stepping pointer.** Every slot field is read through `effect`:
   `for (effect = manager->slots, index = 0; index < 4; index++, effect++)`.
   The unit's draft also stepped `motion` (`&effect->velocity`) and `state`
   (`&effect->flags`) as variables. The ROM's two extra stepping registers
   are loop.c's reduced givs.
2. **The collider position is `&effect->sprite.position`.** The movement
   branch keeps `&sprite->position`.
3. **`AddPosition` uses compound adds**: `out->x += in->x;` and the same for y
   and z.
4. **The homing loop hoists its origin.**
   `struct OrbitEffectPosition *origin = &target->position;` then
   `for (j = 0; j < 2; j++) OrbitEffect_SpawnHomingTrail(effect, origin);`.

Everything else is the unit's draft, simplified:

* The callback is a typed function pointer, called directly. agbcc emits the
  `_call_via_r2` thunk (`sub_08249240`) itself.
* The unused view structs and the copy in `Polar` are gone.
* `params` and `collider` are block locals.
* `continue` replaces `goto next`.

The flag helpers (`ClearFlags`, `TestFlags`), `ReadScale` and the two pause
mask locals stay. They put each constant before its load.

## Mechanism

1. **loop.c makes the extra pointers.** Accesses to the velocity, the
   parameters and the sprite position are address givs of `effect`. They
   combine into one representative at effect + 0xC (r7). The flags and
   resource accesses combine into one at effect + 0x8C (sp+0x24). Their
   increments come before `effect`'s at the latch, which is
   `add r7; [sp+0x24] +=; add r6`. loop.c creates these registers after
   gcse's, so the flags giv takes the last stack slot. With `motion` and
   `state` as variables (the unit's draft and round 8), the state pointer took
   sp+0x20 and `index + 1` took sp+0x24, the reversed pair round 8 reported.
2. **gcse's PRE builds the pre-headers and the frame.** PRE inserts these at
   the end of each trail loop's pre-header (`scope.py FILE.c pre`):
   * the scale pointer;
   * the four scale bytes;
   * the collider and its position;
   * `index + 1`.

   It creates their reaching registers in hash-bucket order. For
   `(plus (reg effect) (const_int O))` the bucket is (13772 + 23 + O) mod 175:
   0x3A4 → 27, 0x3C → 30, 0x3D → 31, 0x50 → 50, 0x68 → 74, 0x69 → 75,
   0x8C → 110 and 0xA0 → 130, then `index + 1` → 147. The spilled ones take
   stack slots in that order: sp+8, sp+0xC, the position at sp+0x10, sp+0x14,
   sp+0x18, flags at sp+0x1C and `index + 1` at sp+0x20. In the pre-header,
   reload's move2add turns the position's `r1 = r6 + 0x50` into
   `adds r1, #0x13`, because r1 still holds effect + 0x3D.

   With `&sprite->position` at the collider, the expression is the movement
   branch's sprite + 0x1C. gcse merged the two and computed the pointer
   before `movementEnabled`, which spilled it (the 2026-10 campaign report's
   first gap). `&effect->sprite.position` is a separate expression, effect +
   0x50, and PRE puts it in the pre-headers.
3. **The dead zero decides the spill list.** Each AddPosition store expands
   through store_bit_field: `(and (mem) 0)`, then an IOR. cse shares that
   HImode zero with the clamp stores, and USEs keep it live for 60 insns. The
   ROM has it as `movs r2, #0`.
   * With the unit's helper (u32 temporaries, a y block), y's loads come in
     the other order, the zero gets r3, and no insn's find_reg picks r3. The
     spill list is r0 to r2, so reload's rotation runs modulo 3 rather than 4,
     and almost every reload register moves (d1: 130 / 126 differing, but
     shape 20).
   * With compound adds, each `out` load is the folded operand that reload
     loads right before its add. The zero gets r2. The y and z reloads then
     need r3, which joins the list.
4. **The homing origin must be a variable.** With `&target->position` written
   in the call, the address is computed inside the loop (12 / 12). As a
   variable it is hoisted before the loop, as in the ROM. The ROM counts j
   down from 1 because loop.c reversed the loop. A `for` loop and the unit's
   `do { } while (--j >= 0)` both match.

## Ruled out (this session)

Numbers are scope's ROM / draft differing instructions.

| Draft | Change | Differing |
|---|---|---|
| The unit's draft | `motion` and `state` stepped as variables | 80 / 78 |
| Round 8 (fa) | per-branch captures of the scale and position pointers | 60 / 58 |
| Round 8 (fb) | fa plus a captured `homing` pointer | 60 / 60 |
| d1 | one pointer, `&effect->sprite.position`, typed callback | 130 / 126 (shape 20) |
| a1 | d1, y written like z | 129 / 125 |
| a2 | d1, `out->x = out->x + in->x` for all three | 12 / 12 |
| a3 | d1, compound adds | 12 / 12 |
| a4 | d1, `in->x + out->x` | 18 / 18 |
| h1 | a3 with the origin local | MATCH |
| h2 | h1 with the unit's do-while | MATCH |
| h3 | a3 with `for (j = 1; j >= 0; j--)` | 12 / 12 |

Simplifications of the match. These still match:

* no view structs;
* no copy in `Polar`;
* `params` with an initializer;
* a `for` loop with `continue`, with `index <= 3` or `index < 4`;
* the collider pointer declared at the top.

These don't:

| Variant | Differing |
|---|---|
| All three flag helpers as plain `&=` and `&` | 8 / 10 |
| Only the first `ClearFlags` plain | 2 / 4 |
| Only `TestFlags` plain | 1 / 1 |
| Only the last `ClearFlags` plain | 5 / 5 |
| `Truncate(effect->scale)` for `ReadScale` | 110 / 108 |
| Plain constants for both pause masks | 9 / 9 |
| Only the 0x400 mask plain | 8 / 8 |
| Only the status mask plain | 1 / 1 |
| `TestFlags(&gUnk_02000560, 4)` for the status check | 1 / 1 |

Earlier rounds: `notes/finish-effects_2026-10-30.md`,
`build/handoff/codex-evidence/effects/scope/campaign/sub_081FE2C4/REPORT.md`
(PRE's merged position, `AGBCC_SPILL` and `AGBCC_NOPRE` diagnostics) and
`build/handoff/frontiers/damage-and-orbit/README.md`. All of them kept the
`motion` and `state` pointers.

## Scope gaps

* **A spill list of the wrong size.** `spill` on d1 found only partial
  rotation shifts (110 at best in 42 compiles). The real cause was that the
  ROM rotates over four registers. `spill` now says so when the ROM reloads
  into a low register that the draft's list lacks, and it names the draft's
  pseudos in that register (here `angle` and the dead zero `=0#1`). It also no
  longer crashes on a matching function (commit `1acd301de`). It doesn't yet
  find the insn whose find_reg made the ROM add the register.
* **Stepping variables against reduced givs.** As in
  `notes/opus-scene-projectile-pool_2026-10-31.md`, nothing flags user
  variables that step with the loop's pointer. A latch whose increments all
  use the same constant, with the BIV's last, means the others are reduced
  givs.
