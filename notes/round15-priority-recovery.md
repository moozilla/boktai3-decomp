# Round 15 — priority source recovery

Tags: #matching #compiler #structs #phase3 #round15

Extended continuation, 2026-11-17. Updated in place after checkpoint 2.
This is a source-recovery report and handoff. The remaining allocation-only forms
are still concessions. Completing an investigation or draining a work queue does
not mean those forms have been recovered as ordinary source.

## Outcome

The continuation recovered eight reported priority sites in production: three
dead arms and one zero expression in the result panel, and four zero pointer
adjustments in Tentacle retraction. Together with Armor and palette transition
from the earlier checkpoints, the round has ten actual priority source removals.
The terrain-walker specialization annotation is a separate justified exemption.
The known Vorn detector false positive remains visible and unchanged.

Substantial owner and interface recoveries also match stock: shared sequence and
keyframe records, scene-actor drawable records, Nidhoggr, Spider, Skeleton,
Hresvelgr defeat, Vanargand, Ratatosk particles and callbacks, SpriteTrail,
Vulcan and Axe sprites, menu palette ownership, and the script-load call.
These changes do not receive priority-removal credit unless the actual report
was removed. Two authentic libgcc representation unions receive explicit
justifications; their removal from the warning count is not source cleanup.

The unresolved priority functions and all three volatile errors remain open.
Diagnostic compiler controls explain machine-code differences; they are not
production changes or evidence that the original authors wrote a given C form.
The three scope/matchsearch fixes in this batch improve diagnostic correctness
without changing the stock compiler or weakening the concession detector.

## Reproduction and acceptance

- Base: `4ce8f0d5d826909e6b9fa1caf60609a0fb7194c3` (`origin/main`). A fresh fetch
  during this continuation found the same commit; merge was already up to date.
- Prior checkpoint: `6268476a887661c9c72907e60786767c413e30d3`.
- Accepted source freeze: `821f3a25ca31e2cabc70239420524dff6f4cdb23`.
  Later documentation commits do not change that verified source context.
- Branch: `codex/round15`; the delivered bundle is cumulative from the base.
- Production compiler: stock agbcc `da598c1d918402c42c0c0d7128ba14567f3175e9`,
  using the repository's established flags and existing library specializations.
- Reference ROM SHA-1: `2651c5e6875ac60abff734510d152166d211c87c`.
- `PROGRESS.md` and `PROGRESS.json` remain unchanged from the base.
- No diagnostic compiler override, assembly, register pin, barrier, replacement
  impossible guard, dummy use, or new warning is accepted in production.

The final combined header check passed **3,007 of 3,007 dependent units** for
37 changed existing headers. Focused gates also cover the new native APIs and
the combined HUD owners: 105/105 HUD/Grid/MenuPrompt/DarkLoan dependents,
94/94 InventoryGrid dependents, 23/23 LevelAdvance API dependents, and all
directly changed complete units. These sets overlap and are not added together.

The final `make -j8` passed with `build/boktai3.gba: OK`; an independent hash
read confirms SHA-1 `2651c5e6875ac60abff734510d152166d211c87c`.
The RAM-literal gate also passes. The broader `tools/ramlayout.py check` is not
clean: it reports seven declaration/manifest inconsistencies and one missing
address comment, exactly as at the round base. The before/after audit is
preserved; there are no new inconsistencies, and these older issues are not
presented as a passing gate.

All 26 experimental full-unit candidates were replayed against archived final
headers and an isolated copy of the post-build ELF. Their source hashes, binary
hashes, lengths and differing-byte counts agree with the delivery manifest.
The combined source patch applies cleanly, and all 26 patched source hashes
verify. This checks reproducibility of their nonmatching status, not acceptance.

## Compiler-inclusive census

The final compiler-inclusive scan reports **3 errors and 4,238 warnings across
6,591 files**. This continuation removes 23 warnings from checkpoint 2; the
whole round removes 33 from the base.

| Kind | Round base | Checkpoint 2 | Final |
|---|---:|---:|---:|
| Volatile errors | 3 | 3 | 3 |
| Dead code | 13 | 12 | 9 |
| Proved zero | 33 | 31 | 26 |
| Once loop | 114 | 114 | 114 |
| Statement expression | 6 | 6 | 6 |
| Constant local | 1,187 | 1,184 | 1,183 |
| Literal local | 2,733 | 2,729 | 2,726 |
| Address/dereference | 41 | 41 | 41 |
| Placeholder name | 8 | 8 | 8 |
| Volatile view | 5 | 5 | 5 |
| Local union | 131 | 131 | 120 |
| **Warnings** | **4,271** | **4,261** | **4,238** |

The 33-warning reduction comprises **30 actual source-site removals and three
justified exemptions**: ten priority forms, nine unions, four constant locals
and seven literal locals were removed; one terrain specialization and two
original libgcc representations were justified. The priority report count falls
from 46 to 35. With the unchanged Vorn false positive separated, **34 real
priority sites remain across 22 functions**, as well as the three volatile errors.

Warning reductions are separated into actual source changes and exemptions.
Unmatched drafts, interface repairs that preserve the same warnings, unreported
concessions, and scanner blind spots do not contribute to the priority count.

## Priority source recoveries

| Function | Accepted change | Stock result and remaining limits |
|---|---|---|
| `ArmorActor_UpdateFixedSpeedMotion`, `08183E4C` | Actual signed X/Y/Z velocity owner and the full fixed-speed/nudge operation | 448 bytes exact; one proved-zero, two const-local and two literal-local warnings removed. The existing raw flag offset is not claimed recovered. |
| `SceneActorPalette_ApplyTransition`, `08071F50` | Complete palette advancement and binding operations; actual manager flags and existing actor-down predicate | 468 bytes exact; one reported zero and two literal locals removed. An earlier unreported negative guard was also removed. Two named category masks remain warned. |
| `WirelessResultPanel_Init`, `0803B40C` | Independent sequence and priority objects replace a reused aggregate; ordinary state stores replace fabricated control flow | 684 bytes exact; three dead-code, one proved-zero and one local-union reports removed. Two unreported resource-copy guards, two zero locals and a flag literal remain. |
| `TentacleActor_UpdateRetraction`, `080AB968` | Direct pointers replace four zero adjustments; existing shared control and state operations replace private clones | 324 bytes exact; four proved-zero, one const-local and one literal-local reports removed. The pointer union, impossible byte-negative guards, redundant directional arms and one-element threshold array remain unreported source debt. |

Relevant commits: Armor `4d65a5f05`; palette `23c8ed5d7` and `8f2863fc5`;
result panel `409ad36bb`; Tentacle `8ab494824` and `0278631d6`.

### Why the Armor and palette recoveries are ordinary source

Armor's owned halfword velocity stores naturally generate the ROM's otherwise
unused zero register: the compiler's halfword-store preservation mask creates
that value, and the real negative-direction nudge stores retain its uses. No
replacement zero expression was inserted. The complete operation computes X/Z
velocity, clears Y, and nudges a zero component toward the terrain-cell centre.

The palette operation advances all sixteen fixed-point RGB channels, packs their
RGB555 values and decrements the remaining count. The binding operation owns the
real behavior flag and drawable-specific binding. The existing `ActorStatus_IsDown`
checks the same null, motion, status and signed-health cases. Those operations
recover the required ordering without a fabricated index, weighted sampler,
address-local permutation or private pointer identity.

### Partial recovery is explicitly partial

Removing the result panel's two remaining impossible resource guards produces
680 bytes rather than 684. The missing copies survive allocation but are deleted
by `jump2`'s inverse-copy scan. The guards keep labels present until that scan.
A private diagnostic that suppresses only the relevant inverse-copy deletion
restores all 684 bytes; no ordinary operation has yet reproduced them.

Tentacle's real completion result permits the four call tails to merge, but PRE
then hoists the actor's completion-field address too early. Narrower native-call
hypotheses disagree with the actual defining routines and were rejected. The
accepted four zero removals do not authenticate the remaining pointer union or
byte-negative arms.

## Accepted ownership and interfaces

These are independent improvements, not claims that adjacent priority forms have
been solved. All accepted unit changes match with the stock compiler.

| Area | Recovery | Validation / remaining boundary |
|---|---|---|
| Shared playback | Actual sequence identity, keyframe state/resource records, widget playback fields, and complete scene-actor drawable owners | `ad6c73d11`, `9556b1564`, `838d2da91`, `3e03e07eb`. Large dependent sets and direct units match; the combined header gate covers the integrated result. The broad animation scalar ABI still has seven sequence-caller mismatches in its separate trial and was not imposed globally. |
| Nidhoggr | Native widget/resource owners for 26 parts | `9b4b15ec1`; 25,420-byte unit and all three dependents exact. The palette address at actor+0x723C is rowBlink[18], not the time palette at 0x72DC; that unresolved alias remains explicit. |
| Spider | Canonical actor/group/room navigation state and signed wander/recovery timers | `dc3672bf5`; 15,172 bytes exact. Two proved-zero reports and the existing compiler diagnostics remain; the clean drafts do not match. |
| Hresvelgr defeat | Actual resource/widget, ContactPosition, complete action record and shared terrain operation | `1320be981`; 1,764 bytes exact. The reported fallback, unreported negative sample guard and two pointer unions remain. |
| Vanargand | Actual positioned-effect coordinates, widget/resource fields, effect origin and palette bank storage | `5b8476803`; 5,064 bytes exact. The one-word preset and dead palette fallback remain visible. |
| Fragment group | Typed singleton, native sprite-render resource and synchronized RAM manifest | `670fd4bdf`, `58d76ab0b`, `3c36c22fa`; both units match, and the manifest follows the canonical singleton declaration. Three zero expressions remain. |
| Terrain operations | Shared cell/height sampling and the retreat caller's use of it | `fd0ad27e1`, `10260f161`; terrain 8,896 bytes and retreat 788 bytes exact. The synthetic random origin in retreat remains unexplained. |
| Skeleton | Native SparkParticle contract, canonical Skeleton owner, revivalDelay and both reward fields, shared state/control operations | `dc627f01a`, `7f4888058`, `f4c437099`; 7,924 bytes exact, three header dependents exact. Two literal locals disappear; two dead arms, one zero and two unions remain. |
| Vorn defeat | Native bestiary-index return contract | `434256e2e`; no priority reduction. Its three zero terms remain, independently of the known detector false positive in another Vorn function. |
| Ratatosk | Real 32-slot pool, 0x78-byte particles, 0x1C-byte configuration, typed singleton and direct behavior callbacks; native sequence stepping | `6a7f980ac`, `b52de00a4`, `482ed3291`, `c854ef133`; all 15 owner dependents and three singleton users match. Spawn retains exactly one proved-zero warning, before and after playback cleanup. Only two behavior-table targets are established; adjacent OrbitEffect pointers do not establish more entries. |
| SpriteTrail | Complete native sprites and graphics resource; actual history/position offsets | `4005e0fd4`; all ten header dependents and fourteen layout assertions pass. The volatile camera error and two constructor once-loops remain. |
| Menu palette | RegionMenu uses its sixteen colors, RecordMenu uses its existing palette[12], and the native pulse operation has one shared declaration | `edf6056d6`, `87ee4edcb`, `8d37fe2f9`; all 303 pulse-header dependents exact. RegionMenu loses one union; RecordMenu is an ownership correction without warning credit. |
| Script VM | Explicit returned-resource argument in the nested script-load call | `a61e35e26`; full 5,800-byte unit exact. The old claim that an unprototyped register passthrough was required is corrected in `notes/script_virtual_machine.md`. GameState's zero mask remains. |
| Vulcan cannon | One actual sprite/resource owner, native playback and a shared complete copy-and-raise effect-position operation | `62c8a2f7f`; all six dependents exact, twelve ARM layout assertions pass, six compiler pointer warnings disappear. Both unions are removed. The two literal locals remain visible. |
| Axe | Native trail sprites and canonical auxiliary sprite/resource/sequence owner shared with contact handlers; emitted trails use the existing Axe sequence operation | `d8fe51260`, `5cccc3e03`, `bc01ec9df`; 29,148- and 2,708-byte units exact, ten offsets and 0xF24 owner size verified. Two unions disappear. Replacing the shared Axe helper with the broader DisplaySequence operation still misses. |
| Native tile-sprite API | Existing 0x28-byte SpriteDrawable owns initialization, requeue, origin, palette and frame selection | `fd3ec6c3d` through `231fa6fdd`; five defining units total 264 bytes and match without diagnostics. |
| SpiralBurst fields | Native drawable priority, signed depth offset, attributes, links, resource and contact-position types | `4128a5590`; 428- and 340-byte units and both header dependents exact. The complete-owner investigation remains nonmatching; three zero expressions and a provisional frame/motion split remain. |
| Inventory callback groups | Actual 0x40-byte entries, 8-byte lifecycle tails and eight-entry 0x208-byte groups | `cdfd3a2ba`, `0ecff2709`; complete 2,728-byte unit exact, thirteen ARM layout assertions. One union disappears; seventeen existing diagnostics in untouched stat comparison are unchanged. |
| LevelAdvance and brief effect | Native SpriteDrawable prefix and initialization, origin, palette, frame and requeue API | `be93031f6` through `1c4c6c95d`; fourteen LevelAdvance dependents total 1,532 bytes and the 108-byte brief effect match. Thirteen layout assertions and 23 API dependents pass. Existing seven warnings remain; the brief effect's private Player-tail view is still partial. |
| InventoryGrid | Canonical HUD fields, four actual action-value digit renderers, and a shared owner API used by the direct dispatch caller | `49ac040ef` through `1c3a13828`; six functions total 1,184 bytes exact, eighteen layout assertions and 94 dependents pass. One union disappears. Byte-strided flag traversal and the erased scene callback declaration remain explicit. |
| MenuPrompt | Shared HUD, Player and DarkLoan owners replace duplicate private panel/account/sprite records | `821f3a25c`; complete 1,736-byte unit exact, combined HUD gate 105/105. One union disappears and four raw pointer diagnostics disappear. The price renderer's separate overlay and generic indexed HUD helpers remain open. |

The union tier has an immutable 130-site classification and individual source
decisions in `notes/round15-union-review.md`. Nine unions have actually been
removed during the round; two further reports have original-library evidence.
The HUD model still needs a common backing-array representation: initialization
and teardown establish 124 consecutive widgets, while current named groups
partition only parts of that storage. Accepted Grid selections stay within the
first 30 widgets; this does not establish that group as an exclusive eight-item
array. The MenuPrompt price renderer and cross-group indexing remain partial.

## Every added exemption

There are three cumulative exemptions so far, each grounded independently of
matching behavior:

1. `src/other/scene_terrain_probe/spread_shot.c`, `SpreadShot_ResolveTerrain`, `08123194`: the existing
   bidirectional walker is specialized for the left-neighbor call. Its forward
   arm is genuinely used by the right-neighbor and runtime-target calls. The
   direct backward-only alternative misses; the annotation is specialization
   evidence, not a fabricated-source removal. Accepted in `5dbd9f911`.
2. `include/lib/libgcc/libgcc_numbers.h`, `SoftDoubleNumber.fraction`: the bundled original
   FSF `fp-bit-base.c` declares the 64-bit fraction and two 32-bit halves in a union.
3. The same header, `SoftSingleNumber.fraction`: the FLOAT specialization of that
   original declaration uses a 32-bit fraction and two 16-bit halves.

The libgcc source is pinned with agbcc at
`da598c1d918402c42c0c0d7128ba14567f3175e9`; its source hash is
`9aa27be26b945edde2c6ef81d0289724d1d9ca1e756a1a98230ab58de46091eb`.
The original union exists even where one specialization only reads `fraction.ll`.
Both library units remain exact, totaling 5,864 bytes; no library compiler setting
changed. These two annotations are accepted in `deda4728e` and count as justified
representations, not two removed steering tricks.

No broad literal-local ignores were added. The palette category masks (`0x8000`
and `0x4000`) remain listed as owner decisions because direct literals change tail
sharing. Other incidental literal reductions came from real operation recovery.

## Diagnostic tooling fixes

| Commit | Defect | Verification |
|---|---|---|
| `58c2ab530` | Some named diagnostic knobs reached matchsearch without scope's variable-to-pseudo resolution and silently did nothing | Named and numeric table controls now agree; ordinary source acceptance remains separate. |
| `ead63bbea` | The debug/no-debug baseline check inherited shell AGBCC_/CSE_ controls even when called with an empty knob map | Both baseline sides isolate controls, retain infrastructure settings and restore the environment in a finally path. Clean normal/debug byte comparisons agree. |
| `f81cc1a86` | Asking why an inserted regmove copy existed did not trace it to its consuming operation | The recognizer validates the next real operation, source/destination relationship and provenance; rejects labels, calls, branches, barriers, unrelated and preexisting copies; reports operand choice without speculative register-forcing advice. |

Focused integration tests passed: 46 tests for baseline isolation/evaluator/named
selectors and 12 tests for inserted-copy attribution/probe summaries. The worker's
broader inserted-copy group passed 34 tests. Existing ResourceWarnings were not
new test failures. No production compiler patch or linter exemption implements
these fixes.

## Open priority and error frontiers

The following source frontiers remain research proposals. Some isolate priority
forms while retaining other inherited source debt. Byte counts are
function extents unless the row explicitly states a whole unit. Instruction-row
counts are alignment diagnostics, not semantic-equivalence percentages.

| Address / function | Source frontier | Complete cause or outstanding investigation |
|---|---|---|
| `08014CA4` Fragment radial spawn | 704 B; 531 differing byte positions; removes three zeros | Two address PRE replacements plus random-table constant equivalence; diagnostic NOPRE 1,2 + NOEQUIV table is exact. Full native owner/iteration/sampler operations were tried. |
| `080154EC` Animated particles | 1,164 vs 1,176 B | Incorrect sharing of type-5/6/7 animation-call tails; selective cross-jump limits reproduce all bytes. Actual owners, native ABI and complete sequence operations do not yet supply the real distinguishing uses. |
| `08016118` Fading particles | 540 B; 122 byte positions | GCSE PRE replacement 4 shares the vertical-velocity load across gravity update, changing allocation and reload rotation. NOPRE 4 is exact. An independent lifetime guard also remains. |
| `08038448` Wireless equipment choices | 408 B; eight/eight differing instruction rows | Unused saved SL home plus independent RNG reload rotation. Diagnostic-only unused-home control and SPILL 11:1 explain all bytes; this is not an ordinary source proposal. |
| `0804373C` Contact retreat | 776 vs 788 B; 13/8 rows | Removing the synthetic random-origin zero drops a dead address load, changes the centre alias home and save/pool layout. Diagnostic-only origin lifetime and allocation controls reproduce 788 B. The value 0x0403B714 has no established machine consumer; no fake origin helper was accepted. |
| `0804CB4C` Tether bounce | 344 vs 356 B; 21/16 rows | Table equivalence/rematerialization changes frame-address placement and reload scratch state. NOEQUIV table is exact. This is not described as a local-priority change alone. |
| `0805B0F0` Scene rig | 412 B; twelve exchanged register instructions | Table/mask homes swap. NOEQUIV table or table priority +138 reproduces all bytes. Real sampling and full rig/motion operations still miss. |
| `0805E1D0` Scene choice | 160 B; two differing bytes | CSE commutes the known-one AND operand, then regmove copies keys rather than confirm. Private suppression of that one commutation is exact; the values already have the correct homes. |
| `0808E838` Slime wait | 360-B function; 15,436-B unit with 177 differing byte positions | Missing unused mask copy plus alignment and offset changes. Shared helper's initializer dies in global allocation; the production helper preserves it until its impossible arm disappears at jump2. Actual animation widths and complete byte control do not restore the copy. |
| `080A4018` Spider wander | 592 vs 584 B; 15,180 vs 15,172-B unit; 27/29 rows | Current zero-free frontier is diagnostic-exact with NOEQUIV table,indexAddress plus FORCE input:4,body:5,table:6,indexAddress:10,mask:9. Names were resolved against its current RTL; old transform IDs were not reused. |
| `080AEE08` Vorn defeat | 988 vs 984 B; 8/9 rows | Final implicit zero receives r4 instead of r5. FORCE 442:5 on the frontier is exact. The partial frontier still contains the completedState local and weighting macro; a broader rewrite is preserved separately. |
| `080BD1F0` Skeleton defeat | Strong partial: 7,924 B, 29/29 rows; broader rewrite 7,888 B | Fake table exit uses reserve SL; otherwise PRE's body-position address takes SL instead of stack+0x98. Three-home diagnostic matches the partial frontier. Actual owner, scripts, full spark/control operations and independent lifetimes were investigated. The partial frontier retains two unions. |
| `0815F5B4` Registry lookup | 68 B, 32 byte positions with correct registers; alternate 68 B with five register substitutions | First form shares the wrong result tail; NOCROSS 67 is exact. Shared-comparison alternate has the right CFG but wrong allocation; ID priority control is exact. Requested ID stays word-sized and original non-advancing mismatch behavior is preserved. |
| `0815F6A8` and `0815F6F0` Registry insertion errors | Plain singleton: 72/68 B against 72/72 B, each 25 byte positions | Two independent alias exclusions: false incoming-r0 structure-return provenance and scalar-versus-struct classification. Neither control alone fixes either; removing both gives exact 72/72 B. Actual callers, node and manager lifecycle rule out invented handles or overlays. |
| `0816E570` Action-icon launch | 376 vs 384 B | Table equivalence changes constant lifetime and allocation. Named/numeric NOEQUIV table now both match 384 B. Nineteen full owner, spin, vector and motion hypotheses are preserved. |
| `0817AEC8` Impact scatter | Priority-only partial: polar-operation draft 296 vs 288 B; 12/15 rows | PRE creates a second sine base; loop hoists it and its low allocation priority causes rematerialization and downstream reload rotation. FORCE 159:2 on this exact frontier matches all 288 bytes with normal/debug parity. NOEQUIV alone does not. The RNG address union, halfword-built address, mask local and private emitter view remain; thirty earlier whole-operation hypotheses are preserved. |
| `081A0BC8` Hresvelgr barrage | 732 B; two differing bytes | RNG mask is evicted from r5 and reload reuses SL; ROM uses IP. FORCE 237:12 is exact. Signed-halfword ScatterOffset fits the coordinate domain but its original API remains inferred. |
| `081A14FC` Hresvelgr defeat | Priority-only partial: 1,764 B, 51/51 rows; retains case-2 RandomTableView | Current frontier is fully factored by NOEQUIV table 246, FORCE Y-sample 355:3, and private commutation of the existing X/Z coordinate ADDs (destinations 389/391). All nine control cases have normal/debug parity. Genuine centre-plus-offset, coordinate locals and vector translation were tried; no source override is accepted. |
| `081AE0C0` Vanargand initializer | 5,048 vs 5,064 B; 22/14 rows | CSE joins a pending-animation constant to a presentation store, plus independent PRE hash/spill ordering. Each control alone leaves differences; combined controls match 5,064 B. |
| `081F2404` SpriteTrail volatile error | Native owner draft 320 vs 304 B; 31/39 rows | False incoming-r0 structure-return alias base permits coupled camera-address/singleton loop hoisting. NOMOVE 164 and an independent no-sret-base compiler control each match 304 B. Struct classification alone changes nothing here, unlike registry insertion. |
| `081FC26C` Ratatosk particle spawn | 948 vs 940 B; 5/8 rows, candidate lint zero | Literal-one allocation controls two-store tail sharing; an orphan copy protected by a synthetic USE accounts for the last ROM-only instruction. Independent controls from ordinary and guarded source produce identical 940-B output with normal/debug parity. |
| `08217430` Object palette fade | 1,324 vs 1,328 B; 15/13 rows | Argument home plus dying-base allocation tie explain the dead fallback; independent black-fade cursor priority explains the once-loop. FORCE bank:3 + NOTIE 61 + PRI src:+1014 reproduces all 1,328 B. |
| `0821B084` Game-state base selection | 88 B; one differing byte in the whole 5,800-B unit | CSE commutes the symbolic base and timer offset. Private symbol-specific PLUS commutation control is exact with normal/debug parity. Five-region ownership and actual sequential allocation operations do not recover the order. |
| `08234540` SpiralBurst initialization | 420 vs 428 B; 210 differing byte positions including eight missing | Random-table REG_EQUIV doubles its lifetime from 146 to 292, leaving priority 1130 below the sprite. Freshly resolved PRI table:+3600 is exact. Native full-owner, constructor, sampling, projection and fresh-ROM rewrites remain nonmatching; forty source nodes are preserved. |

The known `Vorn_BeginPursuitRoute` detector false positive is excluded from the
real-site count. Its source and detector behavior are unchanged.

### Ratatosk's final causal check

Earlier partial CSE-path controls operated on an older draft and did not explain
the final frontier. The independent follow-up uses the current native owner and
normalized real field stores. Forcing its implicit literal one to the ROM home
restores the correct tail merging and produces 940 bytes, leaving only a missing
copy and compensating NOP. Removing only that orphan copy and its synthetic USE
from the exact guarded production source produces the same 940-byte hash:
`23acfbc02a0322df8af22f69f712a1ca2b11e5cdfb20194b22a3e130ae4128da`.
All four controlled/uncontrolled normal/debug pairs agree.

The old guard creates the copy; combine attaches the USE; post-reload flow keeps
it; jump2 removes the compare and branch without a death note that would delete
the copy. This accounts for every frontier hunk without presenting inserted
assembly or a private compiler as source recovery. The source remains open.

## Portable research and next work

The delivery consists of three files:

- `round15-batch3.bundle`: cumulative Git history from the round base, containing
  the accepted source, tools, this maintained report, the union review, and the
  open-candidate patch and manifest. It requires the pinned base commit above.
- `round15-batch3_2026-11-17.md`: a byte-identical standalone copy of this report.
  The Markdown is therefore both inside the Git bundle and separately readable.
- `round15-research.tar.gz`: 26 current complete candidate C files, their patch
  and per-record metadata, 66 exported search histories, selected open-case
  evidence and diagnostic compiler source patches, the immutable union census,
  and compact accepted-owner and validation evidence. Search histories contain
  repeated fork ancestry; they are not 66 independent solved investigations.

The research archive has a per-file SHA-256/size manifest and a README describing
replay and context boundaries. It excludes the ROM, generated assembly/RTL,
compiler binaries and build caches. Historical worker commits and original local
store paths are provenance, not promises that those commits or paths exist in the
cumulative bundle. Complete historical C sources are retained, but old scores
need a fresh matching context after owner/header changes. The current 26-source
patch and manifest are the verified portable replay against the accepted freeze.

Import the bundle into a checkout containing the pinned base, inspect its
`codex/round15` branch, and use the existing kit/toolchain setup. Normal production
builds do not apply the experimental patch. Research compiler controls are only
causal experiments; none is required for the accepted ROM build.

The source-only patch is experimental and is not part of the production build.
Its metadata identifies each candidate's accepted-source base and hash. Preserve
the distinction between a priority-only partial and a fully ordinary rewrite.
If a candidate is replayed after an owner or header change, freeze a new search
context and remeasure it; old pseudo IDs and old near-match counts are not current
proofs. Do not bypass the matchsearch context guard.

The priority and error queues have been investigated, not solved wholesale.
Reopen a holdout when independently evidenced ownership, a real shared operation,
ABI information or compiler provenance supplies a new lead. Do not convert an
empty work queue into a claim that the remaining guards, zero terms, unions,
reused locals or weighting loops are acceptable original source.

Continue the union tier in `notes/round15-union-review.md`. The classified inventory
is a review queue, not a blanket concession list and not a blanket exemption list.
Then follow the assignment's order for once-loops and related forms, const-locals,
and finally the weakest literal-local reports. Do not replace unresolved priority
work with a count-only cleanup or bulk annotation pass.
