# cx-sol04 campaign

Tags: #matching #player #cue #animation #campaign

## Player cue family

- 0814FC80: drafted 96.9% (from 89.7%). Typed byte fields remove redundant reloads and merged stores. Corrected sound selection: the nonzero scale-axis branch uses the ordinary movement sounds. Seven realigned differences remain, four ignoring registers; a direction mask and a repeated null guard remain. Pin experiments worsened structure and were discarded.
- 0814FFA4: skipped at retained 80.1%. Typed fields reduced structural differences from fourteen to ten, but register differences increased. Removing pins reduced structure to eight but worsened total differences. Directly consuming the angle result reduced totals but worsened structure. No replacement retained.
- 081503EC: matched, unpinned, 1,032 bytes. Typed scale bytes and explicit shift/add multiplication preserve subtraction before scaling. The compiler merges the identical phase-two/phase-seven tails naturally; volatile barriers prevented that sharing and were discarded.
- 081516C0: drafted 92.1% (from 91.5%). Explicit shift/add scaling improves two register choices. Twenty-six register-only instruction differences remain. Narrowing the vertical temporary and removing the table pointer worsen structure. Bounded single-process permuter found no improvement.
- 08151A14: skipped at retained 83.1%. Palette arithmetic has long-lived color temporaries and differing phase-zero branch layout; typed-byte conversion has no effect because this draft already uses a struct.
- 08152174: drafted 84.3% (from 81.7%). Typed sprite scale bytes reduce realigned differences from 217 to 180 and structural differences from 37 to 12. Owner-pointer pinning worsens both allocation and structure; discarded.
- 08152A5C: skipped at retained 97.5%, six register-only instruction differences. Explicit speed-pointer and offset pinning loses address reuse. Individual helper-hint removals and register clobbers do not improve the candidate. Reviewed body is 480 bytes.
- 08152C3C: skipped after boundary review: retained byte tail of 08152A5C, with a separate large body. No candidate authored in this pass.
- 08153224: skipped at retained 84.4%. Remaining particle-word initialization, timer-mask lifetimes and constant derivations differ. Kept fully initialized particle options.
- 08153610: skipped at retained 89.9% after exact-relative comparison. Resource selection and timer-mask lifetimes differ; porting the exact recovery sibling reduced structure but worsened total differences.
- 081540D4: skipped at retained 98.5%. Playback completion has a differently shared index-store tail. Explicit common-store label and mask barrier worsen structure; discarded.
- 081543F0: skipped at retained 91.3%. Scale-field constant derivations and animation allocation remain.
- 08155008: drafted 91.1% (from 86.0%). Typed sprite scale bytes; six structural differences remain, mostly scale-address derivations. Owner-pointer pinning regresses substantially.
- 081556E0: skipped at retained 88.3%. Byte-field typing has no measurable improvement. Event-mask and playback ordering remain.
- 081559A4: skipped at retained 87% baseline; byte-field typing regresses to 83.4%, discarded.
- 08155D58: new guarded draft at 74.8%, ported from timed-charge sibling. Reverse hold gating, far particles, pulsing overlay, fixed timer increment and direct completion-phase store recovered from assembly.
- 0815C73C: new guarded draft at 73.0%. Nine attachment descriptor modes and sounds recovered. Halfword setter helper and zero-register pinning regress and were discarded.
- Address-only functions can be absent from the CSV while remaining proper function starts in generated code. Raw-tail bodies can be checked by address and need build-boundary registration when authored.

## Actor display family

- 081EC388: matched, register-pinned category, 292 bytes. An input-only barrier on the one-bit random mask after the table load gives the original short lifetime; depin removes the explicit register binding but retains the barrier.
- 081EC4AC: matched, register-pinned category, 284 bytes. Ported the randomized burst fix to the anchor-based position. Depinned independently.
- 081EC628: matched, register-pinned category, 72 bytes. Restores two display flag words and selects the background visibility mask. Depin retains one branch-local barrier. Contrary to the initial boundary concern, check.py can verify an explicitly reviewed raw-tail address directly; full batch validation still gates integration.
- 081EC6BC: skipped at retained 92.8%. Packed-anchor bitfields do not improve allocation; forcing a packed register or saving an extra register does not recover the missing mask-copy instruction.
- 081ED1A4: new guarded draft at 70.4%. Recovered two sequence initializations, active-view position snapshot, random vertical offset and five velocity phases. A full three-word camera struct copy reduces structural differences compared with independent scalar locals. Callback name is tentative.
- 081EDB74: drafted 98.3% (from 96.1% recorded baseline). Removing a redundant context pointer and changing the callback barrier to input-only eliminates stack preservation and instruction-order differences. Six register-only instructions remain. Specializing the initial playback helper improves two more instructions but duplicates a large helper, so retained the compact candidate.
- 081EDE4C: new guarded draft at 92.3%. Steps past screen X -40 at speed 0x5000, updates random vertical offset, then schedules destruction and sets acceleration -16. Ported shared initialization and decoder from the departure sibling.
- 081EE518: skipped at retained 91.4%. Removing the pinned base reduces two structural differences but increases register differences substantially. Input-only pointer barriers and volatile command reads have no effect.
- 081EEAD8: skipped at retained 94.4%. Palette argument ordering and particle-loop initialization remain. Reordering arguments through an inline wrapper worsens structure.

## Grid attack actor family

- 08029870: skipped at retained 97.1% after bounded permutation. The bounded search found no exact result; two reload-register bytes differ. Coordinate width, declaration and pointer-index variants either leave those bytes unchanged or add instructions.
- 08029BF4: new guarded draft at 67.0%. Preserves the word-aligned target copies, zeroed velocity vector, two random offsets and projectile configuration. Stack scheduling and random-mask lifetimes remain. Volatile local coordinate reads preserve observed halfword accesses.
- 0802ADBC: new guarded draft at 73.6%. Receiver loop, all observed message cases and action transitions recovered. Separate raw coordinate fields reproduce the distinct address lifetimes better than a grouped vector view. Inline action arguments preserve more constant order than a common action label.
- 0802B7C8: new guarded draft at 78.3%. Packed animation, projected height, two shaking effects, target angles and squared distances recovered. The indexed slots at 0x110 and result arrays at 0xC8/0xCC overlap provisional header fields; use local observed-offset views rather than changing the shared layout without an ownership review.
- 0802BAFC: new guarded draft at 57.4%. Collider flags, body refresh, shadow visibility, three palette blend modes, flash countdown, camera tracking and effects recovered. Removing early pointer aliases restores the reviewed 1,076-byte extent but leaves allocation and control-flow differences.
- 0802BFAC: skipped after static review. Script initializer combines target arrays, multiple resources, colliders, projected display and three auxiliary pools. Earlier documented draft is unavailable in this checkout; preserving a partial initializer would not be useful.

## Forging and collection menus

- 08211AC4: matched, unpinned, 436 bytes. Separating the entry-base pointer and index gives the required address grouping. Temporary pin/barrier experiments reached equality; depin removed all eleven hints while retaining the natural C temporaries.
- 08211DE4: skipped at retained 84.0%. Byte-field typing does not change the result; removing animation-pointer locals worsens total differences. Switch tails and field-address scheduling remain.
- 082124D8: skipped at retained 80.4%. Byte-field typing has no effect. Automatic repair could not run because the worktree has no instrumented compiler; no tool or shared output was changed.
- 0821358C: skipped at retained 99.7%. One commutative add operand differs. Pointer arithmetic variants either leave it unchanged or cause broader CSE/allocation changes.
- 08213888: matched, unpinned, 712 bytes. Splitting the selected-entry byte offset into a local gives the original add operand order. Tentative name: CollectionBook_UpdateMenu.

## Registry 1745 family and wrap-up

- 08136F58: skipped at retained 93.6%. Group-pointer register binding and a halfword-mask helper worsen the result; both discarded.
- 0813727C: skipped at retained 79.8%. Pointer-local elimination finds no applicable improvement. Remaining structure includes control-mask scheduling and spill-slot order.
- 081376EC: matched, register-pinned category, 1,008 bytes. Initializing the scan index before the position pointer and adding one input-only index barrier reproduces the original scheduling without disturbing global allocation. Depin retains that one barrier. Tentative name: SceneActorTarget_ApproachNearestStimulus.
- 08137E28: skipped at retained 90.4%. Explicit control-block base arithmetic does not improve the inline reset. No shared header changes retained.

The wrap-up signal appeared before the contact-actor family. No later assigned functions were started. The batch contains seven exact matches; guarded drafts are recorded separately and never counted as exact progress. The raw-tail departure boundary at 081EDE4C was registered in the build-only boundary inventory so split.py can retain its guarded fallback; no function-name inventory was changed.

Boundary follow-up: the supplied 740-byte span at 081EDE4C includes a four-byte leaf at 081EE12C after the departure callback return and literal pool. The guarded fallback retains the entire span; this draft claims no exact credit. Revisit that leaf boundary before promoting the departure body.

## Batch validation

All seven exact functions passed check.py with their reviewed extents. The hinted matches were checked after depin. Rebased onto origin/main, preserving both boundary entries and both history sections in conflicts. The combined build through build/cxbuild printed `build/boktai3.gba: OK`. Exact batch gain: +0.159 percentage points (3,836 bytes), including 1,656 bytes in the Register-pinned category and 2,180 unpinned bytes.

Final worker result: 7 matched, 13 drafted, 21 skipped; 33 unstarted when the wrap-up signal appeared.
