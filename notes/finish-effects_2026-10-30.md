# Finish effects and presentation — 2026-10-30

Tags: #effects #presentation #matching #compiler

The first pass and ongoing continuation on `codex/finish-effects` have matched twenty original target spans in ordinary C, adding +0.581 percentage points of code (+14,016 bytes). Three recovered HUD routine starts make twenty-three newly exact inventory functions. Fourteen targets remain under active matching. The user requested continuing through all matches, then removing remaining register pins and unifying the subsection's structs, data and Studio integration. No new pins, barriers or compiler overrides are committed.

| Address | Function | Bytes | Result | Deciding source property or open mechanism |
|---|---|---:|---|---|
| 080148CC | `ParticleGroup_SpawnRadial` | 860 | Exact | Addressable inline input homes, short direction output captures, typed samples, and phased setup values recover the full frame and remove all eight concessions. |
| 08015A0C | `AnimatedParticles_EmitRandomBurst` | 272 | Exact | Full-width argument, signed local capture, countdown and dead table reset retain the loop-head load. |
| 08017044 | `FloatingItemEffect_Spawn` | 544 | Exact | Projection helper caches x/z; multiplication belongs at the signed-shift call site. |
| 0803EF4C | `SceneArc_UpdateSlot` | 1,112 | Exact | Addressable velocity builder, canonical playback, phase/table split and wide actor flag test. |
| 08047DF4 | `DirectionalParticle_Spawn` | 372 | Exact | Shared raw/packed coordinate storage keeps the first mask in HImode and preserves the ROM reload copy. |
| 08068500 | `SceneParticle_CreateAnchoredRanges` | 644 | Exact | Shared counter/table iteration state, offset-first sampling, and an actual component operand with two definitions preserve the stack table and local clear mask. |
| 0806A160 | `SparkParticle_Spawn` | 704 | Draft | Pool-handle/table storage phases recover table spilling and the loop body; the first pool load and reload setup remain. |
| 0806AF74 | `SpriteEffectController_UpdateTileBounce` | 848 | Exact | Direct controller flags ownership gives frame and flag PRE values the original lifetimes and copy placement. |
| 08111578 | `sub_08111578` | 464 | Exact | Native halfword channel storage, readonly masks, and an addressable palette owner recover channel modes and two-pass invariant order. |
| 08111A48 | `PaletteFade_StartFromScript` | 380 | Exact | Correct blue start channel, phased halfword masks and byte-bank pointer association. |
| 08126D88 | `SpriteProjectile_UpdateRebounds` | 1,260 | Exact | Canonical preincrement playback and shared records remove every pin and barrier. |
| 08127590 | `SpriteProjectile_Create` | 696 | Exact | Real playback-pointer/activation storage phases preserve the r5 activation flag and r4 random origin. |
| 08128268 | `FragmentBurst_UpdateRadialAnimation` | 632 | Exact | Sequence setup captures actual resource and mirror outputs before returning the base, deferring its reduction to pass two. Shared position and animation records replace local duplicates. |
| 08128554 | `FragmentBurst_UpdateImpactAnimation` | 536 | Exact | Animation/origin output capture fixes setup order; a native whole-byte control snapshot prevents the dying-input ties. |
| 08128D24 | `FeatherEffectPool_ClipScatterPath` | 628 | Exact | The incoming cell cursor becomes the random-table cursor after clipping; its earlier nonconstant phase restores table/mask allocation. |
| 08128F98 | `FeatherEffectPool_SpawnGroundScatter` | 828 | Draft | Addressable packed constructor settings recover branch-local argument constants; preheader ordering and two random-sample operand pairs remain. |
| 08129C64 | `SunDiscPool_UpdateTravellingDisc` | 908 | Exact | Shared records, cached sprite view, vector operand order and inline parameter/capture order. |
| 08129FF0 | `SunDiscPool_UpdateFallingDisc` | 604 | Exact | Canonical playback, signed coordinate views, cached sprite owner and terrain conversion boundaries. |
| 0812A928 | `LinkedEffect_SetVariant` | 228 | Open | Real component/operand phases recover every instruction; the table still loses allocation priority to the count and mask. |
| 081745DC | `SceneHud_RefreshEquipmentPreview` | 732 | Exact | Full 732-byte span: existing 168-byte preview plus recovered 564-byte values routine; decimal remainder and typed cursor lifetimes. |
| 08174930 | `SceneHud_RefreshEquipmentAvailability` | 604 | Exact | Full 604-byte span: 276-byte availability display, 148-byte transfer setup and 180-byte action flags. Branch-local view captures and offset-first final sum. |
| 08193874 | `Nidhoggr_ReceivePartContact` | 1,216 | Exact | Two-argument damage-response ABI, compound halfword health update and grouped health/stage views. |
| 081940F8 | `Nidhoggr_InitParts` | 904 | Draft | Field-derived collider offsets delay CSE folding; only zero initialization placement and zero-bound comparison remain, 2/2 instructions. |
| 0819946C | `SceneEffectQueue_PostTypeZeroMessage` | 804 | Draft | All three 268-byte messages reconstructed. Twelve ip/r8 choices remain; one priority mechanism makes the full 804-byte span exact diagnostically. |
| 081C32C8 | `SceneTarget_BeginMove` | 804 | Draft | Correct zero conversion offset, captured conversion outputs and late screen view. Thirteen local register choices; `AGBCC_FORCE=341:5` alone exact. |
| 081D9D3C | `Projectile_SpawnImpactPair` | 476 | Open | Random-table REG_EQUIV priority; `AGBCC_PRI=118:+1060` is exact on an ordinary witness. Original retained as the better production draft. |
| 081DA3D8 | `TimedEffect_UpdateManager` | 580 | Draft | Snapshot output capture restores retained tail naturally; saved-pointer rematerialization and redundant zero test remain coupled. Two barriers removed. |
| 081FCA44 | `SceneProjectile_UpdatePool` | 1,268 | Draft | Shared animation/geometry and typed launch views; in-loop stride 448 prevents cursor BIV recognition. Coupled GIV/allocation remains. |
| 081FE2C4 | `OrbitEffect_UpdateManager` | 896 | Open | Early user state/flags spills precede PRE-generated address slots; owner versus sprite view sharing and reload rotation are coupled. Original retained. |
| 081FFAB8 | `HomingProjectile_UpdateManager` | 704 | Exact | Phased byte control masks, captured motion pointers, direct loop index and shared mask reload lifetime remove eight pins and three barriers. |
| 082314C4 | `EffectGroup_FadeFirstActive` | 60 | Open | One extra success MOV1. A private CSE register-source diagnostic removes it but changes branch threading; original retained. |
| 082335F0 | `ChargedBurst_EmitSpark` | 292 | Draft | Four pins removed. Byte-mask sinking, stride/mask priority and table/sample-pointer storage remain coupled. |
| 08234868 | `HealthRestoreEffect_StartParticleBurst` | 248 | Open | Table REG_EQUIV suppression restores extent; independent GIV setup/update order and reload roles remain. Original retained. |
| 082362EC | `RadialBurstEffect_StartBurst` | 316 | Draft | Four pins removed; full 316-byte instruction shape recovered. Lookup versions, seed alias, local zeros and cursor relationships still differ. |

## Validation and integration

Each exact function passes scope with stock settings, matchsearch stock verification and the independent per-unit check through its reviewed extent. The Nidhoggr shared unit was checked again after landing both its exact callback and its guarded initializer. Each landed guarded draft passes its normal fallback unit check and has a refreshed `--nonmatching --record` score. Unchanged drafts keep their existing recorded metadata. Private diagnostic compilers and search evidence remain under `build/scope/campaign/` and `build/matchsearch/`; none is committed. The shared compiler is unchanged.

The worker does not run a combined ROM build, push, open a PR, regenerate the checked-in progress reports or regroup subsystems. The integrator must perform the combined build and report update when integrating the batch.

Early checks used a local in-memory CSV alias wrapper because the checker initially resolved new names only through the last full-build ELF. User-requested tooling commit `02f36efc6` was cherry-picked as `9680e3b36`; standard `tools/check.py` now reads missing names from authoritative CSV and matchsearch uses short temporary sockets in long worktree paths. A direct 604-byte named HUD check, resident-server check and 18 focused standard-library tests pass. The wrapper is obsolete. Matchsearch contexts are forked after symbol-name changes.

Continuation evidence is under `build/scope/campaign/resume_ADDRESS/REPORT.md` and corresponding `build/matchsearch/resume_ADDRESS/` graphs. Detailed first-pass reports are under `build/scope/campaign/ADDRESS/REPORT.md` (directories use the `sub_` address label). Scope gaps include constant-equivalence rematerialization, equivalent reload-copy deletion, PRE insertion placement, and GIV initialization order. Counterfactual matches are evidence only and earn no exact credit.

## Additional scope evidence to retain

Tags: #effects #scope-gap #compiler

This note records production scores from landed source; search candidates can score differently when unrelated guarded neighbors compile. Ground scatter is 82.7%, Nidhoggr initialization 99.1%, message queue 97.0% and timed effects 86.5%. Fuzzy figures earn no exact credit. Arc movement's former draft has since been replaced by an exact match.

The linked-effect variant, impact-pair spawner and orbit manager retain their existing drafts because their ordinary alternatives did not justify replacement. They still have persistent hypothesis graphs and compiler evidence. The message queue retains one guarded 804-byte extent: its two recovered trailing routines are not registered separately while the span is nonmatching. The HUD exact spans register 08174684, 08174A44 and 08174AD8; independent checks still cover the original 732/604 bytes.

## Exact source decisions recovered in this pass

Tags: #matching #effects #presentation

`081745DC` required retaining a separate decimal remainder, independent control/widget cursors and the forward fill loop that loop.c reverses. Descriptor/resource/frame-base/digit declaration ownership recovers the reviewed stack slots. `08174930` contains two additional routines: icon transfer setup and equipment action flags. The latter retains descriptor base/offset and state-address views on each incoming branch to the level test; the final address sum starts with its byte offset. All 732/604 original bytes match after registering the hidden starts.

`08193874` was missing the collider argument to the damage-response call. Correcting that ABI, using compound halfword health subtraction and the existing grouped stage record recovers the entire 1216-byte routine and preserves its 964-byte sibling. `081FFAB8` uses canonical preincrement playback and byte control unions; these stop an unintended mask hoist. Captured flags/velocity outputs retain the ROM's distinct pointer views. A direct loop index removes an extra lifetime; mask-first clearing restores load order; the common 4 mask kept across the callback restores the final local reload choices.

## Open compiler decisions

Tags: #compiler #scope-gap #effects

The following counterfactuals are diagnostic evidence. Exact credit always uses production bytes with every knob absent. Pseudo/UID numbers refer to the preserved campaign candidate, not an arbitrary later spelling.

- `0806A160`: suppressing table `REG_EQUIV` for pseudo36 alone gives exact 704 bytes in the earlier witness. A real pool-handle/table union now preserves the table stack home in ordinary C; its loop body agrees after the first sample. Reusing the incoming position parameter for pool setup also recovers the first load and complete loop body, but moves the origin capture and pool stores. The remaining boundary is pool capture and initial reload selection.
- `08128F98`: the continuation recovers the packed coordinate operations and terrain mask. A real addressable packed constructor configuration delays field folding until CSE2, restoring branch-local one/zero constants and every constructor instruction. Count/cursor setup order and two random-sample operand pairs remain under active investigation.
- `0812A928`: real sine/mask/sample operand phases and a reused Z component recover all 228 bytes of instruction structure. The table's seven references yield priority 933, below count/mask at 985; a diagnostic priority increase alone matches. A readonly one-pointer bank raises table priority naturally and recovers the body, but initializes its literal too late. No diagnostic setting is accepted as production evidence.
- `081940F8`: a private CSE fold restriction on 26328+84 and 24144+84 proved the collider offset boundary. Field-derived relative offsets reproduce it in ordinary C. A literal-bound witness plus `AGBCC_FORCE=flags#2:8` leaves only the missing zero and padding; a real named lower bound recovers 904-byte size and all allocation but moves zero to the preheader and uses a register comparison. The two-instruction guarded gap is genuine.
- `0819946C`: three 268-byte routines have only twelve ip/r8 choices left. Packet pointer priority 153 loses to retained argument 183; `AGBCC_PRI=199:+31,201:+31` gives exact 804 bytes. A byte conversion used in the final type payload prevents an earlier zero sink. Canonical literal types instead emit 792 bytes. No hidden-start registration is counted yet.
- `081C32C8`: synthetic SI zero (pseudo 341) from the halfword store chooses r4; its non-overlapping world-pointer quantity and the projected sum then take the opposite r4/r5 roles. `AGBCC_FORCE=341:5` alone gives exact 804 bytes. Shared positions, late screen view and returned projection result preserve every instruction and stack slot in the ordinary 13/13 draft.
- `081D9D3C`: a table-capture witness proves the constant-equivalence boundary. `AGBCC_PRI=118:+1060` or private early-equivalence suppression gives exact 476 bytes. Its ordinary shape is worse than the original production draft, which is retained.
- `081DA3D8`: snapshot pointer priority 4285 displaces tail 2163. `AGBCC_PRI=tail:+2200` is only a partial 18/16 witness. An ordinary snapshot-output helper retains tail naturally but rematerializes the saved SP pointer. A supported byte-return helper recovers a redundant zero condition while merging/hoisting Polar's zero; it is omitted from the landed candidate because the combined source boundary is unresolved.
- `081FCA44`: a private `AGBCC_BIV_CONST=448` exposes the in-loop forced stride to basic-induction recognition and restores carried address families, but does not match the whole routine. Ordinary step/stride helpers recover recognition while reducing different GIV families. Cached renderer takeover and geometry/terrain/animation lifetimes remain coupled. The landed ordinary draft is 74.5%, 1232/1268 bytes, 158/140 scope differences. Its pre-existing local descriptive alias is retained; promoting it requires regenerated assembly paths and caller renaming at integration.
- `081FE2C4`: the state/flags user pointers spill before generated field addresses, while ROM needs them after. Using the effect owner for the later collider view avoids premature sprite-position PRE, but reloads and creation order remain coupled. Baseline `AGBCC_SPILL=30:1` improves 80/78 to 51/49; no exact counterfactual is claimed. Indexed state lacks an outer BIV after PRE; addressable, returned, aggregate and relative-offset cursor forms do not recover it.
- `082314C4`: the best production draft has one extra success `MOV1`. Private per-UID register-source selection removes that constant but branch threading then targets the zero test rather than bypassing it. A priority change repairs all operands while leaving that branch destination. Narrow/wide helpers, direct status flow, signed predicates, output captures and metadata views did not recover both; no whole exact counterfactual is claimed.
- `082335F0`: private early-equivalence suppression for pseudo 27 restores the 255 initializer at entry but spills it; `AGBCC_PRI=35:+1000` restores the 1023 mask's register ahead of stride 44. The table/sample frame homes and byte-mask storage remain coupled. Real mask sharing, prefix increments, counter widths and output helpers were tested. The ordinary draft removes four pins and records 67.1%; no exact counterfactual is claimed.
- `08234868`: table pseudo 23 rematerialization removes a four-byte frame home. Private equivalence suppression restores 248-byte size but leaves 14/14 instructions and two independent GIV initialization/increment order differences plus reload 255 choices. Indexed/cursor, RNG-context, reference-output and declaration-lifetime forms do not close both. Existing source is unchanged; refreshing its old fuzzy score is not code progress.
- `082362EC`: a real typed lookup pointer shared between random and first sine sampling prevents unintended invariant hoisting. Separate sample/speed temporaries restore load order; all 316 bytes of instruction structure agree. Twenty-five register-related byte differences remain across lookup version allocation, seed alias, zeros and cursor updates. Direct RNG globals, narrow speed/results/zeros, genuine store-result use, typed aliases and loop forms do not close it. The landed draft removes four pins and records 84.8%; supported combined diagnostics still leave fifteen instructions different.

The two zero/bound instructions at 081940F8 and the final return/threading difference at 082314C4 remain close targets. Particle groups, directional particles, anchored ranges, bounce updates, palette preparation, projectile creation, both fragment updates and scatter clipping now have stock exact solutions. Fixing the compiler/source boundary should precede further surface spelling tests. Private scope instruments are not build overrides and were never used for production acceptance. All branch commits remain local for the integrator; the combined ROM build and checked-in progress report update are still required at integration.

## Continued matching through the original gaps

Tags: #matching #effects #presentation #cleanup

The continuation added nine stock exact spans, +0.236 percentage points (+5,680 bytes), without concessions. Register/mode and real storage boundaries resolved all nine earlier diagnostic gaps: 080148CC, 08047DF4, 08068500, 0806AF74, 08111578, 08127590, 08128268, 08128554 and 08128D24. Each was landed separately, verified with the stock production compiler/relocator, and checked independently across its complete reviewed extent. The radial fragment update also adopts shared ContactPosition and DisplaySequenceState layouts. Broader pin removal, shared data/layout unification and Studio cleanup remain after matching is finished.

A stale scope symbol cache initially missed the renamed Nidhoggr callback after the requested tooling cherry-pick. Rebuilding only the local cache resolves it; no combined build or production compiler edit is needed. The cache key currently omits the checker and function-name inventory, a tooling follow-up for integration.
