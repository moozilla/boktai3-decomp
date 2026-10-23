# cx-sol10 campaign notes

Tags: #matching #scene-actors #register-pinning

- 0812E7EC: skipped after assembly/relative review. Prior attempts identify duplicated state/control tails merged by agbcc; no reusable candidate or Ghidra export is present. Receiver loop and attachment sub-dispatch need fresh reconstruction.
- 0812F3CC: skipped at 72.0% after reconstructing the contact-response gates and state setup. Flags pointer/zero lifetimes and offset reuse still differ; scratch candidate retained under wip/.
- 0812F5D4: skipped at existing 81.1%. Pointer-local sweep found no improvement; duration setup, random-loop allocation and packet/control order remain.
- 0812F8F4: matched, 280 bytes, with register pinning. An opaque r1 offset for the field2B0 getter prevents PRE from retaining its address across the animation call. A separate zero-initializing control inline preserves the first constant-zero placement; the second inline consumes the callback result. No instruction bodies were added.
- 0812FA0C: skipped at existing 84.6%. Pointer-local sweep found no improvement; large inline route-arrival path remains.
- 081301C4: skipped at existing 88.6%. Pointer-local sweep found no improvement; state/control offset reuse, vector stores and packet zeros remain.
- 081304C8: matched, 264 bytes, with register pinning. Typed position fields eliminate a redundant literal copy; opaque counter-address setup prevents retaining it through collision cleanup. Drawable flags preserve their embedded-part base.
- 08130994: skipped at 72.7%. Reconstructed callback/trigger phase; zero lifetimes and duplicated state/control offset patterns remain.
- 08130CAC: matched, 436 bytes, with register pinning. Pinned group lifetime, local state zero and descriptor argument evaluation reproduce all state/turn paths.
- 08131044: drafted 81.7%. Route descriptor completion, destination reset, route-motion flags and saved-control transitions reconstructed. Offset reuse, call arguments and zero allocation differ.
- 081312F4: drafted 84.8%. Entry transition selects return behavior 8/7/2 from motion/route flags. Pinned group and completion offset improve the layout; state/control offsets and flag-result registers remain.
- 081314E8: skipped at 69.0%. Reconstructed contact distance, delayed route and fallback callbacks. Typed vector subtraction improves stack layout; flag pointers, partial-vector lifetimes and control expansion still differ.
- 08131A2C: matched, 204 bytes, without hints. Reused the zero-initializing control inline and consumed contact result for delayed state entry.
- 08131AF8: matched, 72 bytes, without hints. Straight callback gates and phase-counter advance.
- 08131B40: matched, 132 bytes, without hints. Exact sibling 08131BC4 supplies the state/reset idiom; callback table differs at 0x284.
- 08131D5C: skipped at existing 87.5%. Pointer-local sweep found no improvement; random loads, pointer spills and branch layout remain.
- 081EEDDC: matched, 84 bytes, without hints. Independent callback identified in progress_extra_functions.csv; limit-first signed comparison preserves operand order.
- 081EEECC: skipped at 63.0%. Reconstructed shake offsets, particle resource/track selection and departure blink. RNG scheduling, packed position stores and owner aliases remain; explicit high-register aliases worsened the result.
- 081EF440: skipped at existing 93.3%. Pointer-alias and local-register variants did not improve it. A bounded single-process permuter reached a lower score only by leaving `other` uninitialized in the alternate branch; rejected that candidate.
- 081EF750: matched, 88 bytes, without hints. Screen-selection relative supplies the entry-flag clearing and trigger-consumption pattern.
- 081EF880: matched, 112 bytes, without hints. Horizontal-tween relative supplies callback setup; terminal completion flag is unconditional.
- 081EF914: drafted 96.2%, improved from 94.3%. Pinning only the consumed entry flag to r8 fixes its high-register lifetime without constraining the owner pointer. Playback temporaries and timed-phase constant/zero placement remain.
- 081F0734: inspected and skipped. Large receiver command dispatcher has a 66-entry sparse jump table and mixed resource, palette, sound and motion commands; no draft or Ghidra export. Reconstruction deferred.
- 081F1274: inspected and skipped. Large update combines collider insertion, palette interpolation and several particle branches. No draft or Ghidra export; reconstruction deferred.
- 081F1C2C: skipped at existing 80.5%. Pointer-local sweep found no improvement; script/local-vector and initialization register allocation remain.
- 08095C00: skipped at existing 89.1%. Declaration swaps found no improvement. Pinning the motion/sequence pointers and moving the control zero worsened the candidate; octant-mask lifetime and control setup remain.
- 0809628C: skipped at existing 90.5%. Named argument struct and volatile array both retain the same hoisted array-base stores; script argument stack layout remains the blocker.
- 08096D38: skipped at existing 85.2%. Declaration swaps found no improvement; 60 structural differences remain in the route/motion inline.
- 08097798: skipped at 30.7%. Reconstructed cubic route interpolation and phase advance by 128 up to 1023. The scalar/vector coefficient lifetimes and stack allocation differ substantially; scratch only.
- 08097E38: drafted 99.6%, improved from 99.2%. A zero-initializing state helper fixes store scheduling; one player-slot literal reload remains. Explicit slot reads disturbed vector and player-copy allocation, so those variants were rejected.

## Batch 1

Nine matches, four new or improved guarded drafts. Exact code gain: approximately 0.069 percentage points (1,672 bytes), including three register-pinned matches (980 bytes). Notes distinguish inspected functions from functions with compiled candidates. Combined validation: `build/cxbuild` printed `build/boktai3.gba: OK`. The first build exposed stale assembly fallback names in the two improved drafts; restored their promoted names and reran the complete build successfully. The independent callback also required its reviewed boundary in build_extra_functions.csv.

## Wrap-up

Tags: #matching #ghost

- 080980FC: inspected and skipped at existing 90.4%. The inlined animation stepper still differs in address scheduling and tests; no improved candidate retained.
- 08098344: inspected and skipped. Eight-point cubic route sampling shares the preceding interpolation family, but introduces separate motion coefficients and loop stack slots. The wrap-up signal arrived before a C candidate was checked.
- 08098788: inspected and skipped at existing 84.4%. Visibility gates and global/player addressing remain; no improved candidate retained.
- 08098A74: inspected and skipped at existing 87.6%. Player displacement, animation scaling and control setup remain; no improved candidate retained.

The literal-reference sweep on 08097E38 found no improvement. All bounded searches have ended. The wrap-up signal stopped function work; the remaining fifty assigned functions were not attempted. Batch 1 was merged in PR #582 after the full build printed OK. No further source changes followed that validation and merge.

Final: 9 matched, 4 drafted or improved, 21 skipped (including inspection-only skips); 50 not attempted before wrap-up.

## Continuation: carried Orbit effect batch

Tags: #matching #orbit-effect #register-pinning

- 081FD664: matched, 160 bytes, register-pinned. Typed steering and position fields preserve the target reloads; the consumed direction and turn limit use the observed low registers.
- 081FD7EC: matched, 704 bytes, register-pinned. Separate projectile and particle branches retain stack parameter layouts; packed position fields preserve the particle position copy.
- 081FDCB0: matched, 272 bytes, register-pinned. Timed homing trail reconstructs distance gates, periodic heading adjustment and signed polar scaling. Empty compiler constraints preserve the zero lifetime and position reloads; no instruction bodies.

Three carried matches contribute approximately 0.047 percentage points of exact code (1,136 bytes), all in the Register-pinned category. Per-function exact checks passed after rebasing onto current main.
Combined validation: `build/cxbuild` printed `build/boktai3.gba: OK`.

## Continuation d2: scene-actor and slot-seven pass

Tags: #matching #scene-actors #singleton-slot7 #nonmatching #register-pinning

This section supersedes the earlier inspection-only or scratch-only status for the listed functions. Three new exact matches add approximately 0.075 percentage points of code (1,804 bytes), all Register-pinned. Seven new guarded drafts and five improved guarded drafts retain complete C attempts. One existing draft remains unchanged.

- 0812E7EC: drafted 47.8% over the reviewed 1,764-byte extent. Reconstructed the complete receiver loop, movement/turn commands, behavior selection, message forwarding and attachment operations. Typed receiver/route layouts and top-level pointer constraints were tried; variants of state/control zeros, masks and tail barriers did not resolve folded state-reset paths. Final realigned differences: 592 total / 430 structural.
- 0812F3CC: drafted 75.0%, replacing a scratch-only 72% attempt. Inline mask arguments fix load order, and a state-address local fixes constant placement. Flag-result zeros, control offsets and register lifetimes remain (47 total / 10 structural differences). Wide/narrow results, pointer constraints, volatile pointer and zero-lifetime variants stopped improving.
- 0812F5D4: improved 81.1% to 84.2%. A consumed entry-test spelling, a retained callback offset and loop-local RNG constants improve instruction placement. Final differences: 70 total / 23 structural. The compiler debugger identifies allocation/reload effects; no flag-only compiler build was accepted.
- 0812FA0C: improved 84.8% to 85.2%. Address-first state setup removes three structural differences. Packet-pointer reuse, zero/control variants, pointer-taking entry helper and group pinning did not improve the retained result. Final differences: 162 total / 37 structural.
- 081301C4: matched 616 bytes, Register-pinned. An empty if on the taking helper stops CSE from carrying the wrong state setup across entry. Removing the old flags barrier fixes packet-zero allocation; saving the flags pointer after extracting cell coordinates fixes the late copy. A short-lived r1 height value fixes the final stack-vector store.
- 08130994: drafted 76.2%, replacing scratch-only 72.7%. A wide, unpinned consumed entry result removes redundant truncations. Control argument order, state offsets and zero lifetimes remain (45 total / 13 structural differences). Helper, pointer, width and register variants stopped improving.
- 08131044: matched 688 bytes, Register-pinned. Returning the motion-flags pointer from its test keeps the address through descriptor handling. Creating the destination offset after the counter clear and reusing it as the mask reproduces the retained high-register constant. Scoped mask values fix the remaining evaluation order.
- 081312F4: matched 500 bytes, Register-pinned. A word-sized motion-mask helper removes an unnecessary saved register. Short-lived r2 offsets and r0 address variables preserve the original phase-address derivation and saved-control load without consuming their bases.
- 081314E8: drafted 77.1%, replacing scratch-only 69.0%. Retained the motion-flags pointer through route checks, split route handling into a local inline and used address-first state/mask setup. Pointer lifetimes, partial-vector scheduling and repeated control tails remain (118 total / 56 structural differences). Explicit vector zeros and register-bound route-clear variants regressed.
- 08131D5C: skipped at the existing 87.5%. Explicit two-sample RNG state reduces realigned differences from 91 / 25 to 82 / 23 but lowers the fuzzy score. RNG pointer/mask constraints, removing position constraints, signedness and state-address variants found no accepted improvement. Keep the production draft; the alternate remains scratch-only.
- 081EEECC: drafted 66.2%, replacing scratch-only 63.0%. Full shaking departure, particle selection and blink behavior retained. Coalesced RNG stores and a signed byte comparison reduce structural differences; typed owner views, constrained shake pointers, velocity arrays and volatile velocities were compared. Final differences: 115 total / 30 structural.
- 081EF440: improved 93.3% to 97.3%. Capturing the reset address before the alias barrier removes an extra copy; explicit phase and zero locals fix the final constant ordering. Only the first two pointer-copy registers differ (two bytes, two instructions, no structural differences). Hard binding the secondary pointer to r7 produced invalid stack-based addresses and was rejected.
- 081EF914: improved 96.2% to 99.0%. Entry result in r1, address-before-value setup, separate timed-phase byte zeros and retained duration/offset values fix most remaining choices. Playback helper cloning and pointer pinning regressed. Final differences: nine total / five structural; no exact credit.
- 081F0734: drafted 39.8% over 1,776 bytes. Full 66-entry dispatcher, delayed phase commands, motion controls, palette/resource copy paths and consumed commands retained. Phase commands deliberately leave the trigger pending. Typed row arrays, explicit loop variables, owner/receiver/index constraints and a post-copy alias barrier were tried; loop optimization and folded duplicate resource paths remain (568 total / 416 structural differences). The debugger points to the second loop pass.
- 081F1274: drafted 51.5% over 2,208 bytes. Full collision update, flash/interpolated palettes, particle variants, blink, health feedback and defeat script retained. Tested pointer views, API widths, explicit counter pointers and palette barriers; a post-flash alias barrier and hidden-flag evaluation order improve structure. Final differences: 641 total / 277 structural.
- 081F1C2C: improved 80.5% to 90.1%. Corrected the resource lookup's second argument to the already-loaded 0xEBF6 hash; zero was incorrect. Saving the mode pointer before reusing the r4 base for flags frees the state-counter register. Scoped callback, phase and separate byte/word-zero values improve setup. Script zero, vector, RAM-label and pointer variants were checked. Final differences: 55 total / 31 structural.

The compiler cause-search mode was unavailable because its local causebook data was absent. Per-function exact checks passed after rebasing. Combined validation: `build/cxbuild` printed `build/boktai3.gba: OK`.
