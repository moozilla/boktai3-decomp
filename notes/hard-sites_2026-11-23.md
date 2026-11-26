# Hard sites, 2026-11-23

Tags: #matching #compiler #phase3 #linter

One session on the 36 priority sites (dead-code, proved-zero, volatile). Two fell, one detector
false positive is fixed in source (needs a compiler rebuild), and the rest have a sharper diagnosis.

## Fell

| Function | Site | Form |
|---|---|---|
| `Text_FormatDecimal`, 0803209C | `volatile int value` | The terminator is written through the advanced cursor: `*out++ = digit + '0'; *out = 0;` instead of a separate `end = out + 1`. gcse turns the trailing increment into `R = out + 1; out = R`, a second set of `out` that is a plain copy. alias.c `record_set` then drops `out`'s base value, `base_alias_check` can no longer prove the `'-'` store misses the stack slot, and cse2 keeps both reads of `value`. |
| `SceneInput_TickChoice`, 0805E1D0 | zero term `1 + (keys >> 16)` | Plain pad tests that each read `gPadState[1]` (`if (gPadState[1] & A_BUTTON)`), no `keys` local. The u16 load stays an HImode pseudo read through a subreg, which regmove does not tie, so the first and copies the SImode 1 (the andsi3 expander's copy of the constant) and cse shares that 1 with both `widget->flags |= 1` (README fact 37). A `u16 keys` local is a promoted SImode register and loses this. |

Lesson from both: the steering form compensated for a draft that differed elsewhere. Rewriting the
function from the headers in the plainest form and reading the diff was faster than editing the fake.

## Detector: Vorn's `flags->bits &= ~2` (confirmed false positive)

The reported operation is `(and:SI (const_int 0) (reg))`: store_bit_field's `(old & 0) | new` for a
store that replaces a whole halfword bit-field, with a zero pseudo substituted by combine. The
statement happens to contain `&`, so the operator filter let it through. `tools/scope/agbcc.patch`
now reports an and with a literal zero operand as `and:0` and `tools/deadcode.py` drops that detail
(`tests/test_deadcode_own_mask.py`). A zero the C did prove still shows: `g->bits &= x >> 8` reports
`ashiftrt:8` at the same line. Checked with a private build of the patched compiler (the patch applies
and gives `and:0` for vorn_main.c lines 1914 and 2189). **The shared `build/scope/agbcc` was not
rebuilt** (other workers use it): run `sh tools/scope/build_agbcc.sh` in the main checkout; until then
the warning stays.

## Did not fall

### Random table family (`FragmentGroup_SpawnRadial` 08014CA4, `SpiralBurst_InitParticles` 08234540)

* Fragment, plain rewrite (`for` loops, `g->field[i]`, an inline `Random(u32 *rng)` with
  `rng = &gRandomIndex` set next to `j = 0`, the table read directly, `(s32)(u8)Random() <= 128` still
  to settle the last `bgt`): `AGBCC_NOEQUIV` on the table pseudo gives 1 ROM / 1 draft. The rng address
  must be set outside the outer loop (neither loop.c nor LCM-PRE puts it there: the inner loop takes it
  first, and loop.c refuses a second hoist of a set after a branch that is used across blocks), and it
  matches with its REG_EQUIV doubling. The table's priority window is (3412, 4239): undoubled 4121,
  doubled 2060.
* The decision is in update_equiv_regs (local-alloc.c). A single-set pseudo whose insn has a
  REG_EQUAL constant note always becomes REG_EQUIV and has its live length doubled. loop.c's
  move_movables adds the note itself (`m->move_insn`), and cse2 adds it to every constant set and to
  every copy of a register it knows constant. Ways out, all checked in the source: (1) an earlier
  (insn order) non-constant set, CLOBBER or SUBREG set of the same pseudo; (2) preferred class
  STACK_REG with a MEM source, or a REG source whose preferred class is STACK_REG. (2) is ruled out:
  the ROM's uses are `adds r0, r0, rT` (alternative l,l,l), so regclass can only say LO_REGS.
  A pointer global, a `const` pointer (decl_constant_value folds it), an inline parameter (const or
  not), an inline return value and an outer `table` local (cse1/gcse cprop rewrite the uses to the
  outer register, which then lives across both loops and gets no register) are ruled out.
* SpiralBurst shows the same thing from another side. The ROM loads the table first (before the
  cursor), keeps it in r5 for the first draw and copies it (`adds r6, r5, #0`) at the loop preheader.
  A draw through an inline with a table parameter gives that order, and loop + cse2 produce exactly
  that copy (`(set B (reg A))`) but with a REG_EQUAL note, so B is REG_EQUIV; `AGBCC_NOEQUIV=B` gives
  the ROM's size (428) with 40 register differences left, `NOEQUIV=A,B` 31/29. So the original's
  preheader copy had no constant note: cse2 did not know A's value there, or the copy was made after cse2.
* Not tried: a statement macro with `do { } while (0)` (flow counts its mentions one loop deeper); it
  cannot reach the window alone (22 refs, 29 to 31 needed).
* 0x0403B714 = gRandomTable + 0x02000314 = 2 * 0x0201DB8A. No new result.

### `TargetRegistry_RegisterActor` / `RegisterSceneActor` (0815F6A8, 0815F6F0)

Written from `target_registry.h` with the global read everywhere (`for (q = gSceneLinkList->first; q;
q = q->next) if (q == n) return 1; n->next = gSceneLinkList->first; gSceneLinkList->first = n;`) the
function is the ROM except the reload after `n->next = ...` (and the mask-before-load order of the
flag update, which the existing `andm`/`orm` helpers give). The decision is alias.c true_dependence,
called from cse's invalidate, and two conditions must both hold for the store to kill the global:
* the alias sets match: c_get_alias_set gives every pointer type its own set, so
  `struct MotionTarget *` (the `next` member) against `struct MotionTargets *` (the global) never
  conflict; equal types or set 0 (an access through a union member) are needed;
* the last rule: a store to a struct member at a varying address cannot alias a scalar at a fixed
  address. So the global must itself be in an aggregate (MEM_IN_STRUCT_P), or the store must not be a
  member store.
Also: the `n->target`/`n->onTarget` stores must not kill the global (the ROM reuses r1 for the first
`->first`), so the global is not alias set 0 unless the first part caches it in a local. A consistent
original: the registry pointer is a member of a global aggregate with the same pointer type as `next`.
No declaration was changed; ten units use the global.

### `GameState_SelectRamBase` (0821B084)

`u16 timer = REG; u8 *base = gGameStateStorage; base += (timer & 127) * 8;` is one operand order off
(`adds r1, r0, r1` for `adds r1, r1, r0`). cse.c fold_rtx swaps a commutative operation when
const_arg0 is known and const_arg1 is not, in cse1 and again in cse2, and the function is one block,
so `base` cannot be a register cse knows constant at the add. Ruled out (all 1 off): u16/u32/s16
timer, a u16 offset local, shifts for the multiply, `&base[...]`, an integer base. An inline that
returns the sum, or takes `&base`, gives the right order problem a different allocation (the result
ties to the index, 15 off). Array-index spellings load the address after the mask (15 off).

### `Hresvelgr_UpdateBoundaryBarrage` (081A0BC8)

`normalCollisionMask` is not a variable: 0x2F0 is also the offset of `actor->action`, and cse-follow
shares the constant from `&actor->action` into case 2's `actor->collisionMask = 0x2f0`. Written that
way, with `direction = actor->direction4 >> 1`, the function is 2 insns off: the 0x3FF mask of the
two draws takes sl where the ROM has ip. find_reg's pass 0 reuses sl because the 0x2F0 pseudo is not
live in case 1. The ROM needs a pseudo in sl that conflicts with the mask across the draws; no
ordinary use of 0x2F0 after the draws was found (later `&actor->action` uses become copies of r7 in gcse).

### Not started

`WirelessResultPanel_Init` and the other dead-code sites.

## Second attempt (same day): the three `volatile` globals

None fell. The diagnosis above was incomplete; the missing condition rules out every declaration
of a symbol global, so no header was changed.

### New compiler fact (lldb on `build/scope/agbcc/gcc/agbcc`, breakpoints on `true_dependence` / `base_alias_check`)

`init_alias_analysis` gives r0 the base value `(address:SI (reg r0))`: the argument-register entry
(`ADDRESS VOIDmode`) is overwritten by the `struct_value_incoming_rtx` entry, which is r0 on ARM and is
Pmode. `base_alias_check` treats a Pmode ADDRESS as a stack reference and returns 0 against any other
known base. So **a store through a pointer derived from the first argument never conflicts with a
load whose address is a known symbol**, whatever the alias sets and MEM_IN_STRUCT flags are. r1 to r3
keep the VOIDmode entry and do conflict.

Both families store through first-argument pointers: `n` in the two list insertions; `head`, `tail`,
`next` in `SpriteTrail_UpdatePositions` (the arrays are embedded in `p`, so they are `p + const`).
In the camera unit `volatile` buys the load staying in the loop: without it loop.c finds the load
invariant (all six `true_dependence` calls return 0 at the base check) and hoists it (31/39 off).

### Ruled out (scope, on the units)

* `TargetRegistry_RegisterActor`: head global as a struct member, a union member (alias set 0,
  in-struct), an array element: all 4 ROM / 4 draft, the reload missing. Toy check of the same body:
  union and same-pointer-type struct member reload only when `n`'s base is unknown (`n` assigned a second
  time from memory); with `n` a plain first parameter nothing reloads, including the node-typed
  member (`struct { struct N *p; }`, same alias set as `n->next`).
* A local `struct L **pp = &global` and an inline `push(&global, n)`: no reload.
* `SpriteTrail_UpdatePositions`: camera global as struct member or union member: unchanged, 31/39.

### What does produce the reload

The load's address must have no known base: `base_alias_check` returns 1 as soon as one side is
unknown. An integer address does that (`find_base_term (const_int)` is 0):

* `#define gSceneCamera (((struct EG *)0x02000000)->camera)` (member at 0x4B8): the loop reloads,
  304 B / 304 B, 11 insns differ by register only (ROM keeps the address and the 0x124 constant in
  r4 in the first block, the draft has r1 and r0).
* `(((struct EG *)0x02000000)->list)` (member at 0x598) in the list insertion: the reload after
  `n->next = ...` appears, but the address is re-read from the pool where the ROM keeps it in r5
  (10/9 off). A struct placed at 0x02000598 with the member at offset 0 does not reload; neither
  does a local pointer to it.

Not landed: a raw address is itself a concession and not relocatable. The ROM keeps the address in
a register across blocks in both functions, as a symbol does, so the original is more likely a
symbol whose address pseudo alias analysis does not know: `reg_known_value` needs a REG_EQUAL note
and REG_N_SETS == 1, so an address pseudo with two sets (PRE inserting it on two edges, or a pointer
local assigned on two paths) has an unknown base and also "varies". The other way is `n` (or `p`)
not being a single copy of r0. Next attempt: find C that gives either, starting with the camera
unit (already register-only with the integer form).

`WirelessResultPanel_Init`: not started.

## 2026-11-25: SpriteTrail camera reload recovered

`SpriteTrail_UpdatePositions` (081F2404) matches in ordinary C with
`next = head; next++;` and the unqualified camera pointer. Simply removing
the qualifier produced 320 bytes against the ROM's 304. On that draft,
`AGBCC_NOMOVE=164` was exact and `why 417` traced the new high-register copy
to the loop pass.

The split makes a difference even though the resulting address is the same.
At the end of GCSE the split has `next = head` followed by
`next = p + 308`. With the combined initializer, the first statement is
absent. The second set in the split uses `p` in place of `next` as its base,
so alias.c's `record_set` drops the base for `next`. A store through that
cursor can then block hoisting the camera pointer. Later passes remove the
initial copy. The split and the unsplit initializer with `&p->head[1]` were
checked separately: the split matches and the unsplit form still hoists.

The persistent search is in `build/matchsearch/SpriteTrail_UpdatePositions`.
The landed unit passes the stock check at 304 bytes and has zero linter
reports. This removes one volatile error. The precedent for the registry is
a reassignment that survives through the alias pass and gets deleted later.

## 2026-11-25: registry control-flow experiment and native types

`TargetRegistry_RegisterActor` (0815F6A8) matches at 72 bytes using the
existing motion-target and registry types. A shared insertion operation
checks the head before scanning the rest of the list. Empty and singleton
lists join at a short-list link step. Longer lists go through the scan and
link before joining the same head update.

Explicit next-slot locals, two-set pointer reassignments, one-node array
views and an intrusive-link aggregate still removed the second singleton
load. The base test and scalar classification described above explain why
changing just one side was insufficient. `AGBCC_NOSTRICT=1` alone also
left the baseline at 4 ROM / 4 draft instructions off. In the baseline,
`why 87` traced the destination-address change to the first CSE pass.

Duplicating the link step for the empty and nonempty branches restored both
loads but left the initial empty-list branch landing on the store, four
bytes later than in the ROM. `why 91` showed that CSE followed the empty
edge and replaced that branch's loaded head with the register it had just
tested. `CSE_NOFOLLOW=2` matched that draft. Peeling the first node gives
the short-list block multiple predecessors. Both link steps then retain
the loads through CSE, and jump cleanup merges the corresponding machine
instructions. The shared head update retains its separate load.

The short-list rewrite matched both functions but was declined in review.
It duplicates the link step and peels the scan to arrange CSE predecessors.
The final source retains the existing volatile pointer and uses the native
types and a single search loop. Both functions remain exact at 72 bytes.

The review tried pointer-to-pointer traversal, tail insertion, a reassigned
node, a reused link slot, and insertion inlines accepting the registry or
its slot. Tail insertion changes the operation: the ROM prepends the entry.
Most prepend variants still fold the two singleton reads into one.

Reusing a `MotionTarget **` to read the head, then assigning it
`&entry->next` before the search, keeps both singleton loads. The address
survives to the final store, where the ROM uses the immediate field offset.
The reassigned-node frontier
`build/hard-sites/registry/review/actor-reuse-node-slot.c` also keeps both
loads and folds the address into the store. It still has a separate
`adds r1, r3, #0` before the scan and uses r1 for the next-field store.
Using that node for all later operations changes register allocation and
still does not match. When the link slot is assigned immediately before
the store, CSE can fold it back into the incoming pointer before the alias
test. The remaining C question is how to lose the inherited base while
allowing the reassigned node and the entry to share their physical home.

The persistent registry stores are in `build/matchsearch/`. Review drafts
and the stock-verified typed fallback are in
`build/hard-sites/registry/review/`.
