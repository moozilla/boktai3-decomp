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
