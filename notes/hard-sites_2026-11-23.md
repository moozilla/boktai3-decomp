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

## 2026-11-25: Fragment radial sampler copy

`FragmentGroup_SpawnRadial` (08014CA4) has a plain-loop rewrite using the
native arrays and separate halfword and byte draws. The byte helper
returns `s32`, reproducing the signed branch that an unsigned return with
an outer cast missed. The scratch unit `plain-byte-helper.c` has zero
warnings and is exact under diagnostic `AGBCC_NOEQUIV=122`. The table
priority rises from 1988 to 3976 under that check.

Reusing an earlier animation lookup's pointer as the random table
produces a real nonconstant first set. The later set keeps REG_EQUAL
instead of becoming REG_EQUIV. In `adjust-array-addr-animation-temp.c`
the instruction shape is exact with 20 register differences. Lowering
the pointer's priority restores all but the two register operands of the
animation lookup: the earlier pointer really needs r0, whereas the
sampler needs r7. The ROM has no r7 use in the intervening body before the
table load.

A separate source pointer and sample pointer gives the latter a plain
copy without the constant note. `locate-entry-after-rng.c` gets the
register homes and frame right but emits the table literal before the
outer loop. A source copy in the particle body, `copy-in-loop.c`, is
hoisted to the right preheader. GCSE copy propagation then replaces
the sampler's first draw with the original source pointer (UID 278
under scope), leaving only 14 references and a priority of 2441 for the
sample copy. A private `cprop_insn` control limited to that copy
destination, `AGBCC_NOCPROP_REG=55`, together with
`AGBCC_NOREG=39` to rematerialize the source, matches all 704 bytes.
Either control alone leaves a miss. That establishes a second route
which needs both the five sample uses and reload-time rematerialization.
The surviving near-match places the literal too early. The guarded
do-loop form also needs six independent PRE replacements suppressed.

Cursor advances, typed byte and halfword views, sampler structures passed by
value and by pointer, and inlining the entire fill with a table argument
were tried. The nearest source remains in `build/hard-sites/fragment/`
with the graph in `build/matchsearch/FragmentGroup_SpawnRadial`. The
private diagnostic compiler is under `build/hard-sites/diagnostic-agbcc/`.
The checked-in matching unit retains its three proved-zero sites.

## 2026-11-25: SpiralBurst table and mask copies

`SpiralBurst_InitParticles` (08234540) with a direct symbolic table
assignment emits 416 bytes. It retains the table's preheader copy but
allocates it in sb and puts the wrap mask in r6. Suppressing table
equivalence raises the table's priority from 1107 to 2214 without
changing the allocation. In this checkout the first and loop wrap-mask
pseudos still take r6. Forcing the table to r6 restores 428 bytes and
leaves seven substitutions. The independent structural difference is
the wrap-mask copy: the ROM saves it in sb during the phase draw, and
the draft copies it into sb immediately before the body. That also
changes the first mask reload and the counter initialization registers.
Raising table priority by 3600 alone gives 15 register substitutions
with exact instruction shape.

Separate typed table views, a cursor advanced for the first draw,
actual array indexing, an inline phase helper, the existing shared
random sample operations, and wrap-by-table-count forms were tried.
The main-tree source retains the existing zero term. See
`build/matchsearch/SpiralBurst_InitParticles` and
`build/hard-sites/spiral/`.

## 2026-11-25: Hresvelgr mask reload

The clean `Hresvelgr_UpdateBoundaryBarrage` draft (081A0BC8) remains
732 bytes with two differing instructions. Source line 75's 0x3FF
mask is pseudo 219. `why 476` shows it allocated to r5 and then
evicted into sl by reload. `AGBCC_FORCE=219:12` matches. The 0x2F0
action offset in sl is live in the other switch cases, so it does
not interfere with the mask in case 1.

Changing the draw to use the native index, a wrap by table length,
an explicit cursor argument, a direct table reference, or separate
advance and wrap expressions leaves the same two differences.
Readdressing the action in either boundary arm or using a signed
halfword scatter helper changes additional instructions. Combining
both boundary tests under a shared action advance has the original
two differences. The source frontier and trials are in
`build/matchsearch/Hresvelgr_UpdateBoundaryBarrage` and
`build/hard-sites/barrage/`.

## 2026-11-25: GameState late subtraction fold

In `GameState_SelectRamBase` (0821B084), a signed byte displacement
assigned the negated timer offset, followed by subtracting it from
the storage pointer, matches all 88 bytes. It stays MINUS with a
separate NEG through cse2. Combine makes the PLUS after CSE would
have reversed the operands. Moving the negation before multiplying
by eight misses. The same exact form works with the named timer
register. End-relative and midpoint-relative indexing trials did
not match.

The negation and subtraction cancel and there is no separate use
for the negative displacement. The exact trial is retained as
compiler evidence, and the source job remains open for a natural
storage operation producing the late add. See
`build/hard-sites/game-state/trial-signed-subtraction-scaled.c`.

## 2026-11-25: WirelessResultPanel failure returns recovered

Removing the two dead resource guards in `WirelessResultPanel_Init`
(0803B40C) initially gave 680 bytes. The missing instructions were
`adds r0, r2, #0` before each 32-byte resource copy. Both moves still
exist in greg with the ROM registers. Post-reload CSE deletes each as
redundant, since the script call returned the same pointer in r0. It
forgets that equality at a code label.

Separate `return -1` statements for the two real resource failures
preserve the copy labels through reload. With the old text-entry shape,
jump2 then chooses the last failure as the common tail. On that draft,
`AGBCC_NOCROSS=52` matches 684 bytes by leaving the earliest failure
available as a target.

The original shape is reproduced in ordinary C by nesting the text
lookup under the successful keyword search and returning separately
when either fails. The trace shows the text-null and primary failures
merge into the secondary failure, then that tail merges into the
earliest return. The shared script package ID is named and used directly
in both resource calls. The full unit matches at 684 bytes and the two
dead-code reports disappear. The scratch trials and graph are in
`build/hard-sites/wireless/` and
`build/matchsearch/WirelessResultPanel_Init`.

## 2026-11-26: SpriteTrail native camera layout

`scene_tilemap.c` publishes its `TilemapPanel` through `gSceneCamera`
and recovers the panel for its own scroll queries. Using the same
cast and the native `scroll.x` field in `SpriteTrail_UpdatePositions`
keeps both camera reads exact. Use the shared declaration and place
the count and leading-cursor updates in the `for` clause. The split
first cursor advance still survives alias analysis. The stock unit
matches all 304 bytes and reports no concessions. A native indexed
history expression still reverses one operand, so the previously
matched history calculation remains.

## 2026-11-26: Tether bounce frame address and table reload

In `TetherActor_UpdateBounceReturn` (0804CB4C), the symbolic table draft
still emits 344 bytes. Insn 353 assigns pseudo 182 the table symbol and
carries `REG_EQUIV` into global allocation. `AGBCC_NOEQUIV=182` matches
all 356 bytes. This restores the frame-field address in r8 and the r6
literal load into ip, and changes which scratch registers are available
for the later shake stores. Declaring the frame-field pointer across the
three tests gives the same mismatch as the symbolic draft, as does
passing `gRandomTable` directly to both draw calls. Explicit shake-field
pointers, a shared index, and a sample pointer reassigned for the two
indices all change additional instructions. The shared-index and sample
pointer comparisons from this checkout have the same scope score, so
reassigning the sample pointer alone does not recover the late table
load. The source and results are under `build/hard-sites/tether-bounce/`
and `build/matchsearch/TetherActor_UpdateBounceReturn`. The existing
proved-zero site is still present.

## 2026-11-26: Registry next-slot classification and coalescing

The typed next-slot draft keeps both singleton reloads. In the first CSE
dump, the lowered next store is `mem:SI [node + 12]` through the slot
local. Changing only that store to `node->next` gives
`mem/s:SI [node + 12]` and CSE removes the second global read. Both
stores have alias set 5, and the singleton MEM has set 0. In these typed
drafts the member versus scalar classification matters independently of
the alias set. A slot local built directly from the incoming entry also
misses because the second load disappears.

Using the reassigned node for the comparison, the head update, and the
flags gives disjoint input and node lifetimes at the copy.
`AGBCC_FORCE=22:3,28:3` makes both 72-byte functions exact on that draft.
With ordinary allocation the copy survives and the new node takes r2.
The draft that still uses the old entry after the copy puts the two
live pseudos in conflict. Forcing only the copied node there displaces
the incoming argument and does not match.

Caching the head across the scan, checking for membership in the loop
condition, gating a single insertion on the cursor being null, moving
the node copy after the scan, and changing local scope did not match.
Both stock units keep their previously verified typed volatile
implementation. See `build/hard-sites/registry/second-review/` and the
two `build/matchsearch/TargetRegistry_*-second-review` stores.

## 2026-11-26: Fragment sampler constructor

An inline constructor taking a table parameter and returning a one-field
descriptor emits a real early CLOBBER (UID 218, pseudo 95 in the
inner-loop trial). CSE propagates the table from pseudo 94 into the
constructor result. The CLOBBER survives cse2, but flow removes it
because the output no longer uses the clobbered pseudo. The actual
table still gets REG_EQUIV in local allocation. Using the descriptor
from the preheader or inside each draw gives the same mismatch.
Returning a two-field descriptor with both cursor and table changes
the allocation further.

A symbolic name for the table address matches if the existing type
shift is retained. Removing the two active-field shifts via named
array pointers, direct indexing, and incrementing flag cursors still
misses. None of these remove a proved-zero report in matching code.
See `build/hard-sites/fragment/second-review/`.

## 2026-11-26: SpiralBurst phase and draw address

Casting the initial halfword draw to `u8` produces the same code as
the byte-pointer load in the symbolic draft. Splitting index advance
from the table access, returning the sample address from a shared
helper, and reusing one pointer for random and sine lookup all
introduce additional differences. The plain symbolic draft is still
416 of 428 bytes and the mask-copy placement remains unresolved.
See `build/hard-sites/spiral/second-review/`.

## 2026-11-26: Hresvelgr native action and mask placement

Replacing the partial action struct with the native BossActionState
and shrinking the following padding keeps the matching baseline
exact. The clean symbolic draft still differs by two mask operands.
Explicit entry-frame increment, setting the frame to one, and making
the parity checks use tick directly all preserve the two differences.
Passing the table mask as an explicit argument moves its literal ahead
of the cursor load and introduces an extra move into r5. Writing the
two cursor advances inline with a common index also misses. See
`build/hard-sites/barrage/second-review/`. The production zero
term remains pending an exact ordinary source shape.

## 2026-11-26: GameState typed slot cursor

An eight-byte array typedef for each jitter slot, followed by
incrementing the slot pointer in place, gives the same single
operand-order difference. Direct slot indexing changes more register
operands. Indexing backward from the last slot using an inverted timer
phase adds separate instructions. The signed subtraction experiment is
still the only exact clean-lint candidate, and remains scratch because
the displacement is negated solely for the immediate subtraction.
See `build/hard-sites/game-state/second-review/`.

## 2026-11-26: Registry shared wrapper

A shared inline for the entire typed registration, with the existing
actor/collider union passed by value, still loses the singleton reload.
Passing the next slot separately or factoring out the search also
misses in both wrappers. Reusing the search cursor for the new head
and writing through its next slot leaves additional differences.
The ordinary typed volatile implementations remain the exact fallback.
See `build/hard-sites/registry/third-review/`.

## 2026-11-26: Fragment table priority with equivalence retained

The symbolic plain-loop draft matches 704 bytes with
`AGBCC_PRI=122:+1988` while its table retains REG_EQUIV. The priority
change alone reproduces the previous NOEQUIV match. In the ordinary
allocation pass the copied group cursor (pseudo 395) takes r7 first,
and the table is rematerialized at draws. Demoting that cursor alone
does not recover the rest of the allocation. Explicit group cursors
and an inner group alias change additional instructions. The useful
source variable is the table pointer's relative priority across all
of its uses. See `build/hard-sites/fragment/third-review/`.

## 2026-11-26: SpiralBurst phase order isolated

In `shared-mask-first-index.c`, the phase draw uses the incremented
local index and the same wrap mask as the later draws. Raising the
table priority by 3600 matches all 428 bytes, including the phase
draw and the preheader mask copy. The old draft with the unused index
local differed by the position of that increment, even under the
same priority control. Suppressing table equivalence on this new
draft still emits 420 bytes. The table has 11 weighted references
across 148 instructions before equivalence doubles its live length
to 296. Demoting the competing sprite register, using typed particle
cursors, and inlining the full particle fill do not reproduce the
allocation. See `build/hard-sites/spiral/third-review/`.

## 2026-11-26: Other source alternatives

In Hresvelgr, narrowing the boundary direction to the native byte
domain preserves the two mask differences. A direction switch and
selecting the boundary coordinate through a conditional both change
more instructions. In GameState, packing the offset into the
aligned backing pointer with OR or XOR leaves a bit operation, and
splitting the aligned address from its offset adds instructions.
Those trials use the named timer and live under the corresponding
`build/hard-sites/*/third-review/` directories.

SpriteTrail's ordinary byte-pointer arithmetic still reverses the
history address operand order. Moving the decrement into the
non-wrap path adds instructions. Wireless's native flip bitfields
also change the current match. Neither follow-up changes the
verified SpriteTrail or Wireless fixes.

## 2026-11-26: Action-icon launch random table

In sub_0816E570, the direct symbolic table loads into sl and the
376-byte draft loses eight bytes. Insn 107 defines table pseudo 28.
Suppressing its equivalence matches all 384 bytes without other
diagnostic changes. Using the existing ParticleRandom_ReadSample
helper also gives 384 bytes with stock agbcc, leaving 22 differing
instructions and a register-blind distance of two. Raising the
table's priority on the shared-helper draft changes more operands.
Passing the symbol directly, using the named owner type, and
reading the selection member directly preserve the 376-byte miss.
The original proved-zero expression remains. See
`build/hard-sites/action-icon/` and `build/matchsearch/sub_0816E570`.

## 2026-11-26: Spider wander allocation without equivalence control

The ordinary symbolic Spider_UpdateWanderGoal draft emits 592 of
584 bytes. The known combined diagnostic still matches. A new
isolation check shows that suppressing equivalence is unnecessary
in this checkout: `AGBCC_FORCE=29:5,31:6,32:10` is exact. Those
pseudos are the body address, random table, and cursor address.
Suppressing table and cursor equivalence alone emits 580 bytes
and still differs. The body address and cursor eventually take
the expected registers even in the stock draft, but reserving
those homes during allocation changes which other pseudos get
the remaining registers.

Using the table size for the wrap mask and accessing the shared
cursor directly preserve the stock miss. Removing only the timeout
test's zero term, writing that timeout as a conditional expression,
and delaying the distance calculation until projection also miss.
Moving the body pointer closer to projection or using the member
directly increases the miss. Removing the jump to the timer decrement
preserves the stock mismatch.
See `build/hard-sites/spider-wander/` and
`build/matchsearch/Spider_UpdateWanderGoal`.

## 2026-11-26: Registry transfer and FragmentGroup search

Copying the head through a named pointer, using a generic link-transfer
inline, and using a copy call all miss in both registry routines. The
generic inline leaves the same miss as the typed scalar copy. Reusing
the return expression to publish the head also changes the control flow.
There is still no stock match with both singleton reads.

Returning immediately after filling a FragmentGroup produces the same
code as breaking out of the search loop. Splitting the free-group search
from the fill, either in the main function or in a small inline, changes
both the shape and the allocations. See the `fourth-review` trial folders.
The shared RNG helper in the boundary barrage also leaves its two
mask-register differences.

## 2026-11-26: Hresvelgr defeat fallback removal

Removing the bounded motion fallback in sub_081A14FC yields the expected
1,764 bytes and 22 differing instructions. UID 795 writes shifted Y
sample pseudo 255 to r1. Pseudo 353, the cursor needed by the following
draw, then starts in r2. The ROM uses r3 for the sample and r1 for the
cursor. `AGBCC_FORCE=255:3` is exact. Raising the cursor's priority
(`AGBCC_PRI=353:+1559`) reduces the miss to four register substitutions.

Reusing a sample local, exposing the quotient or cursor, and using the
existing shared samplers all miss. Deleting the other negative-sample
arm as well changes more instructions. The source and persistent store
are `build/hard-sites/hresvelgr-defeat/` and `build/matchsearch/sub_081A14FC`.

## 2026-11-26: Impact scatter sine reload

The native polar helper for sub_0817AEC8 emits 296 bytes. UID 241 uses
the second sine-base pseudo created by PRE and hoisted by loop. Its
three weighted references and doubled lifetime (92 to 184) leave
global allocation unable to assign a register. Reload rematerializes
it in r3 at the Z lookup. `AGBCC_FORCE=161:2` is exact at 288 bytes.
The earlier 159 identifier refers to a different draft context.

Splitting the wave samples, keeping a single typed base in the helper,
and changing the heading width or cursor all worsen the stock match.
The original units retain their reported constructs. See
`build/hard-sites/impact-scatter/` and `build/matchsearch/sub_0817AEC8`.

## 2026-11-26: SpriteTrail history extent and signed wrap

The matching camera source now uses the native history array's last
index for the backward cursor and the forward ring update. With the
array-size enumeration in the forward comparison, agbcc initially
emits BLS where the ROM has BLE. Casting the limit to s32 preserves
the original signed comparison and matches all 304 bytes. The scoped
lint reports zero concessions. Starting from the last history pointer
and subtracting the reverse distance changes the address calculation,
so the previously matching initial offset remains.

## 2026-11-26: Skeleton defeat origin and late table

Removing the two dead exits and the zero sound term from
Skeleton_UpdateDefeat still emits 7,924 bytes, with 28 changed ROM
instructions and 28 changed draft instructions. Removing only one
exit or only the sound term performs worse. Reusing one named random
table in both bursts also increases the mismatch.

UID 1095 takes PRE's pseudo 3858 for the body-position pointer.
The draft puts it in sl, while the ROM spills it to stack+0x98.
The first real ROM use of sl after its save is the late random-table
load. Suppressing allocation of 3858 alone spills it into stack+0x94
and moves the category pointer into sl. Suppressing both sends the
early wrap mask to sl. Suppressing those three pseudos sends the
spark-count mask there instead. The earlier exact allocation probe
has no verified equivalent in this checkout. Direct origins,
shared burst code and narrower locations for the wrap mask miss
under stock agbcc. See `build/hard-sites/skeleton-defeat/`.

Suppressing allocation of the two pointers and all five named
constants recovers the stack addresses and leaves 14 ROM / 14
draft instructions. The late table is still in r6, so the missing
sl save and late register move account for the remaining shape
differences. Explicitly forcing sl changes the allocation earlier
in the function and gives a larger miss. The original unit still
passes the stock 7,924-byte check with its two dead-code and one
proved-zero reports.

## 2026-11-26: Shared byte draw and native region alternatives

Using the existing shared draw only for the byte sample in
FragmentGroup changes the stock miss from 98 ROM / 96 draft to
70 ROM / 68 draft instructions. The table still has REG_EQUIV.
Suppressing that note recovers the 704-byte length and leaves 30/30
instruction differences, so the plain inline is still the better
diagnostic basis. A sampler descriptor for SpiralBurst, using the
table and mask as two fields passed by value or address, also misses.

Splitting the Hresvelgr action pointer by case or using the actor
member at each call changes more than the two mask instructions.
Typed GameState RAM-region members and publishing the active region
before deriving its neighbors fail to recover the operand order.
Plain register declarations on the registry candidates preserve the
same allocation. XOR comparisons for Wireless flip flags miss while
the original dead-guard cleanup remains exact. The corresponding
trial folders are under `build/hard-sites/`.

## 2026-11-26: Native registry unlink

`SceneLink_RemoveNode` (0815F738) now walks the native `MotionTargets`
and `MotionTarget` structures and uses the shared `void *` signature.
The previous local list type disagreed with the type of `gSceneLinkList`.
`SingletonSlot7_OnDestroy` (081F1BF8) now declares the same signature.
Both stock units match all 52 bytes and the scoped linter reports no
concessions in either. The matching unlink keeps the traversal and the
head and interior splices. The two insertions still have their volatile
loads. In the ROM, `TriggerManager_InsertTrigger` also reloads its
manager singleton. Its intervening store is to the old head's previous
member via the pointer loaded from the list, unlike the new-entry
store in either target registration.

## 2026-11-26: Vorn defeat spread and idle control

The current-layout port of the round-15 native draft for
`Vorn_UpdateDefeat` (080AEE08) removes all three proved-zero adjustments
and emits 988 bytes. At UID 1238 local allocation puts zero pseudo 442
in r4 while r5 is free. That shifts the subsequent flag address and
causes a separate 0x2A9 offset to appear in the state selector.
`AGBCC_FORCE=442:5` restores the exact 984 bytes. Reusing the cleared
attachment timer, factoring the idle-control reset, and using the
shared chase control and state helpers all miss under stock agbcc.

Removing only the spread adjustment preserves the 984-byte shape and
swaps the spread and origin registers in 17 instructions.
`AGBCC_FORCE=132:5,133:6` is exact in this draft. Initializing the real
origin pointer at the start of the burst block gets both homes. It
moves the two instructions that form the origin from after the first
random draw to before the spread setup. Capturing the origin at the
call, separating the first count, or setting the spread's coordinates
through the pointer fails to recover both homes and timing. The native
draft and the pointer-lifetime trial are in `build/hard-sites/vorn-defeat/`
and the `build/matchsearch/Vorn_UpdateDefeat` store. Retaining the
existing route clear keeps the 988-byte miss with either zero width
or local scope. A combined idle helper passing the cleared phase
between the actual fields increases the allocation differences.
The unchanged production unit matches 984 bytes and retains its three
reports.

## 2026-11-26: SpiralBurst coalescing and remaining named sites

The loop-scoped table pointer in SpiralBurst produces 424 bytes for
a 428-byte target. PRE introduces a copy from pseudo 23 to 45. Both
land in r5, so the copy disappears and the frame value takes r6.
Forcing the copy destination to r6, even with pseudo 23 forced to r5,
moves both to r6 in the allocation dump. Raising the destination
priority leaves the same stock miss. The earlier exact diagnostic
uses the separate first wrapped index and shared mask.

A FragmentGroup helper for its optional bounded direction and height
draws misses the stock instruction shape by more than the plain draw.
Suppressing the boundary-barrage mask's equivalence note leaves both
operand differences. Raising its priority shifts other live values,
even when paired with a lower priority for the action offset. GameState
trials with an inline mapping helper taking the save storage and either
the timer or the typed slot change at least 15 instructions. The direct
symbolic GameState draft still misses on one operand order. Scratch
sources for all four are in the corresponding `sixth-review` folders.

## 2026-11-26: Scene rig initial sample reload

The current `ScenePanel_InitRig` (0805B0F0) already uses `gRandomTable`
for the later draws and has no lint findings. Its first draw still
uses `RANDOM_TABLE_ADDR`. A direct symbolic subscript shifts the first
table load and swaps the registers in five instructions after the
state setter. Naming the selected halfword pointer fixes the load
order and leaves the five register differences at 412 bytes.

In this draft local allocation puts the first table base pseudo 101
in r1. The numeric-address baseline instead reloads the table into
r1 at choice six. `AGBCC_NOREG=101` restores the missing reload with
the symbolic pointer and matches. `AGBCC_SPILL=6:1` independently
matches by advancing the angle-offset register selection. The latter
preserves the stock table allocation. Either probe leaves the later
symbolic table read alone.

Separating the index, reusing a cursor, and writing the collider
choice as a conditional leave the same gap. Holding one table
pointer across setup or using a shared draw moves more instructions.
The unchanged unit matches 412 bytes with zero lint findings. The
stock near match is in `build/hard-sites/scene-panel-rig/symbolic-pointer.c`
and the persistent store is `build/matchsearch/ScenePanel_InitRig`.

## 2026-11-26: Registry head slot and FragmentGroup motion outputs

A pointer-to-pointer scan that reacquires the head slot before the
splice matches both registry search loops. The actor draft is 72 bytes.
It keeps the singleton read that supplies the old head, but CSE reuses
that value across `entry->next` and drops the second read. Caching the
head slot through the scan retains the second read but replaces the
first with a read through the cached pointer and extra address setup.
Both stock insertions retain their volatile declarations. The first
and second reads still need to survive together without an extra node
copy. See `build/hard-sites/registry/seventh-review/`.

Factoring the FragmentGroup lifetime and motion setup into an inline
that takes the group and index preserves the plain symbolic mismatch.
Passing pointers to the real output fields moves the stock shape score
from 110 to 102 but still leaves 96 ROM and 74 draft instruction rows.
The earlier exact table-priority diagnostic remains the closest basis.
See `build/hard-sites/fragment/seventh-review/`.

## 2026-11-26: Fading particle lifetime copies

The matching `FadingParticleGroup_Update` (08016118) now avoids the
old pointer adjustment by capturing the vertical speed before updating
it. Its lifetime code still uses a signed test of an unsigned-byte
load. The scoped priority linter reports zero because the narrow value
comes through the lifetime pointer.

Using one widened lifetime for the animation reset and decrement emits
536 bytes for the 540-byte target. The ROM loads the byte into r0 and
copies it to r1 for the reset test and r2 for the decrement. The clean
draft loads directly into r1 and reuses it for both. In the stock source
those copies are UIDs 760 and 770. The signed guard is present through
local allocation and the branch is gone by jump2, after the separate
registers have been allocated. Moving the typed decrement, using a
switch, and changing the order of the reset tests all miss. The best
scratch draft is `build/hard-sites/fading-update/lifetime-result-in-place.c`.
Its stock bytes and diff are in `build/matchsearch/FadingParticleGroup_Update`.

## 2026-11-26: Shared barrage action and Timer 3 address

`Hresvelgr_UpdateBoundaryBarrage` (081A0BC8) now uses `BossActionState`
and its existing function declarations. Adjusting the following padding
preserves the later members and the full 732-byte match. The scoped lint
reports the same six findings. The single priority site still turns on
the mask's register during the two reloads.

`GameState_SelectRamBase` (0821B084) uses `REG_ADDR_TM3CNT_L` for its
16-bit hardware read. The `REG_TM3CNT_L` lvalue macro was flagged as a
constant local, so the matching source uses the named address and
explicit load. The one proved-zero report remains. Deriving the mapped
regions from the backup or variable pointer and walking between regions
also miss. The direct zero-free draft still differs on the operand order
of the storage base and timer offset.

## 2026-11-26: Registry loop condition and particle helper boundaries

Moving the registry match test into the link-slot loop condition
and checking the result after the loop leaves six differing rows on
each side and two moved branch targets in both insertions. Reusing
the search link to publish the head or naming the scalar slot for
the next store gives the same score. The early-return search with a
reacquired head loop still gives the closer splice. See
`build/hard-sites/registry/eighth-review/`.

The FragmentGroup motion helper with typed output pointers emits
668 bytes. Its table still has REG_EQUIV and priority 2048 as pseudo
108. Suppressing equivalence and adding 2048 to its priority each
move the shape score from 102 to 120. The output-pointer helper
therefore has differences beyond the table allocation that the
same controls resolve in the 704-byte plain-loop draft. Capturing
its animation resource halfword or moving the resource-address
assignment into the call preserves the 20 substitutions in the
separate two-set table draft. Indexing the animation table directly
and advancing its pointer in two steps are farther away.

Factoring the barrage's full projectile calculation into one inline
leaves nine differing instructions on each side. Passing the random
table to it gives the same score. Passing the position instead of the
actor changes more operands. The two-mask-operand draft remains the
closest basis. GameState's low-byte timer, named slot window, and
modulo or byte-offset forms all add differences to the direct add.
The committed matching source in each unit retains its priority site.

The scoped recheck of the seven named units reports the two registry
volatile declarations and five proved-zero expressions. SpriteTrail
and Wireless have no priority findings.

## 2026-11-26: SpriteTrail component copy and shared barrage sampling

`Vector3_CopyComponents` (081F21B0) copies the three named `TrailVector`
coordinates and matches all 16 bytes with zero lint findings. Its
callers already pass vectors of this layout. SpriteTrail's update
still matches 304 bytes, and Wireless still matches 684 bytes with
no priority findings.

`Hresvelgr_UpdateBoundaryBarrage` (081A0BC8) uses the shared
`ContactPosition` and `Terrain_SampleHeight`. This removes the
private vector and duplicate height calculation and matches all
732 bytes. Lint still reports the same six findings. Removing
the direction adjustment from this source brings back the same two
mask operands in sl instead of ip. `AGBCC_FORCE=220:12` is exact
on this source. The GameState variants that
index each published region independently all increase the miss to
27 ROM and 35 draft rows. The direct add still differs at one operand
order. See `build/hard-sites/barrage/native-position/` and
`build/hard-sites/game-state/ninth-review/`.

## 2026-11-26: Real second table assignments in fading and fragment spawn

Removing the address-construction union in `FadingParticleGroup_Spawn`
(08015F60) gives a symbolic 440-byte draft with 26 differing instructions
on each side. The table holds REG_EQUIV as pseudo 35 with priority 1735
and goes to sl. `AGBCC_NOEQUIV=35`, `AGBCC_PRI=35:+2000`, and
`AGBCC_FORCE=35:6` each match all 440 bytes. Shared draws, a local sample
index, and a sampler holding both pointers miss in stock agbcc.

Reusing the table local for the actual animation array gives it a second
set and r6 under stock agbcc. Starting that local within the loop moves
the first sample table load into the loop. Loading it before the loop
and restoring it after `Anim_SetSequence` emits 448 bytes. This preserves
the earlier code but loads the animation array into r6 instead of r1,
adds a move to r1, and reloads the random table on each iteration.
Both drafts are in `build/matchsearch/FadingParticleGroup_Spawn`.
The stock unit matches 440 bytes and has no priority findings.

Reusing the FragmentGroup table for the actual sprite frame array and
restoring it before the byte draw likewise allocates the sampler to r7.
Pseudo 54 has no REG_EQUIV and priority 4062. With an ordinary `for`, the
stock draft emits 708 bytes, with four differing ROM rows and five draft
rows. The sample table load is before the particle-count guard, and
r7 holds both the frame table and the per-iteration random table reload.
Adding a source guard or explicitly copying the sample table worsens the
miss. The closest draft is `frame-data-reuse-preheader.c` under
`build/hard-sites/fragment/ninth-review/` and the forked store
`build/matchsearch/FragmentGroup_SpawnRadial-frame-reuse`.

## 2026-11-26: Registry inline search result reuse

Reusing the pointer returned by the inline registry scan for the new
node gives six differing instructions on each side and two moved branch
targets in each registration. A plain member store, an explicit next
slot, and a reacquired head slot give the same score. The previous
inline search form without result reuse remains a holdout.
