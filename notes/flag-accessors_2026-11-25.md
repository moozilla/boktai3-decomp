# Where Boktai 3 puts the mask in a flag operation

Tags: #compiler #phase3 #tooling

A mask-first test identifies a value available early. Three different source constructs
produce exactly the same four Thumb instructions. The best case for a shared operation
is the combined engine/scene pause query. The strongest single-field case in this sample
is the tracking byte. An isolated pointer-plus-mask conversion earns no recovery credit.

## What agbcc does

The experiment uses the stock build flags `-O2 -mthumb-interwork -fhex-asm`. For a minimal
replay, define `struct P { unsigned flags, other; };` and compare these bodies returning
unsigned. The mask is 4. The order below is that of the data load and mask construction.
An address loaded from a literal pool is a separate event.

| Source | Stock Thumb result |
|---|---|
| `p->flags & 4`, `4 & p->flags`, enum constant, simple expression macro, pointer local `unsigned *f = &p->flags` | LDR, MOV, AND, BX, including the same register numbers |
| Local `unsigned m = 4;` before the expression, inline `test(p,4)` with a scalar mask parameter, GNU expression macro declaring the same temporary | MOV, LDR, AND, BX, identical across all three |
| File-local or external-linkage const with initializer visible | Same as the literal |
| `extern const unsigned otherK;`, definition elsewhere | Two actual data loads. Reversing operands can reverse those loads. The pool reference addresses otherK |
| Signed or unsigned 32-bit field | Same as plain unsigned for this low mask |
| Signed / unsigned halfword, unsigned byte | LDRH / LDRB before MOV. For a raw masked value returned as unsigned, the halfword adds left 16 then arithmetic / logical right 16. A branch on the test can omit the extension |
| 32-bit bit-field at bit 2 | LDR, left 29, logical right 31 to return the bit. In a branch the shift feeds a sign test. A byte container can use LDRB. Plain `!!(flags & 4)` also shifts |
| `(flags & 4) && !(flags & 8)` | One field load, mask 12, compare with 4 |
| `flags \|= 4` and `flags &= ~4` | Load first. With a local initialized before the statement, mask first. This clear builds −5 with MOV 5 and NEG |
| Word at +0x2D0, halfword at +0x2D4 | Plain tests build the offset, read the field, then materialize 4. Same for a branch on halfword mask 0x40 |
| Two plain `\|= 4` operations on adjacent fields | One MOV after the first load and before the second load. No source binding for the mask |

The pinned compiler source under `build/tools/agbcc/gcc/` explains the distinctions.

* `expr.c:7053`, the binop path, pre-expands calls and expands operands 0 then 1.
  An expanded memory operand or CONST_INT can still be an RTL expression. Its position
  in C does not imply that an instruction has already loaded it.
* `optabs.c:420–519`, expand_binop, prefers a register or destination first and a constant
  last for commutative operations. It satisfies operand predicates in order.
  `thumb.md:674–720` requires a register for the word, then forces a general AND
  mask into a register or chooses a specialized clear/extraction. Swapping the source
  operands cannot force a literal into a register before a memory operand.
* `integrate.c:1300`, process_reg_param, copies a constant actual into a mode register,
  records its equivalence and maps the formal to it before copying the body.
  subst_constants and validate_change attempt substitution into copied RTL. A concrete
  Thumb register/register AND cannot accept an immediate just because its value is known.
  In the instrumented initial RTL of the plain example insn 10 reads memory and 12
  sets constant 4. For the inline example 10 sets 4 and copied insn 12 reads memory.
  No subsequent scheduler decision is necessary.
* In the two-store example initial RTL has distinct mask definitions. After the first
  CSE both ORs read pseudo 24. The second definition disappears. GCSE runs dataflow
  propagation across predecessor blocks (`gcse.c`, compute_cprop_avinout, cprop_insn)
  and PRE can insert or replace available expressions. In the conditional-call control
  `if (p->flags & 4) work(); return p->other & 4;` both GCSE passes report zero
  constant and copy propagations. The assembly retains 4 in r5 across BL.
* Flow records uses/deaths. Local allocation groups compatible values within a block
  and tries suggested registers and then available registers. QTY_CMP_PRI in
  `local-alloc.c:1435` scales floor_log2(refs) × refs × register size by the inverse
  birth-to-death interval. Cross-block values can reach global allocation. Allocation
  and reload determine which physical register survives or has to be rematerialized.
  They do not explain the reversed initial RTL in the single-use examples.
* In ActorDebris_Update (081604C0), replace just the WordFlags_Test call with the
  direct expression in a scratch copy. Scope `why all` traces the field load to UID
  10 ahead of UID 12 from initial RTL onward. Raising the mask pseudo's allocation
  priority with `AGBCC_PRI=24:+20000` leaves exactly the same one-instruction order
  hunk. The physical ROM then uses the same 1 for a write to a different field.
  Stock check.py reports MATCH on all 44 bytes with the existing accessor, with a
  single-use local in its place, and with a mask-temporary macro in its place.

## Which sites the ROM supports

The survey read the linked Thumb disassembly after comparing the ELF's entire .text
section with the supplied ROM. The C provides argument types and compiler-measured
field offsets. No C helper name or current warning selects a site. Replay from the root:

```sh
build/venv/bin/python tools/struct_views.py 'struct RoomActor' 'struct ContactCollider' 'struct FrameDisplay' 'struct Player' --json build/flag_layouts.json
build/venv/bin/python tools/flag_order.py build/flag_layouts.json > build/flag_sites.tsv
```

The TSV records the actual operation, load and mask-definition addresses. It follows
immediates, literal pools, shifts, complements, constant offsets, first-member inheritance
and register copies. A test needs a conditional consumer. A set or clear needs a store
of the result to the same address and width. AND-store includes arbitrary keep masks.
It drops provenance at incoming labels, unknown operations and call-clobbered registers.
Tests using shifts only, indexed addresses, indirect jumps and ambiguous identities are
underrepresented. These are conditional counts of classifiable sites, not ROM prevalence.

739 operations in 544 functions across 336 source units. 599 tests, 67 OR stores,
71 AND stores and two BIC stores. 389 mask-first, 348 word-first, two interleaved.
16 rows use a mask definition and register appearing in another classified operation.
There are also 69 additional mask definitions when grouping by function, field and value.
Those are static rematerializations, sometimes on mutually exclusive paths. Seeing a mask
only once in the sample cannot establish a reload or single use.

| Field or operation | Tests, mask / word / interleaved | Stores, mask / word / interleaved |
|---|---:|---:|
| gEngineFlags alone | 1 / 259 / 1 | 2 / 14 / 1 |
| Engine OR scene pause | 94 / 2 / 0 | 0 |
| gEnginePauseFlags (another address) | 0 / 32 / 0 | 28 / 1 / 0 |
| RoomActor.motionFlags | 96 / 2 / 0 | 18 / 0 / 0 |
| RoomActor.statusFlags | 30 / 0 / 0 | 16 / 3 / 0 |
| RoomActor.behaviorFlags | 14 / 4 / 0 | 2 / 0 / 0 |
| RoomActor.tracking.flags | 18 / 0 / 0 | 16 / 0 / 0 |
| ContactCollider.flags | 9 / 2 / 0 | 0 |
| ContactCollider.reactionFlags | 4 / 3 / 0 | 0 |
| Player.statusFlags | 3 / 0 / 0 | 7 / 1 / 0 |

Controls matter. Of 96 mask-first motion tests, 66 use 0x40. The matched IsDown
operation also checks a null actor. Copying that predicate around actor callbacks
creates many examples of one operation. Ghost at 08098A18 and Mimic at 080E7DBE
test the same halfword word-first with other masks. One accessor per physical
offset cannot be inferred from a majority. Conversely, on the tracking byte the test
spans four different masks, and the 34 classified operations occupy 21 units including
Ghoul, Bee, Slime and the actor dispatchers. The pre-run header at 50126e814 already
has SceneActorChase_TrackingFlags. This supports a shared typed flag operation with
moderate confidence. The declaration syntax and spelling of its name are unrecoverable.

Three stock-compiler controls test whether the same typed tracking accessor works
in whole game functions. In scratch copies, replace only the selected test with
`SceneActorChase_TrackingFlags(gate, 8)` or, for the facing reaction,
`SceneActorChase_TrackingFlags(&s->base.tracking, 2)`. First verify that adding the
accessor's header where needed preserves the original function. Each header control
and each accessor call matches the full ROM extent.

| Function | Mask | Exact extent |
|---|---:|---:|
| SceneActorBehavior_RecordDefeat (080789EC) | 8 | 348 bytes |
| SceneActorTarget_ResetContactResponse (0807F4B8) | 8 | 600 bytes |
| SceneActor_TryRequestFacingReaction (080AB604) | 2 | 240 bytes |

The direct field expressions all miss. A tighter control keeps the boolean return,
helper declaration, call and mask argument identical. Copy the typed helper into the
scratch unit immediately before the function. In its body change only
`if (gate->flags & mask)` to `if (gate->flags & 8)` (or 2). Reading the parameter
matches all three functions. Reading the literal changes four bytes in each, preserving
each function's length. Scope traces the difference to the LDRB preceding the mask
MOV and the mask receiving r0 instead of r1. These sites support using one typed
predicate for both masks. Their existing parked-mask forms reproduce the same ROM,
so the original spelling remains uncertain.

The combined pause query occurs across 77 units, with two independently addressed
globals and multiple masks. The ordinary combined-expression branch builds 0x400
after both loads and the OR. Passing 0x400 into the inline builds it before either
load. Homing effect (0800A900 and 0800A90A) reuses a combined value in consecutive
tests, explaining how orders can differ within a single computation. By contrast the
single engine word is overwhelmingly word-first. Trigger_UpdateProximity contains a
  within-function control at 0800729C and 080072AC. Both tests in item_container.c
  spell `gEngineFlags & 2048` directly. The first is word-first and the second
  mask-first because both use the definition at 0800729A. Preserve the combined semantic
operation with high confidence, its original inline-versus-macro spelling with lower
confidence. A generic single-word exception would cover fundamentally different sites.

Holding the mask constant strengthens the pause comparison. For 0x800 the combined
test is mask-first at 42 of 43 sites across 28 units. The single-word test is
word-first at 49 of 50 sites across 32 units. Weather_ActiveStorm supplies a
within-function control for 0x400. At 08017D16 the engine-word load precedes the
mask. At 08017DD8 the mask precedes both loads of the combined test. The matched
Weather_IsInterrupted helper binds 1024 to a local. Giving it an integer mask
parameter and passing 1024 from the caller matches all 496 bytes. Both spellings
therefore carry the same evidence for the pause operation.

There is direct ABI evidence for both styles of interface. ContactSequenceActor_SetFlags
and ClearFlags (08045D18, 08045D20) operate on an actor. RegistryE7BC_SetFlags and
ClearFlags (0811B774, 0811B77C) operate on a pointer. Weather_TestFlags at 08017468
takes two values. These distinct entry points justify their respective APIs. They
cannot identify an inlined call at an unrelated site.

Other reconstructions offer comparisons. The 2003 Konami GBA game Aria of Sorrow's
public decomp, revision bc23d849, uses direct set/clear/test expressions on saveSlots
in src/code_08001004.c. Emerald has field-specific JOY_NEW and JOY_HELD macros over
the expression macro TEST_BUTTON. Neither is original Boktai source. [Aria save-slot code](https://github.com/testyourmine/cvaos/blob/bc23d849d578c35ae12a5cec4e66549c3021a5be/src/code_08001004.c)
and [Emerald input macros](https://github.com/pret/pokeemerald/blob/master/include/global.h).

## Disposition of the November 22 conversions

* Keep the combined pause operation. Merge Engine_PauseBits, Scene_TestPauseFlags,
  RegistryActor_MessagePauseBits and SceneActorBehavior_GlobalPauseBits into one
  engine/scene-owned definition after checking every affected unit. Keep typed
  tracking operations with the tracking component, with separate names for a raw
  mask and a boolean result. A real null guard, callback installation or multi-field
  state transition is part of a domain operation and survives deduplication.
* Keep recovered field and collider types independently of the accessor verdict.
  RoomActor motion/status families warrant investigation by operation, especially
  IsDown. Retain the typed semantic predicate. Avoid introducing a generic raw mask
  accessor for every read. The five Armor reaction-clear conversions following
  119534c21 repeat one behavior in related handlers. They establish a shared combat
  idiom but provide weak independent evidence for a universal ContactCollider API.
  Keep the field, review the repeated combat operation together.
* Revisit the scene1 “staged flag masks through the word-flag helpers” changes,
  including 4c8dcd0ec, 29cb05719, 9517f078c, 677b74658, 1b29b4a09, 7a3b8764d,
  6557fdf4e, b3ec0bde5 and b59f830c9. Revert the staging-only substitution when
  plain C matches, otherwise keep the prior local visible while recovering the owning
  operation. WordFlags_Test and Flags_MaskedWord exchange parameter order. Their
  coincidence with selected code shapes is insufficient evidence for two APIs.
  Review HalfwordFlags_Test/Set/And and Player_SetStatusBits pointer variants on the
  same basis. Preserve unrelated changes within these commits.
* A mask local is honest when it is a named domain quantity with independently
  supported uses or a real addressable object, e.g. a fixed DMA fill source. A common
  physical register, a single successful compile or another decompiler-created clone
  is insufficient. Keep uncertain examples as review debt. No game-source changes
  accompany this investigation.

## Reading the revised metric

The old inventory counted a definition once and required every call to pass a constant.
It counted the definition zero times as soon as any caller supplied a runtime mask.
The revised warning is at each caller and each operand. An expression macro expands
directly. A function macro that first assigns its mask argument to a scalar temporary
and uses that temporary for bit operations also incurs caller debt. A local read twice is charged
twice, including when its two uses are on one source line. A grouped declaration retains
one printable diagnostic with the combined weight. Number-named bindings are included
in the subtotal so renaming a parked literal to `one` cannot help. A declaration with
no reads is still one review item. A direct expression, or a genuine runtime argument
without a parked initializer, has zero charge.

The automatic recovery rule recognizes the actual combined pause expression and the
simple tests/set/clear on a typed SceneActorChaseTracking pointer. Naming another
function the same way confers no exemption. Additional evidence can justify a specific
definition or local through the existing concession-ok convention. The rule purposely
does not turn “this field often looks mask-first” into permission to invent a helper.
It compares textual uses in the matched C. It cannot reconstruct original source,
resolve arbitrary preprocessor dispatch, or count dynamic execution frequency. Source
reorganization and changes to the recognition grammar require a fresh baseline.

The branch test on an unsigned-char bit-field at bit 2 is a particularly good negative
control. Its LDRB, MOV 4, AND, CMP 0 and branch are the same pattern as the plain-byte
branch. Even bit-field inference needs the container width, neighboring members and
other operations for support.

At the worker baseline 9a0f205da the old linter printed 3 errors and 3,607 warnings.
On the same game source the revised accounting yields 3 errors and 7,275 weighted
warnings. Steering debt is 7,037 uses, comprising 2,967 literal-local, 1,199
const-local and 2,871 unresolved constant arguments. These are source-level
review counts. Previously the corresponding categories were 2,133 lines, 708 lines and
528 definitions. The increase measures a change of units and coverage on identical game source.

The whole-tree, single-job summary covered 6,628 files. The final macro extension
was checked incrementally against that run by inspecting all 208 files defining function
macros and recounting all six files mentioning the three eligible macros.
It adds four constant arguments. Applying the same rule to a fixed-mask inline
specialization removes five literal-local charges. A scan of all 69 files mentioning
either owning type or global found those five affected units. The exception requires
one constant binding followed by the exact reviewed operation on that binding.
Additional statements or uses still receive ordinary review. Filtered checks reuse header facts and can skip the
unrelated whole-tree statement-macro inventory.

For a compact expansion-order replay, save the following as build/flag_forms.c:

```c
typedef unsigned Word;
struct Flags { Word word, other; };
static inline Word test(struct Flags *p, Word mask) { return p->word & mask; }
Word direct(struct Flags *p) { return p->word & 4; }
Word parameter(struct Flags *p) { return test(p, 4); }
Word parked(struct Flags *p) { Word m = 4; return p->word & m; }
Word temporary(struct Flags *p) { return ({ Word m = 4; p->word & m; }); }
void shared(struct Flags *p) { p->word |= 4; p->other |= 4; }
```

```sh
build/tools/agbcc/agbcc -O2 -mthumb-interwork -fhex-asm build/flag_forms.c -o build/flag_forms.s
build/scope/agbcc/gcc/agbcc -O2 -mthumb-interwork -fhex-asm -da -dumpbase build/flag_forms build/flag_forms.c -o build/flag_forms_instrumented.s
```

Compare the functions in the .rtl, .cse, .gcse, .flow and .lreg dumps. For the
real-site control, keep the original filename in a scratch directory, replace the
first WordFlags_Test in src/player/effects/burst/sub_081604C0.c with `(display->flags & 1)` in the
copy, and run scope on that copy with `ActorDebris_Update why all`.
