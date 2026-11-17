# Round 14: input, camera, and compiler-warning audit

Tags: #matching #round14 #concessions #input #camera #compiler

The worker started at `4ff682983854fe96f2fae42f5782945d532d7394`.
The accepted changes below use ordinary source and the stock compiler. Diagnostic
compiler switches were used only to establish causes; none is part of a build,
source annotation, or acceptance result.

## Accepted source changes

| Address | Change | Worker commit | Stock result |
| --- | --- | --- | --- |
| 0816B3D4 | Use the shared input-grid state and direct pressed/held key tests; remove the local volatile pad declaration. | `e4a2d2ac0` | 72 bytes exact; all 87 shared-header dependents exact. |
| 0804E8D8 | Assign the actual `SceneVec` particle position and size fields instead of masking artificial packed halfword views. | `f10b8f9eb` | 352 bytes exact; the unit has zero errors and warnings. |
| 0817AEC8 | Assign the vertical offset zero directly instead of using a constant-only local. | `d449e6e75` | 288 bytes exact; the existing sine-pointer report remains. |
| 081AE0C0 | Reset state fields with direct zero values. | `b5ae75fbb` | 5,064 bytes exact; the existing palette-specialization report remains. |

The input change adds `repeatTimer` to the existing `SceneInputGridView` at its
observed byte offset. It does not attempt to reconcile every historical pad-state
declaration. Input is updated by the task-root path, not by an interrupt.

The eleven assigned stage-2 units had **17 warnings** before these changes and
**14 warnings** afterward, with zero errors in either scan. This three-warning
source reduction is separate from the root worker's correction of diagnostic
reports emitted by rejected `try_combine` attempts. The final scoped scan used
the corrected canonical instrumented compiler.

## Camera remains open

`SpriteTrail_UpdatePositions` at 081F2404 is 304 bytes. Removing its per-file
volatile camera pointer produces 320 bytes, with 31 ROM / 39 draft instructions
different. Replacing the copied camera type with the actual `TilemapPanel`
created and destroyed in `scene_tilemap.c` gives the same result. The relevant
coordinate is the panel's `scroll.x` at offset 0x44.

`AGBCC_NOMOVE=164` on the ordinary panel candidate gives an exact diagnostic
result: loop.c otherwise hoists the camera-pointer load out of the loop. An
isolated compiler trace identifies a deeper alias issue. Sprite stores inherit
an `ADDRESS:SImode(r0)` base because `init_alias_analysis` assigns the structure
return register a private base even for this ordinary pointer parameter.
`base_alias_check` consequently rejects dependence with the global camera load.
Changing the member access's aggregate flag alone does not repair that
provenance.

Actual panel types, array and pointer traversals, coordinate helpers, and a
generic task parameter were tested. None matches. Integer parameters and label
loops were retained only as rejected diagnostics. No handle, alias wrapper,
new global, linter exception, or substitute barrier was adopted. The original
volatile declaration remains visible for further work.

## Stage-2 holdouts and evidence

| Address | Ordinary-source result and established cause |
| --- | --- |
| 0805E1D0 | Direct key handling removes the fabricated upper-half zero and leaves only two commuted AND operands different, with no structural difference. A private `CSE_NOCOMMUTE=27` experiment gives all 160 bytes exactly. `fold_rtx` constant canonicalization, rather than the register assignment reported by the current `why all` summary, is the decisive cause. |
| 08071F50 | Plain channel indexing exposes GIV reductions into three extra channel cursors. Narrow offset forms get within 5 ROM / 7 draft instructions but retain a real narrowing instruction and different category-mask operand choices. The zero offset and impossible category sign guard remain source debt; no replacement was accepted. |
| 0816E570 | The actual `gRandomTable` pointer gives 26 ROM / 22 draft differences. `AGBCC_NOEQUIV=table` alone gives an exact diagnostic result. The bitfield pointer constructor was explicitly introduced to avoid the constant-equivalence note, as documented in commit `c80901e54`; it is not evidence of a real address format. |
| 0817AEC8 | Ordinary sine/random-table and polar-helper forms do not match. Removing the pointer mask and impossible negative-radius arm together gives 53 ROM / 49 draft differences. Removing the independent zero local is exact and was accepted. |
| 08183E4C | Direct zeros produce all meaningful operations but lose the ROM's dead `movs r7, #0` and its associated save/restore. Three ROM / three draft instructions differ, including alignment. The byte-shift zero was explicitly manufactured for that leftover instruction in the original matching note. |
| 081A0BC8 | Direct direction-sector handling leaves only two `ip` versus `sl` moves different. A diagnostic force of the random-mask pseudo to `ip` is exact. The fabricated zero term artificially keeps the normal collision mask live through random sampling; normal sample return types and indexed table access do not fix that lifetime. |
| 081A14FC | Both negative tests are impossible for their unsigned sample/side inputs, and the fallback call was explicitly introduced for allocation. Removing them and the pointer-only union gives 280 ROM / 261 draft differences; keeping the union while removing guards gives 86 / 82. Both source forms and their hypotheses remain available. |
| 081AE0C0 | The remaining report differs from the explicit fake guards elsewhere: a constant 0x82 preset specializes a real palette-bank switch, so other bank operations disappear during inlining. However, the by-reference one-word preset was deliberately chosen for allocation, according to the round-4 note. A scalar interface gives 22 / 14 differences and direct palette operations give 34 / 29. This is an unresolved interface/source-authenticity question, not a justified ignore. |
| 081FC26C | Removing the impossible normalized-kind range guard gives 78 / 68 differences. The earlier matching note documents both its CSE path-boundary effect and the late dead copy it manufactures. That note also records a broad prior search; no repeat broad permutation search was performed here. |
| 08217430 | Removing the impossible callback fallback and once-loop gives 15 / 13 differences. Ordinary cursor, postincrement, descending-loop, color-width, shared-bank, and color-helper forms were checked. The initial bank copy and black-fade cursor allocation still differ. No replacement weighting loop or statement macro was introduced. |

Warnings are questions, not interchangeable verdicts. The explicit manufactured
zeros, impossible arms, and pointer-only allocation unions are established debt.
The constant palette specialization needs a more nuanced review. None of these
reports was hidden with a compiler rationale in an exception comment.

## Preserved search state

The worktree's ignored evidence is under `build/round14/stage2/`. It includes
complete candidate translation units, before/after scoped linter output,
diagnostic proof artifacts, and portable `*-source-history.json` exports. The
eleven `build/matchsearch/round14-stage2-*` stores contain 99 unique nodes in the
corrected compiler context; `search-index.json` maps addresses to those stores.
Only the separately committed and stock-verified source changes above were
accepted. Other exact nodes can retain older scaffolding and are not landing
recommendations.

The camera graph remains at
`build/matchsearch/SpriteTrail_UpdatePositions`, preserving its earlier compiler
identity. Its portable history is exported separately; the existing graph was
not silently rebound to the later compiler. Camera details and the private alias
trace are under `build/round14/camera-report.md` and `build/round14/alias.log`.

No full build or whole-tree linter run was performed by this worker. Combined
ROM verification and corpus diagnostic validation belong to the root worker.
