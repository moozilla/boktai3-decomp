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

## Resumed campaign

The orchestrator removed the first landing signal and requested continuation. The next signal is a final stop. After the settings interruption, the working tree contained only the unfinished projectile initializer and creator experiments; no rebase, build, push, or merge was pending.

- 08042F44: retained 66.9%. Tried destination-load ordering, an explicit sine-entry pointer, pointer binding, and a speed-lifetime constraint; none improved the baseline. Revisit after the remaining first pass.
- 0804373C: drafted 53.6% from 51.7%. Restored the observed dead RNG literal and terrain-type switch shape. Explicit pool/slot bindings only improved another tenth of a point and were discarded.
- 08043D2C: drafted 86.1% from 72.7%. Random-table binding and moving its initialization into the loop restore most loop invariant scheduling; corrected the source order of the two-target branch. Direction/sine pointer bindings regress.
- 08068BE4: reviewed physics/terrain and playback relatives, but no full C yet. Must revisit with a complete draft under the deeper-attempt instruction.
- 080697C8: matched, register-pinned category, 412 bytes. Preserved the distinct optional-argument zero store, fill argument ordering, and flags update. Depin removed seven of twelve hints.
- 081BB904: matched, register-pinned category, 1,020 bytes. Opaque mask helper preserves the original operand lifetime; two branch-local address constants recover the scratch-register sequence. Depin retained five hints.
- 081BBE88: retained 96.1%. Tried separate initial zero, counter binding, resource pointer locals, and loop zero binding. Small fuzzy gains increased structural differences, so discarded them. Revisit this register-only baseline.
- 08074EB0: reviewed the complete behavior documentation and animation prefix; no full local draft yet. Must revisit with a complete draft.
- 081B6888: new guarded draft, 62.0%. Full six-sprite and two-collider initialization, including packed size/offset records. Tried loop fill ordering and owner binding; reset widths, invariant order, and collider allocation remain.
- 081B6D20: matched, register-pinned category, 368 bytes. The existing numeric random-table address constant fixes the final reload-register choice. Depin retains the existing target reload barrier.
- 081B6E90: retained 85.7%. Numeric table addressing, counter binding, and widened intermediate-angle lifetime regress; revisit later.
- 081B70A8: retained 80.4%. Full-loop pointer/induction rewrites, indexed history views, scoped pointer changes, and register bindings do not improve structure. Instrumented compiler flips identify no helpful pass override. Revisit later.

Tooling: built the instrumented compiler in this worktree. The debug assembler path needs an initial divided-syntax text directive in a private runner; repository tooling was not edited. Compiler hints and flip probes are now available locally.
- 081B71D4: new guarded draft, 76.4%, ported from the ordinary creator with the retained source pointer and observed initialization order. Pointer-loop, reset-value, and signed-direction variants do not improve it. Compiler flips identify no helpful pass override; history-loop induction remains the main difference.
- 081FCA44: new full guarded draft, 51.0%. Ten-slot delayed launch/homing/terrain/collider updater and packed animation decoder recovered. Corrected the collider extent accessed beyond the sprite boundary. Tried a separate control view, constrained duplicate bases, loop-invariant analysis, and compiler flips. The natural control view reduces structural differences; explicit bindings increase them and were discarded. No useful flag flip found.
- 08070FA8: new full guarded draft, 57.7%. Callback traversal, context removal, regional deactivation, three packed-vector particles, and both palette transition paths recovered using the reviewed registry header. Materialized stop predicate improves 57.1% to 57.7%; symbolic RNG table, signed RNG return, and mask helper variants do not help. Packed writes preserve word masks. Compiler flips offer no useful pass change; long-lived pointers and particle-loop scheduling remain.
- 082377B8: matched, register-pinned category, 384 bytes. A first-term truncation helper restores four copies and an opaque direction mask gives the correct operand lifetime. Depin removes two of three hints, retaining the direction-mask barrier.
- 08237938: new guarded draft, 94.2%. Four-slot orbit/update loop. Separate position and timer views, widened angular offset and parallel timer pointer recover the original body. Remaining differences are initialization and loop-tail ordering. Tried indexed, manual-pointer, timer-inside, constrained-base, compiler flips and loop-invariant diagnostics; retained the best full-size candidate.
- 08237A28: matched, register-pinned category, 388 bytes. Shared signed RNG table, widened angular offset, and short-lived radius binding reproduce repeated indexed accesses and both truncation branches. Depin removes the angle barrier and retains one radius binding.
- 08239848: new full guarded draft, 56.1%. Selective or complete RGB15 blending, source brightness saturation, and form-specific palette destinations recovered. Delayed base/brightness loads, sequential channel clamps, packed shift expressions, and a shared base/output pointer improve the initial 33.2%. Explicit argument bindings regress and were discarded. Compiler probes were also run.
- 0823A3CC: matched, register-pinned category, 332 bytes. Separating the persistent zero from the halfword reset store, after calculating its address, restores the original order. Depin retains one zero-lifetime barrier.

The resumed batch contains six exact matches (2,904 bytes, +0.120 percentage points), all with minimized matching concessions, and eight guarded draft commits. Thirteen assigned functions remain unstarted. Reviewed-only physics and scene movement bodies, plus retained existing drafts, remain on the revisit list required by the deeper-attempt instruction.

## Third batch in progress

- 081266B4: matched, unpinned, 288 bytes. Materialized player-mode predicates, explicit branch targets and a common flag-store temporary recover the blink state. Two initial pointer bindings were both removed by depin.
- 081268A4: matched, register-pinned category, 116 bytes. An opaque initial field offset prevents CSE from reusing the address across the initial guard; distinct initial address binding restores the original reload choice. Three hints retained after depin.
- 08126AD4: matched, register-pinned category, 360 bytes. Separate sprite/slot identities, late RNG table evaluation and short-lived multiplication/selection arguments restore the original lifetimes. Depin removes five of eleven hints.
- 081D33A0: new full guarded draft at 68.5%. Forty-particle initialization, packed local offset, two resource modes, randomized records, sprite setup and anchor copies recovered. Switch dispatch and player-load ordering improve the initial 60.4%. Duplicate-base constraints, indexed views and volatile-counter alternatives were compared; retained the best ordinary draft. Compiler flips also inspected.
- 081D3620: new full guarded draft at 63.6%. Scatters forty stored anchors, raises Y and redirects each anchor pointer. Explicit RNG evaluation and a full-width coordinate helper recover the signed offset; fixed-register and cached-index alternatives do not improve the retained draft.
- 0821DE30: new full guarded draft at 69.3%. Ten-entry trap contact tracking with rectangle containment and enter/stay/leave dispatch. Cached rectangle coordinates, serial-pointer helpers, an explicit initial loop test, and branchless key testing improve 61.0% to 69.3% and restore the full size. A no-GCSE probe narrows remaining differences, which mostly involve hoisted addresses and callback-loop allocation; no compiler override is retained.
- 08160108: new full guarded draft at 56.8%. Sixteen radial burst particles, all randomized parameters, tile-frame choice and lifetime setup recovered. Separate particle-data view, ordered coordinate calculations, and independent phase counter improve 52.2% to 56.8%. Loop diagnostics and single-pass flips show the second loop optimization changes allocation but does not close the gap. No compiler flags retained.
- 0815E034: matched, register-pinned category, 444 bytes. Full jump table, channel response modes and player attribute scaling recovered. Preserving the observed discarded mask, ordering pointer temporaries, and an opaque shared channel mask restore the original register decisions. Depin removes two of nine hints.
- 08163020: matched, register-pinned category, 156 bytes. Initialize the left-loop index before the offset and subtract sixteen before loading scale. One offset binding remains after depin.
- 08163700: matched, unpinned, 208 bytes. Sprite/overlay setup and all optional script parameters recovered; first full draft is exact.
- 08220938: retained 97.1%. Constrained post-callback mask helpers fix the four register bytes but push both outer conditional branches into the long form. Tried combined initial barriers, deleting individual barriers and paired fixed operands; none preserves both the reloads and original loop extent.
- 0805E484: improved guarded draft at 93.4%. The inherited 91.3% annotation reproduced only 46.0% with current tooling. Typed aggregate transfers, explicit short-lived argument registers, low-register constraints and scoped copy temporaries restore most instruction order. Current candidate is 188 versus 180 bytes; remaining differences are tail-step and widget-offset allocation. Avoid explicit r7 bindings here: the compiler treats them as its eliminated frame pointer and can substitute SP into arithmetic. All retained constraints use ordinary allocated low-register operands instead.
- 08227D70: retained 91.9%. The complete existing draft differs only at the missing BIOS Stop instruction and resulting branch/literal displacements. Confirmed in this compiler's Thumb backend that no SWI/SVC pattern or callable intrinsic exists; a private __builtin_trap experiment is rejected as unsupported by the target. Handwritten instructions are prohibited, so the assembly-backed fallback remains.

- Revisit `PlayerCue_UpdateSideArrival`: retained 98.5%; compiler decision flips did not help. A forward-path index barrier prevented cross-jumping but added structure differences (11 versus 7).
- Revisit `sub_0821358C`: retained 99.7%; integer addition and offset/base barriers changed GCSE scheduling and regressed. The sole difference remains commuted add operands.
- 08152C3C: new complete guarded draft at 90.6%, replacing reviewed-only status. Recovered initialization, secondary animation completion, vertical positioning, all five cue phases, visual reset and standing restoration. Typed fields improve the first raw-cast draft from 80.7% to 90.5%; playback bit lifetime and timer-mask probes reach 90.6%. Early zero binding regresses. The compiler hint probe cannot address this previously hidden symbol before a combined build. Registered the independently reviewed build boundary; source remains assembly-backed. Remaining differences include playback branch sharing, literal scheduling and deterministic particle-word initialization versus the original partial-word write.

## Final wrap-up

The real wrap-up signal appeared during the large cue revisit. This batch contains six exact matches, adding about +0.065 percentage points (1,572 bytes): 496 unpinned and 1,076 Register-pinned bytes. Six guarded draft commits retain source recovery or improvements. No new function was started after the signal.

Campaign disposition: 19 matched, 27 drafted, 28 skipped or retained without improvement. Three reviewed-only bodies still need complete drafts: 0802BFAC, 08068BE4, and 08074EB0. The requested revisit pass began with the closest retained drafts but was interrupted by the final signal; other retained drafts remain documented above. These outstanding items are not claimed as completed full attempts.

Final batch validation: rebased onto origin/main, preserving both workers' history additions and regenerating the documentation index. The complete combined build printed `build/boktai3.gba: OK`; the neighboring recoil fallback also checks byte-identical after the new cue boundary. All batch commit metadata uses the campaign identity.
