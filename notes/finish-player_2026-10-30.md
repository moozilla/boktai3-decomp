# Finish-player worker batch

Tags: #matching #player #compiler #structs

Seven of the eleven assigned functions now match in ordinary C on
`codex/finish-player`. This converts 54.188% of the reserved code from guarded
drafts to exact C (+54.188 percentage points within this queue), adding
0.331724 project code percentage points against the starting report's
2,410,438-byte denominator. The exact contribution is 7,996 bytes. Four improved
drafts remain guarded and receive no exact credit.

All eleven normal-build units pass fresh stock per-unit checks over their
explicit reviewed extents. The seven C matches have no register pins, barriers,
handwritten instructions or compiler overrides. The other four still select
their original assembly fallback in normal builds. Each function has its own
source commit, in ROM order; naming metadata and this handoff are separate.

## Results in ROM order

| Address | Bytes | Result | Deciding source property |
|---|---:|---|---|
| 08143B54 | 892 | Exact: `Player_ConstrainTetherTarget` | X-before-Z scaling, a named scale result, an initial position view, and a saved unsigned X copy after both cell coordinates preserve the required value lifetimes without pins or a barrier. |
| 08143ED0 | 1,068 | Exact: `Player_UpdateTetherMovement` | The combined nullable movement predicate and a by-value one-pointer player reference together preserve the null branch and flag-load value boundary. Either change alone fails. |
| 08148318 | 752 | Exact: `Player_UpdateWaveringMovement` | Shared ordinary playback operations replace pins/barriers. Capturing the random-table origin after the first sample and separating facing capture from masking restores the evaluation order. |
| 0814B1F8 | 4,176 | Guarded: `Player_UpdateTargetMotion`, 82.4% fuzzy | Typed player/target records, whole-vector transfers, corrected animation mask, and branch-local polar calculations recover much of the function. PRE-generated output-pointer and sine-alias lifetimes remain different. |
| 0814C3A4 | 2,952 | Exact: `Player_UpdateGrabMotion` | An unsigned 16-bit bitfield mask delays constant folding until combine; an unsigned-byte folded-facing value avoids the extra wide constant reload. Separate branch-local hit results prevent incorrect tail sharing. |
| 0814D264 | 1,216 | Guarded: `Player_UpdateAttackColliderMotion`, 92.7% fuzzy | Ordinary contact/flag operations recover control flow and most allocations. A PRE-generated trig-table pseudo still receives the wrong register; a diagnostic priority increase proves the remaining allocation decision. |
| 08160108 | 456 | Guarded: `PlayerBurst_InitializeParticles`, 73.1% fuzzy | Correct 60-byte records, recovered field offsets and an ordinary advancing sixteen-particle loop repair the draft. CSE still shares the initial and loop masks across the resource call. |
| 0816233C | 604 | Exact: `PlayerAttackEffect_UpdateParticles` | Shared playback value boundaries, per-branch stored/reloaded angle, by-value sprite reference and frame-add operand order reproduce the ROM without the inherited volatile helper. |
| 082003BC | 1,620 | Exact: `PlayerClone_UpdateBehavior` | Typed clone records, delayed player-position capture, signed halfword flags and palette bitfields restore loads/stores. Reusing the height-field pointer to derive the blink drawable supplies the final required lifetime. |
| 08200C94 | 912 | Guarded: `PlayerClone_UpdatePool`, 77.4% fuzzy | Constructing the velocity address after reading movement flags restores ordering. Loop induction still chooses different address families; an optional collider also loses its preferred register. |
| 0823C4B4 | 108 | Exact: `Player_GetLinkedSmoothedDirection` | The byte-identical matched twin at 08141BAC supplies the ordinary C form: a separate wide sample and explicit second history read into a signed halfword result. |

The target-motion, grab, attack-collider, burst/effect and linked-action names
remain tentative. Their low-confidence entries are recorded in
`symbols/functions.csv`. Source address aliases are retained so this worker's
inherited generated assembly and external address references remain compatible;
`tools/split.py` supports these aliases when the integrator regenerates symbols.
No shared declarations were changed.

## Open matching work

### 0814B1F8: output pointer and captured sine table

The selected draft is 4,200 bytes against 4,176 target bytes, with 271 ROM / 281
draft differing instructions and shape 210. It improves the starting 38.7%
draft to 82.4%. Besides the recovered records, real corrections include the
secondary-animation mask (`~1`), whole-vector transfers, signed counter reads
observed through unsigned-halfword views, and splitting the angle adjustment
before multiplication. Separate polar calls retain distinct branch value
boundaries. The target getter's flag test is retained even though its two
branches currently return the same address.

In the separate `polar-relative.c` diagnostic candidate, output-address pseudo
2203 grows from flow lifetime 87 to allocation lifetime 696 (eightfold) under
PRE and has priority 114 versus the captured sine table's 555. A diagnostic
priority increase of 442 restores the low output register, but then the sine
table receives r8 instead of the ROM's r9. These numeric pseudos and lifetime
figures belong to that diagnostic candidate, not the selected `best.c` draft.
Diagnostic role bindings align the polar arithmetic without solving the whole
function. The original low table value must survive a later-removed use while a
captured alias feeds the Z calculation.

Tried pointer/result boundaries, full versus planar vector transfers, store
scopes and table capture after the X product. The second pass checked narrow
halfword X/zero values and delayed-mask folding: the narrow version is
byte-identical, and the required delayed mask already survives until combine.
Shared/typed mask variants regress. No single diagnostic knob establishes a
whole-function match; PRE lifetime/source-sharing remains open.

### 0814D264: one proven table allocation decision

The selected 1,216-byte draft has 40 ROM / 40 draft differing instructions,
shape 20 and no moved branches. Removing two ineffective barriers, simplifying
the contact helper, preserving the heading capture and using an else-local
whole angle sum recover the surrounding register choices. A signed previous
animation value restores shared tails.

Trig-table pseudo 723 has four sets and ten references; PRE expands lifetime
113 to 1,808 (sixteenfold), leaving priority 165 and stock allocation r8 rather
than r3. `AGBCC_PRI=723:+907` is the minimum tested diagnostic increase that
produces MATCH (table priority 1,072 just exceeds radius priority 1,071);
`+906` does not. No override is used in the committed source.

Per-case/per-geometry table captures, sine/cosine readers, scalar/aggregate
polar operations, a scale macro, later table capture and a centered pointer did
not reproduce it in stock C. A second-pass first-radial-value form reaches
shape zero but leaves 82/82 register differences, so the better selected draft
is preserved. The needed source expression must change the table's PRE
lifetime or priority without disturbing the recovered surrounding roles.

### 08160108: constant reuse across initialization and the loop

The selected draft compiles to 444 bytes against 456 target bytes, with 59 ROM /
53 draft differing instructions and shape 22. The former record omitted eight
bytes between position and activity, giving a 52-byte stride instead of 60.
The former loop also failed to advance its index: its increment expression
only evaluated `t1`, while the preceding `t1 = i++` read uninitialized `i`.
The `typeof(i++)` declaration itself did not evaluate the increment. Both bugs
are corrected; the committed draft advances all sixteen records using named
fields.

CSE reuses initial mask 1023 (pseudo 30) for the loop-mask setter at UID 76,
extending it across the resource call. The ROM uses a short initial r1 value
and a separate loop r4 mask; the draft consequently assigns cursor/table/mask
roles differently. The use list and instruction trace show this substitution;
the available CSE keep/canonicalization knobs do not isolate it.

On revisit, deriving ring capacity through a two-pointer helper after capturing
the next cursor delays cancellation until combine and separates the initial
and loop masks correctly. That alternative still gives 67/65 differences,
shape 38, with worse particle/table/index allocation. Its table lifetime grows
from 158 to 316 (priority 1,234 versus particle 4,387). Retain the selected draft;
there is no whole-function diagnostic proof for this blocker. A targeted
constant-substitution control would make the next experiment more decisive.

### 08200C94: loop address families and a local collider allocation

The selected draft has the correct 912-byte extent, 98/98 differing
instructions, shape 76 and three moved branches, versus the starting 104/102,
shape 84 and 908 bytes. It improves fuzzy agreement from 70.1% to 77.4%.

Loop induction reduces the player-attack address (+0x128) and reuses it for the
callback (+0x1A4), while the ROM recomputes the attack address later and reads
the callback from the parameters base. The ROM also steps the facing field
independently. `AGBCC_NOGIV=840` removes that particular reduction but cascades
into 146/146 differences; adding `AGBCC_GIV=39` yields 141/147. These establish
individual address decisions, not an exact solution. The area collider's
pseudo 273 has priority 2,033 versus angle pseudo 280's 5,217;
`AGBCC_PRI=273:+3185` corrects that allocation but leaves 91/91 differences.

Manual field walks, indexed slots, scoped collider/animation/state aliases,
angle widths, typed flag/read helpers, ordered vectors and by-value slot
references were tested. A compound vector addition restores the ROM's
velocity-first loads but worsens allocation. A second pass sharing the facing
pointer for both the early write and later read leaves 98/98 and worsens shape
to 80. Neither is selected. The next useful tool support would explain or
control GIV representative-family combination, including candidate bases and
their benefit/lifetime arithmetic. This is not a stack-slot permutation or a
single proven whole-function allocation blocker.

## Evidence and integration

Final unit logs are under `build/finish-player-validation/ADDR.log`. Searches
and compiler traces are in the ignored, local
`build/scope/campaign/FUNCTION/` and `build/matchsearch/FUNCTION/` directories;
the four blocker descriptions above preserve the important findings in Git.
Every guarded fuzzy score was recorded with `tools/check.py --nonmatching
--record`. Default-build validation uses the assembly fallback for those four;
it does not establish an exact C match.

For 082003BC, the original bare return on the null-pool path is intentional.
ROM +0x3B6 branches directly to the epilogue after the void `Anim_SetKeyframes`
call at +0x3A0, leaving that call's unconstrained r0 residue. Returning zero, a
coordinate, or the clone pointer changes the code. The exact C preserves that
observed path rather than asserting an unsupported return value.

Scope also misclassified a complete candidate with a pointer-returning typed
getter as a snippet and duplicated its records. Direct whole-file scope
compilation accepted it and proved the final behavior match; no production
tool change was needed.

The queue was attempted in ROM order and all four unfinished functions were
revisited after the later matches. Quota remained above the requested 50%
floor. No combined build, report regeneration, regrouping, push or PR was run.
The integrator must validate the combined batch with `build/boktai3.gba: OK`
and regenerate both progress files when integrating it into main.
