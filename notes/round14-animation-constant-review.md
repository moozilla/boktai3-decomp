# Round 14: bounded animation constant-local pass

Tags: #round14 #concessions #matching

Date: 2026-11-16. Trial baseline: `2554f000d` on `codex/round14-fields`.

**59 constant-local warnings were removed in six assigned source units. Every
combined unit was independently byte-exact with stock agbcc.** A later audit of
the already-owned Octopus unit removed both address/dereference warnings, also
with an independent whole-unit match. No warning suppressions or compiler/build
settings changed. The parent integration task owns the full-ROM build gate.

## Accepted source changes

| Unit | Const-local before → after | Stock bytes | Commit |
| --- | ---: | ---: | --- |
| `src/scene_actor_animation.c` | 41 → 13 | 11,544 | `0c162ae12` |
| `src/actor_animation_controller.c` | 36 → 31 | 30,356 | `7474978c5` |
| `src/actor_graphics_and_basic_states.c` | 16 → 10 | 12,420 | `0f429931e` |
| `src/actor_controller_transitions.c` | 13 → 6 | 3,404 | `88e29c2bf` |
| `src/scene_event_actor.c` | 14 → 2 | 4,024 | `0bb3db59d` |
| `src/scene_actor_stored_motion.c` | 6 → 5 | 3,268 | `9e798c993` |

All six combined units total **65,016 exact bytes**. Their final concession scan
has 0 errors and 70 warnings: 67 const-local, 2 once-loop and 1 local-union. It
reports no compiler warnings. All six have no addr-deref sites.

The changes use direct literal assignments and arguments. Existing locals that
still change code generation remain visible. TimeDisplay_InitAnimatedObjects
also uses literal 2 directly, so the successful cleanup is not limited to the
scanner's special zero/one names. No replacement locals or weighting constructs
were introduced.

## Bounded trial method and remaining cases

The first pass selected declarations whose definitions were the same literal
constant, excluded address-taking and mutation, and respected block scope.
The source had 121 eligible zero/one declarations in 89 function/helper groups.
Each group was checked independently. Nonmatching groups containing several
eligible declarations were split into 40 single-site checks, retaining exact
subsets. Four manually inspected forms covered assignment inside function
arguments, an existing accumulated flag value and an aliased zero.

The compact tracked log `round14-animation-constant-trials.jsonl` records those
results and the combined accepted checks. Its mismatch byte counts describe the
whole unit after the indicated single-function/site edit; an early size change
can shift later functions and enlarge that count. The check is a byte acceptance
result, not a claim that every changed byte belongs to that function.

The main residual in actor_animation_controller is a repeated u8-one flag update.
Replacing the temporary by a literal moves the constant load after the flag
address/load. Actor_BeginFadeAnimation isolates this as three reordered
instructions. Constant-first OR, an explicit u8 operand and an explicit u8 result
all produce the same mismatch. Those declarations were retained; no helper was
invented merely to hide or rename them.

Other retained cases include constants shared by separated state stores,
velocity component stores and the hidden/visible callback paths. Direct forms
were tested. The smallest residuals are a handful of load-order/register changes;
several larger results change function size and therefore unit layout. No claim
of exhausting possible original structures is made.

## Octopus address/dereference audit

Only two addr-deref sites remained among the previously owned files, both in
Octopus_UpdateStagger. Deleting `*(&bit)` directly, or replacing the statements
by direct compound assignments, leaves 20 / 20 differing instructions in the
340-byte function. Removing only the reaction-flags occurrence leaves one
constant-load ordering difference. Actual pointers to the named flag fields
also do not match.

Both sites are solved by merging the existing mask with the actual field value
before storing that result. This removes the artificial address/dereference
without a new local or a new warning. Commit `ff28ffb91` contains the change;
all **14,980 stock bytes** in `src/octopus.c` match. The unit has 0 errors and 22
remaining const-local warnings, with no compiler or addr-deref diagnostics.

The complete accepted source delta in this bounded pass removes **61 warnings**:
59 constant-local declarations and 2 addr-deref expressions. The compiler-warning
holdouts from the earlier nine-function review remain unchanged and documented
in `round14-fields-warning-review.md`.
